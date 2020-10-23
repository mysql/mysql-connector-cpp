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
* Example of statements - not to be confused with prepared statements
*
* NOTE: The MySQL Server does not support each and every SQL statement
* to be prepared. The list of statements which can be prepared is available
* in the MySQL Server documentation and the C API documentation:
* http://dev.mysql.com/doc/refman/5.1/en/c-api-prepared-statements.html
* (Link to the MySQL Server 5.1 documentation!)
*
* Connector/C++ is based on the C API and C library "libmysql". Therefore
* it inherits all limitations from the MySQL Server and the MySQL C API.
*
* MySQL 5.1.12 can prepare the following statements:
*
* - CREATE TABLE, DELETE, DO, INSERT, REPLACE, SELECT, SET, UPDATE
* - most SHOW commands
* - ANALYZE TABLE, OPTIMIZE TABLE, REPAIR TABLE
* - CACHE INDEX, CHANGE SOURCE, CHECKSUM {TABLE | TABLES},
* - {CREATE | RENAME | DROP} DATABASE, {CREATE | RENAME | DROP} USER
* - FLUSH {TABLE | TABLES | TABLES WITH READ LOCK | HOSTS | PRIVILEGES | LOGS | STATUS | SOURCE | REPLICA | DES_KEY_FILE | USER_RESOURCES}
* - GRANT, REVOKE, KILL, LOAD INDEX INTO CACHE, RESET {SOURCE | REPLICA | QUERY CACHE}
* - SHOW BINLOG EVENTS, SHOW CREATE {PROCEDURE | FUNCTION | EVENT | TABLE | VIEW}
* - SHOW {AUTHORS | CONTRIBUTORS | WARNINGS | ERRORS}
* - SHOW {SOURCE | BINARY} LOGS, SHOW {SOURCE | REPLICA} STATUS
* - REPLICA {START | STOP}, INSTALL PLUGIN, UNINSTALL PLUGIN
*
*  ... that's pretty much every *core* SQL statement - but not USE as you'll see below.
*
* Connector/C++ does not include a prepared statement emulation
*
* @link http://dev.mysql.com/doc/refman/5.1/en/c-api-prepared-statements.html
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

