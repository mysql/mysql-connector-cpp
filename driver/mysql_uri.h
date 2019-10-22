/*
 * Copyright (c) 2012, 2019, Oracle and/or its affiliates. All rights reserved.
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
    Host_data(sql::SQLString name_, unsigned int port_ = 0)
      : name(name_)
      , port(port_)
    {}

    sql::SQLString  name;
    uint16_t        port;
  };

  using Host_List=std::vector<Host_data>;

private:
  NativeAPI::Protocol_Type protocol;
  uint16_t                 port;

  Host_List                host_list;

  sql::SQLString           schema;
  bool                     has_port = false;

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
    return only first element for Host/Port
  */
  const sql::SQLString &            Host();
  unsigned int                      Port();
  const sql::SQLString &            SocketOrPipe();
  inline	const sql::SQLString &		Schema()	{return schema;}
  inline	NativeAPI::Protocol_Type	Protocol()	{return protocol;}

  void addHost	(const sql::SQLString &host, unsigned int port);
  void setHost  (const sql::SQLString &host);
  void setSocket	(const sql::SQLString &s);
  void setPipe	(const sql::SQLString &p);
  void setPort	(uint16_t p);
  void setSchema	(const sql::SQLString &s)	{schema= s.c_str();}

  void setProtocol(NativeAPI::Protocol_Type p){protocol= p;}

  bool hasPort() { return has_port; }

};


bool tcpProtocol(MySQL_Uri& uri);


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

