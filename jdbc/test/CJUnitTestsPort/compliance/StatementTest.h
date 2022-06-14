/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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



#include "../BaseTestFixture.h"

/**
 * @author mmatthew
 *
 * To change this generated comment edit the template variable "typecomment":
 * Window>Preferences>Java>Templates.
 * To enable and disable the creation of type comments go to
 * Window>Preferences>Java>Code Generation.
 */

namespace testsuite
{
namespace compliance
{

class StatementTest : public BaseTestFixture
{
private:
  typedef BaseTestFixture super;

protected:

public:

  TEST_FIXTURE(StatementTest)
  {
    TEST_CASE(testClearWarnings);
    TEST_CASE(testClose);
    TEST_CASE(testExecute01);
    TEST_CASE(testExecute02);
    TEST_CASE(testExecuteQuery01);
    TEST_CASE(testExecuteQuery02);

    TEST_CASE(testExecuteQuery03);
    TEST_CASE(testExecuteUpdate01);
    TEST_CASE(testExecuteUpdate03);

    TEST_CASE(testGetMoreResults01);
    TEST_CASE(testGetMoreResults02);
    TEST_CASE(testGetMoreResults03);

    TEST_CASE(testGetResultSet01);
    TEST_CASE(testGetResultSet02);
    TEST_CASE(testGetUpdateCount01);
    TEST_CASE(testGetUpdateCount02);
    TEST_CASE(testGetWarnings);

    TEST_CASE(testGetResultSetType01);
    TEST_CASE(testGetResultSetType02);

#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
    TEST_CASE(testGetFetchSize);
    TEST_CASE(testGetMaxFieldSize);
    TEST_CASE(testSetMaxFieldSize02);
    TEST_CASE(testGetMaxRows);
    TEST_CASE(testSetMaxRows02);
    TEST_CASE(testGetQueryTimeout);
    TEST_CASE(testSetQueryTimeout02);
    TEST_CASE(testSetFetchSize02);
    TEST_CASE(testSetFetchSize05);
    TEST_CASE(testSetMaxFieldSize01);
    TEST_CASE(testSetMaxRows01);
#endif

#ifdef MISSING_METHODS_INCLUDED2STATEMENT
    TEST_CASE(testSetFetchDirection04);
    TEST_CASE(testGetResultSetType03);
    TEST_CASE(testGetResultSetConcurrency01);
    TEST_CASE(testGetFetchDirection);
#endif
  }

  /*
   * @testName:         testClearWarnings
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The ClearWarnings clears the SQLWarnings associated with
   *                    the statement object.  After a call to this method, a call
   *                    to getWarnings will return a null SQLWarning object.
   *                    (See JDK 1.2.2 API Documentation)
   *
   * @test_Strategy:    Get a Statement object and call clearWarnings() method
   *                    on the statement object.Further calling the getWarnings()
   *                    method should return a null SQLWarning object
   *
   */
  /* throws std::exception * */
  void testClearWarnings();


  /*
   * @testName:         testClose
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The close method closes the statement object.When a Statement
   *                    object is closed, its current ResultSet object, if one exists,
   *                    is also closed.  (See JDK 1.2.2 API Documentation)
   *
   * @test_Strategy:    Get a Statement object and call close() method and call
   *                    executeQuery() method to check and it should throw sql::DbcException
   *
   */
  /* throws std::exception * */
  void testClose();


  /*
   * @testName:         testExecute01
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The execute(String sql) method returns a boolean value; true
   *                    if the first result is ResultSet or false if it is an integer.
   *                    (See JDK 1.2.2 API Documentation)
   *
   * @test_Strategy:    Call execute(String sql) of updating a row
   *                    It should return a boolean value and the value should be
   *                    equal to false
   *
   */
  /* throws std::exception * */
  void testExecute01();


