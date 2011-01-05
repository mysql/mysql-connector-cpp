/*
Copyright (c) 2008, 2011, Oracle and/or its affiliates. All rights reserved.

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




#include <driver/mysql_connection.h>
#include <driver/mysql_driver.h>
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
			mysql_driver = get_driver_instance();
		}
		if (loops % 2) {
			return mysql_driver->connect(host, user, pass);
		} else {
			sql::ConnectOptionsMap connection_properties;
			connection_properties["hostName"] = host;
			connection_properties["userName"] = user;
			connection_properties["password"] = pass;
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
#define TEST_COMMON_TAP_NAME "driver_test"

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

