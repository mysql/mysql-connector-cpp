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
* Example of sql::ResultSetMetaData - meta data of a result set
*
*/

// Standard C++ includes
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

static void printResultSetMetaData(std::unique_ptr< sql::ResultSet > &res, std::unique_ptr< sql::ResultSet > &ps_res);

using namespace std;

/**
* Meta data of a (simple) statements result set - not prepared statements
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

  stringstream sql;
  unsigned int i;
  int ret;

  cout << boolalpha;
  cout << "1..1" << endl;
  cout << "# Connector/C++ resultset metadata..." << endl;

  try {
    /* Using the Driver to create a conection */
    std::unique_ptr< sql::Connection > con(sql::mysql::get_driver_instance()->connect(url, user, pass));

    con->setSchema(database);

    /* Create a test table demonstrating the use of sql::Statement.execute() */
    std::unique_ptr< sql::Statement > stmt(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, label CHAR(1))");
    cout << "#\t Test table created" << endl;

    /* Populate the test table with data */
    for (i = 0; i < EXAMPLE_NUM_TEST_ROWS; i++) {
      // KLUDGE: You should take measures against SQL injections!
      // example.h contains the test data
      sql.str("");
      sql << "INSERT INTO test(id, label) VALUES (";
      sql << test_data[i].id << ", '" << test_data[i].label << "')";
      std::cout << sql.str() << std::endl;
      stmt->execute(sql.str());
    }
    cout << "#\t Test table populated" << endl;

    {
      std::unique_ptr< sql::ResultSet > res(stmt->executeQuery("SELECT id AS column_alias, label FROM test AS table_alias LIMIT 1"));
      std::unique_ptr< sql::PreparedStatement > prep_stmt(con->prepareStatement("SELECT id AS column_alias, label FROM test AS table_alias LIMIT 1"));
      std::unique_ptr< sql::ResultSet > ps_res(prep_stmt->executeQuery());
      cout << "#\t SELECT id AS column_alias, label FROM test AS table_alias LIMIT 1" << endl;
      printResultSetMetaData(res, ps_res);
    }

    {
      std::unique_ptr< sql::ResultSet > res(stmt->executeQuery("SELECT 1.01, 'Hello world!'"));
      std::unique_ptr< sql::PreparedStatement > prep_stmt(con->prepareStatement("SELECT 1.01, 'Hello world!'"));
      std::unique_ptr< sql::ResultSet > ps_res(prep_stmt->executeQuery());
      cout << "#\t SELECT 1.01, 'Hello world!'" << endl;
      printResultSetMetaData(res, ps_res);
    }

    // Disabled due to a bug on server/libmysqlclient (Bug#89016)
    {
//      std::unique_ptr< sql::ResultSet > res(stmt->executeQuery("DESCRIBE test"));
//      std::unique_ptr< sql::PreparedStatement > prep_stmt(con->prepareStatement("DESCRIBE test"));
//      std::unique_ptr< sql::ResultSet > ps_res(prep_stmt->executeQuery());
//      cout << "# \tDESCRIBE test" << endl;
//      printResultSetMetaData(res, ps_res);
    }

    /* Clean up */
    stmt->execute("DROP TABLE IF EXISTS test");
    cout << "# done!" << endl;
    ret = EXIT_SUCCESS;

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

    ret = EXIT_FAILURE;

  } catch (std::runtime_error &e) {
    cout << "# ERR: runtime_error in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what() << endl;

    ret = EXIT_FAILURE;
  }

  if (ret != EXIT_SUCCESS) {
    cout << "not ";
  }
  cout << "ok 1 - examples/resultset_meta.cpp" << endl;
  return ret;
}

