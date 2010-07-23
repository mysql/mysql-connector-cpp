/*
   Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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

/* Threading stuff */
#include <pthread.h>

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

void* thread_one_action(void *arg);

std::auto_ptr< sql::Connection > con;
std::auto_ptr< sql::Statement > stmt;
std::auto_ptr< sql::ResultSet > res;
sql::Driver * driver;

string url;
string user;
string pass;
string database;

/**
* Usage example for Driver, Connection, (simple) Statement, ResultSet
*/
int main(int argc, const char **argv)
{
	url = (argc >= 2) ? argv[1] : EXAMPLE_HOST;
	user = (argc >= 3) ? argv[2] : EXAMPLE_USER;
	pass = (argc >= 4) ? argv[3] : EXAMPLE_PASS;
	database = (argc >= 5) ? argv[4] : EXAMPLE_DB;

	pthread_t thread_one;
	int status_one;

	cout << endl;
	cout << "Connector/C++ phthreads program example..." << endl;
	cout << endl;

	try {
		driver = sql::mysql::get_driver_instance();

		/* Using the Driver to create a connection */
		con.reset(driver->connect(url, user, pass));
		con->setSchema(database);

		/* Worker thread */
		cout << "Creating Thread 1..." << endl;
		status_one = pthread_create(&thread_one, NULL, thread_one_action, NULL);
		cout << "Thread 1 status: " << status_one << endl;
		if (status_one != 0)
				throw std::runtime_error("Thread creation has failed");

		/*
		stmt.reset(con->createStatement());
	res.reset(stmt->executeQuery("SELECT 'Welcome to Connector/C++' AS _message"));
	cout << "\t... Thread 1: running 'SELECT 'Welcome to Connector/C++' AS _message'" << endl;
	while (res->next()) {
		cout << "\t... Thread 1: MySQL replies: " << res->getString("_message") << endl;
		cout << "\t... Thread 1: say it again, MySQL" << endl;
		cout << "\t....Thread 1: MySQL replies: " << res->getString(1) << endl;
	}
		*/

	} catch (sql::SQLException &e) {

		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;

		return EXIT_FAILURE;

	} catch (std::runtime_error &e) {

		cout << "# ERR: runtime_error in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what() << endl;
		cout << "not ok 1 - examples/connect.php" << endl;

		return EXIT_FAILURE;
	}

	cout << endl;
	cout << "... find more at http://www.mysql.com" << endl;
	cout << endl;
	return EXIT_SUCCESS;
}


void* thread_one_action(void *arg) {
	int status;

	/* In every new thread you must call threadInit() *before* doing anything the connector */
	cout << "\t... Thread 1: driver->threadInit()" << endl;
	driver->threadInit();

	cout << "\t... Thread 1: pthread_detach()" << endl;
	status = pthread_detach(pthread_self());
	if (status != 0) {
		/* You must call threadEnd() when the thread finishes! */
		driver->threadEnd();
		cout << "#\t...Thread 1: ERR: phtread_detach() statu = " << status << endl;
		new std::runtime_error("Thread 1: pthread_detach() failed");
	}

	cout << "\t... Thread 1: running 'SELECT 'Welcome to Connector/C++' AS _message'" << endl;
	stmt.reset(con->createStatement());
	res.reset(stmt->executeQuery("SELECT 'Welcome to Connector/C++' AS _message"));
	while (res->next()) {
		cout << "\t... Thread 1: MySQL replies: " << res->getString("_message") << endl;
		cout << "\t... Thread 1: say it again, MySQL" << endl;
		cout << "\t....Thread 1: MySQL replies: " << res->getString(1) << endl;
	}

	cout << "\t Thread 1: driver->threadEnd()" << endl;
	driver->threadEnd();

	return NULL;
}