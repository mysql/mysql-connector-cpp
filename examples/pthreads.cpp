/*
 * Copyright (c) 2008, 2020, Oracle and/or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
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
*    examples/pthreads.cpp
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

struct st_worker_thread_param {
  sql::Driver *driver;
  sql::Connection *con;
};

using namespace std;

void* thread_one_action(void *arg);
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
  sql::Driver *driver;
  std::unique_ptr< sql::Connection > con;

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
    /*
      A little bloat.
      We don't want global scoped_ptr objects. Therefore
      we wrap the object in an object. An alternative
      would have been to use global sql::Driver, sql::Connection
      objects [plain objects and no scoped_ptr] but then
      we'd have to add bloat for making sure we explicitly
      delete them, e.g. in case of an exception.
      It is not nice in either case. Let's use parameter struct.
    */
    struct st_worker_thread_param *param = new st_worker_thread_param;
    param->driver = driver;
    param->con = con.get();

    status = pthread_create(&thread_one, NULL, thread_one_action, (void *)param);
    if (status != 0)
      throw std::runtime_error("Thread creation has failed");

    status = pthread_join(thread_one, NULL);
    if (status != 0)
      throw std::runtime_error("Joining thread has failed");

    while (thread_finished == 0) {
      /*
      The worker is using global resources which the main thread shall not
      free before the worker thread is done. For example, the worker
      thread is using the Connection object.
      */
      cout << "Main thread: waiting for thread to finish fetching data..." << endl;
      usleep(300000);
    }

    delete param;

    cout << "Main thread: thread 1 has finished fetching data from MySQL..." << endl;

  } catch (sql::SQLException &e) {

    cout << "# ERR: SQLException in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;

    return EXIT_FAILURE;

  } catch (std::runtime_error &e) {

    cout << "# ERR: runtime_error in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
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
  std::unique_ptr< sql::Statement > stmt;
  std::unique_ptr< sql::ResultSet > res;

  struct st_worker_thread_param *handles = (struct st_worker_thread_param*) arg;

  /*
    NOTE:
    In every new thread you must call threadInit() *before*
    doing anything with the Connector. If you omit this
    step anything, including a crash, may happen.
  */
  cout << endl;
  cout << "\tThread 1: driver->threadInit()" << endl;
  handles->driver->threadInit();

  cout << "\tThread 1: ... statement object created" << endl;
  stmt.reset(handles->con->createStatement());

  /*
    Sharing resultset among threads should
    be avoided. Its possible but requires further
    action from you.
  */
  cout << "\tThread 1: ... running 'SELECT SLEEP(1), 'Welcome to Connector/C++' AS _message'" << endl;
  res.reset(stmt->executeQuery("SELECT SLEEP(1), 'Welcome to Connector/C++' AS _message"));

  cout << "\tThread 1: ... fetching result" << endl;
  while (res->next()) {
    cout << "\tThread 1: ... MySQL replies: " << res->getString("_message") << endl;
    cout << "\tThread 1: ... say it again, MySQL" << endl;
    cout << "\tThread 1: ... MySQL replies: " << res->getString(2) << endl;
  }

  cout << "\tThread 1: driver->threadEnd()" << endl;

  /*
    NOTE:
    You must call threadEnd() when the thread
    exits If you omit this step you get messages
    from the C Client Library like this:

    Error in my_thread_global_end(): 1 threads didn't exit

  */
  handles->driver->threadEnd();
  cout << endl;

  thread_finished = 1;

  return NULL;
}
