/*
  Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

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


#include "driver/mysql_public_iface.h"
#include <string>

int loops = 2;

/* {{{	*/
static sql::Connection *
get_connection(const std::string& host, const std::string& user, const std::string& pass)
{
	static sql::Driver * driver = sql::mysql::get_driver_instance();
	if (loops % 2) {
		return driver->connect(host, /*port,*/ user, pass);
	} else {
		sql::ConnectOptionsMap connection_properties;
		connection_properties["hostName"] = host;
		connection_properties["userName"] = user;
		connection_properties["password"] = pass;
		return driver->connect(connection_properties);
	}
}
/* }}} */

#define TEST_COMMON_TAP_NAME "static_test"
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
