/*
Copyright (c) 2009, 2011, Oracle and/or its affiliates. All rights reserved.

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
