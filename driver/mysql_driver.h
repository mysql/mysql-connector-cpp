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

#ifndef _MYSQL_DRIVER_H_
#define _MYSQL_DRIVER_H_

#include <cppconn/driver.h>


namespace sql
{
namespace mysql
{
class db_mgmt_Connection;
class Connection;


class CPPDBC_PUBLIC_FUNC MySQL_Driver : public sql::Driver
{
protected:
	virtual ~MySQL_Driver();
	MySQL_Driver();

public:

	static MySQL_Driver* Instance();

	virtual sql::Connection *connect(const std::string& hostName, 
									const std::string& userName, 
									const std::string& password);


	virtual int getMajorVersion() { return 5; }
	virtual int getMinorVersion() { return 1; }

	virtual std::string getName() { return std::string("Connector C++ (libmysql)"); }

private:
	/* Prevent use of these */
	MySQL_Driver(const MySQL_Driver &);
	void operator=(MySQL_Driver &);
};

CPPDBC_PUBLIC_FUNC MySQL_Driver *get_mysql_driver_instance();


}; /* namespace mysql */
}; /* namespace sql */

#endif // _MYSQL_DRIVER_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
