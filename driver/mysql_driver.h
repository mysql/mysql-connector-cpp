/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_DRIVER_H_
#define _MYSQL_DRIVER_H_

#include <cppconn/driver.h>

extern "C"
{
CPPCONN_PUBLIC_FUNC void  * sql_mysql_get_driver_instance();
}

namespace sql
{
namespace mysql
{
namespace util
{

class LibmysqlDynamicProxy;

}

class Connection;
class ConnectProperty;

class CPPCONN_PUBLIC_FUNC MySQL_Driver : public sql::Driver
{

public:
	MySQL_Driver(); /* DON'T CALL THIS, USE Instance() */
	virtual ~MySQL_Driver();/* DON'T CALL THIS, MEMORY WILL BE AUTOMAGICALLY CLEANED */

	static MySQL_Driver * Instance();

	sql::Connection * connect(const sql::SQLString& hostName, const sql::SQLString& userName, const sql::SQLString& password);

	sql::Connection * connect(sql::ConnectOptionsMap & options);

	int getMajorVersion();

	int getMinorVersion();

	int getPatchVersion();

	const sql::SQLString & getName();

private:
	/* Prevent use of these */
	MySQL_Driver(const MySQL_Driver &);
	void operator=(MySQL_Driver &);
};

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
