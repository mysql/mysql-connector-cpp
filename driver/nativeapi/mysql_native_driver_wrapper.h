/*
   Copyright (c) 2009, 2010, Oracle and/or its affiliates. All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_NATIVE_DRIVER_WRAPPER_H_
#define _MYSQL_NATIVE_DRIVER_WRAPPER_H_

#include <boost/shared_ptr.hpp>

#include "native_driver_wrapper.h"

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

class IMySQLCAPI;

class MySQL_NativeDriverWrapper : public NativeDriverWrapper
{
	boost::shared_ptr<IMySQLCAPI> api;

	MySQL_NativeDriverWrapper(){}

public:
	MySQL_NativeDriverWrapper(const ::sql::SQLString & clientFileName);
	~MySQL_NativeDriverWrapper();

	NativeConnectionWrapper & conn_init();
		;
	void thread_end();

	void thread_init();
};

} /* NativeAPI */
} /* mysql */
} /* sql */

#endif /* _MYSQL_NATIVE_DRIVER_WRAPPER_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