bool prepare_execute(std::unique_ptr< sql::Connection > & con, const char *sql);
sql::Statement* emulate_prepare_execute(std::unique_ptr< sql::Connection > & con, const char *sql);

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

  /* Driver Manager */
  sql::Driver *driver;
  /* sql::ResultSet.rowsCount() returns size_t */
  size_t row;
  stringstream sql;
  stringstream msg;
  int i, num_rows;

  cout << boolalpha;
  cout << "1..1" << endl;;
  cout << "# Connector/C++ prepared statement example.." << endl;

  try {
    /* Using the Driver to create a connection */
    driver = sql::mysql::get_driver_instance();
    std::unique_ptr< sql::Connection > con(driver->connect(url, user, pass));

    /* The usage of USE is not supported by the prepared statement protocol */
    std::unique_ptr< sql::Statement > stmt(con->createStatement());
    stmt->execute("USE " + database);

    /*
    Prepared statement are unhandy for queries which you execute only once!

    prepare() will send your SQL statement to the server. The server
    will do a SQL syntax check, perform some static rewriting like eliminating
    dead expressions such as "WHERE 1=1" and simplify expressions
    like "WHERE a > 1 AND a > 2" to "WHERE a > 2". Then control gets back
    to the client and the server waits for execute() (or close()).
    On execute() another round trip to the server is done.

    In case you execute your prepared statement only once - like shown below -
    you get two round trips. But using "simple" statements - like above - means
    only one round trip.

    Therefore, the below is *bad* style. WARNING: Although its *bad* style,
    the example program will continue to do it to demonstrate the (ab)use of
    prepared statements (and to prove that you really can do more than SELECT with PS).
    */
    std::unique_ptr< sql::PreparedStatement > prep_stmt(con->prepareStatement("DROP TABLE IF EXISTS test"));
    prep_stmt->execute();

    prepare_execute(con, "CREATE TABLE test(id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, label CHAR(1))");
    cout << "#\t Test table created" << endl;

    /*
    The first useful example - prepare() once, execute() n + 1 times
    NOTE: The MySQL Server does not support named parameters. You have to use
    the placeholder syntax shown below. There is no emulation which would you
    allow to use named parameter like ':param1'. Use '?'. Parameters are 1-based.
    */
    num_rows = 0;
    prep_stmt.reset(con->prepareStatement("INSERT INTO test(id, label) VALUES (?, ?)"));
    for (i = 0; i < EXAMPLE_NUM_TEST_ROWS; i++) {
      prep_stmt->setInt(1, test_data[i].id);
      prep_stmt->setString(2, test_data[i].label);
      /* executeUpdate() returns the number of affected = inserted rows */
      num_rows += prep_stmt->executeUpdate();
    }

    if (EXAMPLE_NUM_TEST_ROWS != num_rows) {
      msg.str("");
      msg << "Expecting " << EXAMPLE_NUM_TEST_ROWS << "rows, reported " << num_rows;
      throw runtime_error(msg.str());
    }
    cout << "#\t Test table populated" << endl;

    /* We will reuse the SELECT a bit later... */
    std::unique_ptr< sql::PreparedStatement > prep_select(con->prepareStatement("SELECT id, label FROM test ORDER BY id ASC"));
    cout << "#\t Running 'SELECT id, label FROM test ORDER BY id ASC'" << endl;
    std::unique_ptr< sql::ResultSet > res(prep_select->executeQuery());
    row = 0;
    while (res->next()) {
      cout << "#\t\t Row " << row << " - id = " << res->getInt("id");
      cout << ", label = '" << res->getString("label") << "'" << endl;
      row++;
    }
    res.reset(NULL);

    if (EXAMPLE_NUM_TEST_ROWS != row) {
      msg.str("");
      msg << "Expecting " << EXAMPLE_NUM_TEST_ROWS << "rows, reported " << row;
      throw runtime_error(msg.str());
    }

    cout << "#\t Simple PS 'emulation' for USE and another SELECT" << endl;
    stmt.reset(emulate_prepare_execute(con, string("USE " + database).c_str()));
    stmt.reset(emulate_prepare_execute(con, string("USE " + database).c_str()));

    stmt.reset(emulate_prepare_execute(con, "SELECT id FROM test ORDER BY id ASC"));
    res.reset(stmt->getResultSet());
    if (res.get() != NULL) {
      row = 0;
      while (res->next()) {
        cout << "#\t\t Row " << row << " - id = " << res->getInt("id") << endl;
        row++;
      }
      res.reset(NULL);
    }
    stmt.reset(NULL);

    /* Running the SELECT again but fetching in reverse order */
    cout << "#\t SELECT and fetching in reverse order" << endl;

    res.reset(prep_select->executeQuery());
    row = res->rowsCount();
    cout << "#\t\t res->getRowsCount() = " << res->rowsCount() << endl;
    if (res->rowsCount() != EXAMPLE_NUM_TEST_ROWS) {
      msg.str("");
      msg << "Expecting " << EXAMPLE_NUM_TEST_ROWS << "rows, found " << res->rowsCount();
      throw runtime_error(msg.str());
    }

    /* Position the cursor after the last row */
    cout << "#\t\t Position the cursor after the last row\n";
    res->afterLast();
    cout << "#\t\t res->isafterLast()\t= " << res->isAfterLast() << endl;
    cout << "#\t\t res->isLast()\t\t= " << res->isLast() << endl;
    if (!res->isAfterLast() || res->isLast())
      throw runtime_error("Moving the cursor after the last row failed");

    while (res->previous()) {
      cout << "#\t\t res->previous()\n";
      cout << "#\t\t Row " << row << " - id = " << res->getInt("id");
      cout << ", label = '" << res->getString("label") << "'" << endl;
      row--;
    }
    cout << "#\t\t Should be before the first\n";
    cout << "#\t\t res->isFirst()\t\t= " << res->isFirst() << endl;
    cout << "#\t\t res->isBeforeFirst()\t= " << res->isBeforeFirst() << endl;
    if (res->isFirst() || !res->isBeforeFirst())
      throw runtime_error("Cursor should be before first row");

    /* Now that the cursor is before the first, fetch the first */
    cout << "#\t\t Now that the cursor is before the first, fetch the first\n";
    cout << "#\t\t calling next() to fetch first row" << endl;
    row++;
    res->next();
    cout << "#\t\t res->isFirst()\t\t= " << res->isFirst() << endl;
    cout << "#\t\t Row " << row << " - id = " << res->getInt("id");
    cout << ", label = '" << res->getString("label") << "'" << endl;
    row--;

    /* For more on cursors see resultset.cpp example */

    /* Clean up */
    res.reset(NULL);
    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    cout << "#done!" << endl;

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
    cout << "not ok 1 - examples/prepared_statement.cpp" << endl;

    return EXIT_FAILURE;
  } catch (std::runtime_error &e) {

    cout << endl;
    cout << "# ERR: runtime_error in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what() << endl;
    cout << "not ok 1 - examples/prepared_statement.cpp" << endl;

    return EXIT_FAILURE;
  }

  cout << "ok 1 - examples/prepared_statement.cpp" << endl;
  return EXIT_SUCCESS;
}


bool prepare_execute(std::unique_ptr< sql::Connection > & con, const char *sql)
{
  sql::PreparedStatement * prep_stmt;

  prep_stmt = con->prepareStatement(sql);
  prep_stmt->execute();
  delete prep_stmt;

  return true;
}


sql::Statement* emulate_prepare_execute(std::unique_ptr< sql::Connection > & con, const char *sql)
{
  sql::PreparedStatement *prep_stmt;
  sql::Statement *stmt = NULL;

  cout << "#\t\t 'emulation': " << sql << endl;

  try {

    prep_stmt = con->prepareStatement(sql);
    prep_stmt->execute();
    cout << "#\t\t 'emulation': use of sql::PreparedStatement possible" << endl;
    /* safe upcast - PreparedStatement is derived from Statement */
    stmt = prep_stmt;

  } catch (sql::SQLException &e) {
    /*
    Maybe the command is not supported by the MySQL Server?

    http://dev.mysql.com/doc/refman/5.1/en/error-messages-server.html
    Error: 1295 SQLSTATE: HY000 (ER_UNSUPPORTED_PS)

    Message: This command is not supported in the prepared statement protocol yet
    */

    if (e.getErrorCode() != 1295) {
      // The MySQL Server should be able to prepare the statement
      // but something went wrong. Let the caller handle the error.
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

