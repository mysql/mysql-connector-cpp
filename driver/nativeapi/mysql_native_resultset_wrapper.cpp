/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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



#include "mysql_native_resultset_wrapper.h"
#include "mysql_client_api.h"


namespace sql
{
namespace mysql
{

class MySQL_DebugLogger;
class MySQL_ConnectionMetaData;

namespace NativeAPI
{

/* {{{ MySQL_NativeResultsetWrapper::MySQL_NativeResultsetWrapper */
MySQL_NativeResultsetWrapper::MySQL_NativeResultsetWrapper(::MYSQL_RES * res, boost::shared_ptr< NativeAPI::IMySQLCAPI > & _capi
                      /*, boost::shared_ptr< MySQL_DebugLogger > & l*/)
  : /*logger(l),*/ capi(_capi), rs(res)
{
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::~MySQL_NativeResultsetWrapper */
MySQL_NativeResultsetWrapper::~MySQL_NativeResultsetWrapper()
{
  capi->free_result(rs);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::data_seek */
void
MySQL_NativeResultsetWrapper::data_seek(uint64_t offset)
{
  capi->data_seek(rs, offset);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::fetch_field */
::MYSQL_FIELD *
MySQL_NativeResultsetWrapper::fetch_field()
{
  return capi->fetch_field(rs);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::fetch_field_direct */
::MYSQL_FIELD *
MySQL_NativeResultsetWrapper::fetch_field_direct(unsigned int field_nr)
{
  return capi->fetch_field_direct(rs, field_nr);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::fetch_lengths */
unsigned long *
MySQL_NativeResultsetWrapper::fetch_lengths()
{
  return capi->fetch_lengths(rs);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::fetch_row */
char**
MySQL_NativeResultsetWrapper::fetch_row()
{
  return capi->fetch_row(rs);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::num_fields */
unsigned int
MySQL_NativeResultsetWrapper::num_fields()
{
  return capi->num_fields(rs);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::num_rows */
uint64_t
MySQL_NativeResultsetWrapper::num_rows()
{
  return capi->num_rows(rs);
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
