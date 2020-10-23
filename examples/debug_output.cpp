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
* Debug/Tracefile generation
*
* Two types of debug/trace files are available with Connector/C++. You can make
* Connector/C++ write a trace file and/or activate tracing on the level of the
* underlying MySQL Client Libarary.
*
* Connector/C++ is based on the MySQL Client Library (C-API, AKA libmysql).
* Connector/C++ is a proxy/wrapper of the MySQL Client Library.
* Debug versions of the MySQL Client Library can write a trace file. On the C-level
* this is done by passing a string with debug instructions to the C function
* mysql_debug(). You can use the Connector/C++ method
* setClientOption("libmysql_debug", const string debug) to pass a string to the
* C-API function mysql_debug(). See the MySQL C-API documentation
*
* Example of a libmysql trace:
* >mysql_stmt_init
* | >_mymalloc
* | | enter: Size: 816
* | | exit: ptr: 0x68e7b8
* | <_mymalloc
* | >init_alloc_root
* | | enter: root: 0x68e7b8
* | | >_mymalloc
* | | | enter: Size: 2064
* | | | exit: ptr: 0x68eb28
* [...]
*
* If you do not have compiled Connector/C++ against a debug version of the MySQL
* Client Library but you have compiled  Connector/C++ with tracing enabled, then
* you may activate the Connector/C++ internal trace using
* setClientOption("clientTrace", bool on_off).
*
* To compile Connector/C++ with tracing enabled, do:
*   cmake -DMYSQLCPPCONN_TRACE_ENABLE:BOOL=1
*
* Example of a Connector/C++ trace:
* |  INF: Tracing enabled
* <MySQL_Connection::setClientOption
* >MySQL_Prepared_Statement::setInt
* |  INF: this=0x69a2e0
* |  >MySQL_Prepared_Statement::checkClosed
* |  <MySQL_Prepared_Statement::checkClosed
* | <MySQL_Prepared_Statement::setInt
*
* Note that the traces will contain your SQL statements. You may not want
* to share sensitive SQL statements.
*
* @link http://dev.mysql.com/doc/refman/5.1/en/mysql-debug.html
*/

/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

/* Public interface of the MySQL Connector/C++ */
#include <jdbc.h>
/* Connection parameter and sample data */
#include "examples.h"

using namespace std;

int main(int argc, const char **argv)
{
  const char* mysql_host = getenv("MYSQL_HOST");
  const char* mysql_port = getenv("MYSQL_PORT");
  const char* mysql_user = getenv("MYSQL_USER");
  const char* mysql_password = getenv("MYSQL_PASSWORD");

  string url(EXAMPLE_HOST);
  string user(EXAMPLE_USER);
  string pass(EXAMPLE_PASS);
  string database(argc >= 5 ? argv[4] : EXAMPLE_DB);

  if(argc >= 2)
  {
    url = argv[1];
  }
  else if(mysql_host || mysql_port)
  {
    url = "mysql://";
    url+= mysql_host ? mysql_host : "localhost";
    url+= ":";
    url+= mysql_port ? mysql_port : "3306";
  }

  if(argc >= 3)
  {
    user = argv[2];
  }
  else if(mysql_user)
  {
    user = mysql_user;
  }

  if(argc >= 4)
  {
    pass = argv[3];
  }
  else if(mysql_password)
  {
    pass = mysql_password;
  }

  sql::Driver *driver;
  int i;
  int on_off = 1;

  cout << boolalpha;
  cout << "1..1" << endl;;
  cout << "# Connector/C++ debug / client_options example.." << endl;

  try {
    /* Using the Driver to create a connection */
    driver = sql::mysql::get_driver_instance();
    std::unique_ptr< sql::Connection > con(driver->connect(url, user, pass));

    /*
     Activate debug trace of the MySQL Client Library (C-API)
    Only available with a debug build of the MySQL Client Library!
    */
    con->setClientOption("libmysql_debug", "d:t:O,client.trace");

    con->setSchema(database);

    std::unique_ptr< sql::Statement > stmt(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, label CHAR(1))");
    cout << "#\t Test table created" << endl;

    std::unique_ptr< sql::PreparedStatement > prep_stmt(con->prepareStatement("INSERT INTO test(id, label) VALUES (?, ?)"));
    for (i = 0; i < EXAMPLE_NUM_TEST_ROWS; i++) {
      prep_stmt->setInt(1, test_data[i].id);
      prep_stmt->setString(2, test_data[i].label);
      prep_stmt->executeUpdate();

      /*
      Pointless activity to demo how to
      activating and de-activating the Connector/C++ internal trace
      */
      on_off = (i + 2) % 2;
      con->setClientOption("clientTrace", &on_off);
    }

    stmt->execute("DROP TABLE IF EXISTS test");

  } catch (sql::SQLException &e) {
    /*
    The MySQL Connector/C++ throws three different exceptions:

    - sql::MethodNotImplementedException (derived from sql::SQLException)
    - sql::InvalidArgumentException (derived from sql::SQLException)
    - sql::SQLException (derived from std::runtime_error)
    */
    cout << endl;
    cout << "# ERR: DbcException in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    // Use what(), getErrorCode() and getSQLState()
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;

    if (e.getErrorCode() == 1047) {
      /*
      Error: 1047 SQLSTATE: 08S01 (ER_UNKNOWN_COM_ERROR)
      Message: Unknown command
      */
      cout << "# ERR: Your server seems not to support PS at all because its MYSQL <4.1" << endl;
    }
    cout << "not ok 1 - examples/debug.cpp" << endl;

    return EXIT_FAILURE;
  } catch (std::runtime_error &e) {

    cout << endl;
    cout << "# ERR: runtime_error in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what() << endl;
    cout << "not ok 1 - examples/debug.cpp" << endl;

    return EXIT_FAILURE;
  }

  cout << "ok 1 - examples/debug.cpp" << endl;
  return EXIT_SUCCESS;
}
