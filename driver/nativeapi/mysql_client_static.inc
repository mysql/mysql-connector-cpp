/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/


#include "mysql_client_static.h"


namespace sql
{
namespace mysql
{
namespace NativeAPI
{

/* {{{ MySQL_Client_Loader::MySQL_Client_Static() */
MySQL_Client_Static::MySQL_Client_Static()
{
	this->library_init(0, NULL, NULL);
}
/* }}} */

/* {{{ MySQL_Client_Loader::~MySQL_Client_Static() */
MySQL_Client_Static::~MySQL_Client_Static()
{
	this->library_end();
}
/* }}} */

// MySQL C-API calls wrappers

/* {{{ MySQL_Client_Loader::affected_rows() */
my_ulonglong
MySQL_Client_Static::affected_rows(MYSQL * mysql)
{
	return ::mysql_affected_rows(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::autocommit() */
my_bool
MySQL_Client_Static::autocommit(MYSQL * mysql, my_bool mode)
{
	return ::mysql_autocommit(mysql, mode);
}
/* }}} */


/* {{{ MySQL_Client_Loader::close() */
void
MySQL_Client_Static::close(MYSQL * mysql)
{
	return ::mysql_close(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::commit() */
my_bool
MySQL_Client_Static::commit(MYSQL * mysql)
{
	return ::mysql_commit(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::data_seek() */
void
MySQL_Client_Static::data_seek(MYSQL_RES * result, my_ulonglong offset)
{
	return ::mysql_data_seek(result, offset);
}
/* }}} */

/* {{{ MySQL_Client_Loader::debug() */
void
MySQL_Client_Static::debug(const char * debug)
{
	return ::mysql_debug(debug);
}
/* }}} */


/* {{{ MySQL_Client_Loader::mysql_errno() */
unsigned int
MySQL_Client_Static::mysql_errno(MYSQL * mysql)
{
	return ::mysql_errno(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::error() */
const char *
MySQL_Client_Static::error(MYSQL * mysql)
{
	return ::mysql_error(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::fetch_field() */
MYSQL_FIELD *
MySQL_Client_Static::fetch_field(MYSQL_RES * result)
{
	return ::mysql_fetch_field(result);
}
/* }}} */

/* {{{ MySQL_Client_Loader::fetch_field_direct() */
MYSQL_FIELD *
MySQL_Client_Static::fetch_field_direct(MYSQL_RES * result, unsigned int fieldnr)
{
	return ::mysql_fetch_field_direct(result, fieldnr);
}
/* }}} */


/* {{{ MySQL_Client_Loader::fetch_lengths() */
unsigned long *
MySQL_Client_Static::fetch_lengths(MYSQL_RES * result)
{
	return ::mysql_fetch_lengths(result);
}
/* }}} */


/* {{{ MySQL_Client_Loader::fetch_row() */
MYSQL_ROW
MySQL_Client_Static::fetch_row(MYSQL_RES * result)
{
	return ::mysql_fetch_row(result);
}
/* }}} */


/* {{{ MySQL_Client_Loader::field_count() */
unsigned int
MySQL_Client_Static::field_count(MYSQL * mysql)
{
	return ::mysql_field_count(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::free_result() */
void
MySQL_Client_Static::free_result(MYSQL_RES * result)
{
	return ::mysql_free_result(result);
}
/* }}} */


/* {{{ MySQL_Client_Loader::get_client_version() */
unsigned long
MySQL_Client_Static::get_client_version()
{
	return ::mysql_get_client_version();
}
/* }}} */


/* {{{ MySQL_Client_Loader::get_server_info() */
const char *
MySQL_Client_Static::get_server_info(MYSQL * mysql)
{
	return ::mysql_get_server_info(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::get_server_version() */
unsigned long
MySQL_Client_Static::get_server_version(MYSQL * mysql)
{
	return ::mysql_get_server_version(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::init() */
MYSQL *
MySQL_Client_Static::init(MYSQL * mysql)
{
	return ::mysql_init(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::library_init() */
int
MySQL_Client_Static::library_init(int argc,char **argv,char **groups)
{
	return ::mysql_library_init(argc, argv, groups);
}
/* }}} */


/* {{{ MySQL_Client_Loader::library_end() */
void
MySQL_Client_Static::library_end()
{
	return ::mysql_library_end();
}
/* }}} */


/* {{{ MySQL_Client_Loader::more_results() */
my_bool
MySQL_Client_Static::more_results(MYSQL * mysql)
{
	return ::mysql_more_results(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::next_result() */
int
MySQL_Client_Static::next_result(MYSQL * mysql)
{
	return ::mysql_next_result(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::num_fields() */
unsigned int
MySQL_Client_Static::num_fields(MYSQL_RES * result)
{
	return ::mysql_num_fields(result);
}
/* }}} */


/* {{{ MySQL_Client_Loader::num_rows() */
my_ulonglong
MySQL_Client_Static::num_rows(MYSQL_RES * result)
{
	return ::mysql_num_rows(result);
}
/* }}} */


/* {{{ MySQL_Client_Loader::options() */
int
MySQL_Client_Static::options(MYSQL * mysql, enum mysql_option option, const void *arg)
{
	// in 5.0 mysql_options's 3rd parameter is "const char *"
	return ::mysql_options(mysql, option, static_cast<const char *>(arg));
}
/* }}} */


/* {{{ MySQL_Client_Loader::data_seek() */
int
MySQL_Client_Static::query(MYSQL * mysql, const char *stmt_str)
{
	return ::mysql_query(mysql, stmt_str);
}
/* }}} */


/* {{{ MySQL_Client_Loader::real_connect() */
MYSQL *
MySQL_Client_Static::real_connect(MYSQL * mysql,
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


/* {{{ MySQL_Client_Loader::real_query() */
int
MySQL_Client_Static::real_query(MYSQL *mysql,const char *stmt_str, unsigned long len)
{
	return ::mysql_real_query(mysql, stmt_str, len);
}
/* }}} */


/* {{{ MySQL_Client_Loader::rollback() */
my_bool
MySQL_Client_Static::rollback(MYSQL * mysql)
{
	return ::mysql_rollback(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::sqlstate() */
const char *
MySQL_Client_Static::sqlstate(MYSQL * mysql)
{
	return ::mysql_sqlstate(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::ssl_set() */
my_bool
MySQL_Client_Static::ssl_set(MYSQL* mysql,
							const char * key,
							const char * cert,
							const char * ca,
							const char * capath,
							const char * cipher)
{
	return ::mysql_ssl_set(mysql, key, cert, ca, capath, cipher);
}
/* }}} */


/* {{{ MySQL_Client_Loader::store_result() */
MYSQL_RES *
MySQL_Client_Static::store_result(MYSQL * mysql)
{
	return ::mysql_store_result(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::use_result() */
MYSQL_RES *
MySQL_Client_Static::use_result(MYSQL * mysql)
{
	return ::mysql_use_result(mysql);
}
/* }}} */


/* Prepared Statement mysql_stmt_* functions */
/* {{{ MySQL_Client_Loader::stmt_affected_rows() */
my_ulonglong
MySQL_Client_Static::stmt_affected_rows(MYSQL_STMT *stmt)
{
	return ::mysql_stmt_affected_rows(stmt);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_attr_set() */
my_bool
MySQL_Client_Static::stmt_attr_set(MYSQL_STMT * stmt, enum enum_stmt_attr_type option, const void * arg)
{
	return ::mysql_stmt_attr_set(stmt, option, arg);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_bind_param() */
my_bool
MySQL_Client_Static::stmt_bind_param(MYSQL_STMT * stmt, MYSQL_BIND * bind)
{
	return ::mysql_stmt_bind_param(stmt, bind);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_bind_result() */
my_bool
MySQL_Client_Static::stmt_bind_result(MYSQL_STMT * stmt, MYSQL_BIND * bind)
{
	return ::mysql_stmt_bind_result(stmt, bind);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_close() */
my_bool
MySQL_Client_Static::stmt_close(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_close(stmt);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_data_seek() */
void
MySQL_Client_Static::stmt_data_seek(MYSQL_STMT * stmt, my_ulonglong row_nr)
{
	return ::mysql_stmt_data_seek(stmt, row_nr);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_errno() */
unsigned int
MySQL_Client_Static::stmt_errno(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_errno(stmt);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_error() */
const char *
MySQL_Client_Static::stmt_error(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_error(stmt);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_execute() */
int
MySQL_Client_Static::stmt_execute(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_execute(stmt);
}
/* }}} */


/* {{{ MySQL_Client_Loader::data_seek() */
int
MySQL_Client_Static::stmt_fetch(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_fetch(stmt);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_field_count() */
unsigned int
MySQL_Client_Static::stmt_field_count(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_field_count(stmt);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_init() */
MYSQL_STMT *
MySQL_Client_Static::stmt_init(MYSQL * mysql)
{
	return ::mysql_stmt_init(mysql);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_num_rows() */
my_ulonglong
MySQL_Client_Static::stmt_num_rows(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_num_rows(stmt);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_param_count() */
unsigned long
MySQL_Client_Static::stmt_param_count(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_param_count(stmt);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_prepare() */
int
MySQL_Client_Static::stmt_prepare(MYSQL_STMT * stmt, const char * stmt_str, unsigned long len)
{
	return ::mysql_stmt_prepare(stmt, stmt_str, len);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_result_metadata() */
MYSQL_RES *
MySQL_Client_Static::stmt_result_metadata(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_result_metadata(stmt);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_send_long_data() */
my_bool
MySQL_Client_Static::stmt_send_long_data(MYSQL_STMT * stmt,
										unsigned int  par_number,
										const char *  data,
										unsigned long len)
{
	return ::mysql_stmt_send_long_data(stmt, par_number, data, len);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_sqlstate() */
const char *
MySQL_Client_Static::stmt_sqlstate(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_sqlstate(stmt);
}
/* }}} */


/* {{{ MySQL_Client_Loader::stmt_store_result() */
int
MySQL_Client_Static::stmt_store_result(MYSQL_STMT * stmt)
{
	return ::mysql_stmt_store_result(stmt);
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
