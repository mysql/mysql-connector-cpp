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



#include "mysql_client_api.h"
#include "mysql_native_driver_wrapper.h"
#include "mysql_native_connection_wrapper.h"

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

/* {{{ createStMysqlWrapper() */
NativeDriverWrapper *
createNativeDriverWrapper(const SQLString & clientFileName)
{
  return new MySQL_NativeDriverWrapper(clientFileName);
}
/* }}} */


/* {{{ MySQL_NativeDriverWrapper::MySQL_NativeDriverWrapper() */
MySQL_NativeDriverWrapper::MySQL_NativeDriverWrapper(const ::sql::SQLString & clientFileName)
  : api(::sql::mysql::NativeAPI::getCApiHandle(clientFileName))
{
}
/* }}} */


/* {{{ MySQL_NativeDriverWrapper::~MySQL_NativeDriverWrapper() */
MySQL_NativeDriverWrapper::~MySQL_NativeDriverWrapper()
{
}
/* }}} */


NativeConnectionWrapper & MySQL_NativeDriverWrapper::conn_init()
{
  return *(new MySQL_NativeConnectionWrapper(api));
}


/* {{{ MySQL_NativeDriverWrapper::thread_end() */
void
MySQL_NativeDriverWrapper::thread_end()
{
  return api->thread_end();
}
/* }}} */


/* {{{ MySQL_NativeDriverWrapper::thread_init() */
void
MySQL_NativeDriverWrapper::thread_init()
{
  return api->thread_init();
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
