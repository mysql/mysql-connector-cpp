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
*   -o pthread
*   -I/usr/local/include/cppconn/
*   -Wl,-Bdynamic -lmysqlcppconn -pthread
*    examples/pthread.cpp
*
* To run the example on Linux try something similar to:
*
*  LD_LIBRARY_PATH=/usr/local/lib/ ./pthread
*
* or:
*
*  LD_LIBRARY_PATH=/usr/local/lib/ ./pthread host user password database
*
*/


/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

/* usleep() */
#include <unistd.h>

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

sql::Driver *driver;
std::auto_ptr< sql::Connection > con;
std::auto_ptr< sql::Statement > stmt;
std::auto_ptr< sql::ResultSet > res;
int thread_finished = 0;

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

	int status;
	pthread_t thread_one;


	cout << endl;
	cout << "Main thread: Connector/C++ phthreads program example..." << endl;
	cout << endl;

	try {
		driver = sql::mysql::get_driver_instance();

		/* Using the Driver to create a connection */
		con.reset(driver->connect(url, user, pass));
		con->setSchema(database);

		/* Worker thread */
		cout << "Main thread: creating thread 1..." << endl;
		status = pthread_create(&thread_one, NULL, thread_one_action, NULL);
		cout << "Main thread: thread 1 status = " << status << " (0 = no error)" << endl;
		if (status != 0)
				throw std::runtime_error("Thread creation has failed");

		while (thread_finished == 0) {
			cout << "Main thread: waiting for thread to finish fetching data..." << endl;
			usleep(300000);
		}
		cout << "Main thread: thread 1 has finished fetching data from MySQL..." << endl;

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
	cout << "Main thread: ... find more at http://www.mysql.com" << endl;
	cout << endl;
	return EXIT_SUCCESS;
}


void* thread_one_action(void *arg) {
	int status;

	/*
		NOTE:
		In every new thread you must call threadInit() *before*
		doing anything with the Connector. If you omit this
		step anything, including a crash, may happen.
	*/
	cout << "\tThread 1: driver->threadInit()" << endl;
	driver->threadInit();

	status = pthread_detach(pthread_self());
	if (0 != status) {
		cout << "\tThread 1: detach failed with status = " << status << endl;
		/* See below */
		driver->threadEnd();
		throw std::runtime_error("Thread 1: pthread_detach failed");
	}

	cout << "\tThread 1: ... statement object created" << endl;
	stmt.reset(con->createStatement());

	/*
		Sharing resultset among threads should
		be avoided. Its possible but requires further
		action from you.
	*/
	cout << "\tThread 1: ... running 'SELECT SLEEP(1), 'Welcome to Connector/C++' AS _message'" << endl;
	res.reset(stmt->executeQuery("SELECT SLEEP(1), 'Welcome to Connector/C++' AS _message"));

	cout << "\tThread 1: ... fetching result" << endl;
	while (res->next()) {
		cout << "\t... Thread 1: MySQL replies: " << res->getString("_message") << endl;
		cout << "\t... Thread 1: say it again, MySQL" << endl;
		cout << "\t....Thread 1: MySQL replies: " << res->getString(2) << endl;
	}

	cout << "\tThread 1: ... driver->threadEnd()" << endl;

	/*
		NOTE:
		You must call threadEnd() when the thread
		exits If you omit this step you get messages
		from the C Client Library like this:

		Error in my_thread_global_end(): 1 threads didn't exit

	*/
	driver->threadEnd();

	thread_finished = 1;

	return NULL;
}