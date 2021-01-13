/*
 * Copyright (c) 2012, 2020, Oracle and/or its affiliates.
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


#ifndef _MYSQL_URI_H_
#define _MYSQL_URI_H_

#include "nativeapi/native_connection_wrapper.h"
#include <cppconn/sqlstring.h>
#include <vector>

namespace sql
{
namespace mysql
{

class MySQL_Uri
{
public:
  struct Host_data
  {
    Host_data(sql::SQLString name_, unsigned int port_ = 0, bool has_port_ = false)
      : name(name_)
      , port(port_)
      , protocol(NativeAPI::PROTOCOL_TCP)
      , has_port(has_port_)
    {}

    Host_data();


    const SQLString & Host();
    unsigned int      Port();
    const SQLString & SocketOrPipe();
    inline
    NativeAPI::Protocol_Type Protocol() {return protocol;}
    bool hasPort() { return has_port; }

    void setProtocol(NativeAPI::Protocol_Type p) { protocol= p; }

    void setHost(const sql::SQLString &h, uint16_t p)
    {
      setProtocol(NativeAPI::PROTOCOL_TCP);
      name = h;
      port = p;
      has_port = true;
    }
    void setHost(const sql::SQLString &host)
    {
      setProtocol(NativeAPI::PROTOCOL_TCP);
      name = host;
    }

    void setSocket(const sql::SQLString &s)
    {
      setProtocol(NativeAPI::PROTOCOL_SOCKET);
      name = s;
    }

    void setPipe(const sql::SQLString &p)
    {
     setProtocol(NativeAPI::PROTOCOL_PIPE);
     name = p;
    }

    void setPort(uint16_t p)
    {
      setProtocol(NativeAPI::PROTOCOL_TCP);
      port = p;
      has_port = true;
    }


  private:
    sql::SQLString           name;
    uint16_t                 port;
    NativeAPI::Protocol_Type protocol;
    bool                     has_port;
  };

  using Host_List=std::vector<Host_data>;

private:

  Host_List                host_list;

  sql::SQLString           schema;
  uint16_t default_port;

public:
  MySQL_Uri();

  /*
    Iterate over host list
  */
  Host_List::iterator begin() { return host_list.begin(); }
  Host_List::iterator end() { return host_list.end(); }
  size_t size() const { return host_list.size(); }

  /*
    Clear host list
  */
  void clear() { host_list.clear(); }

  /*
    Remove host from the list
  */
  void erase(Host_List::iterator el)
  {
    host_list.erase(el);
  }

  /*
    Add hosts to list
  */
  void addHost(Host_data d)
  {
    host_list.push_back(d);
  }

  void setHost(Host_data d)
  {
    host_list.clear();
    host_list.push_back(d);
  }

  void setDefaultPort(uint16_t port) { default_port = port;}
  uint16_t DefaultPort() { return default_port; }

  inline
  sql::SQLString Schema() { return schema; }

  void setSchema(const sql::SQLString &s) { schema= s.c_str(); }

};


bool tcpProtocol(MySQL_Uri::Host_data &uri);


bool parseUri(const sql::SQLString & str, MySQL_Uri& uri);


} /* namespace mysql */
} /* namespace sql */

#endif /*_MYSQL_URI_H_*/
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

