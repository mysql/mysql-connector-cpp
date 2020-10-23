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
* Basic example demonstrating connect and simple queries
*
*/


/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <list>

/* Public interface of the MySQL Connector/C++ */
#include <jdbc.h>
/* Connection parameter and sample data */
#include "examples.h"

using namespace std;

/**
* Usage example for Driver, Connection, (simple) Statement, ResultSet
*/
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



  static std::list< sql::SQLString > table_types;
  table_types.push_back("TABLE");

  unsigned int column, row;
  string ddl;

  cout << boolalpha;
  cout << "1..1" << endl;
  cout << "# Connector/C++ connection meta data example.." << endl;
  cout << "#" << endl;


  try {
    /* Using the Driver to create a connection */
    sql::Driver * driver = sql::mysql::get_driver_instance();
    cout << "# " << driver->getName() << ", version ";
    cout << driver->getMajorVersion() << "." << driver->getMinorVersion();
                cout << "." << driver->getPatchVersion() << endl;

    std::unique_ptr< sql::Connection > con(driver->connect(url, user, pass));
    sql::DatabaseMetaData * con_meta = con->getMetaData();

    cout << "# CDBC (API) major version = " << con_meta->getCDBCMajorVersion() << endl;
    if (con_meta->getCDBCMajorVersion() <= 0) {
      throw runtime_error("API major version must not be 0");
    }
    cout << "# CDBC (API) minor version = " << con_meta->getCDBCMinorVersion() << endl;

    {
      /*
      Which schemata/databases exist?
      NOTE Connector C++ defines catalog = n/a, schema = MySQL database
      */
      cout << "# List of available schemata/databases: ";
      std::unique_ptr< sql::ResultSet > res(con_meta->getSchemas());

      /* just for fun... of course you can scroll and fetch in whatever order you want */
      res->afterLast();
      while (res->previous()) {
        cout << res->getString("TABLE_SCHEM");
        if (!res->isFirst()) {
          cout << ", ";
        }
      }
      cout << endl;
      if (!res->isBeforeFirst() || res->isFirst())
        throw runtime_error("Cursor should be positioned before first row");
    }

    {
      /* What object types does getSchemaObjects support? */
      cout << "# Supported Object types: ";
      std::unique_ptr< sql::ResultSet > res(con_meta->getSchemaObjectTypes());
      while (res->next()) {
        cout << res->getString(1);
        if (!res->isLast()) {
          cout << ", ";
        }
      }
      cout << endl;
      if (!res->isAfterLast() || res->isLast())
        throw runtime_error("Cursor should be positioned after last row");
    }


    std::unique_ptr< sql::Statement > stmt(con->createStatement());
    stmt->execute("USE " + database);
    stmt->execute("DROP TABLE IF EXISTS test1");
    stmt->execute("CREATE TABLE test1(id INT, label CHAR(1))");
    stmt->execute("DROP TABLE IF EXISTS test2");
    stmt->execute("CREATE TABLE test2(id INT, label CHAR(1))");

    /* "" = empty string requests all types of objects */
    std::unique_ptr< sql::ResultSet > res(con_meta->getSchemaObjects(con->getCatalog(), con->getSchema(), ""));
    row = 1;
    sql::ResultSetMetaData * res_meta = res->getMetaData();
    while (res->next()) {
      if (row++ > 2)
        break;

      cout << "#\t Object " << res->getRow() << " (" << res->getString("OBJECT_TYPE") << ")" << endl;
      if (res->getString(1) != res->getString("OBJECT_TYPE")) {
        throw runtime_error("Fetch using numeric/non-numeric index failed for OBJECT_TYPE");
      }
      if (res->getString("OBJECT_TYPE").length() == 0) {
        throw runtime_error("OBJECT_TYPE must not be empty");
      }

      cout << "#\t Catalog = " << res->getString("CATALOG");
      cout << " (empty: " << (res->getString("CATALOG").length() == 0) << ")" << endl;
      if (res->getString(2) != res->getString("CATALOG")) {
        throw runtime_error("Fetch using numeric/non-numeric index failed for CATALOG");
      }

      cout << "#\t Schema = " << res->getString("SCHEMA");
      cout << " (empty: " << (res->getString("SCHEMA").length() == 0) << ")" << endl;
      if (res->getString(3) != res->getString("SCHEMA")) {
        throw runtime_error("Fetch using numeric/non-numeric index failed for SCHEMA");
      }

      cout << "#\t Name = " << res->getString("NAME");
      cout << " (empty: " << (res->getString("NAME").length() == 0) << ")" << endl;
      if (res->getString(4) != res->getString("NAME")) {
        throw runtime_error("Fetch using numeric/non-numeric index failed for NAME");
      }

      ddl = res->getString("DDL");
      cout << "#\t DDL = " << setw(40);
      cout << ddl.substr(0, ddl.find_first_of("\n", 1) - 1) << "..." << endl;
      cout << "#\t DDL is empty: " << ddl.empty() << endl;
      if (res->getString(5) != res->getString("DDL")) {
        throw runtime_error("Fetch using numeric/non-numeric index failed for DDL");
      }

    }
    cout << "#" << endl;
    cout << "#\t\t Using different getter methods at the example of the DDL column" << endl;
    cout << "#\t\t Column DDL is of type " << res_meta->getColumnTypeName(5);
    cout << " / Code: " << res_meta->getColumnType(5) << endl;
    /* scroll back to last row in set */
    res->previous();
    cout << "#\t\t DDL (as String) = " << setw(30) << ddl.substr(0, ddl.find_first_of("\n", 1) - 1) << "..." << endl;
    cout << "#\t\t DDL (as Boolean) = " << res->getBoolean("DDL") << "/" << res->getBoolean(5) << endl;
    cout << "#\t\t DDL (as Double) = " << res->getDouble("DDL") << "/" << res->getDouble(5) << endl;
    cout << "#\t\t DDL (as Int) = " << res->getInt("DDL") << "/" << res->getInt(5) << endl;
    cout << "#\t\t DDL (as Long) = " << res->getInt64("DDL") << "/" << res->getInt64(5) << endl;
    cout << "#" << endl;

    cout << "#\t\t Meta data on the result set at the example of the DDL column" << endl;
    cout << "#\t\t Column count = " << res_meta->getColumnCount() << " (Columns: ";
    for (column = 1; column <= res_meta->getColumnCount(); column++) {
      cout << res_meta->getColumnName(column);
      if (column < res_meta->getColumnCount()) {
        cout << ", ";
      }
    }
    cout << ")" << endl;

    cout << "#\t\t getCatalogName() = " << res_meta->getCatalogName(5) << endl;
    /* cout << "#\t\t getColumnDisplaySize() = " << res_meta->getDisplaySize("DDL") << endl; */
    cout << "#\t\t getColumnLabel() = " << res_meta->getColumnLabel(5) << endl;
    cout << "#\t\t getColumnName() = " << res_meta->getColumnName(5) << endl;
    if (res_meta->getColumnName(5) != res_meta->getColumnLabel(5)) {
      throw runtime_error("column names differ for column DDL");
    }

    cout << "#\t\t getColumnType() = " << res_meta->getColumnType(5) << endl;
    cout << "#\t\t getColumnTypeName() " << res_meta->getColumnTypeName(5) << endl;
    /* cout << "#\t\t getPrecision() = " << res_meta->getPrecision(5) << endl; */
    cout << "#\t\t getSchemaName() = " << res_meta->getSchemaName(5) << endl;
    cout << "#\t\t getTableName() = " << res_meta->getTableName(5) << endl;
    cout << "#\t\t isAutoIncrement() = " << res_meta->isAutoIncrement(5) << endl;
    cout << "#\t\t isCaseSensitive() = " << res_meta->isCaseSensitive(5) << endl;
    cout << "#\t\t isCurrency() = " << res_meta->isCurrency(5) << endl;
    cout << "#\t\t isDefinitelyWritable() = " << res_meta->isDefinitelyWritable(5) << endl;
    cout << "#\t\t isNullable() = " << res_meta->isNullable(5) << endl;
    cout << "#\t\t isReadOnly() = " << res_meta->isReadOnly(5) << endl;
    cout << "#\t\t isSearchable() = " << res_meta->isSearchable(5) << endl;
    cout << "#\t\t isSigned() = " << res_meta->isSigned(5) << endl;
    cout << "#\t\t isWritable() = " << res_meta->isWritable(5) << endl;

    std::unique_ptr< sql::ResultSet > res_tables(con_meta->getTables(con->getCatalog(), database, "t%", table_types));
    sql::ResultSetMetaData * res_meta_tables = res_tables->getMetaData();

    cout << "#" << endl;
    cout << "# Tables with names like 't%':" << endl;;
    cout << "#\t rowsCount() = " << res_tables->rowsCount() << endl;
    cout << "#\t getColumnCount() = " << res_meta_tables->getColumnCount() << endl;
    cout << "#\t" << endl;
    while (res_tables->next()) {
      cout << "#\t ";
      for (column = 1; column < res_meta_tables->getColumnCount(); column++) {
        cout << "'" << res_tables->getString(column) << "' ";
      }
      cout <<  endl;
    }
    cout << "#" << endl;

      res_tables->first();
    std::unique_ptr< sql::ResultSet > res_columns(con_meta->getColumns(con->getCatalog(), database, "test1", "%"));

    cout << "#" << "Columns in the table 'test1'..." << endl;
    cout << "#\t rowsCount() = " << res_columns->rowsCount() << endl;
    if (res_columns->rowsCount() != 2)
      throw runtime_error("Table test1 has two columns, no more!");

    while (res_columns->next()) {
      cout << "#\t Column name = '" << res_columns->getString("COLUMN_NAME") << "'" << endl;
    }
    cout << "#" << endl;

    cout << "# done!" << endl;
    cout << "ok 1 - examples/connection_meta_schemaobj.cpp" << endl;

  } catch (sql::SQLException &e) {
    /*
    The MySQL Connector/C++ throws three different exceptions:

    - sql::MethodNotImplementedException (derived from sql::SQLException)
    - sql::InvalidArgumentException (derived from sql::SQLException)
    - sql::SQLException (derived from std::runtime_error)
    */
    cout << "# ERR: SQLException in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    /* Use what() (derived from std::runtime_error) */
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    cout << "not ok 1 - examples/connection_meta_schemaobj.cpp" << endl;

    return EXIT_FAILURE;
  } catch (std::runtime_error &e) {

    cout << "# ERR: runtime_error in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what() << endl;
    cout << "not ok 1 - examples/connection_meta_schemaobj.cpp" << endl;

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
