/*
 * Copyright (c) 2015, 2019, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef CDK_DATA_SOURCE_H
#define CDK_DATA_SOURCE_H

#include <mysql/cdk/foundation.h>

PUSH_SYS_WARNINGS_CDK
#include <functional>
#include <algorithm>
#include <set>
#include <random>
#include "api/expression.h"
POP_SYS_WARNINGS_CDK


namespace cdk {

// Data source

namespace ds {


struct Attr_processor
{
  virtual ~Attr_processor() {}
  virtual void attr(const string &key, const string &val)=0;
};

class Session_attributes
  : public cdk::api::Expr_base<Attr_processor>
{};


/*
 * Generic session options which are valid for any data source.
 */



template <class Base>
class Options
    : public Base
    , public Session_attributes
    , public Attr_processor
{
public:

  Options()
    : m_usr("root"), m_has_pwd(false), m_has_db(false)
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

  void set_attributes(std::map<std::string,std::string> &connection_attr)
  {
    m_connection_attr = connection_attr;
  }

  const Session_attributes* attributes() const
  {
    if (m_connection_attr.empty())
      return nullptr;
    return this;
  }

  void process(Processor &prc) const override
  {
    for (auto &el :  m_connection_attr)
    {
      prc.attr(el.first, el.second);
    }
  }

  void attr(const string &key, const string &val) override
  {
    m_connection_attr[key]=val;
  }

protected:

  string m_usr;
  bool   m_has_pwd;
  std::string m_pwd;

  bool    m_has_db;
  string  m_db;
  std::map<std::string,std::string> m_connection_attr;

};


namespace mysqlx {

/*
 * A TCPIP data source represents a MySQL server accessible via TCP/IP
 * connection using the X Protocol.
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


class Protocol_options
{

  public:

  enum auth_method_t {
    DEFAULT,
    PLAIN,
    MYSQL41,
    EXTERNAL,
    SHA256_MEMORY
  };

  virtual auth_method_t auth_method() const = 0;

};


class Options
  : public ds::Options<Protocol_options>,
    public foundation::connection::Socket_base::Options
{
protected:

  auth_method_t m_auth_method = DEFAULT;

public:

  Options()
  {}

  Options(const string &usr, const std::string *pwd =NULL)
    : ds::Options<Protocol_options>(usr, pwd)
  {}

  void set_auth_method(auth_method_t auth_method)
  {
    m_auth_method = auth_method;
  }

  auth_method_t auth_method() const
  {
    return m_auth_method;
  }

};


class TCPIP::Options
  : public ds::mysqlx::Options
{
public:

  typedef cdk::connection::TLS::Options  TLS_options;

private:

#ifdef WITH_SSL
  cdk::connection::TLS::Options m_tls_options;
#endif

  bool m_dns_srv = false;

public:

  Options()
  {}

  Options(const string &usr, const std::string *pwd =NULL)
    : ds::mysqlx::Options(usr, pwd)
  {}

#ifdef WITH_SSL

  void set_tls(const TLS_options& options)
  {
    m_tls_options = options;
  }

  const TLS_options& get_tls() const
  {
    return m_tls_options;
  }

  bool get_dns_srv() const
  {
    return m_dns_srv;
  }

  void set_dns_srv(bool dns_srv)
  {
    m_dns_srv = dns_srv;
  }

#endif

};


#ifndef _WIN32
class Unix_socket
{
protected:
  std::string m_path;

public:

  class Options;

  Unix_socket(const std::string &path)
    : m_path(path)
  {
    if (path.empty() || 0 == path.length())
      throw_error("invalid empty socket path");
  }

  virtual ~Unix_socket() {}

  virtual const std::string& path() const { return m_path; }
};

class Unix_socket::Options
  : public ds::mysqlx::Options
{
  public:

  Options()
  {}

  Options(const string &usr, const std::string *pwd = NULL)
    : ds::mysqlx::Options(usr, pwd)
  {}

};
#endif //_WIN32

} // mysqlx


namespace mysql {

/*
 * Future Session with MYSQL over legacy protocol.
 */

class Protocol_options
{};

class TCPIP : public cdk::ds::mysqlx::TCPIP
{
public:

  TCPIP(const std::string &_host="localhost", unsigned short _port =3306)
  : cdk::ds::mysqlx::TCPIP(_host, _port)
  {}

  virtual ~TCPIP() {}

  typedef ds::Options<Protocol_options> Options;
};

} //mysql

}  // ds


//TCPIP defaults to mysqlx::TCPIP
namespace ds {

  typedef mysqlx::TCPIP TCPIP;
#ifndef _WIN32
  typedef mysqlx::Unix_socket Unix_socket;
#endif //_WIN32
  typedef mysql::TCPIP TCPIP_old;

  template <typename DS_t, typename DS_opt>
  struct DS_pair : public std::pair<DS_t, DS_opt>
  {
    DS_pair(const DS_pair&) = default;
#ifdef HAVE_MOVE_CTORS
    DS_pair(DS_pair&&) = default;
#endif
    DS_pair(DS_t &ds, DS_opt &opt) : std::pair<DS_t, DS_opt>(ds, opt)
    {}
  };

