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

/**
* Basic example of creating a stand alone program linked against Connector/C++
*
* This example is not integrated into the Connector/C++ build environment.
* You must run "make install" prior to following the build instructions
* given here.
*
* To compile the standalone example on Linux try something like:
*
* /usr/bin/c++
*   -o standalone
*   -I/usr/local/include/cppconn/
*   -Wl,-Bdynamic -lmysqlcppconn
*    examples/standalone_example.cpp
*
* To run the example on Linux try something similar to:
*
*  LD_LIBRARY_PATH=/usr/local/lib/ ./standalone
*
* or:
*
*  LD_LIBRARY_PATH=/usr/local/lib/ ./standalone host user password database
*
*/


/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>


/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"
#include "mysql_driver.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#define EXAMPLE_HOST "localhost"
#define EXAMPLE_USER "root"
#define EXAMPLE_PASS ""
#define EXAMPLE_DB "test"

using namespace std;

/**
* Usage example for Driver, Connection, (simple) Statement, ResultSet
*/
int main(int argc, const char **argv)
{
	string url(argc >= 2 ? argv[1] : EXAMPLE_HOST);
	const string user(argc >= 3 ? argv[2] : EXAMPLE_USER);
	const string pass(argc >= 4 ? argv[3] : EXAMPLE_PASS);
	const string database(argc >= 5 ? argv[4] : EXAMPLE_DB);

	cout << endl;
	cout << "Connector/C++ standalone program example..." << endl;
	cout << endl;

	try {
		sql::Driver * driver = sql::mysql::get_driver_instance();
		/* Using the Driver to create a connection */
		std::auto_ptr< sql::Connection > con(driver->connect(url, user, pass));
		con->setSchema(database);

		std::auto_ptr< sql::Statement > stmt(con->createStatement());
		std::auto_ptr< sql::ResultSet > res(stmt->executeQuery("SELECT 'Welcome to Connector/C++' AS _message"));
		cout << "\t... running 'SELECT 'Welcome to Connector/C++' AS _message'" << endl;
		while (res->next()) {
			cout << "\t... MySQL replies: " << res->getString("_message") << endl;
			cout << "\t... say it again, MySQL" << endl;
			cout << "\t....MySQL replies: " << res->getString(1) << endl;
		}

	} catch (sql::SQLException &e) {
		/*
		The MySQL Connector/C++ throws three different exceptions:

		- sql::MethodNotImplementedException (derived from sql::SQLException)
		- sql::InvalidArgumentException (derived from sql::SQLException)
		- sql::SQLException (derived from std::runtime_error)
		*/
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		/* Use what() (derived from std::runtime_error) to fetch the error message */
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;

		return EXIT_FAILURE;
	}

	cout << endl;
	cout << "... find more at http://www.mysql.com" << endl;
	cout << endl;
	return EXIT_SUCCESS;
}
