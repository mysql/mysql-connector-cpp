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
*  LD_LIBRARY_PATH=/usr/local/lib/ ./standalone localhost root root
*
*/


/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>


#include "mysql_connection.h"
#include "mysql_exception.h"

#include <cppconn/driver.h>
#include <cppconn/metadata.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/statement.h>
#include <cppconn/warning.h>

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
		sql::Driver * driver = get_driver_instance();
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
