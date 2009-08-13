/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/


#include "mysql_client_loader.h"

#include <cppconn/exception.h>

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

#ifdef _WIN32
static const char * const baseName = "libmysql.dll";
#elif defined(__APPLE__)
static const char * const baseName = "libmysqlclient_r.dylib";
#else
static const char * const baseName = "libmysqlclient_r.so";
#endif

template<typename FunctionType>
FunctionType symbol_safe_cast( ::sql::mysql::util::SymbolHandle raw )
{
    return *reinterpret_cast<FunctionType*>(&raw);
}

/* {{{ Libmysql_Dynamic_Proxy::Libmysql_Dynamic_Proxy() */
Libmysql_Dynamic_Proxy::Libmysql_Dynamic_Proxy()
	: LibraryLoader(baseName)
{
	init_loader();
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::Libmysql_Dynamic_Proxy() */
Libmysql_Dynamic_Proxy::Libmysql_Dynamic_Proxy(const SQLString & path2libFile)
	: LibraryLoader(path2libFile.length() > 0 ? path2libFile : baseName)
{
	init_loader();
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::Libmysql_Dynamic_Proxy() */
Libmysql_Dynamic_Proxy::Libmysql_Dynamic_Proxy(const SQLString & dir2look, const SQLString & libFileName)
	: LibraryLoader(dir2look, libFileName.length() > 0 ? libFileName : baseName)
{
	init_loader();
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::~Libmysql_Dynamic_Proxy() */
Libmysql_Dynamic_Proxy::~Libmysql_Dynamic_Proxy()
{
    ptr2mysql_library_end endProc= symbol_safe_cast<ptr2mysql_library_end>(GetProcAddr("mysql_server_end"));
	
	if (endProc != NULL) {
		(*endProc)();
	}
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::init_loader() */
void
Libmysql_Dynamic_Proxy::init_loader()
{
	ptr2mysql_library_init initProc = symbol_safe_cast<ptr2mysql_library_init>(GetProcAddr("mysql_server_init"));
	
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
/* {{{ Libmysql_Dynamic_Proxy::affected_rows() */
my_ulonglong
Libmysql_Dynamic_Proxy::affected_rows(MYSQL * mysql)
{
	ptr2mysql_affected_rows ptr2_affected_rows = symbol_safe_cast<ptr2mysql_affected_rows>(GetProcAddr("mysql_affected_rows"));

	return (*ptr2_affected_rows)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::autocommit() */
my_bool
Libmysql_Dynamic_Proxy::autocommit(MYSQL * mysql, my_bool mode)
{
	ptr2mysql_autocommit ptr2_autocommit = symbol_safe_cast<ptr2mysql_autocommit>(GetProcAddr("mysql_autocommit"));

	return (*ptr2_autocommit)(mysql, mode);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::close() */
void
Libmysql_Dynamic_Proxy::close(MYSQL * mysql)
{
    ptr2mysql_close ptr2_close = symbol_safe_cast<ptr2mysql_close>(GetProcAddr("mysql_close"));

	return (*ptr2_close)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::commit() */
my_bool
Libmysql_Dynamic_Proxy::commit(MYSQL * mysql)
{
	ptr2mysql_commit ptr2_commit = symbol_safe_cast<ptr2mysql_commit>(GetProcAddr("mysql_commit"));

	return (*ptr2_commit)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::data_seek() */
void
Libmysql_Dynamic_Proxy::data_seek(MYSQL_RES * result, my_ulonglong offset)
{
	ptr2mysql_data_seek ptr2_data_seek = symbol_safe_cast<ptr2mysql_data_seek>(GetProcAddr("mysql_data_seek"));

	return (*ptr2_data_seek)(result, offset);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::debug() */
void
Libmysql_Dynamic_Proxy::debug(const char * debug)
{
	ptr2mysql_debug ptr2_debug = symbol_safe_cast<ptr2mysql_debug>(GetProcAddr("mysql_debug"));

	return (*ptr2_debug)(debug);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::errno() */
unsigned int
Libmysql_Dynamic_Proxy::mysql_errno(MYSQL * mysql)
{
	ptr2mysql_errno ptr2_errno = symbol_safe_cast<ptr2mysql_errno>(GetProcAddr("mysql_errno"));

	return (*ptr2_errno)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::error() */
const char *
Libmysql_Dynamic_Proxy::error(MYSQL * mysql)
{
	ptr2mysql_error ptr2_error = symbol_safe_cast<ptr2mysql_error>(GetProcAddr("mysql_error"));

	return (*ptr2_error)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::fetch_field() */
MYSQL_FIELD *
Libmysql_Dynamic_Proxy::fetch_field(MYSQL_RES * result)
{
	ptr2mysql_fetch_field ptr2_fetch_field = symbol_safe_cast<ptr2mysql_fetch_field>(GetProcAddr("mysql_fetch_field"));

	return (*ptr2_fetch_field)(result);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::fetch_field_direct() */
MYSQL_FIELD *
Libmysql_Dynamic_Proxy::fetch_field_direct(MYSQL_RES * result, unsigned int fieldnr)
{
	ptr2mysql_fetch_field_direct ptr2fetchFieldDirect=
        symbol_safe_cast<ptr2mysql_fetch_field_direct>(GetProcAddr("mysql_fetch_field_direct"));

	return (*ptr2fetchFieldDirect)(result, fieldnr);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::fetch_lengths() */
unsigned long *
Libmysql_Dynamic_Proxy::fetch_lengths(MYSQL_RES * result)
{
	ptr2mysql_fetch_lengths ptr2_fetch_lengths = symbol_safe_cast<ptr2mysql_fetch_lengths>(GetProcAddr("mysql_fetch_lengths"));

	return (*ptr2_fetch_lengths)(result);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::fetch_row() */
MYSQL_ROW
Libmysql_Dynamic_Proxy::fetch_row(MYSQL_RES * result)
{
	ptr2mysql_fetch_row ptr2_fetch_row = symbol_safe_cast<ptr2mysql_fetch_row>(GetProcAddr("mysql_fetch_row"));

	return (*ptr2_fetch_row)(result);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::field_count() */
unsigned int
Libmysql_Dynamic_Proxy::field_count(MYSQL * mysql)
{
	ptr2mysql_field_count ptr2_field_count = symbol_safe_cast<ptr2mysql_field_count>(GetProcAddr("mysql_field_count"));

	return (*ptr2_field_count)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::free_result() */
void
Libmysql_Dynamic_Proxy::free_result(MYSQL_RES * result)
{
	ptr2mysql_free_result ptr2_free_result = symbol_safe_cast<ptr2mysql_free_result>(GetProcAddr("mysql_free_result"));

	return (*ptr2_free_result)(result);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::get_client_version() */
unsigned long
Libmysql_Dynamic_Proxy::get_client_version()
{
	ptr2mysql_get_client_version ptr2_get_client_version=
		(ptr2mysql_get_client_version)(GetProcAddr("mysql_get_client_version"));

	return (*ptr2_get_client_version)();
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::get_server_info() */
const char *
Libmysql_Dynamic_Proxy::get_server_info(MYSQL * mysql)
{
	ptr2mysql_get_server_info ptr2_get_server_info = symbol_safe_cast<ptr2mysql_get_server_info>(GetProcAddr("mysql_get_server_info"));

	return (*ptr2_get_server_info)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::get_server_version() */
unsigned long
Libmysql_Dynamic_Proxy::get_server_version(MYSQL * mysql)
{
	ptr2mysql_get_server_version ptr2_get_server_version = symbol_safe_cast<ptr2mysql_get_server_version>(GetProcAddr("mysql_get_server_version"));

	return (*ptr2_get_server_version)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::init() */
MYSQL *
Libmysql_Dynamic_Proxy::init(MYSQL * mysql)
{
	ptr2mysql_init ptr2init = symbol_safe_cast<ptr2mysql_init>(GetProcAddr("mysql_init"));

	return (*ptr2init)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::library_init() */
int
Libmysql_Dynamic_Proxy::library_init(int argc,char **argv,char **groups)
{
	ptr2mysql_library_init ptr2_library_init = symbol_safe_cast<ptr2mysql_library_init>(GetProcAddr("mysql_library_init"));

	return (*ptr2_library_init)(argc, argv, groups);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::library_end() */
void
Libmysql_Dynamic_Proxy::library_end()
{
	ptr2mysql_library_end ptr2_library_end = symbol_safe_cast<ptr2mysql_library_end>(GetProcAddr("mysql_library_end"));

	return (*ptr2_library_end)();
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::more_results() */
my_bool
Libmysql_Dynamic_Proxy::more_results(MYSQL * mysql)
{
	ptr2mysql_more_results ptr2_more_results = symbol_safe_cast<ptr2mysql_more_results>(GetProcAddr("mysql_more_results"));

	return (*ptr2_more_results)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::next_result() */
int
Libmysql_Dynamic_Proxy::next_result(MYSQL * mysql)
{
	ptr2mysql_next_result ptr2_next_result = symbol_safe_cast<ptr2mysql_next_result>(GetProcAddr("mysql_next_result"));

	return (*ptr2_next_result)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::num_fields() */
unsigned int
Libmysql_Dynamic_Proxy::num_fields(MYSQL_RES * result)
{
	ptr2mysql_num_fields ptr2_num_fields = symbol_safe_cast<ptr2mysql_num_fields>(GetProcAddr("mysql_num_fields"));

	return (*ptr2_num_fields)(result);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::num_rows() */
my_ulonglong
Libmysql_Dynamic_Proxy::num_rows(MYSQL_RES * result)
{
	ptr2mysql_num_rows ptr2_num_rows = symbol_safe_cast<ptr2mysql_num_rows>(GetProcAddr("mysql_num_rows"));

	return (*ptr2_num_rows)(result);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::options() */
int
Libmysql_Dynamic_Proxy::options(MYSQL * mysql, enum mysql_option option, const void *arg)
{
	ptr2mysql_options ptr2_options = symbol_safe_cast<ptr2mysql_options>(GetProcAddr("mysql_options"));

	return (*ptr2_options)(mysql, option, arg);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::query() */
int
Libmysql_Dynamic_Proxy::query(MYSQL * mysql, const char *stmt_str)
{
	ptr2mysql_query ptr2_query = symbol_safe_cast<ptr2mysql_query>(GetProcAddr("mysql_query"));

	return (*ptr2_query)(mysql, stmt_str);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::real_connect() */
MYSQL *
Libmysql_Dynamic_Proxy::real_connect(MYSQL * mysql,
												const char * host,
												const char * user,
												const char * passwd,
												const char * db,
												unsigned int port,
												const char * unix_socket,
												unsigned long client_flag)
{
	ptr2mysql_real_connect ptr2_real_connect=
		symbol_safe_cast<ptr2mysql_real_connect>(GetProcAddr("mysql_real_connect"));

	return (*ptr2_real_connect)(mysql, host, user, passwd, db, port, unix_socket, client_flag);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::real_query() */
int
Libmysql_Dynamic_Proxy::real_query(MYSQL *mysql,const char *stmt_str, unsigned long len)
{
	ptr2mysql_real_query ptr2_real_query = symbol_safe_cast<ptr2mysql_real_query>(GetProcAddr("mysql_real_query"));

	return (*ptr2_real_query)(mysql, stmt_str, len);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::rollback() */
my_bool
Libmysql_Dynamic_Proxy::rollback(MYSQL * mysql)
{
	ptr2mysql_rollback ptr2_rollback = symbol_safe_cast<ptr2mysql_rollback>(GetProcAddr("mysql_rollback"));

	return (*ptr2_rollback)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::sqlstate() */
const char *
Libmysql_Dynamic_Proxy::sqlstate(MYSQL * mysql)
{
	ptr2mysql_sqlstate ptr2_sqlstate = symbol_safe_cast<ptr2mysql_sqlstate>(GetProcAddr("mysql_sqlstate"));

	return (*ptr2_sqlstate)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::ssl_set() */
my_bool
Libmysql_Dynamic_Proxy::ssl_set(MYSQL * mysql,
											const char * key,
											const char * cert,
											const char * ca,
											const char * capath,
											const char * cipher)
{
	ptr2mysql_ssl_set ptr2_ssl_set = symbol_safe_cast<ptr2mysql_ssl_set>(GetProcAddr("mysql_ssl_set"));

	return (*ptr2_ssl_set)(mysql, key, cert, ca, capath, cipher);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::store_result() */
MYSQL_RES *
Libmysql_Dynamic_Proxy::store_result(MYSQL * mysql)
{
	ptr2mysql_store_result ptr2_store_result = symbol_safe_cast<ptr2mysql_store_result>(GetProcAddr("mysql_store_result"));

	return (*ptr2_store_result)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::use_result() */
MYSQL_RES *
Libmysql_Dynamic_Proxy::use_result(MYSQL * mysql)
{
	ptr2mysql_use_result ptr2_use_result = symbol_safe_cast<ptr2mysql_use_result>(GetProcAddr("mysql_use_result"));

	return (*ptr2_use_result)(mysql);
}
/* }}} */


/* Prepared Statement mysql_stmt_* functions */

/* {{{ Libmysql_Dynamic_Proxy::stmt_affected_rows() */
my_ulonglong
Libmysql_Dynamic_Proxy::stmt_affected_rows(MYSQL_STMT *stmt)
{
	ptr2mysql_stmt_affected_rows ptr2_stmt_affected_rows = symbol_safe_cast<ptr2mysql_stmt_affected_rows>(GetProcAddr("mysql_stmt_affected_rows"));

	return (*ptr2_stmt_affected_rows)(stmt);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_attr_set() */
my_bool
Libmysql_Dynamic_Proxy::stmt_attr_set(MYSQL_STMT * stmt, enum enum_stmt_attr_type option, const void * arg)
{
	ptr2mysql_stmt_attr_set ptr2_stmt_attr_set = symbol_safe_cast<ptr2mysql_stmt_attr_set>(GetProcAddr("mysql_stmt_attr_set"));

	return (*ptr2_stmt_attr_set)(stmt, option, arg);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_bind_param() */
my_bool
Libmysql_Dynamic_Proxy::stmt_bind_param(MYSQL_STMT * stmt, MYSQL_BIND * bind)
{
	ptr2mysql_stmt_bind_param ptr2_stmt_bind_param = symbol_safe_cast<ptr2mysql_stmt_bind_param>(GetProcAddr("mysql_stmt_bind_param"));

	return (*ptr2_stmt_bind_param)(stmt, bind);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_bind_result() */
my_bool
Libmysql_Dynamic_Proxy::stmt_bind_result(MYSQL_STMT * stmt, MYSQL_BIND * bind)
{
	ptr2mysql_stmt_bind_result ptr2_stmt_bind_result = symbol_safe_cast<ptr2mysql_stmt_bind_result>(GetProcAddr("mysql_stmt_bind_result"));

	return (*ptr2_stmt_bind_result)(stmt, bind);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_close() */
my_bool
Libmysql_Dynamic_Proxy::stmt_close(MYSQL_STMT * stmt)
{
	ptr2mysql_stmt_close ptr2_stmt_close = symbol_safe_cast<ptr2mysql_stmt_close>(GetProcAddr("mysql_stmt_close"));

	return (*ptr2_stmt_close)(stmt);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_data_seek() */
void
Libmysql_Dynamic_Proxy::stmt_data_seek(MYSQL_STMT * stmt, my_ulonglong row_nr)
{
	ptr2mysql_stmt_data_seek ptr2_stmt_data_seek = symbol_safe_cast<ptr2mysql_stmt_data_seek>(GetProcAddr("mysql_stmt_data_seek"));

	return (*ptr2_stmt_data_seek)(stmt, row_nr);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_errno() */
unsigned int
Libmysql_Dynamic_Proxy::stmt_errno(MYSQL_STMT * stmt)
{
	ptr2mysql_stmt_errno ptr2_stmt_errno = symbol_safe_cast<ptr2mysql_stmt_errno>(GetProcAddr("mysql_stmt_errno"));

	return (*ptr2_stmt_errno)(stmt);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_error() */
const char *
Libmysql_Dynamic_Proxy::stmt_error(MYSQL_STMT * stmt)
{
	ptr2mysql_stmt_error ptr2_stmt_error = symbol_safe_cast<ptr2mysql_stmt_error>(GetProcAddr("mysql_stmt_error"));

	return (*ptr2_stmt_error)(stmt);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_execute() */
int
Libmysql_Dynamic_Proxy::stmt_execute(MYSQL_STMT * stmt)
{
	ptr2mysql_stmt_execute ptr2_stmt_execute = symbol_safe_cast<ptr2mysql_stmt_execute>(GetProcAddr("mysql_stmt_execute"));

	return (*ptr2_stmt_execute)(stmt);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_fetch() */
int
Libmysql_Dynamic_Proxy::stmt_fetch(MYSQL_STMT * stmt)
{
	ptr2mysql_stmt_fetch ptr2_stmt_fetch = symbol_safe_cast<ptr2mysql_stmt_fetch>(GetProcAddr("mysql_stmt_fetch"));

	return (*ptr2_stmt_fetch)(stmt);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_field_count() */
unsigned int
Libmysql_Dynamic_Proxy::stmt_field_count(MYSQL_STMT * stmt)
{
	ptr2mysql_stmt_field_count ptr2_stmt_field_count = symbol_safe_cast<ptr2mysql_stmt_field_count>(GetProcAddr("mysql_stmt_field_count"));

	return (*ptr2_stmt_field_count)(stmt);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_init() */
MYSQL_STMT *
Libmysql_Dynamic_Proxy::stmt_init(MYSQL * mysql)
{
	ptr2mysql_stmt_init ptr2_stmt_init = symbol_safe_cast<ptr2mysql_stmt_init>(GetProcAddr("mysql_stmt_init"));

	return (*ptr2_stmt_init)(mysql);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_num_rows() */
my_ulonglong
Libmysql_Dynamic_Proxy::stmt_num_rows(MYSQL_STMT * stmt)
{
	ptr2mysql_stmt_num_rows ptr2_stmt_num_rows = symbol_safe_cast<ptr2mysql_stmt_num_rows>(GetProcAddr("mysql_stmt_num_rows"));

	return (*ptr2_stmt_num_rows)(stmt);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_param_count() */
unsigned long
Libmysql_Dynamic_Proxy::stmt_param_count(MYSQL_STMT * stmt)
{
	ptr2mysql_stmt_param_count ptr2_stmt_param_count = symbol_safe_cast<ptr2mysql_stmt_param_count>(GetProcAddr("mysql_stmt_param_count"));

	return (*ptr2_stmt_param_count)(stmt);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_prepare() */
int
Libmysql_Dynamic_Proxy::stmt_prepare(MYSQL_STMT * stmt, const char * stmt_str, unsigned long len)
{
	ptr2mysql_stmt_prepare ptr2_stmt_prepare = symbol_safe_cast<ptr2mysql_stmt_prepare>(GetProcAddr("mysql_stmt_prepare"));

	return (*ptr2_stmt_prepare)(stmt, stmt_str, len);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_result_metadata() */
MYSQL_RES *
Libmysql_Dynamic_Proxy::stmt_result_metadata(MYSQL_STMT * stmt)
{
	ptr2mysql_stmt_result_metadata ptr2_stmt_result_metadata = symbol_safe_cast<ptr2mysql_stmt_result_metadata>(GetProcAddr("mysql_stmt_result_metadata"));

	return (*ptr2_stmt_result_metadata)(stmt);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_send_long_data() */
my_bool
Libmysql_Dynamic_Proxy::stmt_send_long_data(MYSQL_STMT * stmt, unsigned int par_number, const char * data, unsigned long len)
{
	ptr2mysql_stmt_send_long_data ptr2_stmt_send_long_data = symbol_safe_cast<ptr2mysql_stmt_send_long_data>(GetProcAddr("mysql_stmt_send_long_data"));

	return (*ptr2_stmt_send_long_data)(stmt, par_number, data, len);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_sqlstate() */
const char *
Libmysql_Dynamic_Proxy::stmt_sqlstate(MYSQL_STMT * stmt)
{
	ptr2mysql_stmt_sqlstate ptr2_stmt_sqlstate = symbol_safe_cast<ptr2mysql_stmt_sqlstate>(GetProcAddr("mysql_stmt_sqlstate"));

	return (*ptr2_stmt_sqlstate)(stmt);
}
/* }}} */


/* {{{ Libmysql_Dynamic_Proxy::stmt_store_result() */
int
Libmysql_Dynamic_Proxy::stmt_store_result(MYSQL_STMT * stmt)
{
	ptr2mysql_stmt_store_result ptr2_stmt_store_result = symbol_safe_cast<ptr2mysql_stmt_store_result>(GetProcAddr("mysql_stmt_store_result"));

	return (*ptr2_stmt_store_result)(stmt);
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
