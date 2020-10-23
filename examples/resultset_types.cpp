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
* Basic example demonstrating how to check the type of a result set column
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

#ifdef _WIN32
#define L64(x) x##i64
#else
#define L64(x) x##LL
#endif

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

  struct _test_data min, max;
  int c_int1;
  bool c_bool1 = true, c_bool2;
  /* TODO: long long is not C++, its C99 !!! */
  int64_t c_long1 = L64(9223372036854775807), c_long2;
  double c_double1 = -999.9999, c_double2;

  cout << boolalpha;
  cout << "1..1" << endl;
  cout << "# Connector/C++ result set.." << endl;

  try {
    /* Using the Driver to create a connection */
    driver = sql::mysql::get_driver_instance();
    std::unique_ptr< sql::Connection > con(driver->connect(url, user, pass));
    con->setSchema(database);

    std::unique_ptr< sql::Statement > stmt(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");

    /*
    Note that MySQL has its very own mapping from SQL type (e.g. BOOLEAN)
    specified in a SQL statement and type actually used. Check the MySQL
    manual - conversions are a common cause of false bug reports!
    Also, don't get confused by the precision of float/double columns.
    For precision math use DECIMAL!
    */
    stmt->execute("CREATE TABLE test(id INT, label CHAR(1), c_bool BOOLEAN, "
      "c_long BIGINT, c_double DOUBLE, c_null INT DEFAULT NULL)");
    cout << "#\t Test table created" << endl;

    std::unique_ptr< sql::PreparedStatement> prep_stmt(
      con->prepareStatement("INSERT INTO test(id, label, c_bool, c_long, "
        " c_double) VALUES (?, ?, ?, ?, ?)"));

    /* Populate the test table with data */
    min = max = test_data[0];
    for (unsigned int i = 0; i < EXAMPLE_NUM_TEST_ROWS; i++) {
      /* Remember min/max for further testing */
      if (test_data[i].id < min.id) {
        min = test_data[i];
      }
      if (test_data[i].id > max.id) {
        max = test_data[i];
      }

      prep_stmt->setInt(1, test_data[i].id);
      prep_stmt->setString(2, test_data[i].label);
      prep_stmt->setBoolean(3, c_bool1);
      prep_stmt->setInt64(4, c_long1);
      prep_stmt->setDouble(5, c_double1);
      prep_stmt->execute();
    }
    cout << "#\t Test table populated" << endl;

    std::unique_ptr< sql::ResultSet > res(stmt->executeQuery("SELECT id, label, c_bool, c_long, c_double, c_null FROM test ORDER BY id ASC"));
    while (res->next()) {
      /* sql::ResultSet.rowsCount() returns size_t */
      size_t row = res->getRow() - 1;

      cout << "#\t\t Row " << res->getRow() << endl;
      cout << "#\t\t\t id INT = " << res->getInt("id") << endl;
      cout << "#\t\t\t id (as Integer) = " << res->getInt("id") << endl;
      cout << "#\t\t\t id (as String) = " << res->getString("id") << endl;
      cout << "#\t\t\t id (as Boolean) = " << res->getBoolean("id") << endl;
      cout << "#\t\t\t id (as Long) = " << res->getInt64("id") << endl;
      cout << "#\t\t\t id (as Double) = " << res->getDouble("id") << endl;
      cout << "#" << endl;
      if (test_data[row].id != res->getInt(1)) {
        throw runtime_error("Wrong results for column id");
      }

      std::string c_string = res->getString(2);
      cout << "#\t\t\t label CHAR(1) = " << c_string << endl;
      cout << "#\t\t\t label (as Integer) = " << res->getInt(2) << endl;
      cout << "#\t\t\t label (as String) = " << res->getString(2) << endl;
      cout << "#\t\t\t label (as Boolean) = " << res->getBoolean(2) << endl;
      cout << "#\t\t\t label (as Long) = " << res->getInt64(2) << endl;
      cout << "#\t\t\t label (as Double) = " << res->getDouble(2) << endl;
      cout << "#" << endl;
      if (test_data[row].label != c_string) {
        throw runtime_error("Wrong result for column label");
      }

      c_bool2 = res->getBoolean("c_bool");
      cout << "#\t\t\t c_bool CHAR(1) = " << c_bool2 << endl;
      cout << "#\t\t\t c_bool (as Integer) = " << res->getInt(3) << endl;
      cout << "#\t\t\t c_bool (as String) = " << res->getString(3) << endl;
      cout << "#\t\t\t c_bool (as Boolean) = " << res->getBoolean(3) << endl;
      cout << "#\t\t\t c_bool (as Long) = " << res->getInt64(3) << endl;
      cout << "#\t\t\t c_bool (as Double) = " << res->getDouble(3) << endl;
      cout << "#" << endl;
      if (c_bool1 != c_bool2) {
        throw runtime_error("Wrong result for column c_bool");
      }

      c_long2 = res->getInt64("c_long");
      cout << "#\t\t\t c_long BIGINT = " << c_long2 << endl;
      cout << "#\t\t\t c_long (as Integer) = " << res->getInt("c_long") << endl;
      cout << "#\t\t\t c_long (as String) = " << res->getString("c_long") << endl;
      cout << "#\t\t\t c_long (as Boolean) = " << res->getBoolean("c_long") << endl;
      cout << "#\t\t\t c_long (as Long) = " << res->getInt64("c_long") << endl;
      cout << "#\t\t\t c_long (as Double) = " << res->getDouble("c_long") << endl;
      cout << "#" << endl;
      if (c_long1 != c_long2) {
        throw runtime_error("Wrong result for column c_long");
      }

      c_double2 = res->getDouble("c_double");
      cout << "#\t\t\t c_double DOUBLE = " << c_double2 << endl;
      cout << "#\t\t\t c_double (as Integer) = " << res->getInt("c_double") << endl;
      cout << "#\t\t\t c_double (as String) = " << res->getString("c_double") << endl;
      cout << "#\t\t\t c_double (as Boolean) = " << res->getBoolean("c_double") << endl;
      cout << "#\t\t\t c_double (as Long) = " << res->getInt64("c_double") << endl;
      cout << "#\t\t\t c_double (as Double) = " << res->getDouble("c_double") << endl;
      cout << "#\t\t\t c_double wasNull() = " << res->wasNull() << endl;
      cout << "#" << endl;
      if (c_double1 != c_double2) {
        throw runtime_error("Wrong result for column c_double");
      }

      c_int1 = res->getInt("c_null");
      cout << "#\t\t\t c_null INT DEFAULT NULL = " << c_int1;
      cout << " (isNull = " << res->isNull("c_null") << ")" << endl;
      cout << "#\t\t\t c_null (as Integer) = " << res->getInt("c_null") << endl;
      cout << "#\t\t\t c_null (as String) = " << res->getString("c_null") << endl;
      cout << "#\t\t\t c_null (as Boolean) = " << res->getBoolean("c_null") << endl;
      cout << "#\t\t\t c_null (as Long) = " << res->getInt64("c_null") << endl;
      cout << "#\t\t\t c_null (as Double) = " << res->getDouble("c_null") << endl;
      cout << "#\t\t\t c_null wasNull() = " << res->wasNull() << endl;
      cout << "#" << endl;
      if (!res->isNull(6) || !res->wasNull()) {
        throw runtime_error("isNull() or wasNull() has not reported NULL value of column c_null");
      }

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
    /* Use what(), getErrorCode() and getSQLState() */
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    cout << "not ok 1 - examples/resultset_types.cpp" << endl;

    return EXIT_FAILURE;
  } catch (std::runtime_error &e) {

    cout << "# ERR: runtime_error in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what() << endl;
    cout << "not ok 1 - examples/resultset_types.cpp" << endl;

    return EXIT_FAILURE;
  }

  cout << "ok 1 - examples/resultset_types.cpp" << endl;
  return EXIT_SUCCESS;
}
