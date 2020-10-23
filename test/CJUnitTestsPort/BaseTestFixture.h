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




#ifndef _BASE_TEST_FIXTURE_
#define _BASE_TEST_FIXTURE_

#include <math.h>
#include <boost/scoped_ptr.hpp>

#include "resources.h"
#include <jdbc.h>

#include "../framework/framework.h"

#include "../common/ccppTypes.h"
#include "../common/stringutils.h"

#define MESSAGE(msg)  TestsListener::messagesLog() << msg << std::endl;


/**
 * Base class for all test cases. Creates connections, statements, etc. and
 * closes them.
 *
 * @author Mark Matthews
 * @version $Id: BaseTestCase.java 5440 2006-06-27 17:00:53 +0000 (Tue, 27 Jun
 *          2006) mmatthews $
 */

namespace testsuite
{
typedef boost::scoped_ptr<sql::Connection> Connection;
typedef boost::scoped_ptr<sql::PreparedStatement> PreparedStatement;
typedef boost::scoped_ptr<sql::Statement> Statement;
typedef boost::scoped_ptr<sql::ResultSet> ResultSet;
typedef sql::Driver Driver;
typedef sql::ResultSetMetaData * ResultSetMetaData;
typedef sql::DatabaseMetaData * DatabaseMetaData;


class value_object
{
private:
  String  asString;
  bool    wasNull;

public:

  enum value_type
  {
    vtDouble=0, vtFloat, vtByte, vtLast
  };

  value_object();
  value_object(const sql::ResultSet *, int colNum);

  bool isNull() const;

  String toString() const
  {
    return asString;
  }

  int       intValue() const;
  float     floatValue() const;
  long long longValue() const;

  /**
  Based on correspondent methods implementation in the mysql_resultset
      class
   */
  double doubleValue() const
  {
    return floatValue();
  }

  bool booleanValue() const
  {
    return intValue() != 0;
  }

  //Little hack
  bool instanceof(value_type type);
};

typedef boost::scoped_ptr<value_object> Object;

value_object * getObject(sql::ResultSet * rs, int colNum);

// TODO: Move everything from TestFixtureCommon to BaseTestFixture

struct TestFixtureCommon
{
  TestFixtureCommon();

  String extractVal(const String & sTableName
                    , int count
                    , Properties & sqlProps
                    , Connection & conn);

  static void logMsg(String message);

  void logErr(String message);

  static String randomString();

protected:

  void init();

  static const String NO_MULTI_HOST_PROPERTY_NAME;

  String host;
  String port;
  String login;
  String passwd;
  String db;

  static int instanceCount;

  static int propsLoaded;
  static Properties sqlProps;
  static Driver *driver;
};

/************************************************************************/
/*                                                                      */

/************************************************************************/

class BaseTestFixture : public TestSuite, public TestFixtureCommon
{
  typedef TestSuite super;

  List createdObjects;

  /** My instance number */
  int myInstanceNumber;

protected:

  /** Connection to server, initialized in setUp() Cleaned up in tearDown(). */

  Connection conn;

  /** The driver to use */

  String dbClass;

  /**
   * PreparedStatement to be used in tests, not initialized. Cleaned up in
   * tearDown().
   */
  PreparedStatement pstmt;

  /**
   * Statement to be used in tests, initialized in setUp(). Cleaned up in
   * tearDown().
   */
  Statement stmt;

  /**
   * ResultSet to be used in tests, not initialized. Cleaned up in tearDown().
   */
  ResultSet rs;

  bool hasSps;

  enum standard_tables {
    TABLE_CTSTABLE1 = 0,
    TABLE_CTSTABLE2,
    TABLE_INTEGERTAB,
    TABLE_BITTAB,
    TABLE_DOUBLETAB,
    TABLE_BIGINTTAB,
    TABLE_CHARTAB,
    TABLE_VARCHARTAB,
    TABLE_FLOATTAB,
    TABLE_SMALLINTTAB,
    TABLE_LONGVARCHARNULLTAB
  };

  /**
   * Standard SQL tables shared by many tests
   */
  void createStandardTable(standard_tables table);

  /* throws SQLException */

  void createSchemaObject(String objectType, String objectName,
                          String columnsAndOtherStuff);


  /* throws SQLException */

  void createFunction(String functionName, String functionDefn);


  /* throws SQLException */

  void dropFunction(String functionName);


  /* throws SQLException */

  void createProcedure(String procedureName, String procedureDefn);


  /* throws SQLException */

  void dropProcedure(String procedureName);


  /* throws SQLException */

  void createTable(String tableName, String columnsAndOtherStuff);


  /* throws SQLException */

  void dropTable(String tableName);


  /* throws SQLException */
  void dropSchemaObject(String objectType, String objectName);

