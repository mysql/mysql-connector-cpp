/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include "mysql_connection.h"

#include <cppconn/exception.h>

// Looks like this one should go after private_iface
#include "mysql_driver.h"


extern "C"
{
CPPCONN_PUBLIC_FUNC void * sql_mysql_get_driver_instance()
{
	return sql::mysql::get_driver_instance();
}
} /* extern "C" */

namespace sql
{
namespace mysql
{

static bool module_already_loaded = 0;


CPPCONN_PUBLIC_FUNC sql::mysql::MySQL_Driver * get_driver_instance()
{
	return sql::mysql::MySQL_Driver::Instance();
}

static sql::mysql::MySQL_Driver d;

MySQL_Driver * MySQL_Driver::Instance()
{
	return &d;
}


MySQL_Driver::MySQL_Driver()
{
	if (!module_already_loaded) {
		//mysql_library_init(0, NULL, NULL);
		module_already_loaded = true;
	} else {
		throw sql::InvalidArgumentException("You should not call directly the constructor");
	}
}



MySQL_Driver::~MySQL_Driver()
{
}


sql::Connection * MySQL_Driver::connect(const sql::SQLString& hostName,
										const sql::SQLString& userName,
										const sql::SQLString& password)
{
	return new MySQL_Connection(hostName, userName, password);
}


sql::Connection * MySQL_Driver::connect(sql::ConnectOptionsMap & properties)
{
	return new MySQL_Connection(properties);
}


int MySQL_Driver::getMajorVersion()
{
	return 1;
}

int MySQL_Driver::getMinorVersion()
{
	return 0;
}

int MySQL_Driver::getPatchVersion()
{
	return 6;
}

const sql::SQLString & MySQL_Driver::getName()
{
	static const sql::SQLString name("MySQL Connector C++ (libmysql)");
	return name;
}

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
