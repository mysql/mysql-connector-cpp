/*
Copyright 2008 Sun Microsystems, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

There are special exceptions to the terms and conditions of the GPL
as it is applied to this software. View the full text of the
exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
software distribution.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _EXAMPLE_BASE_TEST_FIXTURE_
#define _EXAMPLE_BASE_TEST_FIXTURE_

#include <math.h>

#include "common/ccppTypes.h"
#include "common/stringutils.h"
#include "resources.h"
#include <driver/mysql_public_iface.h>

#include "framework/framework.h"

namespace testsuite
{
typedef std::auto_ptr<sql::Connection> Connection;
typedef std::auto_ptr<sql::PreparedStatement> PreparedStatement;
typedef std::auto_ptr<sql::Statement> Statement;
typedef std::auto_ptr<sql::ResultSet> ResultSet;
typedef sql::Driver Driver;
typedef std::auto_ptr<sql::ResultSetMetaData> ResultSetMetaData;
typedef std::auto_ptr<sql::DatabaseMetaData> DatabaseMetaData;

class example_fixture : public TestSuite
{
		
public:	
  typedef TestSuite super;

	/**
	 * List of SQL schema objects created during the test run
	 *
	 * Used by tearDown() to clean up the database after the test run
	 */
  List created_objects;
	
	/**
	 * Driver to be used
	 *
	 */
  static Driver *driver;
	
  static void logMsg(String message);
  void logErr(String message);

protected:

	/**
	 * 
	 *
	 *
	 */
  void init();
		
	String url;
	String db;
	String user;
	String passwd;
	


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
  /* throws SQLException */

  void createSchemaObject(String objectType, String objectName,
                          String columnsAndOtherStuff);
	void dropSchemaObject(String objectType, String objectName);

  void createTable(String tableName, String columnsAndOtherStuff);
  void dropTable(String tableName);

  sql::Connection * getConnection();

  
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

  bool versionMeetsMinimum(int major, int minor);
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

  bool versionMeetsMinimum(int major
                           , int minor
                           , int subminor);

  void closeMemberJDBCResources();

public:

  /**
   * Creates a new BaseTestCase object.
   *
   * @param name
   *            The name of the JUnit test case
   */

  example_fixture(const String & name);

  void logDebug(const String & message);
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

} /* namespace testsuite */

#define EXAMPLE_TEST_FIXTURE( theFixtureClass ) typedef theFixtureClass TestSuiteClass;\
  theFixtureClass( const String & name ) \
  : example_fixture( #theFixtureClass )

#endif // _EXAMPLE_BASE_TEST_FIXTURE_