  /*
   * @testName:         testExecute02
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The execute(String sql) method returns a boolean value;
   *                    true if the first result is ResultSet or false if it is
   *                    an integer. (See JDK 1.2.2 API Documentation)
   *
   * @test_Strategy:    Get a Statement object and call execute(String sql)
   *                    of selecting rows from the database
   *                    It should return a boolean value and the value should be equal
   *                    to true
   *
   */
  /* throws std::exception * */
  void testExecute02();


  /*
   * @testName:         testExecuteQuery01
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The executeQuery(String sql) method returns a ResultSet object;
   *                    It may return an empty ResultSet object but never returns null.
   *                    This method throws sql::DbcException if an error occurs in processing
   *                    SQL statement or if the SQL statement generates a row count
   *                    instead of ResultSet. (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call executeQuery(String sql)
   *                    to select a row from the database
   *                    It should return a ResultSet object
   *
   */
  /* throws std::exception * */
  void testExecuteQuery01();


  /*
   * @testName:         testExecuteQuery02
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The executeQuery(String sql) method returns a ResultSet object;
   *                    It may return an empty ResultSet object but never returns null.
   *                    This method throws sql::DbcException if an error occurs in processing
   *                    SQL statement or if the SQL statement generates a row count
   *                    instead of ResultSet. (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call executeQuery(String sql)
   *                    to select a non-existent row from the database
   *                    It should return a ResultSet object which is empty and call
   *                    ResultSet.next() method to check and it should return a false
   *
   */
  /* throws std::exception * */
  void testExecuteQuery02();


  /*
   * @testName:         testExecuteQuery03
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The executeQuery(String sql) method returns a ResultSet object;
   *                    It may return an empty ResultSet object but never returns null.
   *                    This method throws sql::DbcException if an error occurs in processing
   *                    SQL statement or if the SQL statement generates a row count
   *                    instead of ResultSet. (See JDK1.2.2 API documentation)
   *
   *
   * @test_Strategy:    Get a Statement object and call executeQuery(String sql)
   *                    to insert a row from the database
   *                    It should throw sql::DbcException
   *
   */
  /* throws std::exception * */
  void testExecuteQuery03();


  /*
   * @testName:         testExecuteUpdate01
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The executeUpdate(String sql) method returns a integer value;
   *                    The value indicates the number of rows affected by INSERT, DELETE
   *                    or UPDATE specified in the sql; 0 if no rows were affected or the
   *                    statement executed was a DDL statement.
   *                    This method throws sql::DbcException if an error occurs in processing
   *                    SQL statement or if the SQL statement generates a ResultSet.
   *                    (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call executeUpdate(String sql)
   *                    It should return an int value which is equal to row count
   */
  /* throws std::exception * */
  void testExecuteUpdate01();


