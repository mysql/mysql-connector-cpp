/*
  Copyright (c) 2009, 2010, Oracle and/or its affiliates. All rights reserved.

  The MySQL Connector/C++ is licensed under the terms of the GPLv2
  <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
  MySQL Connectors. There are special exceptions to the terms and
  conditions of the GPLv2 as it is applied to this software, see the
  FLOSS License Exception
  <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/


#include "libmysql_static_proxy.h"


namespace sql
{
namespace mysql
{
namespace NativeAPI
{

/* {{{ LibmysqlStaticProxy::LibmysqlStaticProxy() */
LibmysqlStaticProxy::LibmysqlStaticProxy()
{
	this->library_init(0, NULL, NULL);
}
/* }}} */

/* {{{ LibmysqlStaticProxy::~LibmysqlStaticProxy() */
LibmysqlStaticProxy::~LibmysqlStaticProxy()
{
	this->library_end();
}
/* }}} */

// MySQL C-API calls wrappers

/* {{{ LibmysqlStaticProxy::affected_rows() */
my_ulonglong
LibmysqlStaticProxy::affected_rows(MYSQL * mysql)
{
	return ::mysql_affected_rows(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::autocommit() */
my_bool
LibmysqlStaticProxy::autocommit(MYSQL * mysql, my_bool mode)
{
	return ::mysql_autocommit(mysql, mode);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::close() */
void
LibmysqlStaticProxy::close(MYSQL * mysql)
{
	return ::mysql_close(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::commit() */
my_bool
LibmysqlStaticProxy::commit(MYSQL * mysql)
{
	return ::mysql_commit(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::data_seek() */
void
LibmysqlStaticProxy::data_seek(MYSQL_RES * result, my_ulonglong offset)
{
	return ::mysql_data_seek(result, offset);
}
/* }}} */

/* {{{ LibmysqlStaticProxy::debug() */
void
LibmysqlStaticProxy::debug(const char * debug)
{
	return ::mysql_debug(debug);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::mysql_errno() */
unsigned int
LibmysqlStaticProxy::mysql_errno(MYSQL * mysql)
{
	return ::mysql_errno(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::error() */
const char *
LibmysqlStaticProxy::error(MYSQL * mysql)
{
	return ::mysql_error(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::fetch_field() */
MYSQL_FIELD *
LibmysqlStaticProxy::fetch_field(MYSQL_RES * result)
{
	return ::mysql_fetch_field(result);
}
/* }}} */

/* {{{ LibmysqlStaticProxy::fetch_field_direct() */
MYSQL_FIELD *
LibmysqlStaticProxy::fetch_field_direct(MYSQL_RES * result, unsigned int fieldnr)
{
	return ::mysql_fetch_field_direct(result, fieldnr);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::fetch_lengths() */
unsigned long *
LibmysqlStaticProxy::fetch_lengths(MYSQL_RES * result)
{
	return ::mysql_fetch_lengths(result);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::fetch_row() */
MYSQL_ROW
LibmysqlStaticProxy::fetch_row(MYSQL_RES * result)
{
	return ::mysql_fetch_row(result);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::field_count() */
unsigned int
LibmysqlStaticProxy::field_count(MYSQL * mysql)
{
	return ::mysql_field_count(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::free_result() */
void
LibmysqlStaticProxy::free_result(MYSQL_RES * result)
{
	return ::mysql_free_result(result);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::get_client_version() */
unsigned long
LibmysqlStaticProxy::get_client_version()
{
	return ::mysql_get_client_version();
}
/* }}} */


/* {{{ LibmysqlStaticProxy::get_server_info() */
const char *
LibmysqlStaticProxy::get_server_info(MYSQL * mysql)
{
	return ::mysql_get_server_info(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::get_server_version() */
unsigned long
LibmysqlStaticProxy::get_server_version(MYSQL * mysql)
{
	return ::mysql_get_server_version(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::init() */
MYSQL *
LibmysqlStaticProxy::init(MYSQL * mysql)
{
	return ::mysql_init(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::library_init() */
int
LibmysqlStaticProxy::library_init(int argc,char **argv,char **groups)
{
	return ::mysql_library_init(argc, argv, groups);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::library_end() */
void
LibmysqlStaticProxy::library_end()
{
	return ::mysql_library_end();
}
/* }}} */


/* {{{ LibmysqlStaticProxy::more_results() */
my_bool
LibmysqlStaticProxy::more_results(MYSQL * mysql)
{
	return ::mysql_more_results(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::next_result() */
int
LibmysqlStaticProxy::next_result(MYSQL * mysql)
{
	return ::mysql_next_result(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::num_fields() */
unsigned int
LibmysqlStaticProxy::num_fields(MYSQL_RES * result)
{
	return ::mysql_num_fields(result);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::num_rows() */
my_ulonglong
LibmysqlStaticProxy::num_rows(MYSQL_RES * result)
{
	return ::mysql_num_rows(result);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::options() */
int
LibmysqlStaticProxy::options(MYSQL * mysql, enum mysql_option option, const void *arg)
{
	// in 5.0 mysql_options's 3rd parameter is "const char *"
	return ::mysql_options(mysql, option, static_cast<const char *>(arg));
}
/* }}} */


/* {{{ LibmysqlStaticProxy::query() */
int
LibmysqlStaticProxy::query(MYSQL * mysql, const char *stmt_str)
{
	return ::mysql_query(mysql, stmt_str);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::ping() */
int
LibmysqlStaticProxy::ping(MYSQL * mysql)
{
	return ::mysql_ping(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::real_connect() */
MYSQL *
LibmysqlStaticProxy::real_connect(MYSQL * mysql,
								const char * host,
								const char * user,
								const char * passwd,
								const char * db,
								unsigned int port,
								const char * unix_socket,
								unsigned long client_flag)
{
	return ::mysql_real_connect(mysql, host, user, passwd, db, port, unix_socket, client_flag);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::real_escape_string() */
unsigned long
LibmysqlStaticProxy::real_escape_string(MYSQL * mysql, char * to, const char * from, unsigned long length)
{
	return ::mysql_real_escape_string(mysql, to, from, length);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::real_query() */
int
LibmysqlStaticProxy::real_query(MYSQL *mysql,const char *stmt_str, unsigned long len)
{
	return ::mysql_real_query(mysql, stmt_str, len);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::rollback() */
my_bool
LibmysqlStaticProxy::rollback(MYSQL * mysql)
{
	return ::mysql_rollback(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::sqlstate() */
const char *
LibmysqlStaticProxy::sqlstate(MYSQL * mysql)
{
	return ::mysql_sqlstate(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::ssl_set() */
my_bool
LibmysqlStaticProxy::ssl_set(MYSQL* mysql,
							const char * key,
							const char * cert,
							const char * ca,
							const char * capath,
							const char * cipher)
{
	return ::mysql_ssl_set(mysql, key, cert, ca, capath, cipher);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::store_result() */
MYSQL_RES *
LibmysqlStaticProxy::store_result(MYSQL * mysql)
{
	return ::mysql_store_result(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::use_result() */
MYSQL_RES *
LibmysqlStaticProxy::use_result(MYSQL * mysql)
{
	return ::mysql_use_result(mysql);
}
/* }}} */


/* Prepared Statement mysql_stmt_* functions */
/* {{{ LibmysqlStaticProxy::stmt_affected_rows() */
my_ulonglong
LibmysqlStaticProxy::stmt_affected_rows(MYSQL_STMT *stmt)
{
	return ::mysql_stmt_affected_rows(stmt);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_attr_set() */
my_bool
LibmysqlStaticProxy::stmt_attr_set(MYSQL_STMT * stmt, enum enum_stmt_attr_type option, const void * arg)
{
	return ::mysql_stmt_attr_set(stmt, option, arg);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_bind_param() */
my_bool
LibmysqlStaticProxy::stmt_bind_param(MYSQL_STMT * stmt, MYSQL_BIND * bind)
{
	return ::mysql_stmt_bind_param(stmt, bind);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_bind_result() */
my_bool
LibmysqlStaticProxy::stmt_bind_result(MYSQL_STMT * stmt, MYSQL_BIND * bind)
{
	return ::mysql_stmt_bind_result(stmt, bind);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_close() */
my_bool
LibmysqlStaticProxy::stmt_close(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_close(stmt);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_data_seek() */
void
LibmysqlStaticProxy::stmt_data_seek(MYSQL_STMT * stmt, my_ulonglong row_nr)
{
	return ::mysql_stmt_data_seek(stmt, row_nr);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_errno() */
unsigned int
LibmysqlStaticProxy::stmt_errno(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_errno(stmt);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_error() */
const char *
LibmysqlStaticProxy::stmt_error(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_error(stmt);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_execute() */
int
LibmysqlStaticProxy::stmt_execute(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_execute(stmt);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::data_seek() */
int
LibmysqlStaticProxy::stmt_fetch(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_fetch(stmt);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_field_count() */
unsigned int
LibmysqlStaticProxy::stmt_field_count(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_field_count(stmt);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_init() */
MYSQL_STMT *
LibmysqlStaticProxy::stmt_init(MYSQL * mysql)
{
	return ::mysql_stmt_init(mysql);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_num_rows() */
my_ulonglong
LibmysqlStaticProxy::stmt_num_rows(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_num_rows(stmt);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_param_count() */
unsigned long
LibmysqlStaticProxy::stmt_param_count(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_param_count(stmt);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_prepare() */
int
LibmysqlStaticProxy::stmt_prepare(MYSQL_STMT * stmt, const char * stmt_str, unsigned long len)
{
	return ::mysql_stmt_prepare(stmt, stmt_str, len);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_result_metadata() */
MYSQL_RES *
LibmysqlStaticProxy::stmt_result_metadata(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_result_metadata(stmt);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_send_long_data() */
my_bool
LibmysqlStaticProxy::stmt_send_long_data(MYSQL_STMT * stmt,
										unsigned int  par_number,
										const char *  data,
										unsigned long len)
{
	return ::mysql_stmt_send_long_data(stmt, par_number, data, len);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_sqlstate() */
const char *
LibmysqlStaticProxy::stmt_sqlstate(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_sqlstate(stmt);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::stmt_store_result() */
int
LibmysqlStaticProxy::stmt_store_result(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_store_result(stmt);
}
/* }}} */


/* {{{ LibmysqlStaticProxy::thread_end() */
void
LibmysqlStaticProxy::thread_end()
{
	::mysql_thread_end();
}
/* }}} */


/* {{{ LibmysqlStaticProxy::thread_init() */
void
LibmysqlStaticProxy::thread_init()
{
	::mysql_thread_init();
}
/* }}} */

} /* namespace NativeAPI */
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
