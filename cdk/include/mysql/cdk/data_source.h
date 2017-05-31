/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * This code is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FLOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef CDK_DATA_SOURCE_H
#define CDK_DATA_SOURCE_H

#include <mysql/cdk/foundation.h>
#include <functional>


namespace cdk {

// Data source

namespace ds {

/*
 * Generic session options which are valid for any data source.
 */


class Options
{
public:

  Options()
    : m_usr(L"root"), m_has_pwd(false), m_has_db(false)
  {
  }

  Options(const Options &other)
    : m_usr(other.m_usr)
    , m_has_pwd(other.m_has_pwd), m_pwd(other.m_pwd)
    , m_has_db(other.m_has_db), m_db(other.m_db)
  {
  }

  Options(const string &usr, const std::string *pwd =NULL)
    : m_usr(usr), m_has_pwd(false), m_has_db(false)
  {
    if (pwd)
    {
      m_has_pwd = true;
      m_pwd= *pwd;
    }
  }

  virtual ~Options() {}

  virtual const string& user() const { return m_usr; }
  virtual const std::string* password() const
  { return m_has_pwd ? &m_pwd : NULL; }


  virtual const string* database() const
  {
    return m_has_db ? &m_db : NULL;
  }

  void set_database(const string &db)
  {
    m_db = db;
    m_has_db = true;
  }

protected:

  string m_usr;
  bool   m_has_pwd;
  std::string m_pwd;

  bool    m_has_db;
  string  m_db;

};


namespace mysqlx {

/*
 * A TCPIP data source represents a MySQL server accessible via TCP/IP
 * connection.
 */

class TCPIP
{
protected:
  unsigned short m_port;
  std::string m_host;

public:

  class Options;


  TCPIP(const std::string &_host="localhost", unsigned short _port =33060)
  : m_port(_port), m_host(_host)
  {
    if (_host.empty() || 0 == _host.length())
      throw_error("invalid empty host name");
  }

  virtual ~TCPIP() {}

  virtual unsigned short port() const { return m_port; }
  virtual const std::string& host() const { return m_host; }
};


class TCPIP::Options : public ds::Options
{
public:

  Options()
  {}

  Options(const string &usr, const std::string *pwd =NULL)
    : ds::Options(usr, pwd)
  {}

#ifdef WITH_SSL

  void set_tls(const cdk::connection::TLS::Options& options)
  {
    m_tls_options = options;
  }

  const cdk::connection::TLS::Options& get_tls() const
  {
    return m_tls_options;
  }

#endif

private:

#ifdef WITH_SSL
  cdk::connection::TLS::Options m_tls_options;
#endif

};

} // mysqlx

namespace mysql {

/*
 * Future Session with MYSQL
 */


class TCPIP : public cdk::ds::mysqlx::TCPIP
{
public:

  TCPIP(const std::string &_host="localhost", unsigned short _port =3306)
  : cdk::ds::mysqlx::TCPIP(_host, _port)
  {}

  virtual ~TCPIP() {}

  typedef ds::Options Options;
};

} //mysql

}  // ds


//TCPIP defaults to mysqlx::TCPIP
namespace ds {

  typedef mysqlx::TCPIP TCPIP;
  typedef mysql::TCPIP TCPIP_old;

  template <typename DS_t, typename DS_opt>
  struct DS_pair : public std::pair<DS_t, DS_opt>
  {
    DS_pair(DS_t &ds, DS_opt &opt) : std::pair<DS_t, DS_opt>(ds, opt)
    {}
  };

  class Multi_source
  {

  private:

    typedef cdk::foundation::variant <DS_pair<cdk::ds::TCPIP, cdk::ds::TCPIP::Options>,
                                      DS_pair<cdk::ds::TCPIP_old, cdk::ds::TCPIP_old::Options>> DS_variant;

    bool m_is_prioritized;
    unsigned short m_counter;

    typedef std::multimap<unsigned short, DS_variant, std::greater<unsigned short>> DS_list;
    DS_list m_ds_list;