  /*
   * @testName:         testExecuteUpdate03
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The executeUpdate(String sql) method returns a integer value;
   *                    The value indicates the number of rows affected by INSERT, DELETE
   *                    or UPDATE specified in the sql; 0 if no rows were affected or the
   *                    statement executed was a DDL statement.
   *                    This method throws sql::DbcException if an error occurs in processing
   *                    SQL statement or if the SQL statement generates a ResultSet.
   *                    (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call executeUpdate(String sql)
   *                    for selecting row from the table
   *                    It should throw a SQL std::exception *
   *
   */
  /* throws std::exception * */
  void testExecuteUpdate03();


#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  /*
   * @testName:         testGetFetchSize
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getFetchSize() method returns a integer value;
   *                    The value that is been set by the setFetchSize method.
   *                    If no fetch size has been set, the return value is
   *                    implementation specific. (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a ResultSet object and call the getFetchSize() method
   *                    It should return a int value
   *
   */
  /* throws std::exception * */
  void testGetFetchSize();


  /*
   * @testName:         testGetMaxFieldSize
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getMaxFieldSize() method returns a integer value;
   *                    The value representing the current maximum number of bytes
   *                    that a ResultSet column may contain.  Zero means that there
   *                    is no limit. (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call the getMaxFieldSize() method
   *                    It should return a int value
   *
   */
  /* throws std::exception * */
  void testGetMaxFieldSize();


  /*
   * @testName:         testGetMaxRows
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getMaxRows() method returns a integer value;
   *                    The value representing the current maximum number of rows
   *                    that a ResultSet object can contain.  Zero means that there
   *                    is no limit. (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call the getMaxRows() method
   *                    It should return a int value
   *
   */
  /* throws std::exception * */
  void testGetMaxRows();
#endif


  /*
   * @testName:         testGetMoreResults01
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getMoreResults() method returns a boolean value;
   *                    true if the next result is ResultSet object; false if it is
   *                    an integer indicating that it is an update count or there are
   *                    no more results. There are no more results when the following
   *                    condition is satisfied.
   *                    (getMoreResults==false && getUpdatecount==-1)
   *                    (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call the execute() method for
   *                    selecting a row and call getMoreResults() method
   *                    It should return a boolean value
   *
   */
  /* throws std::exception * */
  void testGetMoreResults01();


  /*
   * @testName:         testGetMoreResults02
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getMoreResults() method returns a boolean value;
   *                    true if the next result is ResultSet object; false if it is
   *                    an integer indicating that it is an update count or there are
   *                    no more results. There are no more results when the following
   *                    condition is satisfied.
   *                    (getMoreResults==false && getUpdatecount==-1)
   *                    (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call the execute() method for
   *                    selecting a non-existent row and call getMoreResults() method
   *                    It should return a boolean value and the value should be
   *                    equal to false
   *
   */
  /* throws std::exception * */
  void testGetMoreResults02();


  /*
   * @testName:         testGetMoreResults03
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getMoreResults() method returns a boolean value;
   *                    true if the next result is ResultSet object; false if it is
   *                    an integer indicating that it is an update count or there are
   *                    no more results. There are no more results when the following
   *                    condition is satisfied.
   *                    (getMoreResults==false && getUpdatecount==-1)
   *                    (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call the execute() method for
   *                    updating a row and call getMoreResults() method
   *                    It should return a boolean value and the value should be
   *                    equal to false
   *
   */
  /* throws std::exception * */
  void testGetMoreResults03();


#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  /*
   * @testName:         testGetQueryTimeout
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getQueryTimeout() method returns a integer value;
   *                    The value indicates the current query timeout limit in
   *                    seconds. Zero means that there is no time limit.
   *                    (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call getMoreResults() method
   *                    It should return a int value
   *
   */
  /* throws std::exception * */
  void testGetQueryTimeout();
#endif


  /*
   * @testName:         testGetResultSet01
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getResultSet() method returns a ResultSet object;
   *                    the current result set as a ResultSet object; null if the
   *                    result is an integer indicating that it is an update count
   *                    or there are no more results.(See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call execute() method for
   *                    selecting a row and call getResultSet() method
   *                    It should return a ResultSet object
   *
   */
  /* throws std::exception * */
  void testGetResultSet01();


  /*
   * @testName:         testGetResultSet02
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getResultSet() method returns a ResultSet object;
   *                    the current result set as a ResultSet object; null if the
   *                    result is an integer indicating that it is an update count
   *                    or there are no more results.(See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call execute() method for
   *                    updating a row.Then call getResultSet() method
   *                    It should return a  Null ResultSet object
   */
  /* throws std::exception * */
  void testGetResultSet02();


  /*
   * @testName:         testGetUpdateCount01
   *
   * @assertion:        A driver must provide support for Statement and
   *                    ResultSet.  This implies that the methods in the
   *                    Statement interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.        (See
   *                    Section :40.3 Statement Methods JDBC 2.0 API Tutorial
   *                    & Reference).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getUpdateCount() method should return a integer value;
   *                    the value might be greater than 0 representing the rows affected;
   *                    0 if no rows are affected or if DDL statement; -1 if the result
   *                    is a ResultSet object or there are no more results
   *                    (See JDK 1.2.2 API Documentation)
   *
   * @test_Strategy:    Get a Statement object and call the execute() method for
   *                    updating a row and call getUpdateCount() method
   *                    It should return a int value and the value should be
   *                    equal to number of rows with the specified condition for update
   */
  /* throws std::exception * */
  void testGetUpdateCount01();


  /*
   * @testName:         testGetUpdateCount02
   *
   * @assertion:        A driver must provide support for Statement and
   *                    ResultSet.  This implies that the methods in the
   *                    Statement interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.        (See
   *                    Section :40.3 Statement Methods JDBC 2.0 API Tutorial
   *                    & Reference).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getUpdateCount() method should return a integer value;
   *                    the value might be greater than 0 representing the rows affected;
   *                    0 if no rows are affected or if DDL statement; -1 if the result
   *                    is a ResultSet object or there are no more results
   *                    (See JDK 1.2.2 API Documentation)
   *
   * @test_Strategy:    Get a Statement object and call the execute() method for
   *                    selecting a non-existent row and call getUpdateCount() method
   *                    It should return a int value and the value should be
   *                    equal to -1
   */

  /* throws std::exception * */

  void testGetUpdateCount02();
  /*
   * @testName:         testGetWarnings
   *
   * @assertion:        A driver must provide support for Statement and
   *                    ResultSet.  This implies that the methods in the
   *                    Statement interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.        (See
   *                    Section :40.3 Statement Methods JDBC 2.0 API Tutorial
   *                    & Reference).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getWarnings() method should return a SQLWarning object;
   *                    or null if there are no warnings (See JDK 1.2.2 API
   *                    Documentation)
   *
   * @test_Strategy:    Get a Statement object and call getWarnings() method
   *                    should return an SQLWarning object
   */
  /* throws std::exception * */
  void testGetWarnings();


#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  /*
   * @testName:         testSetFetchSize02
   * @assertion:        A driver must provide support for Statement and
   *                    ResultSet.  This implies that the methods in the
   *                    Statement interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.        (See
   *                    Section :40.3 Statement Methods JDBC 2.0 API Tutorial
   *                    & Reference).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The setFetchSize(int rowsize) method sets the number of rows
   *                    to fetch from the database specified by the value of rowsize.
   *                    The setFetchSize does not return any value. (See JDK 1.2.2
   *                    API Documentation)
   *
   * @test_Strategy:    Get a Statement object and call the setFetchSize(int rows)
   *                    method with the value of Statement.getMaxRows and call
   *                    getFetchSize() method and it should return a int value
   *                    that is been set
   */
  /* throws std::exception * */
  void testSetFetchSize02();


  /*
   * @testName:         testSetFetchSize05
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The setFetchSize(int size) method sets the number of rows
   *                    to fetch from the database specified by the value size.
   *                    The method does not return any value and throws sql::DbcException
   *                    if a database access error occurs or the condition
   *                    0 <= size <= this.getMaxRows is not satisfied.
   *                    (See JDK 1.2.2 API Documentation)
   *
   * @test_Strategy:    Get a Statement object and call the setFetchSize(int rows)
   *                    method with the negative value and it should throw
   *                    sql::DbcException
   *
   */
  /* throws std::exception * */
  void testSetFetchSize05();


  /*
   * @testName:         testSetMaxFieldSize01
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The setMaxFieldSize(int maxsize) method sets the maximum size
   *                    for a column in a result set specified by the value maxsize (in
   *                    bytes). For maximum portability, the maximum field size should
   *                    be set to a value greater than 256. If the value maxsize is 0
   *                    then it means that there is no limit to the size of a column.
   *                    The setMaxFieldSize(int maxsize) does not return any value.
   *                    (See JDK 1.2.2 API Documentation)
   *
   * @test_Strategy:    Get a Statement object and call the setMaxFieldSize(int max)
   *                    method and call getMaxFieldSize() method and it should return
   *                    an integer value that is been set
   *
   */
  /* throws std::exception * */
  void testSetMaxFieldSize01();


  /*
   * @testName:         testSetMaxFieldSize02
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The setMaxFieldSize(int maxsize) method sets the maximum size
   *                    for a column in a result set specified by the value maxsize (in
   *                    bytes). For maximum portability, the maximum field size should
   *                    be set to a value greater than 256. If the value maxsize is 0
   *                    then it means that there is no limit to the size of a column.
   *                    The setMaxFieldSize(int maxsize) does not return any value.
   *                    (See JDK 1.2.2 API Documentation)
   *
   * @test_Strategy:    Get a Statement object and call the setMaxFieldSize(int max)
   *                    method with an invalid value (negative value) and It should
   *                    throw a sql::DbcException
   *
   */
  /* throws std::exception * */
  void testSetMaxFieldSize02();


  /*
   * @testName:         testSetMaxRows01
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The setMaxRows(int maxsize) method sets the maximum number
   *                    of rows that any ResultSet object can contain is specified
   *                    by the value maxsize. If the value maxsize is 0 then it means
   *                    that there is no limit. The setMaxRows(int maxsize) does not
   *                    return any value.(See JDK 1.2.2 API Documentation)
   *
   * @test_Strategy:    Get a Statement object and call the setMaxRows(int rows)
   *                    method and call getMaxRows() method and it should return a
   *                    integer value that is been set
   *
   */
  /* throws std::exception * */
  void testSetMaxRows01();


  /*
   * @testName:         testSetMaxRows02
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The setMaxRows(int maxsize) method sets the maximum number
   *                    of rows that any ResultSet object can contain is specified
   *                    by the value maxsize. If the value maxsize is 0 then it means
   *                    that there is no limit. The setMaxRows(int maxsize) does not
   *                    return any value.(See JDK 1.2.2 API Documentation)
   *
   * @test_Strategy:    Get a Statement object and call the setMaxRows(int rows)
   *                    method with an invalid value (negative value) and It should
   *                    throw an sql::DbcException
   *
   */
  /* throws std::exception * */
  void testSetMaxRows02();

  /*
   * @testName:         testSetQueryTimeout02
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The setQueryTimeout(int secval) method sets the time limit
   *                    for the number of secval seconds a driver will wait for a
   *                    statement object to be executed. If the value secval is 0
   *                    then it means that there is no limit. The setQueryTimeout
   *                    method does not return any value. (See JDK 1.2.2 API
   *                    Documentation)
   *
   * @test_Strategy:    Get a Statement object and call the setQueryTimeout(int secval)
   *                    method with an invalid value (negative value)and It should
   *                    throw an sql::DbcException
   *
   */
  /* throws std::exception * */
  void testSetQueryTimeout02();
#endif

  /*
  * @testName:         testGetResultSetType01
  * @assertion:        The Statement object provides methods for executing SQL
  *                    statements and retrieving results.(See section 40.1 of
  *                    JDBC 2.0 API Reference & Tutorial second edition).
  *
  *                    The driver must provide full support for Statement methods.
  *                    The driver must also support all the methods for executing
  *                    SQL Statements in a single batch (Batch Updates). (See
  *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
  *                    Specification v1.2)
  *
  *                    The getResultSetType() method returns an integer value;
  *                    the value representing the type of the ResultSet objects
  *                    and the value can be any one of the following
  *                    ResultSet.TYPE_FORWARD_ONLY, ResultSet.TYPE_SCROLL_SENSITIVE
  *                    and ResultSet.TYPE_SCROLL_INSENSITIVE.
  *                    (See JDK1.2.2 API documentation)
  *
  * @test_Strategy:    Get a Statement object and call getResultSetType() method
  *                    It should return an int value which should be either
  *                    TYPE_FORWARD_ONLY or TYPE_SCROLL_INSENSITIVE or TYPE_SCROLL_SENSITIVE
  */
  /* throws std::exception * */
  void testGetResultSetType01();


  /*
  * @testName:         testGetResultSetType02
  * @assertion:        The Statement object provides methods for executing SQL
  *                    statements and retrieving results.(See section 40.1 of
  *                    JDBC 2.0 API Reference & Tutorial second edition).
  *
  *                    The driver must provide full support for Statement methods.
  *                    The driver must also support all the methods for executing
  *                    SQL Statements in a single batch (Batch Updates). (See
  *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
  *                    Specification v1.2)
  *
  *                    The getResultSetType() method returns an integer value;
  *                    the value representing the type of the ResultSet objects
  *                    and the value can be any one of the following
  *                    ResultSet.TYPE_FORWARD_ONLY, ResultSet.TYPE_SCROLL_SENSITIVE
  *                    and ResultSet.TYPE_SCROLL_INSENSITIVE.
  *                    (See JDK1.2.2 API documentation)
  *
  * @test_Strategy:    Call Connection.createStatement with the Type mode as
  *                    TYPE_FORWARD_ONLY and call getResultSetType() method
  *                    It should return a int value and the value should be equal
  *                    to ResultSet.TYPE_FORWARD_ONLY
  */
  /* throws std::exception * */
  void testGetResultSetType02();


#ifdef MISSING_METHODS_INCLUDED2STATEMENT
  /*
   * @testName:         testSetFetchDirection04
   * @assertion:        A driver must provide support for Statement and
   *                    ResultSet.  This implies that the methods in the
   *                    Statement interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.        (See
   *                    Section :40.3 Statement Methods JDBC 2.0 API Tutorial
   *                    & Reference).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The setFetchDirection(int dir) method sets the statement
   *                    object's fetch direction.  The setFetchDirection method does
   *                    not return any value. (See JDK 1.2.2 API Documentation)
   *
   * @test_Strategy:    Get a Statement object and call the setFetchDirection(int direction)
   *                    method with an invalid value and it should throw an sql::DbcException
   */

  /* throws std::exception * */

  void testSetFetchDirection04();

  /*
   * @testName:         testGetResultSetConcurrency01
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getResultSetConcurrency() method returns an integer value;
   *                    the value representing the concurrency mode for the ResultSet
   *                    objects and the value can be any one of the following
   *                    ResultSet.CONCUR_READ_ONLY and ResultSet.CONCUR_UPDATABLE.
   *                    (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call getResultSetConcurrency() method
   *                    It should return an int value either CONCUR_READ_ONLY or
   *                    CONCUR_UPDATABLE.
   */
  /* throws std::exception * */
  void testGetResultSetConcurrency01();


  /*
   * @testName:         testGetResultSetType03
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getResultSetType() method returns an integer value;
   *                    the value representing the type of the ResultSet objects
   *                    and the value can be any one of the following
   *                    ResultSet.TYPE_FORWARD_ONLY, ResultSet.TYPE_SCROLL_SENSITIVE
   *                    and ResultSet.TYPE_SCROLL_INSENSITIVE.
   *                    (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Call Connection.createStatement with the Type mode as
   *                    TYPE_SCROLL_INSENSITIVE and call getResultSetType() method
   *                    It should return a int value and the value should be equal
   *                    to ResultSet.TYPE_SCROLL_INSENSITIVE
   */
  /* throws std::exception * */
  void testGetResultSetType03();


  /*
   * @testName:         testGetFetchDirection
   * @assertion:        The Statement object provides methods for executing SQL
   *                    statements and retrieving results.(See section 40.1 of
   *                    JDBC 2.0 API Reference & Tutorial second edition).
   *
   *                    The driver must provide full support for Statement methods.
   *                    The driver must also support all the methods for executing
   *                    SQL Statements in a single batch (Batch Updates). (See
   *                    section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
   *                    Specification v1.2)
   *
   *                    The getFetchDirection() method returns a integer value;
   *                    The value that is been set by the setFetchDirection method.
   *                    If no fetch direction has been set, the return value is
   *                    implementation specific. (See JDK1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Statement object and call the getFetchDirection() method
   *                    It should return a int value and the value should be equal to
   *                    any of the values FETCH_FORWARD or FETCH_REVERSE or FETCH_UNKNOWN
   *
   */

  /* throws std::exception * */
  void testGetFetchDirection();
#endif
};

REGISTER_FIXTURE(StatementTest);

}
}