/**
* Prints all meta data associated with an result set
*
*/
static void printResultSetMetaData(std::unique_ptr< sql::ResultSet > &res, std::unique_ptr< sql::ResultSet > &ps_res)
{
  /* ResultSetMetaData object */
  sql::ResultSetMetaData * meta;
  sql::ResultSetMetaData * ps_meta;
  unsigned int column;

  if (res->rowsCount() == 0) {
    throw runtime_error("FAILURE - no rows");
  }

  /* Get the meta data - we leave all the exception handling to the caller... */
  meta = res->getMetaData();
  ps_meta = ps_res->getMetaData();

  cout << "#\t Printing result set meta data" << endl;
  cout << "#\t res->rowsCount() = " << res->rowsCount() << endl;
  if (res->rowsCount() != ps_res->rowsCount())
    throw runtime_error("rowsCount() different for PS and non-PS");

  cout << "#\t meta->getColumnCount() = " << meta->getColumnCount() << endl;
  if (meta->getColumnCount() != ps_meta->getColumnCount())
    throw runtime_error("getColumnCount() different for PS and non-PS");

  /*
  Dump information for every column
  NOTE: column indexing is 1-based not zero-based!
  */
  for (column = 1; column <= meta->getColumnCount(); column++) {
    cout << "#\t\t Column " << column << "\t\t\t= " << meta->getColumnName(column);
    cout << "/" << ps_meta->getColumnName(column) << endl;
    if (meta->getColumnName(column) != ps_meta->getColumnName(column))
      throw runtime_error("getColumnName different for PS and non-PS");

    cout << "#\t\t meta->getCatalogName()\t\t= " << meta->getCatalogName(column);
    cout  << "/" << ps_meta->getCatalogName(column) << endl;
    if (meta->getCatalogName(column) != ps_meta->getCatalogName(column))
      throw runtime_error("getCatalogName different for PS and non-PS");

    cout << "#\t\t meta->getColumnDisplaySize() = " << meta->getColumnDisplaySize(column);
    cout  << "/" << ps_meta->getColumnDisplaySize(column) << endl;
    if (meta->getColumnDisplaySize(column) != ps_meta->getColumnDisplaySize(column))
      throw runtime_error("getColumnDisplaySize different for PS and non-PS");

    cout << "#\t\t meta->getColumnLabel()\t\t= " << meta->getColumnLabel(column);
    cout << "/" << ps_meta->getColumnLabel(column) << endl;
    if (meta->getColumnLabel(column) != ps_meta->getColumnLabel(column))
      throw runtime_error("getColumnLabel different for PS and non-PS");

    cout << "#\t\t meta->getColumnName()\t\t= " << meta->getColumnName(column);
    cout << "/" << ps_meta->getColumnName(column) << endl;
    if (meta->getColumnName(column) != ps_meta->getColumnName(column))
      throw runtime_error("getColumnName different for PS and non-PS");

    cout << "#\t\t meta->getColumnType()\t\t= " << meta->getColumnType(column);
    cout << "/" << ps_meta->getColumnType(column) << endl;
    if (meta->getColumnType(column) != ps_meta->getColumnType(column))
      throw runtime_error("getColumnType different for PS and non-PS");

    cout << "#\t\t meta->getColumnTypeName()\t= " << meta->getColumnTypeName(column);
    cout << "/" << ps_meta->getColumnTypeName(column) << endl;
    if (meta->getColumnTypeName(column) != ps_meta->getColumnTypeName(column))
      throw runtime_error("getColumnTypeName different for PS and non-PS");

    cout << "#\t\t meta->getPrecision()\t\t= " << meta->getPrecision(column);
    cout << "/" << ps_meta->getPrecision(column) << endl;

    //TODO: FIX IT - Giving different results
//    if (meta->getPrecision(column) != ps_meta->getPrecision(column))
//      throw runtime_error("getPrecision different for PS and non-PS");

    cout << "#\t\t meta->getScale()\t\t= " << meta->getScale(column);
    cout << "/" << ps_meta->getScale(column) << endl;
    if (meta->getScale(column) != ps_meta->getScale(column))
      throw runtime_error("getScale different for PS and non-PS");

    cout << "#\t\t meta->getSchemaName()\t\t= " << meta->getSchemaName(column);
    cout << "/" << ps_meta->getSchemaName(column) << endl;
    if (meta->getSchemaName(column) != ps_meta->getSchemaName(column))
      throw runtime_error("getSchemaName different for PS and non-PS");

    cout << "#\t\t meta->getTableName()\t\t= " << meta->getTableName(column);
    cout << "/" << ps_meta->getTableName(column) << endl;
    if (meta->getTableName(column) != ps_meta->getTableName(column))
      throw runtime_error("getTableName different for PS and non-PS");

    cout << "#\t\t meta->isAutoIncrement()\t= " << meta->isAutoIncrement(column);
    cout << "/" << ps_meta->isAutoIncrement(column) << endl;
    if (meta->isAutoIncrement(column) != ps_meta->isAutoIncrement(column))
      throw runtime_error("isAutoIncrement different for PS and non-PS");

    cout << "#\t\t meta->isCaseSensitive()\t= " << meta->isCaseSensitive(column);
    cout << "/" << ps_meta->isCaseSensitive(column) << endl;
    if (meta->isCaseSensitive(column) != ps_meta->isCaseSensitive(column))
      throw runtime_error("isCaseSensitive different for PS and non-PS");

    cout << "#\t\t meta->isCurrency()\t\t= " << meta->isCurrency(column);
    cout << "/" << ps_meta->isCurrency(column) << endl;
    if (meta->isCurrency(column) != ps_meta->isCurrency(column))
      throw runtime_error("isCurrency different for PS and non-PS");

    cout << "#\t\t meta->isDefinitelyWritable()\t= " << meta->isDefinitelyWritable(column);
    cout << "/" << ps_meta->isDefinitelyWritable(column) << endl;
    if (meta->isDefinitelyWritable(column) != ps_meta->isDefinitelyWritable(column))
      throw runtime_error("isDefinitelyWritable different for PS and non-PS");

    cout << "#\t\t meta->isNullable()\t\t= " << meta->isNullable(column);
    cout << "/" << ps_meta->isNullable(column) << endl;
    if (meta->isNullable(column) != ps_meta->isNullable(column))
      throw runtime_error("isNullable different for PS and non-PS");

    cout << "#\t\t meta->isReadOnly()\t\t= " << meta->isReadOnly(column);
    cout << "/" << ps_meta->isReadOnly(column) << endl;
    if (meta->isReadOnly(column) != ps_meta->isReadOnly(column))
      throw runtime_error("isReadOnly different for PS and non-PS");

    cout << "#\t\t meta->isSearchable()\t\t= " << meta->isSearchable(column);
    cout << "/" << ps_meta->isSearchable(column) << endl;
    if (meta->isSearchable(column) != ps_meta->isSearchable(column))
      throw runtime_error("isSearchable different for PS and non-PS");

    cout << "#\t\t meta->isSigned()\t\t= " << meta->isSigned(column);
    cout << "/" << ps_meta->isSigned(column) << endl;
    if (meta->isSigned(column) != ps_meta->isSigned(column))
      throw runtime_error("isSigned different for PS and non-PS");

    cout << "#\t\t meta->isWritable()\t\t= " << meta->isWritable(column);
    cout << "/" << ps_meta->isWritable(column) << endl;
    if (meta->isWritable(column) != ps_meta->isWritable(column))
      throw runtime_error("isWritable different for PS and non-PS");
  }

  try {
    res->close();
    meta = res->getMetaData();
  } catch (sql::SQLException &e) {
    cout << "#\t Excepted exception (I) when trying to get meta data after close:" << endl;
    cout << "#\t " << e.what() << endl;
  }

  try {
    ps_res->close();
    ps_meta = ps_res->getMetaData();
  } catch (sql::SQLException &e) {
    cout << "#\t Excepted exception (II) when trying to get meta data after close:" << endl;
    cout << "#\t " << e.what() << endl;
  }
}
