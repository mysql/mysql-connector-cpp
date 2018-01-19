/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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



#ifndef _MYSQL_CONNECTION_PROXY_H_
#define _MYSQL_CONNECTION_PROXY_H_

#include "native_connection_wrapper.h"

#include <cppconn/sqlstring.h>
#include <boost/shared_ptr.hpp>


namespace sql
{
namespace mysql
{
namespace NativeAPI
{
class IMySQLCAPI;


inline const char * nullIfEmpty(const ::sql::SQLString & str)
{
  return str.length() > 0 ? str.c_str() : NULL;
}


class MySQL_NativeConnectionWrapper : public NativeConnectionWrapper
{
  /* api should be declared before mysql here */
  boost::shared_ptr< IMySQLCAPI >	api;


#if (MYCPPCONN_STATIC_MYSQL_VERSION_ID > 80004)
struct MYSQL* mysql;
#else
struct st_mysql* mysql;
#endif

  ::sql::SQLString				serverInfo;


  MySQL_NativeConnectionWrapper(){}

public:

  MySQL_NativeConnectionWrapper(boost::shared_ptr<IMySQLCAPI> _api);

  virtual ~MySQL_NativeConnectionWrapper();


  uint64_t affected_rows();

  bool autocommit(bool);

  bool connect(const ::sql::SQLString & host,
        const ::sql::SQLString & user,
        const ::sql::SQLString & passwd,
        const ::sql::SQLString & db,
        unsigned int			 port,
        const ::sql::SQLString & socket_or_pipe,
        unsigned long			client_flag);

  bool commit();

  void debug(const ::sql::SQLString &);

  unsigned int errNo();

  ::sql::SQLString error();

  ::sql::SQLString escapeString(const ::sql::SQLString &);

  unsigned int field_count();

  unsigned long get_client_version();

  const ::sql::SQLString & get_server_info();

  unsigned long get_server_version();

  void get_character_set_info(void *cs);

  bool more_results();

  int next_result();

  int options(::sql::mysql::MySQL_Connection_Options, const void * );
  int options(::sql::mysql::MySQL_Connection_Options,
        const ::sql::SQLString &);
  int options(::sql::mysql::MySQL_Connection_Options, const bool &);
  int options(::sql::mysql::MySQL_Connection_Options, const int &);
  int options(::sql::mysql::MySQL_Connection_Options,
        const ::sql::SQLString &, const ::sql::SQLString &);

  int get_option(::sql::mysql::MySQL_Connection_Options, const void * );
  int get_option(::sql::mysql::MySQL_Connection_Options,
        const ::sql::SQLString &);
  int get_option(::sql::mysql::MySQL_Connection_Options, const bool &);
  int get_option(::sql::mysql::MySQL_Connection_Options, const int &);

  int query(const ::sql::SQLString &);

  int ping();

  /* int real_query(const ::sql::SQLString &, uint64_t);*/


  bool rollback();

  ::sql::SQLString sqlstate();

  bool ssl_set(const ::sql::SQLString & key,
        const ::sql::SQLString & cert,
        const ::sql::SQLString & ca,
        const ::sql::SQLString & capath,
        const ::sql::SQLString & cipher);

  ::sql::SQLString info();

  NativeResultsetWrapper * store_result();

  int use_protocol(Protocol_Type protocol);

  NativeResultsetWrapper * use_result();

  NativeStatementWrapper & stmt_init();

  unsigned int warning_count();
};

} /* namespace NativeAPI */
} /* namespace mysql */
} /* namespace sql */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
