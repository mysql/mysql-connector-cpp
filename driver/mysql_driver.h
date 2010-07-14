/*
   Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_DRIVER_H_
#define _MYSQL_DRIVER_H_

#include <boost/scoped_ptr.hpp>

#include <cppconn/driver.h>

extern "C"
{
CPPCONN_PUBLIC_FUNC void  * sql_mysql_get_driver_instance();
}

namespace sql
{
namespace mysql
{
namespace NativeAPI
{
	class NativeDriverWrapper;
}

//class sql::mysql::NativeAPI::NativeDriverWrapper;

class CPPCONN_PUBLIC_FUNC MySQL_Driver : public sql::Driver
{
	boost::scoped_ptr< ::sql::mysql::NativeAPI::NativeDriverWrapper > proxy;

public:
	MySQL_Driver();
	MySQL_Driver(const ::sql::SQLString & clientLib);

	virtual ~MySQL_Driver();

	sql::Connection * connect(const sql::SQLString& hostName, const sql::SQLString& userName, const sql::SQLString& password);

	sql::Connection * connect(sql::ConnectOptionsMap & options);

	int getMajorVersion();

	int getMinorVersion();

	int getPatchVersion();

	const sql::SQLString & getName();

	void threadInit();

	void threadEnd();

private:
	/* Prevent use of these */
	MySQL_Driver(const MySQL_Driver &);
	void operator=(MySQL_Driver &);
};

/** We do not hide the function if MYSQLCLIENT_STATIC_BINDING(or anything else) not defined
    because the counterpart C function is declared in the cppconn and is always visible.
    If dynamic loading is not enabled then its result is just like of get_driver_instance()
*/
CPPCONN_PUBLIC_FUNC MySQL_Driver * get_driver_instance_by_name(const char * const clientlib);

CPPCONN_PUBLIC_FUNC MySQL_Driver * get_driver_instance();
static inline MySQL_Driver * get_mysql_driver_instance() { return get_driver_instance(); }


} /* namespace mysql */
} /* namespace sql */

#endif // _MYSQL_DRIVER_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
