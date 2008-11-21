/* Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   There are special exceptions to the terms and conditions of the GPL 
   as it is applied to this software. View the full text of the 
   exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this 
   software distribution.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "mysql_driver.h"
#include "mysql_connection.h"

#include "mysql_private_iface.h"

/*
extern "C"
{*/

CPPDBC_PUBLIC_FUNC sql::Driver *get_driver_instance()
{
	return sql::mysql::MySQL_Driver::Instance();
}
//} /* extern_c */


namespace sql
{
namespace mysql
{

//static MySQL_Driver driver;


CPPDBC_PUBLIC_FUNC MySQL_Driver *get_mysql_driver_instance()
{
	//return &driver;
	return sql::mysql::MySQL_Driver::Instance();
}

MySQL_Driver * MySQL_Driver::Instance()
{
	static sql::mysql::MySQL_Driver d;

	return &d;
}

MySQL_Driver::MySQL_Driver()
{
	mysql_library_init(0, NULL, NULL);
}


MySQL_Driver::~MySQL_Driver()
{
	mysql_library_end();
}


sql::Connection * MySQL_Driver::connect(const std::string& hostName, 
										const std::string& userName, 
										const std::string& password)
{
	return new MySQL_Connection(hostName, userName, password);
}

}; /* namespace mysql */
}; /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
