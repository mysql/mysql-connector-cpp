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



/* *
* Basic example demonstrating how to load a different driver.
*
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

static void validateResultSet(std::unique_ptr< sql::ResultSet > & res, struct _test_data *min, struct _test_data *max);

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
  /* Connection properties */
  sql::ConnectOptionsMap connection_properties;

  /* sql::ResultSet.rowsCount() returns size_t */
  stringstream sql;
  int i;
  struct _test_data min, max;

  cout << boolalpha;
  cout << "1..1" << endl;
  cout << "# Connector/C++ dynamic loading.." << endl;

  try {
    /*
      Driver / client library to load.

      The preprocessor constant DYNLOAD_MYSQL_LIB comes from the build environment.
      It should contain the path of the C library which Connector/C++ has been
      linked against.

      For testing the functionality, feed free to replace DYNLOAD_MYSQL_LIB with
      any constant string containing the path of an alternate client library.

      get_driver_instance_by_name(const char * const clientlib)

      If you use get_driver_instance_by_name Connector/C++ will not load the
      default C client library to establish a connection to MySQL but the one
      you specify. The default C library is the one found during cmake (configure)
      and used for building Connector/C++. On most systems this
      will be the MySQL Client Library shipped together with the
      MySQL Server found on the build host. However, at runtime you can
      advise Connector/C++ to dynamically load a different MySQL Client Library.
      This can be a MySQL Client Library from another MySQL Server or the
      library of Connector/C.

      CAUTION: In some development versions of C/C++ 1.1.0 a connection property
      has been used to set the library. This has changed. The temporary syntax
      has been like this:

        sql::SQLString lib(DYNLOAD_MYSQL_LIB);
        connection_properties["clientlib"] = lib;

    */
    driver = sql::mysql::get_driver_instance_by_name(DYNLOAD_MYSQL_LIB);

    /* 'standard' connection properties */
    connection_properties["hostName"] = url;
    connection_properties["userName"] = user;
    connection_properties["password"] = pass;
    connection_properties["schema"] = database;

    std::unique_ptr< sql::Connection > con(driver->connect(connection_properties));

    /* Creating a "simple" statement - "simple" = not a prepared statement */
    std::unique_ptr< sql::Statement > stmt(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT, label CHAR(1))");
    cout << "#\t Test table created" << endl;

    /* Populate the test table with data */
    min = max = test_data[0];
    for (i = 0; i < EXAMPLE_NUM_TEST_ROWS; i++) {
      /* Remember mnin/max values for further testing */
      if (test_data[i].id < min.id) {
        min = test_data[i];
      }
      if (test_data[i].id > max.id) {
        max = test_data[i];
      }

      /*
      KLUDGE: You should take measures against SQL injections!
      */
      sql.str("");
      sql << "INSERT INTO test(id, label) VALUES (";
      sql << test_data[i].id << ", '" << test_data[i].label << "')";
      stmt->execute(sql.str());
    }
    cout << "#\t Test table populated" << endl;

    /*
    This is an example how to fetch in reverse order using the ResultSet cursor.
    Every ResultSet object maintains a cursor, which points to its current
    row of data. The cursor is 1-based. The first row has the cursor position 1.

    NOTE: The Connector/C++ preview/alpha uses buffered results.
    THe driver will always fetch all data no matter how big the result set is!
    */
    cout << "#\t Testing sql::Statement based resultset" << endl;
    {
      std::unique_ptr< sql::ResultSet > res(stmt->executeQuery("SELECT id, label FROM test ORDER BY id ASC"));
      validateResultSet(res, &min, &max);
    }

    cout << "#" << endl;
    cout << "#\t Testing sql::PreparedStatment based resultset" << endl;

    {
      std::unique_ptr< sql::PreparedStatement > prep_stmt(con->prepareStatement("SELECT id, label FROM test ORDER BY id ASC"));
      std::unique_ptr< sql::ResultSet > res(prep_stmt->executeQuery());
      validateResultSet(res, &min, &max);
    }

    /* Clean up */
    stmt->execute("DROP TABLE IF EXISTS test");
    cout << "# done!" << endl;

  } catch (sql::SQLException &e) {
    /*
    The MySQL Connector/C++ throws three different exceptions:

    - sql::MethodNotImplementedException (derived from sql::SQLException)
    - sql::InvalidArgumentException (derived from sql::SQLException)
    - sql::SQLException (derived from std::runtime_error)
    */
    cout << "# ERR: SQLException in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    // Use what(), getErrorCode() and getSQLState()
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    cout << "not ok 1 - examples/resultset.cpp" << endl;

    return EXIT_FAILURE;
  } catch (std::runtime_error &e) {

    cout << "# ERR: runtime_error in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what() << endl;
    cout << "not ok 1 - examples/resultset.cpp" << endl;

    return EXIT_FAILURE;
  }

  cout << "ok 1 - examples/dynamic_load.cpp" << endl;
  return EXIT_SUCCESS;
}

static void validateRow(std::unique_ptr< sql::ResultSet > & res, struct _test_data *exp)
{
  stringstream msg;

  cout << "#\t\t Fetching the first row, id = " << res->getInt("id");
  cout << ", label = '" << res->getString("label") << "'" << endl;

  if ((res->getInt("id") != exp->id) || (res->getString("label") != exp->label)) {
    msg.str("Wrong results");
    msg << "Expected (" << exp->id << "," << exp->label << ")";
    msg << " got (" << res->getInt("id") <<", " << res->getString("label") << ")";
    throw runtime_error(msg.str());
  }
}

