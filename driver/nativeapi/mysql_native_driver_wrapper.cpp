/*
   Copyright (c) 2009, 2010, Oracle and/or its affiliates. All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