  public:

    const unsigned short priority_range = 101;

    Multi_source() : m_is_prioritized(false), m_counter(65535)
    {
      std::srand((unsigned int)time(NULL));
    }

    template <class DS_t, class DS_opt>
    Multi_source(const DS_t &ds, const DS_opt &opt, unsigned short prio)
      : Multi_source()
    {
      add(ds, opt, prio);
    }

    template <class DS_t, class DS_opt>
    void add(const DS_t &ds, const DS_opt &opt,
             unsigned short prio)
    {
      if (m_ds_list.size() == 0)
      {
        m_is_prioritized = (prio > 0);
      }
      else
      {
        if (m_is_prioritized && prio == 0)
          throw Error(cdkerrc::generic_error,
          "Adding un-prioritized items to prioritized list is not allowed");

        if (!m_is_prioritized && prio > 0)
          throw Error(cdkerrc::generic_error,
          "Adding prioritized items to un-prioritized list is not allowed");
      }

      /*
        The internal placement of priorities will be as this:
        if list is a no-priority one the map has to retain the order of
        elements at the time of the placement. Therefore, it will count-down
        from max(unsigned short)
      */
      DS_pair<DS_t, DS_opt> pair(const_cast<DS_t&>(ds),
                               const_cast<DS_opt&>(opt));
      if (m_is_prioritized)
        m_ds_list.emplace(prio > priority_range ? priority_range : prio, pair);
      else
      {
        /*
          When list is not prioritized the map should keep the order of elements.
          This is achieved by decrementing the counter every time a new element
          goes into the list.
        */
        m_ds_list.emplace(m_counter--, pair);
      }
    }

    private:

    template <typename Visitor>
    struct Variant_visitor
    {
      Visitor *vis;
      bool stop_processing;

      Variant_visitor() : stop_processing(false)
      { }

      template <class DS_t, class DS_opt>
      void operator () (const DS_pair<DS_t, DS_opt> &ds_pair)
      {
        stop_processing = (bool)(*vis)(ds_pair.first, ds_pair.second);
      }
    };


    public:

    /*
      Call visitor(ds,opts) for each data source ds with options
      opts in the list. Do it in decreasing priority order, choosing
      random data source from several data sources with the same priority.
      If visitor(...) call returns true, stop the process.
    */
    template <class Visitor>
    void visit(Visitor &visitor)
    {
      bool stop_processing = false;

      for (auto it = m_ds_list.begin(); it != m_ds_list.end(); ++it)
      {
        DS_variant *item = NULL;

        if (m_is_prioritized)
        {
          // Get the range of items with the same priority
          auto same_prio = m_ds_list.equal_range(it->first);
          size_t number = (size_t)std::distance(same_prio.first, same_prio.second);

          if (number > 1)
          {
            // Randomly pick from the range
            size_t adv = std::rand() % number;
            std::advance(same_prio.first, adv);
            item = &same_prio.first->second;

            // Advance the main iterator to the next priority
            it = same_prio.second;
          }
          else
          {
            item = &it->second;
          }
        } // if (m_is_prioritized)
        else
        {
          // Just get the next item from the list if no priority is given
          item = &it->second;
        }

        // Give values to the visitor
        Variant_visitor<Visitor> variant_visitor;
        variant_visitor.vis = &visitor;
        /*
          Cannot use lambda because auto type for lambdas is only
          supported in C++14
        */
        item->visit(variant_visitor);
        stop_processing = variant_visitor.stop_processing;

        /* Exit if visit reported true or if we advanced to the end of the list */
        if (stop_processing || it == m_ds_list.end())
          break;

      } // for
    }

    void clear()
    {
      m_ds_list.clear();
      m_is_prioritized = false;
    }

    size_t size()
    {
      return m_ds_list.size();
    }

    struct Access;
    friend Access;
  };
}


} // cdk

#endif // CDK_DATA_SOURCE_H