  class Multi_source
  {

  private:

    typedef cdk::foundation::variant <
      DS_pair<cdk::ds::TCPIP, cdk::ds::TCPIP::Options>
#ifndef _WIN32
      ,DS_pair<cdk::ds::Unix_socket, cdk::ds::Unix_socket::Options>
#endif //_WIN32
      ,DS_pair<cdk::ds::TCPIP_old, cdk::ds::TCPIP_old::Options>
    >
    DS_variant;

    bool m_is_prioritized;
    unsigned short m_counter;

    struct Prio
    {
      unsigned short prio;
      uint16_t weight;
      operator unsigned short() const
      {
        return prio;
      }

      bool operator < (const Prio &other) const
      {
        return prio < other.prio;
      }
    };

    typedef std::multimap<Prio, DS_variant, std::greater<Prio>> DS_list;
    DS_list m_ds_list;

  public:

    Multi_source() : m_is_prioritized(false), m_counter(65535)
    {
      std::srand((unsigned int)time(NULL));
    }

    template <class DS_t, class DS_opt>
    void add(const DS_t &ds, const DS_opt &opt,
             unsigned short prio, uint16_t weight = 1)
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
        m_ds_list.emplace(Prio({prio,weight}), pair);
      else
      {
        /*
          When list is not prioritized the map should keep the order of elements.
          This is achieved by decrementing the counter every time a new element
          goes into the list.
        */
        m_ds_list.emplace(Prio({m_counter--,weight}), pair);
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
      randomly among data sources with the same priority.
      If visitor(...) call returns true, stop the process.
    */

    template <class Visitor>
    void visit(Visitor &visitor)
    {
      bool stop_processing = false;
      std::vector<DS_variant*> same_prio;
      std::vector<uint16_t> weights;


      struct DNS_SRV_visitor
      {
        Visitor &visitor;
        cdk::ds::Multi_source src;
        uint16_t total_weight = 0;
        bool has_srv = false;

        DNS_SRV_visitor(Visitor &_visitor)
          : visitor(_visitor)
        {}

        bool operator() (const ds::TCPIP &ds, const ds::TCPIP::Options &opts)
        {
          has_srv = opts.get_dns_srv();

          if(has_srv)
          {
            ds::TCPIP::Options new_opts(opts);
            new_opts.set_dns_srv(false);

            auto list = cdk::foundation::connection::srv_list(ds.host());


            if (list.empty())
            {
              std::string err = "Unable to locate any hosts for ";
              err+= ds.host();
              throw_error(err.c_str());
            }

            auto &tls= const_cast<ds::TCPIP::Options::TLS_options&>(opts.get_tls());

            for (auto &el : list)
            {
              TCPIP::Options::TLS_options tls (new_opts.get_tls());
              tls.set_host_name(el.name);
              new_opts.set_tls(tls);
              //Prio is negative because for URI prio, less is better, but for
              //SRV record, more is better
              src.add(ds::TCPIP(el.name, el.port),new_opts,-el.prio,el.weight);

              total_weight+=el.weight;
            }

            src.visit(visitor);

          }

          return has_srv;
        }



      #ifndef WIN32
        bool operator() (const ds::Unix_socket&ds, const ds::Unix_socket::Options &options)
        {
          return false;
        }
      #endif
        bool operator() (const ds::TCPIP_old &ds, const ds::TCPIP_old::Options &options)
        {
          return false;
        }

      };

      DNS_SRV_visitor dns_srv_visitor {visitor};

      //SRV only possible with 1 host
      if(m_ds_list.size() == 1)
      {
        // Give values to the visitor
        Variant_visitor<DNS_SRV_visitor> variant_visitor;
        variant_visitor.vis = &dns_srv_visitor;

        m_ds_list.begin()->second.visit(variant_visitor);

        if(variant_visitor.stop_processing)
          return;
      }


      for (auto it = m_ds_list.begin(); !stop_processing;)
      {
        DS_variant *item = NULL;

        if (m_is_prioritized)
        {
          if (same_prio.empty())
          {
            if (it == m_ds_list.end())
              break;

            //  Get items with the same priority and store them in same_prio set

            auto same_range = m_ds_list.equal_range(it->first);
            it = same_range.second;

            for (auto it1 = same_range.first; it1 != same_range.second; ++it1)
            {
              //If weight is not specified, we need to set all weight values
              //with same, os that discrete_distribution works as expected
              weights.push_back(
                    dns_srv_visitor.total_weight == 0 ? 1 : it1->first.weight);
              same_prio.push_back(&(it1->second));
            }
          }

          std::random_device generator;
          std::discrete_distribution<int> distribution(
                weights.begin(), weights.end());



          auto el = same_prio.begin();

          int pos = 0;

          if (same_prio.size() > 1)
          {
            pos = distribution(generator);
            std::advance(el, pos);
          }

          item = *el;
          same_prio.erase(same_prio.begin()+pos);
          weights.erase(weights.begin()+pos);

        } // if (m_is_prioritized)
        else
        {
          if (it == m_ds_list.end())
            break;

          // Just get the next item from the list if no priority is given
          item = &(it->second);
          ++it;
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
