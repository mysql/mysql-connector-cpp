/* Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

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


#include <driver/mysql_driver.h>
#include <driver/mysql_connection.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/metadata.h>

#include <string>
#include <iostream>

using namespace std;

int loops = 2;

static sql::Driver * mysql_driver = NULL;

/* {{{	*/
static sql::Connection *
get_connection(const std::string & host, const std::string & user, const std::string & pass)
{
	try {
		/* There will be concurrency problem if we had threads, but don't have, then it's ok */
		if (!mysql_driver) {
			mysql_driver = sql::mysql::MySQL_Driver::Instance();
		}
		if (loops % 2) {
			return mysql_driver->connect(host, user, pass);
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
			return mysql_driver->connect(connection_properties);
		}
	} catch (sql::SQLException &e) {
		cout << "sql::SQLException caught during connect" << endl;
		cout << e.what() << endl;
		throw;
	}
}
/* }}} */

#define DRIVER_TEST 1

#include "test_common.cpp"

static void driver_test_new_driver_exception()
{
	try {
		new sql::mysql::MySQL_Driver();
		ensure("Exception not thrown", false);
	} catch (sql::InvalidArgumentException) { }
}

/* {{{	*/
int main(int argc, const char **argv)
{
	driver_test_new_driver_exception();

	return run_tests(argc, argv);
}
/* }}} */

