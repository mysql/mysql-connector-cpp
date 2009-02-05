/*
Copyright 2008 - 2009 Sun Microsystems, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

There are special exceptions to the terms and conditions of the GPL
as it is applied to this software. View the full text of the
exception in file EXCEPTIONS-conECTOR-C++ in the directory of this
software distribution.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _UNIT_FIXTURE_
#define _UNIT_FIXTURE_

#include <driver/mysql_public_iface.h>
#include "../CJUnitTestsPort/framework/framework.h"

namespace testsuite
{
/* TODO - document */
typedef std::auto_ptr<sql::Connection> Connection;
typedef std::auto_ptr<sql::PreparedStatement> PreparedStatement;
typedef std::auto_ptr<sql::Statement> Statement;
typedef std::auto_ptr<sql::ResultSet> ResultSet;
typedef sql::Driver Driver;
typedef std::auto_ptr<sql::ResultSetMetaData> ResultSetMetaData;
typedef std::auto_ptr<sql::DatabaseMetaData> DatabaseMetaData;

class unit_fixture : public TestSuite
{
private:
  typedef TestSuite super;

protected:
  /**
   * List of SQL schema objects created during the test run
   *
   * Used by tearDown() to clean up the database after the test run
   */
  List created_objects;

  /**
   * Driver to be used
   *
   * The element is managed by getConnection(). You do not need
   * to worry about the creation of a connection or a driver object,
   * use getConnection() to obtain a connection object.
   */
  static Driver *driver;

  /**
   * Copies command line connection parameter into protected slots
   *
   */
  void init();

  /**
   * Effective database connection URL
   *
   * URL refers to the conector/C++ connection URL syntax
   *
   * Fallback to default_url, see also default_url
   */
  String url;

  /**
   * Name of the schema to use (USE <db>)
   *
   * The schema will be selected during setUp() before running a test
   * Fallback to default_db, see also default_db
   */
  String db;

  /**
   * Database user for getConnection()
   *
   * Fallback to default_user, see also default_user
   */
  String user;

  /**
   * Database user password
   *
   * Fallback to default_password, see also default_password
   */
  String passwd;

  /**
   * Database connection, initiated during setUp() and cleaned up in tearDown()
   *
   */
  Connection con;

  /**
   * PreparedStatement to be used in tests, not initialized. Cleaned up in tearDown()
   */
  PreparedStatement pstmt;

  /**
   * Statement to be used in tests, not initialized. Cleaned up in tearDown()
   */
  Statement stmt;

  /**
   * ResultSet to be used in tests, not initialized. Cleaned up in tearDown()
   */
  ResultSet res;

  /**
   * Creates a SQL schema object
   *
   * Use this method to create arbitrary SQL object if you want the test
   * framework to clean up SQL schema objects for you at the end of the test
   * in tearDown(). The framework will register all objects created with any
   * of the createXYZ() methods and call the appropriate dropXYZ() method during
   * tearDown().
   *
   * Although this method is protected and available in the test, you should
   * prefer calling any of the specialized wrappers, for example createTable().
   *
   * @param	object_type							SQL type, e.g. TABLE, FUNCTION, ...
   * @param object_name							SQL element name, e.g. "table1"
   * @param	columns_and_other_stuff Additional SQL definitions
   * @throws SQLException &
   */

  void createSchemaObject(String object_type, String object_name,
                          String columns_and_other_stuff);

  /**
   * Drops a SQL schema object
   *
   * All SQL objects created by createSchemaObject() will be implicitly
   * dropped during tearDown().
   *
   * @param object_type			SQL type e.g. TABLE, FUNCTION, ...
   * @param object_name			SQL element name, e.g. "table1"
   * @throws SQLException &
   */
  void dropSchemaObject(String object_type, String object_name);

  /**
   * Create a SQL table of the given name and with the specified SQL definition
   *
   * @param table_name							SQL table name
   * @param	columns_and_other_stuff Additional SQL definitions
   * @throws SQLException &
   */
  void createTable(String table_name, String columns_and_other_stuff);

  /**
   * Drops a SQL table
   *
   * Note: all SQL tables created by createTable() will implicitly dropped
   * during tearDown().
   *
   * @param	table_name	SQL table name
   * @throws SQLException &
   */
  void dropTable(String table_name);

  /**
   * Returns a Connector/C++ connection object
   *
   * Note: you are responsible for handling the object, you might
   * want to use typedef std::auto_ptr<sql::Connection>con(getConnection())
   * or similar for convenience reasons.
   *
   * @throws SQLException &
   */
  sql::Connection * getConnection();

  /**
   * Checks if the passed SQLException is caused by the specified error
   *
   */
  std::string exceptionIsOK(sql::SQLException &e, const std::string& sql_state, int errno);

  /**
   * Checks if the passed exception has the SQLState HY000 and the (vendor) error code 0
   */
  std::string exceptionIsOK(sql::SQLException &e);

public:

  /**
   * Creates a new BaseTestCase object.
   *
   * @param name The name of the unit test case
   */
  unit_fixture(const String & name);

  /**
   * Writes a message to the debug log
   *
   * @param message
   */
  void logDebug(const String message);

  /**
   * Write (info) message to test protocol
   *
   * @param	message	Message to be included in the protocol
   */
  void logMsg(const String message);

  /**
   * Write error to test protocol
   *
   * @param	message Message to be included in the protocol
   */
  void logErr(const String message);

  /**
   * Creates resources used by all tests.
   *
   * Opens a connection and stores the object in this->con.
   * All other members (this->stmt, this->pstmt, this->res) are uninitialized!
   *
   * @throws SQLException &
   */
  virtual void setUp();

  /**
   * Destroys resources created during the test case.
   *
   * @throws SQLException &
   */
  virtual void tearDown();

};

} /* namespace testsuite */

#define EXAMPLE_TEST_FIXTURE( theFixtureClass ) typedef theFixtureClass TestSuiteClass;\
  theFixtureClass( const String & name ) \
  : unit_fixture( #theFixtureClass )

#endif // _UNIT_FIXTURE_
