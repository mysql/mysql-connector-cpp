/*
 * Copyright (c) 2009, 2019, Oracle and/or its affiliates. All rights reserved.
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




#include "libmysql_dynamic_proxy.h"

#include <cppconn/exception.h>

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

#if defined(_WIN32)
static const char * const baseName = "libmysql.dll";
#elif defined(__APPLE__)
static const char * const baseName = "libmysqlclient_r.dylib";
#elif defined(__hpux) && defined(__hppa)
static const char * const baseName = "libmysqlclient_r.sl";
#else
static const char * const baseName = "libmysqlclient_r.so";
#endif

template<typename FunctionType>
FunctionType symbol_safe_cast(::sql::mysql::util::SymbolHandle raw)
{
    return *reinterpret_cast< FunctionType* >(&raw);
}

/* {{{ LibmysqlDynamicProxy::LibmysqlDynamicProxy() */
LibmysqlDynamicProxy::LibmysqlDynamicProxy()
  : LibraryLoader(baseName)
{
  init_loader();
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::LibmysqlDynamicProxy() */
LibmysqlDynamicProxy::LibmysqlDynamicProxy(const SQLString & path2libFile)
  : LibraryLoader(path2libFile.length() > 0 ? path2libFile.asStdString() : baseName)
{
  init_loader();
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::LibmysqlDynamicProxy() */
LibmysqlDynamicProxy::LibmysqlDynamicProxy(const SQLString & dir2look, const SQLString & libFileName)
  : LibraryLoader(dir2look.asStdString(), libFileName.length() > 0 ? libFileName.asStdString() : baseName)
{
  init_loader();
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::~LibmysqlDynamicProxy() */
LibmysqlDynamicProxy::~LibmysqlDynamicProxy()
{
  static ptr2mysql_library_end endProc = symbol_safe_cast<ptr2mysql_library_end>(GetProcAddr("mysql_server_end"));

  if (endProc != NULL) {
    (*endProc)();
  }
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::init_loader() */
void
LibmysqlDynamicProxy::init_loader()
{
  static ptr2mysql_library_init initProc = symbol_safe_cast<ptr2mysql_library_init>(GetProcAddr("mysql_server_init"));

  if (initProc != NULL) {
    (*initProc)(0, NULL, NULL);
  } else {
    throw ::sql::InvalidArgumentException("Loaded library doesn't contain mysql_library_init");
  }
}
/* }}} */


/************************************************************************/
/* MySQL C-API calls wrappers											*/
/************************************************************************/
/* {{{ LibmysqlDynamicProxy::affected_rows() */
my_ulonglong
LibmysqlDynamicProxy::affected_rows(MYSQL * mysql)
{
  static ptr2mysql_affected_rows ptr2_affected_rows = symbol_safe_cast<ptr2mysql_affected_rows>(GetProcAddr("mysql_affected_rows"));

  return (*ptr2_affected_rows)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::autocommit() */
my_bool
LibmysqlDynamicProxy::autocommit(MYSQL * mysql, my_bool mode)
{
  static ptr2mysql_autocommit ptr2_autocommit = symbol_safe_cast<ptr2mysql_autocommit>(GetProcAddr("mysql_autocommit"));

  return (*ptr2_autocommit)(mysql, mode);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::close() */
void
LibmysqlDynamicProxy::close(MYSQL * mysql)
{
  static ptr2mysql_close ptr2_close = symbol_safe_cast<ptr2mysql_close>(GetProcAddr("mysql_close"));

  return (*ptr2_close)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::commit() */
my_bool
LibmysqlDynamicProxy::commit(MYSQL * mysql)
{
  static ptr2mysql_commit ptr2_commit = symbol_safe_cast<ptr2mysql_commit>(GetProcAddr("mysql_commit"));

  return (*ptr2_commit)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::data_seek() */
void
LibmysqlDynamicProxy::data_seek(MYSQL_RES * result, my_ulonglong offset)
{
  static ptr2mysql_data_seek ptr2_data_seek = symbol_safe_cast<ptr2mysql_data_seek>(GetProcAddr("mysql_data_seek"));

  return (*ptr2_data_seek)(result, offset);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::debug() */
void
LibmysqlDynamicProxy::debug(const char * debug)
{
  static ptr2mysql_debug ptr2_debug = symbol_safe_cast<ptr2mysql_debug>(GetProcAddr("mysql_debug"));

  return (*ptr2_debug)(debug);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::errno() */
unsigned int
LibmysqlDynamicProxy::mysql_errno(MYSQL * mysql)
{
  static ptr2mysql_errno ptr2_errno = symbol_safe_cast<ptr2mysql_errno>(GetProcAddr("mysql_errno"));

  return (*ptr2_errno)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::error() */
const char *
LibmysqlDynamicProxy::error(MYSQL * mysql)
{
  static ptr2mysql_error ptr2_error = symbol_safe_cast<ptr2mysql_error>(GetProcAddr("mysql_error"));

  return (*ptr2_error)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::fetch_field() */
MYSQL_FIELD *
LibmysqlDynamicProxy::fetch_field(MYSQL_RES * result)
{
  static ptr2mysql_fetch_field ptr2_fetch_field = symbol_safe_cast<ptr2mysql_fetch_field>(GetProcAddr("mysql_fetch_field"));

  return (*ptr2_fetch_field)(result);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::fetch_field_direct() */
MYSQL_FIELD *
LibmysqlDynamicProxy::fetch_field_direct(MYSQL_RES * result, unsigned int fieldnr)
{
  static ptr2mysql_fetch_field_direct ptr2fetchFieldDirect=
        symbol_safe_cast<ptr2mysql_fetch_field_direct>(GetProcAddr("mysql_fetch_field_direct"));

  return (*ptr2fetchFieldDirect)(result, fieldnr);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::fetch_lengths() */
unsigned long *
LibmysqlDynamicProxy::fetch_lengths(MYSQL_RES * result)
{
  static ptr2mysql_fetch_lengths ptr2_fetch_lengths = symbol_safe_cast<ptr2mysql_fetch_lengths>(GetProcAddr("mysql_fetch_lengths"));

  return (*ptr2_fetch_lengths)(result);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::fetch_row() */
MYSQL_ROW
LibmysqlDynamicProxy::fetch_row(MYSQL_RES * result)
{
  static ptr2mysql_fetch_row ptr2_fetch_row = symbol_safe_cast<ptr2mysql_fetch_row>(GetProcAddr("mysql_fetch_row"));

  return (*ptr2_fetch_row)(result);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::field_count() */
unsigned int
LibmysqlDynamicProxy::field_count(MYSQL * mysql)
{
  static ptr2mysql_field_count ptr2_field_count = symbol_safe_cast<ptr2mysql_field_count>(GetProcAddr("mysql_field_count"));

  return (*ptr2_field_count)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::free_result() */
void
LibmysqlDynamicProxy::free_result(MYSQL_RES * result)
{
  static ptr2mysql_free_result ptr2_free_result = symbol_safe_cast<ptr2mysql_free_result>(GetProcAddr("mysql_free_result"));

  return (*ptr2_free_result)(result);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::get_client_version() */
unsigned long
LibmysqlDynamicProxy::get_client_version()
{
  static ptr2mysql_get_client_version ptr2_get_client_version=
    (ptr2mysql_get_client_version)(GetProcAddr("mysql_get_client_version"));

  return (*ptr2_get_client_version)();
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::get_server_info() */
const char *
LibmysqlDynamicProxy::get_server_info(MYSQL * mysql)
{
  static ptr2mysql_get_server_info ptr2_get_server_info = symbol_safe_cast<ptr2mysql_get_server_info>(GetProcAddr("mysql_get_server_info"));

  return (*ptr2_get_server_info)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::get_server_version() */
unsigned long
LibmysqlDynamicProxy::get_server_version(MYSQL * mysql)
{
  static ptr2mysql_get_server_version ptr2_get_server_version = symbol_safe_cast<ptr2mysql_get_server_version>(GetProcAddr("mysql_get_server_version"));

  return (*ptr2_get_server_version)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::get_character_set_info() */
void
LibmysqlDynamicProxy::get_character_set_info(MYSQL * mysql, void *cs)
{
  static ptr2mysql_get_character_set_info ptr2_get_character_set_info = symbol_safe_cast<ptr2mysql_get_character_set_info>(GetProcAddr("mysql_get_character_set_info"));

  return (*ptr2_get_character_set_info)(mysql, static_cast<MY_CHARSET_INFO *>(cs));
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::info() */
const char *
LibmysqlDynamicProxy::info(MYSQL * mysql)
{
  static ptr2mysql_info ptr2_info = symbol_safe_cast<ptr2mysql_info>(GetProcAddr("mysql_info"));

  return (*ptr2_info)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::init() */
MYSQL *
LibmysqlDynamicProxy::init(MYSQL * mysql)
{
  static ptr2mysql_init ptr2init = symbol_safe_cast<ptr2mysql_init>(GetProcAddr("mysql_init"));

  return (*ptr2init)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::library_init() */
int
LibmysqlDynamicProxy::library_init(int argc,char **argv,char **groups)
{
  static ptr2mysql_library_init ptr2_library_init = symbol_safe_cast<ptr2mysql_library_init>(GetProcAddr("mysql_library_init"));

  return (*ptr2_library_init)(argc, argv, groups);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::library_end() */
void
LibmysqlDynamicProxy::library_end()
{
  static ptr2mysql_library_end ptr2_library_end = symbol_safe_cast<ptr2mysql_library_end>(GetProcAddr("mysql_library_end"));

  return (*ptr2_library_end)();
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::more_results() */
my_bool
LibmysqlDynamicProxy::more_results(MYSQL * mysql)
{
  static ptr2mysql_more_results ptr2_more_results = symbol_safe_cast<ptr2mysql_more_results>(GetProcAddr("mysql_more_results"));

  return (*ptr2_more_results)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::next_result() */
int
LibmysqlDynamicProxy::next_result(MYSQL * mysql)
{
  static ptr2mysql_next_result ptr2_next_result = symbol_safe_cast<ptr2mysql_next_result>(GetProcAddr("mysql_next_result"));

  return (*ptr2_next_result)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::num_fields() */
unsigned int
LibmysqlDynamicProxy::num_fields(MYSQL_RES * result)
{
  static ptr2mysql_num_fields ptr2_num_fields = symbol_safe_cast<ptr2mysql_num_fields>(GetProcAddr("mysql_num_fields"));

  return (*ptr2_num_fields)(result);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::num_rows() */
my_ulonglong
LibmysqlDynamicProxy::num_rows(MYSQL_RES * result)
{
  static ptr2mysql_num_rows ptr2_num_rows = symbol_safe_cast<ptr2mysql_num_rows>(GetProcAddr("mysql_num_rows"));

  return (*ptr2_num_rows)(result);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::options() */
int
LibmysqlDynamicProxy::options(MYSQL * mysql, enum mysql_option option, const void *arg)
{
  static ptr2mysql_options ptr2_options = symbol_safe_cast<ptr2mysql_options>(GetProcAddr("mysql_options"));

  if ((*ptr2_options)(mysql, option, arg)) {
    throw sql::InvalidArgumentException("Unsupported option provided to mysql_options()");
  } else {
    return 0;
  }
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::options() */
int
LibmysqlDynamicProxy::options(MYSQL * mysql, enum mysql_option option, const void *arg1, const void *arg2)
{
  static ptr2mysql_options4 ptr2_options = symbol_safe_cast<ptr2mysql_options4>(GetProcAddr("mysql_options4"));
  if (ptr2_options != NULL) {
    if (((*ptr2_options)(mysql, option, arg1, arg2))) {
      throw sql::InvalidArgumentException("Unsupported option provided to mysql_options4()");
    } else {
      return 0;
    }
  } else {
    throw ::sql::MethodNotImplementedException("::mysql_options4()");
  }
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::get_option() */
int
LibmysqlDynamicProxy::get_option(MYSQL * mysql, enum mysql_option option, const void *arg)
{
  static ptr2mysql_get_option ptr2_get_option = symbol_safe_cast<ptr2mysql_options>(GetProcAddr("mysql_get_option"));
  if (ptr2_get_option != NULL) {
    if (((*ptr2_get_option)(mysql, option, arg))) {
      throw sql::InvalidArgumentException("Unsupported option provided to mysql_get_option()");
    } else {
      return 0;
    }
  } else {
    throw ::sql::MethodNotImplementedException("::mysql_get_option()");
  }
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::get_option() */
st_mysql_client_plugin*
LibmysqlDynamicProxy::client_find_plugin(MYSQL *mysql, const char *plugin_name, int plugin_type)
{
  static ptr2mysql_client_find_plugin ptr2_client_find_plugin = symbol_safe_cast<ptr2mysql_client_find_plugin>(GetProcAddr("mysql_client_find_plugin"));
  if (ptr2_client_find_plugin != NULL) {
    st_mysql_client_plugin *plugin = (*ptr2_client_find_plugin)(mysql, plugin_name, plugin_type);
    if(!plugin)
    {
      std::string err("Couldn't load plugin ");
      err+=plugin_name;
      throw sql::MethodNotImplementedException(err);
    }
    return plugin;
  } else {
    throw ::sql::MethodNotImplementedException("::mysql_client_find_plugin()");
  }
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::plugin_options() */
int
LibmysqlDynamicProxy::plugin_options(st_mysql_client_plugin *plugin, const char *option, const void *value)
{
  static ptr2mysql_plugin_options ptr2_plugin_options = symbol_safe_cast<ptr2mysql_plugin_options>(GetProcAddr("mysql_plugin_options"));
  if (ptr2_plugin_options != NULL) {
    if (((*ptr2_plugin_options)(plugin, option, value))) {
      std::string err("Failed to set plugin option");
      err += " '" + std::string(option) + "'";
      throw sql::InvalidArgumentException(err);
    } else {
      return 0;
    }
  } else {
    throw ::sql::MethodNotImplementedException("::mysql_plugin_options");
  }
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::plugin_get_option() */
int
LibmysqlDynamicProxy::plugin_get_option(st_mysql_client_plugin *plugin, const char *option, void *value)
{
  static ptr2mysql_plugin_get_option ptr2_plugin_get_option = symbol_safe_cast<ptr2mysql_plugin_get_option>(GetProcAddr("mysql_plugin_get_option"));
  if (ptr2_plugin_get_option != NULL) {
    if (((*ptr2_plugin_get_option)(plugin, option, value))) {
      throw sql::InvalidArgumentException("Unsupported option provided to mysql_plugin_get_option()");
    } else {
      return 0;
    }
  } else {
    throw ::sql::MethodNotImplementedException("::mysql_plugin_get_option");
  }
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::query() */
int
LibmysqlDynamicProxy::query(MYSQL * mysql, const char *stmt_str)
{
  static ptr2mysql_query ptr2_query = symbol_safe_cast<ptr2mysql_query>(GetProcAddr("mysql_query"));

  return (*ptr2_query)(mysql, stmt_str);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::ping() */
int
LibmysqlDynamicProxy::ping(MYSQL * mysql)
{
  static ptr2mysql_ping ptr2_ping = symbol_safe_cast<ptr2mysql_ping>(GetProcAddr("mysql_ping"));

  return (*ptr2_ping)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::real_connect() */
MYSQL *
LibmysqlDynamicProxy::real_connect(MYSQL * mysql,
                        const char * host,
                        const char * user,
                        const char * passwd,
                        const char * db,
                        unsigned int port,
                        const char * unix_socket,
                        unsigned long client_flag)
{
  static ptr2mysql_real_connect ptr2_real_connect=
    symbol_safe_cast<ptr2mysql_real_connect>(GetProcAddr("mysql_real_connect"));

  return (*ptr2_real_connect)(mysql, host, user, passwd, db, port, unix_socket, client_flag);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::real_connect_dns_srv() */
MYSQL *
LibmysqlDynamicProxy::real_connect_dns_srv(MYSQL * mysql,
                        const char * host,
                        const char * user,
                        const char * passwd,
                        const char * db,
                        unsigned long client_flag)
{
  static ptr2mysql_real_connect_dns_srv ptr2_real_connect_dns_srv=
    symbol_safe_cast<ptr2mysql_real_connect_dns_srv>(GetProcAddr("mysql_real_connect_dns_srv"));

  return (*ptr2_real_connect_dns_srv)(mysql, host, user, passwd, db, client_flag);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::bind_param() */
bool
LibmysqlDynamicProxy::bind_param(MYSQL *mysql, unsigned n_params,
                                 MYSQL_BIND *binds, const char **names)
{
  static ptr2mysql_bind_param ptr2_bind_param=
    symbol_safe_cast<ptr2mysql_bind_param>(GetProcAddr("mysql_bind_param"));

  return (*ptr2_bind_param)(mysql, n_params, binds, names);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::real_escape_string() */
unsigned long
LibmysqlDynamicProxy::real_escape_string(MYSQL * mysql, char * to, const char * from, unsigned long length)
{
  static ptr2mysql_real_escape_string ptr2_realescapestring = symbol_safe_cast<ptr2mysql_real_escape_string>(GetProcAddr("mysql_real_escape_string"));

  return (*ptr2_realescapestring)(mysql, to, from, length);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::real_query() */
int
LibmysqlDynamicProxy::real_query(MYSQL *mysql,const char *stmt_str, unsigned long len)
{
  static ptr2mysql_real_query ptr2_real_query = symbol_safe_cast<ptr2mysql_real_query>(GetProcAddr("mysql_real_query"));

  return (*ptr2_real_query)(mysql, stmt_str, len);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::rollback() */
my_bool
LibmysqlDynamicProxy::rollback(MYSQL * mysql)
{
  static ptr2mysql_rollback ptr2_rollback = symbol_safe_cast<ptr2mysql_rollback>(GetProcAddr("mysql_rollback"));

  return (*ptr2_rollback)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::sqlstate() */
const char *
LibmysqlDynamicProxy::sqlstate(MYSQL * mysql)
{
  static ptr2mysql_sqlstate ptr2_sqlstate = symbol_safe_cast<ptr2mysql_sqlstate>(GetProcAddr("mysql_sqlstate"));

  return (*ptr2_sqlstate)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::ssl_set() */
my_bool
LibmysqlDynamicProxy::ssl_set(MYSQL * mysql,
                      const char * key,
                      const char * cert,
                      const char * ca,
                      const char * capath,
                      const char * cipher)
{
  static ptr2mysql_ssl_set ptr2_ssl_set = symbol_safe_cast<ptr2mysql_ssl_set>(GetProcAddr("mysql_ssl_set"));

  return (*ptr2_ssl_set)(mysql, key, cert, ca, capath, cipher);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::store_result() */
MYSQL_RES *
LibmysqlDynamicProxy::store_result(MYSQL * mysql)
{
  static ptr2mysql_store_result ptr2_store_result = symbol_safe_cast<ptr2mysql_store_result>(GetProcAddr("mysql_store_result"));

  return (*ptr2_store_result)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::use_result() */
MYSQL_RES *
LibmysqlDynamicProxy::use_result(MYSQL * mysql)
{
  static ptr2mysql_use_result ptr2_use_result = symbol_safe_cast<ptr2mysql_use_result>(GetProcAddr("mysql_use_result"));

  return (*ptr2_use_result)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::warning_count() */
unsigned int
LibmysqlDynamicProxy::warning_count(MYSQL * mysql)
{
  static ptr2mysql_warning_count ptr2_warning_count= symbol_safe_cast<ptr2mysql_warning_count>(GetProcAddr("mysql_warning_count"));

  return (*ptr2_warning_count)(mysql);
}
/* }}} */


/* Prepared Statement mysql_stmt_* functions */

/* {{{ LibmysqlDynamicProxy::stmt_affected_rows() */
my_ulonglong
LibmysqlDynamicProxy::stmt_affected_rows(MYSQL_STMT *stmt)
{
  static ptr2mysql_stmt_affected_rows ptr2_stmt_affected_rows = symbol_safe_cast<ptr2mysql_stmt_affected_rows>(GetProcAddr("mysql_stmt_affected_rows"));

  return (*ptr2_stmt_affected_rows)(stmt);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_attr_set() */
my_bool
LibmysqlDynamicProxy::stmt_attr_set(MYSQL_STMT * stmt, enum enum_stmt_attr_type option, const void * arg)
{
  static ptr2mysql_stmt_attr_set ptr2_stmt_attr_set = symbol_safe_cast<ptr2mysql_stmt_attr_set>(GetProcAddr("mysql_stmt_attr_set"));

  return (*ptr2_stmt_attr_set)(stmt, option, arg);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_bind_param() */
my_bool
LibmysqlDynamicProxy::stmt_bind_param(MYSQL_STMT * stmt, MYSQL_BIND * bind)
{
  static ptr2mysql_stmt_bind_param ptr2_stmt_bind_param = symbol_safe_cast<ptr2mysql_stmt_bind_param>(GetProcAddr("mysql_stmt_bind_param"));

  return (*ptr2_stmt_bind_param)(stmt, bind);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_bind_result() */
my_bool
LibmysqlDynamicProxy::stmt_bind_result(MYSQL_STMT * stmt, MYSQL_BIND * bind)
{
  static ptr2mysql_stmt_bind_result ptr2_stmt_bind_result = symbol_safe_cast<ptr2mysql_stmt_bind_result>(GetProcAddr("mysql_stmt_bind_result"));

  return (*ptr2_stmt_bind_result)(stmt, bind);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_close() */
my_bool
LibmysqlDynamicProxy::stmt_close(MYSQL_STMT * stmt)
{
  static ptr2mysql_stmt_close ptr2_stmt_close = symbol_safe_cast<ptr2mysql_stmt_close>(GetProcAddr("mysql_stmt_close"));

  return (*ptr2_stmt_close)(stmt);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_data_seek() */
void
LibmysqlDynamicProxy::stmt_data_seek(MYSQL_STMT * stmt, my_ulonglong row_nr)
{
  static ptr2mysql_stmt_data_seek ptr2_stmt_data_seek = symbol_safe_cast<ptr2mysql_stmt_data_seek>(GetProcAddr("mysql_stmt_data_seek"));

  return (*ptr2_stmt_data_seek)(stmt, row_nr);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_errno() */
unsigned int
LibmysqlDynamicProxy::stmt_errno(MYSQL_STMT * stmt)
{
  static ptr2mysql_stmt_errno ptr2_stmt_errno = symbol_safe_cast<ptr2mysql_stmt_errno>(GetProcAddr("mysql_stmt_errno"));

  return (*ptr2_stmt_errno)(stmt);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_error() */
const char *
LibmysqlDynamicProxy::stmt_error(MYSQL_STMT * stmt)
{
  static ptr2mysql_stmt_error ptr2_stmt_error = symbol_safe_cast<ptr2mysql_stmt_error>(GetProcAddr("mysql_stmt_error"));

  return (*ptr2_stmt_error)(stmt);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_execute() */
int
LibmysqlDynamicProxy::stmt_execute(MYSQL_STMT * stmt)
{
  static ptr2mysql_stmt_execute ptr2_stmt_execute = symbol_safe_cast<ptr2mysql_stmt_execute>(GetProcAddr("mysql_stmt_execute"));

  return (*ptr2_stmt_execute)(stmt);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_fetch() */
int
LibmysqlDynamicProxy::stmt_fetch(MYSQL_STMT * stmt)
{
  static ptr2mysql_stmt_fetch ptr2_stmt_fetch = symbol_safe_cast<ptr2mysql_stmt_fetch>(GetProcAddr("mysql_stmt_fetch"));

  return (*ptr2_stmt_fetch)(stmt);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_field_count() */
unsigned int
LibmysqlDynamicProxy::stmt_field_count(MYSQL_STMT * stmt)
{
  static ptr2mysql_stmt_field_count ptr2_stmt_field_count = symbol_safe_cast<ptr2mysql_stmt_field_count>(GetProcAddr("mysql_stmt_field_count"));

  return (*ptr2_stmt_field_count)(stmt);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_init() */
MYSQL_STMT *
LibmysqlDynamicProxy::stmt_init(MYSQL * mysql)
{
  static ptr2mysql_stmt_init ptr2_stmt_init = symbol_safe_cast<ptr2mysql_stmt_init>(GetProcAddr("mysql_stmt_init"));

  return (*ptr2_stmt_init)(mysql);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_num_rows() */
my_ulonglong
LibmysqlDynamicProxy::stmt_num_rows(MYSQL_STMT * stmt)
{
  static ptr2mysql_stmt_num_rows ptr2_stmt_num_rows = symbol_safe_cast<ptr2mysql_stmt_num_rows>(GetProcAddr("mysql_stmt_num_rows"));

  return (*ptr2_stmt_num_rows)(stmt);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_param_count() */
unsigned long
LibmysqlDynamicProxy::stmt_param_count(MYSQL_STMT * stmt)
{
  static ptr2mysql_stmt_param_count ptr2_stmt_param_count = symbol_safe_cast<ptr2mysql_stmt_param_count>(GetProcAddr("mysql_stmt_param_count"));

  return (*ptr2_stmt_param_count)(stmt);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_prepare() */
int
LibmysqlDynamicProxy::stmt_prepare(MYSQL_STMT * stmt, const char * stmt_str, unsigned long len)
{
  static ptr2mysql_stmt_prepare ptr2_stmt_prepare = symbol_safe_cast<ptr2mysql_stmt_prepare>(GetProcAddr("mysql_stmt_prepare"));

  return (*ptr2_stmt_prepare)(stmt, stmt_str, len);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_result_metadata() */
MYSQL_RES *
LibmysqlDynamicProxy::stmt_result_metadata(MYSQL_STMT * stmt)
{
  static ptr2mysql_stmt_result_metadata ptr2_stmt_result_metadata = symbol_safe_cast<ptr2mysql_stmt_result_metadata>(GetProcAddr("mysql_stmt_result_metadata"));

  return (*ptr2_stmt_result_metadata)(stmt);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_send_long_data() */
my_bool
LibmysqlDynamicProxy::stmt_send_long_data(MYSQL_STMT * stmt, unsigned int par_number, const char * data, unsigned long len)
{
  static ptr2mysql_stmt_send_long_data ptr2_stmt_send_long_data = symbol_safe_cast<ptr2mysql_stmt_send_long_data>(GetProcAddr("mysql_stmt_send_long_data"));

  return (*ptr2_stmt_send_long_data)(stmt, par_number, data, len);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_sqlstate() */
const char *
LibmysqlDynamicProxy::stmt_sqlstate(MYSQL_STMT * stmt)
{
  static ptr2mysql_stmt_sqlstate ptr2_stmt_sqlstate = symbol_safe_cast<ptr2mysql_stmt_sqlstate>(GetProcAddr("mysql_stmt_sqlstate"));

  return (*ptr2_stmt_sqlstate)(stmt);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_store_result() */
int
LibmysqlDynamicProxy::stmt_store_result(MYSQL_STMT * stmt)
{
  static ptr2mysql_stmt_store_result ptr2_stmt_store_result = symbol_safe_cast<ptr2mysql_stmt_store_result>(GetProcAddr("mysql_stmt_store_result"));

  return (*ptr2_stmt_store_result)(stmt);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_next_result() */
int
LibmysqlDynamicProxy::stmt_next_result(MYSQL_STMT * stmt)
{
  static ptr2mysql_stmt_next_result ptr2_stmt_next_result = symbol_safe_cast<ptr2mysql_stmt_next_result>(GetProcAddr("mysql_stmt_next_result"));
  if (ptr2_stmt_next_result != NULL) {
    return (*ptr2_stmt_next_result)(stmt);
  } else {
    throw ::sql::MethodNotImplementedException("::mysql_stmt_next_result()");
  }
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::stmt_free_result() */
bool
LibmysqlDynamicProxy::stmt_free_result(MYSQL_STMT * stmt)
{
  static ptr2mysql_stmt_free_result ptr2_stmt_free_result = symbol_safe_cast<ptr2mysql_stmt_free_result>(GetProcAddr("mysql_stmt_free_result"));

  return (*ptr2_stmt_free_result)(stmt);
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::thread_end() */
void
LibmysqlDynamicProxy::thread_end()
{
  static ptr2mysql_thread_end ptr2_thread_end = symbol_safe_cast<ptr2mysql_thread_end>(GetProcAddr("mysql_thread_end"));

  (*ptr2_thread_end)();
}
/* }}} */


/* {{{ LibmysqlDynamicProxy::thread_init() */
void
LibmysqlDynamicProxy::thread_init()
{
  static ptr2mysql_thread_init ptr2_thread_init = symbol_safe_cast<ptr2mysql_thread_init>(GetProcAddr("mysql_thread_init"));

  (*ptr2_thread_init)();
}
/* }}} */


} /* namespace util */
} /* namespace mysql */
} /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
