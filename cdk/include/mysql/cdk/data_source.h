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
    , m_has_db(false)
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
#ifdef WITH_SSL
    : m_tls_options(false)
#endif
  {}

  Options(const string &usr, const std::string *pwd =NULL)
    : ds::Options(usr, pwd)
  #ifdef WITH_SSL
    ,m_tls_options(false)
  #endif
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
}


} // cdk

#endif // CDK_DATA_SOURCE_H
