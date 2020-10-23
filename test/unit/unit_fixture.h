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



#ifndef _UNIT_FIXTURE_
#define _UNIT_FIXTURE_

#include <jdbc.h>
#include "../framework/framework.h"
#include <vector>
#include <stdlib.h>
#include <cppconn/connection.h>

#include <boost/scoped_ptr.hpp>

namespace testsuite
{
/* TODO - document */
typedef boost::scoped_ptr<sql::Connection> Connection;
typedef boost::scoped_ptr<sql::PreparedStatement> PreparedStatement;
typedef sql::ParameterMetaData ParameterMetaData;
typedef boost::scoped_ptr<sql::Statement> Statement;
typedef boost::scoped_ptr<sql::Savepoint> Savepoint;
typedef boost::scoped_ptr<sql::ResultSet> ResultSet;
typedef sql::Driver Driver;
typedef sql::ResultSetMetaData ResultSetMetaData;
typedef sql::DatabaseMetaData DatabaseMetaData;

struct columndefinition
{
  std::string name; // TYPE_NAME
  std::string sqldef;
  int ctype; // DATA_TYPE
  std::string value;
  bool is_signed;
  unsigned int precision; // COLUMN_SIZE
  int decimal_digits; // DECIMAL_DIGITS
  bool is_nullable; // IS_NULLABLE
  int nullable; // NULLABLE
  std::string column_def; // COLUMN_DEF
  bool is_case_sensitive;
  std::string remarks; // REMARKS
  int is_searchable;
  bool fixed_prec_scale;
  bool auto_increment;
  std::string local_type_name;
  int minimum_scale;
  int maximum_scale;
  int num_prec_radix; // NUM_PREC_RADIX
  int sql_data_type; // SQL_DATA_TYPE
  int sql_datetime_sub; // SQL_DATA_TYPE_SUB
  unsigned int char_octet_length;
  std::string is_autoincrement;
  bool is_negative;
  std::string as_string; // value as it should be returns by getString()
  bool check_as_string;

  columndefinition(const std::string & _name, const std::string & _sqldef, int _ctype,
                   const std::string & _value, bool _is_signed, unsigned int _precision,
                   int _decimal_digits, bool _is_nullable, std::string _column_def,
                   unsigned int _char_octet_length, const std::string & _is_autoincrement, bool _is_negative) :
  name(_name),
  sqldef(_sqldef),
  ctype(_ctype),
  value(_value),
  is_signed(_is_signed),
  precision(_precision),
  decimal_digits(_decimal_digits),
  is_nullable(_is_nullable),
  nullable((_is_nullable) ? sql::DatabaseMetaData::columnNullable : sql::DatabaseMetaData::columnNoNulls),
  column_def(_column_def),
  is_case_sensitive(false),
  remarks(""),
  is_searchable(sql::DatabaseMetaData::typeSearchable),
  fixed_prec_scale(false),
  auto_increment(false),
  local_type_name(""),
  minimum_scale(0),
  maximum_scale(0),
  num_prec_radix(10),
  sql_data_type(0),
  sql_datetime_sub(0),
  char_octet_length(_char_octet_length),
  is_autoincrement(_is_autoincrement),
  is_negative(_is_negative),
  check_as_string(false)
  {
  }

  columndefinition(const std::string & _name, const std::string & _sqldef, int _ctype,
                   const std::string & _value, bool _is_signed, unsigned int _precision,
                   int _decimal_digits, bool _is_nullable, const std::string & _column_def,
                   int _char_octet_length, const std::string & _is_autoincrement,
                   bool _is_negative, const std::string & _as_string) :
  name(_name),
  sqldef(_sqldef),
  ctype(_ctype),
  value(_value),
  is_signed(_is_signed),
  precision(_precision),
  decimal_digits(_decimal_digits),
  is_nullable(_is_nullable),
  nullable((_is_nullable) ? sql::DatabaseMetaData::columnNullable : sql::DatabaseMetaData::columnNoNulls),
  column_def(_column_def),
  is_case_sensitive(false),
  remarks(""),
  is_searchable(sql::DatabaseMetaData::typeSearchable),
  fixed_prec_scale(false),
  auto_increment(false),
  local_type_name(""),
  minimum_scale(0),
  maximum_scale(0),
  num_prec_radix(10),
  sql_data_type(0),
  sql_datetime_sub(0),
  char_octet_length(_char_octet_length),
  is_autoincrement(_is_autoincrement),
  is_negative(_is_negative),
  as_string(_as_string),
  check_as_string(true)
  {
  }

};


struct udtattribute
{
  std::string name;
  int ctype;

  udtattribute(std::string n, int c) :
  name(n),
  ctype(c)
  {
  }

};


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
   * List of all column types known by MySQL
   *
   */
  std::vector< columndefinition > columns;

  /**
   * List of all columns which getAttribute() should deliver
   *
   */
  std::vector< udtattribute > attributes;


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
   * want to use typedef boost::scoped_ptr<sql::Connection>con(getConnection())
   * or similar for convenience reasons.
   *
   * @throws SQLException &
   */
  sql::Connection * getConnection(sql::ConnectOptionsMap *additional_opts=NULL);

  /**
   * Checks if the passed SQLException is caused by the specified error
   *
   */
  std::string exceptionIsOK(sql::SQLException &e, const std::string& sql_state, int errNo);

  /**
   * Checks if the passed exception has the SQLState HY000 and the (vendor) error code 0
   */
  std::string exceptionIsOK(sql::SQLException &e);

  /**
   * Helper function to check scrolling through a result set
   */
  void checkResultSetScrolling(ResultSet &res);


  /**
   * Helper function to fetch the MySQL Server version as a classical integer in the range from 30000 - 99999
   */
  int getMySQLVersion(Connection &con);

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
  void logDebug(const String & message);

  /**
   * Write (info) message to test protocol
   *
   * @param	message	Message to be included in the protocol
   */
  void logMsg(const String & message);

  /**
   * Write error to test protocol
   *
   * @param	message Message to be included in the protocol
   */
  void logErr(const String & message);

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
  theFixtureClass( const String & /* name */ ) \
  : unit_fixture( #theFixtureClass )

#endif // _UNIT_FIXTURE_
