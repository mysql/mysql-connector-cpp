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


#include "driver/mysql_public_iface.h"
#include <string>

int loops = 2;

/* {{{	*/
static sql::Connection *
get_connection(const std::string& host, const std::string& user, const std::string& pass)
{
	static sql::Driver * driver = get_driver_instance();
	if (loops % 2) {
		return driver->connect(host, /*port,*/ user, pass);
	} else {
		std::map<std::string, sql::ConnectPropertyVal> connection_properties;
		{
			sql::ConnectPropertyVal tmp;
			tmp.str.val = host.c_str();
			tmp.str.len = host.length();
			connection_properties[std::string("hostName")] = tmp;
		}
		{
			sql::ConnectPropertyVal tmp;
			tmp.str.val = user.c_str();
			tmp.str.len = user.length();
			connection_properties[std::string("userName")] = tmp;
		}
		{
			sql::ConnectPropertyVal tmp;
			tmp.str.val = pass.c_str();
			tmp.str.len = pass.length();
			connection_properties[std::string("password")] = tmp;
		}
		return new sql::mysql::MySQL_Connection(connection_properties);
	}
}
/* }}} */

#include "test_common.cpp"

static void driver_test_new_driver_exception()
{
	try {
//		new sql::mysql::MySQL_Driver();
//		ensure("Exception not thrown", false);
	} catch (sql::InvalidArgumentException) { }
}


int main(int argc, const char **argv)
{
	driver_test_new_driver_exception();

	return run_tests(argc, argv);
}