  sql::Connection * getConnection();

  /* throws SQLException */
  sql::Connection * getAdminConnection();


  /* throws SQLException */
  sql::Connection * getAdminConnectionWithProps(Properties props);


  /* throws SQLException */
  sql::Connection * getConnectionWithProps(const String & propsList);


  /* throws SQLException */
  sql::Connection * getConnectionWithProps(const String & url, const String & propsList);

  /**
   * Returns a new connection with the given properties
   *
   * @param props
   *            the properties to use (the URL will come from the standard for
   *            this testcase).
   *
   * @return a new connection using the given properties.
   *
   * @throws SQLException
   *             DOCUMENT ME!
   */

  /* throws SQLException */

  sql::Connection * getConnectionWithProps(const Properties & props);


  /* throws SQLException */

  sql::Connection * getConnectionWithProps(const String & url, const Properties & props);
  /**
   * Returns the per-instance counter (for messages when multi-threading
   * stress tests)
   *
   * @return int the instance number
   */

  int getInstanceNumber();


  /* throws SQLException */

  String getMysqlVariable(Connection & c, const String & variableName);
  /**
   * Returns the named MySQL variable from the currently connected server.
   *
   * @param variableName
   *            the name of the variable to return
   *
   * @return the value of the given variable, or NULL if it doesn't exist
   *
   * @throws SQLException
   *             if an error occurs
   */

  /* throws SQLException */

  String getMysqlVariable(const String & variableName);
  /**
   * Returns the properties that represent the default URL used for
   * connections for all testcases.
   *
   * @return properties parsed from com.mysql.jdbc.testsuite.url
   *
   * @throws SQLException
   *             if parsing fails
   */

  /* throws SQLException */

  /*Properties getPropertiesFromTestsuiteUrl() ;*/


  /* throws SQLException */

  int getRowCount(const String & tableName);


  /* throws SQLException */

  value_object getSingleIndexedValueWithQuery(Connection & c,
                                              int columnIndex, const String & query);


  /* throws SQLException */

  value_object getSingleIndexedValueWithQuery(int columnIndex,
                                              const String & query);


  /* throws SQLException */
  value_object getSingleValue(const String & tableName, const String & columnName,
                              const String & whereClause);


  /* throws SQLException */
  value_object getSingleValueWithQuery(const String & query);

  bool isAdminConnectionConfigured();


  /* throws SQLException */

  bool isServerRunningOnWindows();


  /* throws IOException */
  /*File newTempBinaryFile(String name, long size) ;*/


  bool runLongTests();
  /**
   * Checks whether a certain system property is defined, in order to
   * run/not-run certain tests
   *
   * @param propName
   *            the property name to check for
   *
   * @return true if the property is defined.
   */

  bool runTestIfSysPropDefined(const String & propName);

  bool runMultiHostTests();


  /**
   * Checks whether the database we're connected to meets the given version
   * minimum
   *
   * @param major
   *            the major version to meet
   * @param minor
   *            the minor version to meet
   *
   * @return boolean if the major/minor is met
   *
   * @throws SQLException
   *             if an error occurs.
   */
  /* throws SQLException */
  bool versionMeetsMinimum(unsigned int major
                           , unsigned int minor
                           , unsigned int subminor= 0 );

  /*bool          isClassAvailable            (String classname);*/

  /* See comments in cpp file*/
  /*void cleanupTempFiles(const File exampleTempFile, const String tempfilePrefix) ;*/


  /* throws Exception */

  void assertResultSetsEqual(ResultSet & control, ResultSet & test);

  void initTable(const String & sTableName, Properties & sqlProps
                 , Connection & conn);
  void clearTable(const String & sTableName, Connection & conn);

public:

  /**
   * Creates a new BaseTestCase object.
   *
   * @param name
   *            The name of the JUnit test case
   */

  BaseTestFixture (const String & name);

  void logDebug   (const String & message);


  void selectDb   ( Statement & st );


  /**
   * Creates resources used by all tests.
   *
   * @throws Exception
   *             if an error occurs.
   */
  /* throws Exception */
  virtual void setUp();


  /**
   * Destroys resources created during the test case.
   *
   * @throws Exception
   *             DOCUMENT ME!
   */

  /* throws Exception */
  virtual void tearDown();
};
}

/* Macros to use instead of one of junit assertEquals calls */
#define MyAssertEquals(str, a, b) ASSERT_MESSAGE(a==b, str)

// Redefining TEST_FIXTURE
#ifdef TEST_FIXTURE
#undef TEST_FIXTURE
#endif

#define TEST_FIXTURE( theFixtureClass ) typedef theFixtureClass TestSuiteClass;\
  theFixtureClass( const String & name ) \
  : BaseTestFixture( name )

#endif // _BASE_TEST_FIXTURE_
