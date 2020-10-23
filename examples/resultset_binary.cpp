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
* Handling binary data using getString()
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
  stringstream sql;
  int i;
  struct _test_data min, max;

  cout << boolalpha;
  cout << "1..1" << endl;
  cout << "# Connector/C++ result set.." << endl;

  try {
    /* Using the Driver to create a connection */
    driver = sql::mysql::get_driver_instance();
    std::unique_ptr< sql::Connection > con(driver->connect(url, user, pass));

    con->setSchema(database);

    /* Creating a "simple" statement - "simple" = not a prepared statement */
    std::unique_ptr< sql::Statement > stmt(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT, label CHAR(1), col_binary BINARY(4), col_varbinary VARBINARY(10))");
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
      sql << "INSERT INTO test(id, label, col_binary, col_varbinary) VALUES (";
      sql << test_data[i].id << ", '" << test_data[i].label << "', ";
      sql << "\"a\\0b\", '" << i * 5 << "\\0abc')";
      stmt->execute(sql.str());
    }
    cout << "#\t Test table populated" << endl;

    cout << "#\t Testing sql::Statement based resultset" << endl;
    {
      std::unique_ptr< sql::ResultSet > res(stmt->executeQuery("SELECT * FROM test ORDER BY id ASC"));
      int row = 0;
      while (res->next()) {
        cout << "#\t\t Row " << row << ", getRow() " << res->getRow();
        cout << " id = " << res->getInt("id");
        cout << ", label = '" << res->getString("label") << "'";
        /* cout might "hide" \0 and other special characters from the output */
        cout << ", col_binary = '" << res->getString("col_binary") << "'";
        cout << ", col_varbinary = '" << res->getString("col_varbinary") << "'" << endl;

        /* fixed length column - length = size of the column! */
        if (res->getString("col_binary").length() != 4) {
          throw runtime_error("BINARY(n) should return std::string of length n regardless how long the value stored in the column is.");
        }

        if (res->getString("col_binary").compare(0, 1, "a")) {
          throw runtime_error("First sign from BINARY(n) seems wrong");
        }

        if (res->getString("col_binary").compare(2, 1, "b")) {
          throw runtime_error("Third sign from BINARY(n) seems wrong");
        }

        if (res->getString("col_varbinary").length() != 5 && res->getString("col_varbinary").length() != 6) {
          throw runtime_error("VARBINARY(n) should return std::string of length n which holds the length of the actual column value.");
        }
        sql::ResultSetMetaData * meta = res->getMetaData();
        cout << "#\t\t COLUMN_SIZE = " << meta->getColumnDisplaySize(3) << endl;
        row++;
      }
    }

    /* Clean up */
    /* stmt->execute("DROP TABLE IF EXISTS test");*/
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
    cout << "not ok 1 - examples/resultset_binary.cpp" << endl;

    return EXIT_FAILURE;
  } catch (std::runtime_error &e) {

    cout << "# ERR: runtime_error in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what() << endl;
    cout << "not ok 1 - examples/resultset_binary.cpp" << endl;

    return EXIT_FAILURE;
  }

  cout << "ok 1 - examples/resultset_binary.cpp" << endl;
  return EXIT_SUCCESS;
}
