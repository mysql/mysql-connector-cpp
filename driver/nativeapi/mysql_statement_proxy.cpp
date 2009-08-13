/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include <sqlstring.h>

#include "mysql_client_api.h"
#include "mysql_statement_proxy.h"
#include "mysql_resultset_proxy.h"
#include "native_connection_wrapper.h"

#include "../mysql_util.h"

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

/*
 * 
 * 
 * 
 */
/* {{{ MySQL_Statement_Proxy::MySQL_Statement_Proxy() */
MySQL_Statement_Proxy::MySQL_Statement_Proxy(::st_mysql_stmt * _stmt, boost::shared_ptr<IMySQLCAPI> _api, NativeConnectionWrapper * connProxy)
	: api (_api), stmt(_stmt), conn(connProxy)
{
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::~MySQL_Statement_Proxy() */
MySQL_Statement_Proxy::~MySQL_Statement_Proxy()
{
	api->stmt_close(stmt);
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::affected_rows() */
uint64_t
MySQL_Statement_Proxy::affected_rows()
{
	return api->stmt_affected_rows(stmt);
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::attr_set */
bool
MySQL_Statement_Proxy::attr_set(MySQL_Statement_Options option, const void *arg)
{
	return (api->stmt_attr_set(stmt, static_cast< enum_stmt_attr_type >(option), arg) != '\0');
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::bind_param() */
bool
MySQL_Statement_Proxy::bind_param(::st_mysql_bind * bind )
{
	return (api->stmt_bind_param(stmt, bind) != '\0');
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::bind_result() */
bool
MySQL_Statement_Proxy::bind_result(::st_mysql_bind * bind)
{
	return (api->stmt_bind_result(stmt, bind) != '\0');
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::data_seek() */
void
MySQL_Statement_Proxy::data_seek(uint64_t offset)
{
	api->stmt_data_seek(stmt, offset);
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::errNo() */
unsigned int
MySQL_Statement_Proxy::errNo()
{
	return api->stmt_errno(stmt);
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::error() */
::sql::SQLString
MySQL_Statement_Proxy::error()
{
	return api->stmt_error(stmt);
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::execute() */
int
MySQL_Statement_Proxy::execute()
{
	return api->stmt_execute(stmt);
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::fetch() */
int
MySQL_Statement_Proxy::fetch()
{
	return api->stmt_fetch(stmt);
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::field_count() */
unsigned int
MySQL_Statement_Proxy::field_count()
{
	return api->stmt_field_count(stmt);
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::more_results() */
bool
MySQL_Statement_Proxy::more_results()
{
	return conn->more_results();
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::next_result() */
int
MySQL_Statement_Proxy::next_result()
{
	return conn->next_result();
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::num_rows() */
uint64_t
MySQL_Statement_Proxy::num_rows()
{
	return api->stmt_num_rows(stmt);
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::param_count() */
unsigned long
MySQL_Statement_Proxy::param_count()
{
	return api->stmt_param_count(stmt);
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::prepare() */
int
MySQL_Statement_Proxy::prepare(const ::sql::SQLString & stmt_str)
{
	return api->stmt_prepare(stmt, stmt_str.c_str(), stmt_str.length());
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::result_metadata() */
Resultset_Proxy *
MySQL_Statement_Proxy::result_metadata()
{
	::st_mysql_res * raw= api->stmt_result_metadata(stmt);

	if (raw == NULL) {
		return NULL;
	}

	return new MySQL_Resultset_Proxy(raw, api);
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::send_long_data() */
bool
MySQL_Statement_Proxy::send_long_data(unsigned int par_number, const char * data, unsigned long len)
{
	return api->stmt_send_long_data(stmt, par_number, data, len) != '\0';
}
/* }}} */


/* {{{ MySQL_Statement_Proxy::sqlstate() */
::sql::SQLString
MySQL_Statement_Proxy::sqlstate()
{
	return api->stmt_sqlstate(stmt);
}


/* {{{ MySQL_Statement_Proxy::store_result() */
int
MySQL_Statement_Proxy::store_result()
{
	return api->stmt_store_result(stmt);
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
