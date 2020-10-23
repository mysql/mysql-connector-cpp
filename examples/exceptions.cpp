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
* Types of execptions thrown by Connector/C++
*
*/

/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

/*
  Public interface of the MySQL Connector/C++.
  You might not use it but directly include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
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

  // Driver Manager
  sql::Driver *driver;
  int i;

  cout << boolalpha;
  cout << "1..1" << endl;
  cout << "# Connector/C++ types of exceptions.." << endl;

  try {
    /* Using the Driver to create a connection */
    driver = sql::mysql::get_driver_instance();
    std::unique_ptr< sql::Connection > con(driver->connect(url, user, pass));

    /* Run in autocommit mode */
    con->setAutoCommit(1);
    std::unique_ptr< sql::Savepoint > savepoint;
    try {
      // It makes no sense to set a savepoint in autocommit mode
      savepoint.reset(con->setSavepoint(string("before_insert")));
      throw new runtime_error("Setting a savepoint should not be allow in autocommit mode");
    } catch (sql::InvalidArgumentException &e) {
      cout << "#\t Invalid Argument: " << e.what();
      cout << " (MySQL error code: " << e.getErrorCode();
      cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    con->setSchema(database);

    std::unique_ptr< sql::Statement > stmt(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, label CHAR(1))");
    cout << "#\t Test table created" << endl;

    try {
      con->setReadOnly(true);
    } catch (sql::MethodNotImplementedException &e) {
      cout << "#\t Not implemented: " << e.what();
      cout << " (MySQL error code: " << e.getErrorCode();
      cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    con->setAutoCommit(0);
    try {
      savepoint.reset(con->setSavepoint(string("")));
    } catch (sql::InvalidArgumentException &e) {
      cout << "#\t Invalid Argument: " << e.what();
      cout << " (MySQL error code: " << e.getErrorCode();
      cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
    savepoint.reset(con->setSavepoint(string("before_insert")));

    {
      std::unique_ptr< sql::PreparedStatement > prep_stmt(con->prepareStatement("INSERT INTO test(id, label) VALUES (?, ?)"));
      for (i = 0; i < EXAMPLE_NUM_TEST_ROWS; i++) {
        prep_stmt->setInt(1, test_data[i].id);
        prep_stmt->setString(2, test_data[i].label);
        prep_stmt->executeUpdate();
      }
    }

    try {
      std::unique_ptr< sql::PreparedStatement > prep_stmt(con->prepareStatement("INSERT INTO test(id, label) VALUES (?, ?)"));
      prep_stmt->setInt(1, test_data[0].id);
      /* This will cause a duplicate index error */
      prep_stmt->executeUpdate();
      throw new runtime_error("Query not executed or duplicate key not detected");
    } catch (sql::SQLException &e) {
      cout << "#\t SQL Exception: " << e.what();
      cout << " (MySQL error code: " << e.getErrorCode();
      cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    /* Switching back to auto commit will invalidate the savepoint!	*/
    con->setAutoCommit(1);
    try {
      con->releaseSavepoint(savepoint.get());
      throw new runtime_error("Releasing a savepoint should not be allow in autocommit mode");
    } catch (sql::InvalidArgumentException &e) {
      cout << "#\t Invalid Argument: " << e.what();
      cout << " (MySQL error code: " << e.getErrorCode();
      cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    stmt->execute("DROP TABLE IF EXISTS test");
    cout << "#done!" << endl;

  } catch (sql::SQLException &e) {
    /*
    The MySQL Connector/C++ throws three different exceptions:

    - sql::MethodNotImplementedException (derived from sql::SQLException)
    - sql::InvalidArgumentException (derived from sql::SQLException)
    - sql::SQLException (derived from std::runtime_error)

                 sql::SQLException is the base class.
    */
    cout << endl;
    cout << "# ERR: DbcException in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    /* Use what(), getErrorCode() and getSQLState() */
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
    cout << "not ok 1 - examples/exceptions.cpp" << endl;

    return EXIT_FAILURE;
  } catch (std::runtime_error &e) {

    cout << endl;
    cout << "# ERR: runtime_error in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what() << endl;
    cout << "not ok 1 - examples/exceptions.cpp" << endl;

    return EXIT_FAILURE;
  }

  cout << "ok 1 - examples/exceptions.cpp" << endl;
  return EXIT_SUCCESS;
}


bool prepare_execute(sql::Connection *con, const char *sql)
{
  sql::PreparedStatement *prep_stmt;

  prep_stmt = con->prepareStatement(sql);
  prep_stmt->execute();
  delete prep_stmt;

  return true;
}


sql::Statement* emulate_prepare_execute(sql::Connection *con, const char *sql)
{
  sql::PreparedStatement *prep_stmt;
  sql::Statement *stmt = NULL;

  cout << "#\t\t 'emulation': " << sql << endl;

  try {

    prep_stmt = con->prepareStatement(sql);
    prep_stmt->execute();
    cout << "#\t\t 'emulation': use of sql::PreparedStatement possible" << endl;
    // safe upcast - PreparedStatement is derived from Statement
    stmt = prep_stmt;

  } catch (sql::SQLException &e) {
    /*
    Maybe the command is not supported by the MySQL Server?

    http://dev.mysql.com/doc/refman/5.1/en/error-messages-server.html
    Error: 1295 SQLSTATE: HY000 (ER_UNSUPPORTED_PS)

    Message: This command is not supported in the prepared statement protocol yet
    */
    if (e.getErrorCode() != 1295) {
      /*
      The MySQL Server should be able to prepare the statement
      but something went wrong. Let the caller handle the error.
      */
      throw ;
    }
    cout << "#\t\t 'emulation': ER_UNSUPPORTED_PS and fallback to sql::Statement" << endl;
    cout << "#\t\t ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;

    stmt = con->createStatement();
    stmt->execute(sql);
  }

  return stmt;
}
