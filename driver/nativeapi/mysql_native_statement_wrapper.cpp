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



#include <cppconn/sqlstring.h>

#include "mysql_client_api.h"
#include "mysql_native_statement_wrapper.h"
#include "mysql_native_resultset_wrapper.h"
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
/* {{{ MySQL_NativeStatementWrapper::MySQL_NativeStatementWrapper() */
MySQL_NativeStatementWrapper::MySQL_NativeStatementWrapper(::MYSQL_STMT * _stmt, boost::shared_ptr<IMySQLCAPI> _api, NativeConnectionWrapper * connProxy)
  : api (_api), stmt(_stmt), conn(connProxy)
{
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::~MySQL_NativeStatementWrapper() */
MySQL_NativeStatementWrapper::~MySQL_NativeStatementWrapper()
{
    api->stmt_close(stmt);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::affected_rows() */
uint64_t
MySQL_NativeStatementWrapper::affected_rows()
{
  return api->stmt_affected_rows(stmt);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::attr_set */
bool
MySQL_NativeStatementWrapper::attr_set(MySQL_Statement_Options option, const void *arg)
{
  return (api->stmt_attr_set(stmt, static_cast< enum_stmt_attr_type >(option), arg) != '\0');
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::bind_param() */
bool
MySQL_NativeStatementWrapper::bind_param(::MYSQL_BIND * bind )
{
  return (api->stmt_bind_param(stmt, bind) != '\0');
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::bind_result() */
bool
MySQL_NativeStatementWrapper::bind_result(::MYSQL_BIND * bind)
{
  return (api->stmt_bind_result(stmt, bind) != '\0');
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::data_seek() */
void
MySQL_NativeStatementWrapper::data_seek(uint64_t offset)
{
  api->stmt_data_seek(stmt, offset);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::errNo() */
unsigned int
MySQL_NativeStatementWrapper::errNo()
{
  return api->stmt_errno(stmt);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::error() */
::sql::SQLString
MySQL_NativeStatementWrapper::error()
{
  return api->stmt_error(stmt);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::execute() */
int
MySQL_NativeStatementWrapper::execute()
{
  return api->stmt_execute(stmt);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::fetch() */
int
MySQL_NativeStatementWrapper::fetch()
{
  return api->stmt_fetch(stmt);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::field_count() */
unsigned int
MySQL_NativeStatementWrapper::field_count()
{
  return api->stmt_field_count(stmt);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::more_results() */
bool
MySQL_NativeStatementWrapper::more_results()
{
  return conn->more_results();
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::next_result() */
int
MySQL_NativeStatementWrapper::next_result()
{
  return conn->next_result();
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::num_rows() */
uint64_t
MySQL_NativeStatementWrapper::num_rows()
{
  return api->stmt_num_rows(stmt);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::param_count() */
unsigned long
MySQL_NativeStatementWrapper::param_count()
{
  return api->stmt_param_count(stmt);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::prepare() */
int
MySQL_NativeStatementWrapper::prepare(const ::sql::SQLString & stmt_str)
{
  return api->stmt_prepare(stmt, stmt_str.c_str(), static_cast<unsigned long>(stmt_str.length()));
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::result_metadata() */
NativeResultsetWrapper *
MySQL_NativeStatementWrapper::result_metadata()
{
  ::MYSQL_RES * raw = api->stmt_result_metadata(stmt);

  if (raw == NULL) {
    return NULL;
  }

  return new MySQL_NativeResultsetWrapper(raw, api);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::send_long_data() */
bool
MySQL_NativeStatementWrapper::send_long_data(unsigned int par_number, const char * data, unsigned long len)
{
  return api->stmt_send_long_data(stmt, par_number, data, len) != '\0';
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::sqlstate() */
::sql::SQLString
MySQL_NativeStatementWrapper::sqlstate()
{
  return api->stmt_sqlstate(stmt);
}


/* {{{ MySQL_NativeStatementWrapper::store_result() */
int
MySQL_NativeStatementWrapper::store_result()
{
  return api->stmt_store_result(stmt);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::next_result() */
int
MySQL_NativeStatementWrapper::stmt_next_result()
{
  return api->stmt_next_result(stmt);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::stmt_free_result() */
bool
MySQL_NativeStatementWrapper::stmt_free_result()
{
  return api->stmt_free_result(stmt);
}
/* }}} */


/* {{{ MySQL_NativeStatementWrapper::warning_count() */
unsigned int
MySQL_NativeStatementWrapper::warning_count()
{
  return conn->warning_count();
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