static void validateResultSet(std::unique_ptr< sql::ResultSet > & res, struct _test_data *min, struct _test_data *max) {

  size_t row;

  cout << "#\t Selecting in ascending order but fetching in descending (reverse) order" << endl;
  /* Move the cursor after the last row - n + 1 */
  res->afterLast();
  if (true != res->isAfterLast())
    throw runtime_error("Position should be after last row (1)");

  row = res->rowsCount() - 1;
  /* Move the cursor backwards to: n, n - 1, ... 1, 0. Return true if rows are available. */
  while (res->previous()) {
    cout << "#\t\t Row " << row << " id = " << res->getInt("id");
    cout << ", label = '" << res->getString("label") << "'" << endl;
    row--;
  }
  /*
  The last call to res->previous() has moved the cursor before the first row
  Cursor position is 0, recall: rows are from 1 ... n
  */
  cout << "#\t\t isBeforeFirst() = " << res->isBeforeFirst() << endl;
  cout << "#\t\t isFirst() = " << res->isFirst() << endl;
  if (!res->isBeforeFirst()) {
    throw runtime_error("Cursor should be positioned before the first row");
  }
  /* Move the cursor forward again to position 1 - the first row */
  res->next();
  cout << "#\t Positioning cursor to 1 using next(), isFirst() = " << res->isFirst() << endl;
  validateRow(res, min);
  /* Move the cursor to position 0 = before the first row */
  if (false != res->absolute(0)) {
    throw runtime_error("Call did not fail although its not allowed to move the cursor before the first row");
  }
  cout << "#\t Positioning before first row using absolute(0), isFirst() = " << res->isFirst() << endl;
  /* Move the cursor forward to position 1 = the first row */
  res->next();
  validateRow(res, min);

  /* Move the cursor to position 0 = before the first row */
  res->beforeFirst();
  cout << "#\t Positioning cursor using beforeFirst(), isFirst() = " << res->isFirst() << endl;
  /* Move the cursor forward to position 1 = the first row */
  res->next();
  cout << "#\t\t Moving cursor forward using next(), isFirst() = " << res->isFirst() << endl;
  validateRow(res, min);

  cout << "#\t Finally, reading in descending (reverse) order again" << endl;
  /* Move the cursor after the last row - n + 1 */
  res->afterLast();
  row = res->rowsCount() - 1;
  /* Move the cursor backwards to: n, n - 1, ... 1, 0. Return true if rows are available.*/
  while (res->previous()) {
    cout << "#\t\t Row " << row << ", getRow() " << res->getRow();
    cout << " id = " << res->getInt("id");
    cout << ", label = '" << res->getString("label") << "'" << endl;
    row--;
  }
  /*
  The last call to res->previous() has moved the cursor before the first row
  Cursor position is 0, recall: rows are from 1 ... n
  */
  cout << "#\t\t isBeforeFirst() = " << res->isBeforeFirst() << endl;
  if (true != res->isBeforeFirst()) {
    throw runtime_error("Position should be 0 = before first row");
  }

  cout << "#\t And in regular order..." << endl;
  res->beforeFirst();
  if (true != res->isBeforeFirst()) {
    throw runtime_error("Cursor should be positioned before the first row");
  }
  row = 0;
  while (res->next()) {
    cout << "#\t\t Row " << row << ", getRow() " << res->getRow();
    cout << " id = " << res->getInt("id");
    cout << ", label = '" << res->getString("label") << "'" << endl;
    row++;
  }
  cout << "#\t\t isAfterLast() = " << res->isAfterLast() << endl;
  if (true != res->isAfterLast()) {
    throw runtime_error("next() has returned false and the cursor should be after the last row");
  }
  /* Move to the last entry using a negative offset for absolute() */
  cout << "#\t Trying absolute(-1) to fetch last entry..." << endl;
  if (true != res->absolute(-1)) {
    throw runtime_error("Call did fail although -1 is valid");
  }
  cout << "#\t\t isAfterLast() = " << res->isAfterLast() << endl;
  if (false != res->isAfterLast()) {
    throw runtime_error("Cursor should be positioned to the last row and not after the last row");
  }
  cout << "#\t\t isLast() = " << res->isLast() << endl;
  if (true != res->isLast()) {
    throw runtime_error("Cursor should be positioned to the last row");
  }
  validateRow(res, max);
  /* Another way to move after the last entry */
  cout << "#\t Trying absolute(NUMROWS + 10) to move cursor after last row and fetch last entry..." << endl;
  if (false != res->absolute(static_cast<int>(res->rowsCount() + 10))) {
    throw runtime_error("Call did fail although parameter is valid");
  }
  if (true != res->isAfterLast()) {
    throw runtime_error("Cursor should be positioned after the last row");
  }
  cout << "#\t\t isLast() = " << res->isLast() << endl;
  if (false != res->isLast()) {
    throw runtime_error("Cursor should be positioned after the last row");
  }
  try {
    res->getString(1);
    throw runtime_error("Fetching is possible although cursor is out of range");
  } catch (sql::InvalidArgumentException &) {
    cout << "#\t\t OK, fetching not allowed when cursor is out of range..." << endl;
  }
  /* absolute(NUM_ROWS + 10) is internally aligned to NUM_ROWS + 1 = afterLastRow() */
  res->previous();
  validateRow(res, max);
}
