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



#include "StatementTest.h"

namespace testsuite
{
namespace compliance
{
/*
 * @testName:         testClearWarnings
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The ClearWarnings clears the SQLWarnings associated with
 *            the statement object.  After a call to this method, a call
 *            to getWarnings will return a null SQLWarning object.
 *            (See JDK 1.2.2 API Documentation)
 *
 * @test_Strategy:    Get a Statement object and call clearWarnings() method
 *            on the statement object.Further calling the getWarnings()
 *            method should return a null SQLWarning object
 *
 */

/* throws Exception */
void StatementTest::testClearWarnings()
{
  const sql::SQLWarning * sWarning=stmt->getWarnings();

  if (sWarning != NULL) {
    logMsg("Calling clearWarnings method ");
    stmt->clearWarnings();
    sWarning=stmt->getWarnings();

    if (sWarning == NULL)
    {
      logMsg("clearWarnings clears the SQLWarnings");
    } else
    {
      logErr("clearWarnings does not clear the SQLWarning");
      FAIL("Call to clearWarnings is Failed!");
    }
  } else {
    logErr("getWarnings() returns a NULL SQLWarning object");
  }
}

/*
 * @testName:         testClose
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The close method closes the statement object.When a Statement
 *            object is closed, its current ResultSet object, if one exists,
 *            is also closed.  (See JDK 1.2.2 API Documentation)
 *
 * @test_Strategy:    Get a Statement object and call close() method and call
 *            executeQuery() method to check and it should throw SQLException
 *
 */

/* throws Exception */
void StatementTest::testClose()
{
  Statement statemt;
  bool sqlExceptFlag=false;

  statemt.reset(conn->createStatement());
  logMsg("Calling close method");
  statemt->close();
  String sSelCoffee=sqlProps["SelCoffeeAll"];
  logMsg(String("Query String : ") + sSelCoffee);

  try
  {
    rs.reset(statemt->executeQuery(sSelCoffee));
  } catch (sql::SQLException &) {
    sqlExceptFlag=true;
  }

  if (sqlExceptFlag) {
    logMsg("close method closes the Statement object");
  } else {
    logErr("close method does not close the Statement object");
    FAIL("Call to close method is Failed!");
  }
}

/*
 * @testName:         testExecute01
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The execute(String sql) method returns a boolean value; true
 *            if the first result is ResultSet or false if it is an integer.
 *            (See JDK 1.2.2 API Documentation)
 *
 * @test_Strategy:    Call execute(String sql) of updating a row
 *            It should return a boolean value and the value should be
 *            equal to false
 *
 */

/* throws Exception */
void StatementTest::testExecute01()
{
  createStandardTable(TABLE_CTSTABLE2);
  bool executeFlag=false;

  String sSqlStmt=sqlProps[ "Upd_Coffee_Tab" ];
  logMsg(String("SQL Statement to be executed  ") + sSqlStmt);
  logMsg("Calling execute method ");
  executeFlag=stmt->execute(sSqlStmt);

  if (!executeFlag) {
    logMsg("execute method executes the SQL Statement ");
  } else {
    logErr("execute method does not execute the SQL Statement");
    FAIL("Call to execute is Failed!");
  }
}

/*
 * @testName:         testExecute02
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The execute(String sql) method returns a boolean value;
 *            true if the first result is ResultSet or false if it is
 *            an integer. (See JDK 1.2.2 API Documentation)
 *
 * @test_Strategy:    Get a Statement object and call execute(String sql)
 *            of selecting rows from the database
 *            It should return a boolean value and the value should be equal
 *            to true
 *
 */

/* throws Exception */
void StatementTest::testExecute02()
{
  createStandardTable(TABLE_CTSTABLE2);
  bool executeFlag=false;

  String sSqlStmt=sqlProps[ "Sel_Coffee_Tab" ];

  logMsg(String("Sql Statement to be executed  ") + sSqlStmt);
  logMsg("Calling execute method ");
  executeFlag=stmt->execute(sSqlStmt);
  if (executeFlag) {
    logMsg("execute method executes the SQL Statement ");
  } else {
    logErr("execute method does not execute the SQL Statement");
    FAIL("Call to execute is Failed!");
  }
}

/*
 * @testName:         testExecuteQuery01
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The executeQuery(String sql) method returns a ResultSet object;
 *            It may return an empty ResultSet object but never returns null.
 *            This method throws SQLException if an error occurs in processing
 *            SQL statement or if the SQL statement generates a row count
 *            instead of ResultSet. (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call executeQuery(String sql)
 *            to select a row from the database
 *            It should return a ResultSet object
 *
 */

/*
 *throws Exception
 * Pretty much useless in C++ */
void StatementTest::testExecuteQuery01()
{
  createStandardTable(TABLE_CTSTABLE2);
  ResultSet reSet;

  String sSqlStmt=sqlProps[ "SelCoffeeAll" ];
  logMsg(String("SQL Statement to be executed  :  ") + sSqlStmt);
  logMsg("Calling executeQuery method ");

  reSet.reset(stmt->executeQuery(sSqlStmt));

  if (reSet.get() != NULL) {
    logMsg("executeQuery method returns a ResultSet object");
  } else {
    logErr("executeQuery method does not return a ResultSet object");
    FAIL("Call to executeQuery is Failed!");
  }
}

/*
 * @testName:         testExecuteQuery02
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The executeQuery(String sql) method returns a ResultSet object;
 *            It may return an empty ResultSet object but never returns null.
 *            This method throws SQLException if an error occurs in processing
 *            SQL statement or if the SQL statement generates a row count
 *            instead of ResultSet. (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call executeQuery(String sql)
 *            to select a non-existent row from the database
 *            It should return a ResultSet object which is empty and call
 *            ResultSet.next() method to check and it should return a false
 *
 */

/* throws Exception */
void StatementTest::testExecuteQuery02()
{
  createStandardTable(TABLE_CTSTABLE2);
  ResultSet reSet;
  String sSqlStmt=sqlProps[ "SelCoffeeNull" ];

  logMsg(String("SQL Statement to be executed  :  ") + sSqlStmt);
  logMsg("Calling executeQuery method ");

  reSet.reset(stmt->executeQuery(sSqlStmt));

  if (!reSet->next()) {
    logMsg(
           "executeQuery method returns an Empty ResultSet for Non-Existent row");
  } else {
    logErr(
           "executeQuery method does not return an Empty ResultSet for non-existent row");
    FAIL("Call to executeQuery is Failed!");
  }
}

/*
 * @testName:         testExecuteQuery03
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The executeQuery(String sql) method returns a ResultSet object;
 *            It may return an empty ResultSet object but never returns null.
 *            This method throws SQLException if an error occurs in processing
 *            SQL statement or if the SQL statement generates a row count
 *            instead of ResultSet. (See JDK1.2.2 API documentation)
 *
 *
 * @test_Strategy:    Get a Statement object and call executeQuery(String sql)
 *            to insert a row from the database
 *            It should throw SQLException
 *
 */

/* throws Exception */
void StatementTest::testExecuteQuery03()
{
  ResultSet reSet;
  bool sqlExceptFlag=false;

  String sSqlStmt=sqlProps[ "Ins_Coffee_Tab" ];
  logMsg(String("SQL Statement to be executed  :  ") + sSqlStmt);

  try
  {
    logMsg("Calling executeQuery method ");
    reSet.reset(stmt->executeQuery(sSqlStmt));
  } catch (sql::SQLException &) {
    sqlExceptFlag=true;
  }

  if (!sqlExceptFlag) {
    logErr("executeQuery method executes an Insert Statement");
    FAIL("Call to executeQuery is Failed!");
  } else {
    logMsg(
           "executeQuery method does not execute an Insert Statement");
  }

}

/*
 * @testName:         testExecuteUpdate01
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The executeUpdate(String sql) method returns a integer value;
 *            The value indicates the number of rows affected by INSERT, DELETE
 *            or UPDATE specified in the sql; 0 if no rows were affected or the
 *            statement executed was a DDL statement.
 *            This method throws SQLException if an error occurs in processing
 *            SQL statement or if the SQL statement generates a ResultSet.
 *            (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call executeUpdate(String sql)
 *            It should return an int value which is equal to row count
 */

/* throws Exception */
void StatementTest::testExecuteUpdate01()
{
  createStandardTable(TABLE_CTSTABLE2);
  int updCount=0;
  int retRowCount=0;
  String sSqlStmt=sqlProps[ "Upd_Coffee_Tab" ];

  logMsg(String("Update String  : ") + sSqlStmt);

  logMsg("Calling executeUpdate method ");
  updCount=stmt->executeUpdate(sSqlStmt);

  String countQuery=sqlProps[ "Coffee_Updcount_Query" ];

  logMsg(String("Query String :  ") + countQuery);
  rs.reset(stmt->executeQuery(countQuery));

  rs->next();
  retRowCount=rs->getInt(1);

  TestsListener::messagesLog()
          << "Number of rows in the table with the specified condition  "
          << retRowCount << std::endl;

  if (updCount == retRowCount) {
    logMsg("executeUpdate executes the SQL Statement ");
  } else {
    logErr("executeUpdate does not execute the SQL Statement ");
    FAIL("Call to executeUpdate is Failed!");
  }
}

/*
 * @testName:         testExecuteUpdate03
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The executeUpdate(String sql) method returns a integer value;
 *            The value indicates the number of rows affected by INSERT, DELETE
 *            or UPDATE specified in the sql; 0 if no rows were affected or the
 *            statement executed was a DDL statement.
 *            This method throws SQLException if an error occurs in processing
 *            SQL statement or if the SQL statement generates a ResultSet.
 *            (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call executeUpdate(String sql)
 *            for selecting row from the table
 *            It should throw a SQL Exception
 *
 */

/* throws Exception */
void StatementTest::testExecuteUpdate03()
{
  bool sqlExceptFlag=false;

  String sSqlStmt=sqlProps[ "Sel_Coffee_Tab" ];
  logMsg(String("SQL String of non - existent row  :  ") + sSqlStmt);

  try
  {
    logMsg("Calling executeUpdate method ");
    stmt->executeUpdate(sSqlStmt);
  } catch (sql::SQLException &) {
    sqlExceptFlag=true;
  }

  if (sqlExceptFlag) {
    logMsg(
           "executeUpdate does not execute the SQL statement on non-existent row");
  } else {
    logErr(
           "executeUpdate executes the SQL statement on non-existent row");
    FAIL("Call to executeUpdate is Failed!");
  }
}

/*
 * @testName:         testGetFetchDirection
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getFetchDirection() method returns a integer value;
 *            The value that is been set by the setFetchDirection method.
 *            If no fetch direction has been set, the return value is
 *            implementation specific. (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call the getFetchDirection() method
 *            It should return a int value and the value should be equal to
 *            any of the values FETCH_FORWARD or FETCH_REVERSE or FETCH_UNKNOWN
 *
 */

/* throws Exception
 *
 * Not included into sql::statement (and thus into MySQL_Statement)
 * Disabled so far.
 */

#ifdef MISSING_METHODS_INCLUDED2STATEMENT

void StatementTest::testGetFetchDirection()
{
  int fetchDirVal=0;

  logMsg("Calling getFetchDirection method ");

  fetchDirVal=stmt->getFetchDirection();

  if (fetchDirVal == ResultSet::FETCH_FORWARD) {
    logMsg(
           "getFetchDirection method returns ResultSet.FETCH_FORWARD ");
  } else if (fetchDirVal == ResultSet.FETCH_REVERSE) {
    logMsg(
           "getFetchDirection method returns ResultSet.FETCH_REVERSE");
  } else if (fetchDirVal == ResultSet.FETCH_UNKNOWN) {
    logMsg(
           "getFetchDirection method returns ResultSet.FETCH_UNKNOWN");
  } else {
    logErr(" getFetchDirection method returns a invalid value");
    FAIL("Call to getFetchDirection is Failed");
  }
}
#endif


#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
/*
 * @testName:         testGetFetchSize
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getFetchSize() method returns a integer value;
 *            The value that is been set by the setFetchSize method.
 *            If no fetch size has been set, the return value is
 *            implementation specific. (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a ResultSet object and call the getFetchSize() method
 *            It should return a int value
 *
 */
/* throws Exception */
void StatementTest::testGetFetchSize()
{

  logMsg("Calling getFetchSize on Statement");
  int fetchSizeVal=stmt->getFetchSize();

  if (fetchSizeVal >= 0) {
    TestsListener::messagesLog()
            << "getFetchSize method returns :" << fetchSizeVal << std::endl;
  } else {
    logErr(" getFetchSize method returns a invalid value");
    FAIL("Call to getFetchSize is Failed!");
  }
}


/*
 * @testName:         testGetMaxFieldSize
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getMaxFieldSize() method returns a integer value;
 *            The value representing the current maximum number of bytes
 *            that a ResultSet column may contain.  Zero means that there
 *            is no limit. (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call the getMaxFieldSize() method
 *            It should return a int value
 *
 */
/* throws Exception */
void StatementTest::testGetMaxFieldSize()
{

  logMsg("Calling getMaxFieldSize on Statement");
  int maxFieldSizeVal=stmt->getMaxFieldSize();

  if (maxFieldSizeVal >= 0) {
    TestsListener::messagesLog()
            << "getMaxFieldSize method returns :" << maxFieldSizeVal << std::endl;
  } else {
    logErr(" getMaxFieldSize method returns a invalid value");
    FAIL("Call to getMaxFieldSize is Failed!");
  }
}


/*
 * @testName:         testGetMaxRows
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getMaxRows() method returns a integer value;
 *            The value representing the current maximum number of rows
 *            that a ResultSet object can contain.  Zero means that there
 *            is no limit. (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call the getMaxRows() method
 *            It should return a int value
 *
 */
/* throws Exception */
void StatementTest::testGetMaxRows()
{

  logMsg("Calling getMaxRows on Statement");
  int maxRowsVal=static_cast<int> (stmt->getMaxRows());

  if (maxRowsVal >= 0) {
    TestsListener::messagesLog()
            << "getMaxRows method returns :" << maxRowsVal << std::endl;
  } else {
    logErr(" getMaxRows method returns a invalid value");
    FAIL("Call to getMaxRows is Failed!");
  }
}
#endif


/*
 * @testName:         testGetMoreResults01
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getMoreResults() method returns a boolean value;
 *            true if the next result is ResultSet object; false if it is
 *            an integer indicating that it is an update count or there are
 *            no more results. There are no more results when the following
 *            condition is satisfied.
 *            (getMoreResults==false && getUpdatecount==-1)
 *            (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call the execute() method for
 *            selecting a row and call getMoreResults() method
 *            It should return a boolean value
 *
 */

/* throws Exception */
void StatementTest::testGetMoreResults01()
{
  createStandardTable(TABLE_CTSTABLE2);
  String sSqlStmt=sqlProps[ "SelCoffeeAll" ];

  logMsg(String("Query String : ") + sSqlStmt);
  stmt->execute(sSqlStmt);

  logMsg("Calling getMoreResults on Statement");
  bool moreResVal=stmt->getMoreResults();

  // Hmm... smth is wrong here. Bad test
  if ((moreResVal == true) || (moreResVal == false)) {
    TestsListener::messagesLog()
            << "getMoreResults method returns :" << moreResVal << std::endl;
    TestsListener::setTestExecutionComment("This test needs to be changed or removed. It's wrong");
  } else {
    logErr(" getMoreResults method returns a invalid value");
    FAIL("Call to getMoreResults is Failed!");
  }
}

/*
 * @testName:         testGetMoreResults02
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getMoreResults() method returns a boolean value;
 *            true if the next result is ResultSet object; false if it is
 *            an integer indicating that it is an update count or there are
 *            no more results. There are no more results when the following
 *            condition is satisfied.
 *            (getMoreResults==false && getUpdatecount==-1)
 *            (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call the execute() method for
 *            selecting a non-existent row and call getMoreResults() method
 *            It should return a boolean value and the value should be
 *            equal to false
 *
 */

/* throws Exception */
void StatementTest::testGetMoreResults02()
{
  createStandardTable(TABLE_CTSTABLE2);
  String sSqlStmt=sqlProps[ "SelCoffeeNull" ];

  logMsg(String("Query String : ") + sSqlStmt);
  stmt->execute(sSqlStmt);

  logMsg("Calling getMoreResults on Statement");
  bool moreResVal=stmt->getMoreResults();

  if (!moreResVal) {
    TestsListener::messagesLog()
            << "getMoreResults method returns :" << moreResVal << std::endl;
  } else {
    logErr(" getMoreResults method returns a invalid value");
    FAIL("Call to getMoreResults is Failed!");
  }
}

/*
 * @testName:         testGetMoreResults03
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getMoreResults() method returns a boolean value;
 *            true if the next result is ResultSet object; false if it is
 *            an integer indicating that it is an update count or there are
 *            no more results. There are no more results when the following
 *            condition is satisfied.
 *            (getMoreResults==false && getUpdatecount==-1)
 *            (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call the execute() method for
 *            updating a row and call getMoreResults() method
 *            It should return a boolean value and the value should be
 *            equal to false
 *
 */

/* throws Exception */
void StatementTest::testGetMoreResults03()
{
  createStandardTable(TABLE_CTSTABLE2);
  String sSqlStmt=sqlProps[ "Upd_Coffee_Tab" ];

  logMsg(String("Query String : ") + sSqlStmt);
  stmt->execute(sSqlStmt);

  logMsg("Calling getMoreResults on Statement");
  bool moreResVal=stmt->getMoreResults();

  if (!moreResVal) {
    TestsListener::messagesLog()
            << "getMoreResults method returns :" << moreResVal << std::endl;
  } else {
    logErr(" getMoreResults method returns a invalid value");
    FAIL("Call to getMoreResults is Failed!");
  }
}


#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
/*
 * @testName:         testGetQueryTimeout
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getQueryTimeout() method returns a integer value;
 *            The value indicates the current query timeout limit in
 *            seconds. Zero means that there is no time limit.
 *            (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call getMoreResults() method
 *            It should return a int value
 *
 */
/* throws Exception */
void StatementTest::testGetQueryTimeout()
{
  int queryTimeout=0;

  logMsg("Calling getQueryTimeout on Statement");
  queryTimeout=stmt->getQueryTimeout();

  if (queryTimeout >= 0) {
    TestsListener::messagesLog()
            << "getQueryTimeout method returns :" << queryTimeout << std::endl;
  } else {
    logErr(" getQueryTimeout method returns a invalid value");
    FAIL("Call to getQueryTimeout is Failed!");
  }
}
#endif


/*
 * @testName:         testGetResultSet01
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getResultSet() method returns a ResultSet object;
 *            the current result set as a ResultSet object; null if the
 *            result is an integer indicating that it is an update count
 *            or there are no more results.(See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call execute() method for
 *            selecting a row and call getResultSet() method
 *            It should return a ResultSet object
 *
 */

/* throws Exception */
void StatementTest::testGetResultSet01()
{
  createStandardTable(TABLE_CTSTABLE2);
  ResultSet retResSet /*= NULL*/;
  String sSqlStmt=sqlProps[ "SelCoffeeAll" ];

  logMsg(String("Query String :  ") + sSqlStmt);
  logMsg("Calling getResultSet on Statement");
  stmt->execute(sSqlStmt);

  retResSet.reset(stmt->getResultSet());

  if (retResSet.get() != NULL) {
    logMsg("getResultSet method returns a ResultSet object ");
  } else {
    logErr(
           " getResultSet method does not return a ResultSet object");
    FAIL("Call to getResultSet is Failed!");
  }
}

/*
 * @testName:         testGetResultSet02
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getResultSet() method returns a ResultSet object;
 *            the current result set as a ResultSet object; null if the
 *            result is an integer indicating that it is an update count
 *            or there are no more results.(See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call execute() method for
 *            updating a row.Then call getResultSet() method
 *            It should return a  Null ResultSet object
 */

/* throws Exception */
void StatementTest::testGetResultSet02()
{
  createStandardTable(TABLE_CTSTABLE2);
  ResultSet retResSet /*= NULL*/;
  String sSqlStmt=sqlProps[ "Upd_Coffee_Tab" ];

  logMsg(String("Query String :  ") + sSqlStmt);
  logMsg("Calling getResultSet on Statement");

  stmt->execute(sSqlStmt);
  retResSet.reset(stmt->getResultSet());

  if (retResSet.get() == NULL) {
    logMsg("getResultSet method returns a Null ResultSet object ");
  } else {
    logErr(
           " getResultSet method does (not) return a ResultSet object");
    FAIL("Call to getResultSet is Failed!");
  }
}


/*
* @testName:         testGetResultSetType01
* @assertion:        The Statement object provides methods for executing SQL
*            statements and retrieving results.(See section 40.1 of
*            JDBC 2.0 API Reference & Tutorial second edition).
*
*            The driver must provide full support for Statement methods.
*            The driver must also support all the methods for executing
*            SQL Statements in a single batch (Batch Updates). (See
*            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
*            Specification v1.2)
*
*            The getResultSetType() method returns an integer value;
*            the value representing the type of the ResultSet objects
*            and the value can be any one of the following
*            ResultSet.TYPE_FORWARD_ONLY, ResultSet.TYPE_SCROLL_SENSITIVE
*            and ResultSet.TYPE_SCROLL_INSENSITIVE.
*            (See JDK1.2.2 API documentation)
*
* @test_Strategy:    Get a Statement object and call getResultSetType() method
*            It should return an int value which should be either
*            TYPE_FORWARD_ONLY or TYPE_SCROLL_INSENSITIVE or TYPE_SCROLL_SENSITIVE
*/
void StatementTest::testGetResultSetType01()
{
  int rsType=0;

  rsType=stmt->getResultSetType();

  if (rsType == sql::ResultSet::TYPE_FORWARD_ONLY)
  {
    TestsListener::messagesLog() << "getResultSetType method returns TYPE_FORWARD_ONLY"
      << rsType << std::endl;
  }
  else if (rsType == sql::ResultSet::TYPE_SCROLL_INSENSITIVE)
  {
    TestsListener::messagesLog() <<
      "getResultSetType method returns TYPE_SCROLL_INSENSITIVE "
      << rsType << std::endl;
  }
  else if (rsType == sql::ResultSet::TYPE_SCROLL_SENSITIVE)
  {
    TestsListener::messagesLog() <<
      "getResultSetType method returns TYPE_SCROLL_SENSITIVE  "
      << rsType << std::endl;
  }
  else
  {
    logErr(" getResultSetType method does not return a valid value");
    FAIL("Call to getResultSetType is Failed!");
  }
}


/*
* @testName:         testGetResultSetType02
* @assertion:        The Statement object provides methods for executing SQL
*            statements and retrieving results.(See section 40.1 of
*            JDBC 2.0 API Reference & Tutorial second edition).
*
*            The driver must provide full support for Statement methods.
*            The driver must also support all the methods for executing
*            SQL Statements in a single batch (Batch Updates). (See
*            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
*            Specification v1.2)
*
*            The getResultSetType() method returns an integer value;
*            the value representing the type of the ResultSet objects
*            and the value can be any one of the following
*            ResultSet.TYPE_FORWARD_ONLY, ResultSet.TYPE_SCROLL_SENSITIVE
*            and ResultSet.TYPE_SCROLL_INSENSITIVE.
*            (See JDK1.2.2 API documentation)
*
* @test_Strategy:    Call Connection.createStatement with the Type mode as
*            TYPE_FORWARD_ONLY and call getResultSetType() method
*            It should return a int value and the value should be equal
*            to ResultSet.TYPE_FORWARD_ONLY
*/
void StatementTest::testGetResultSetType02()
{
  int rsType=0;

  logMsg("Creating Statement object with the ResultSet Type");

  Statement statemt( conn->createStatement() );

  statemt->setResultSetType( sql::ResultSet::TYPE_FORWARD_ONLY );

//    sql::ResultSet::CONCUR_READ_ONLY);
  rsType=statemt->getResultSetType();

  if ( rsType == sql::ResultSet::TYPE_FORWARD_ONLY )
  {
    TestsListener::messagesLog() << "getResultSetType method returns TYPE_FORWARD_ONLY "
      << rsType << std::endl;
  }
  else
  {
    logErr(" getResultSetType method does not return a valid value");
    FAIL("Call to getResultSetType is Failed!");
  }
}


/*
 * @testName:         testGetResultSetConcurrency01
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getResultSetConcurrency() method returns an integer value;
 *            the value representing the concurrency mode for the ResultSet
 *            objects and the value can be any one of the following
 *            ResultSet.CONCUR_READ_ONLY and ResultSet.CONCUR_UPDATABLE.
 *            (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Statement object and call getResultSetConcurrency() method
 *            It should return an int value either CONCUR_READ_ONLY or
 *            CONCUR_UPDATABLE.
 */

/* getResultSetConcurrency not included into statement interface
 *
 *
 * throws Exception
 */
#ifdef MISSING_METHODS_INCLUDED2STATEMENT
void StatementTest::testGetResultSetConcurrency01()
{
  int rsConcur=0;

  rsConcur=stmt->getResultSetConcurrency();
  if ((rsConcur == sql::ResultSet::CONCUR_READ_ONLY)
      || (rsConcur == sql::ResultSet::CONCUR_UPDATABLE)) {
    logMsg(
           "getResultSetConcurrency method returns ResultSet Concurrency mode  "
           + rsConcur);
  } else {
    logErr(
           " getResultSetConcurrency method does not return a valid value");
    FAIL("Call to getResultSetConcurrency is Failed!");
  }
}


/*
 * @testName:         testGetResultSetType03
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getResultSetType() method returns an integer value;
 *            the value representing the type of the ResultSet objects
 *            and the value can be any one of the following
 *            ResultSet.TYPE_FORWARD_ONLY, ResultSet.TYPE_SCROLL_SENSITIVE
 *            and ResultSet.TYPE_SCROLL_INSENSITIVE.
 *            (See JDK1.2.2 API documentation)
 *
 * @test_Strategy:    Call Connection.createStatement with the Type mode as
 *            TYPE_SCROLL_INSENSITIVE and call getResultSetType() method
 *            It should return a int value and the value should be equal
 *            to ResultSet.TYPE_SCROLL_INSENSITIVE
 */
/* throws Exception */
void StatementTest::testGetResultSetType03()
{
  int rsType=0;
  Statement statemt /*= NULL*/;

  logMsg("Creating Statement object with the ResultSet Type and Type");
  statemt=conn->createStatement(sql::ResultSet::TYPE_SCROLL_INSENSITIVE,
                                sql::ResultSet::CONCUR_READ_ONLY);
  rsType=statemt->getResultSetType();
  if (rsType == sql::ResultSet::TYPE_SCROLL_INSENSITIVE) {
    logMsg(
           "getResultSetType method returns TYPE_SCROLL_INSENSITIVE "
           + rsType);
  } else {
    statemt->close();
    logErr(" getResultSetType method does not return a valid value");
    FAIL("Call to getResultSetType is Failed!");
  }
  statemt->close();
}

/*
 * @testName:         testGetUpdateCount01
 *
 * @assertion:        A driver must provide support for Statement and
 *            ResultSet.  This implies that the methods in the
 *            Statement interface must be implemented and must behave as
 *            specified in the JDBC 1.0 and 2.0 specifications.        (See
 *            Section :40.3 Statement Methods JDBC 2.0 API Tutorial
 *            & Reference).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getUpdateCount() method should return a integer value;
 *            the value might be greater than 0 representing the rows affected;
 *            0 if no rows are affected or if DDL statement; -1 if the result
 *            is a ResultSet object or there are no more results
 *            (See JDK 1.2.2 API Documentation)
 *
 * @test_Strategy:    Get a Statement object and call the execute() method for
 *            updating a row and call getUpdateCount() method
 *            It should return a int value and the value should be
 *            equal to number of rows with the specified condition for update
 */
#endif

/* throws Exception */
void StatementTest::testGetUpdateCount01()
{
  createStandardTable(TABLE_CTSTABLE2);
  int updCountVal=0;
  int rowsAffectVal=0;
  String sSqlStmt=sqlProps[ "Upd_Coffee_Tab" ];

  logMsg(String("Query String : ") + sSqlStmt);
  stmt->execute(sSqlStmt);

  logMsg("Calling getUpdateCount on Statement");
  updCountVal=static_cast<int> (stmt->getUpdateCount());

  String sQuery=sqlProps[ "Coffee_Updcount_Query" ];
  logMsg(String("Query String : ") + sQuery);

  ResultSet rs1(stmt->executeQuery(sQuery));
  rs1->next();

  rowsAffectVal=rs1->getInt(1);

  TestsListener::messagesLog()
          << "Number of Rows Affected by Update Statement " << rowsAffectVal
          << std::endl;

  if (updCountVal == rowsAffectVal) {
    TestsListener::messagesLog()
            << "getUpdateCount method returns :" << updCountVal << std::endl;
  } else {
    logErr(" getUpdateCount method returns a invalid value");
    FAIL("Call to getUpdateCount is Failed!");
  }
}

/*
 * @testName:         testGetUpdateCount02
 *
 * @assertion:        A driver must provide support for Statement and
 *            ResultSet.  This implies that the methods in the
 *            Statement interface must be implemented and must behave as
 *            specified in the JDBC 1.0 and 2.0 specifications.        (See
 *            Section :40.3 Statement Methods JDBC 2.0 API Tutorial
 *            & Reference).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getUpdateCount() method should return a integer value;
 *            the value might be greater than 0 representing the rows affected;
 *            0 if no rows are affected or if DDL statement; -1 if the result
 *            is a ResultSet object or there are no more results
 *            (See JDK 1.2.2 API Documentation)
 *
 * @test_Strategy:    Get a Statement object and call the execute() method for
 *            selecting a non-existent row and call getUpdateCount() method
 *            It should return a int value and the value should be
 *            equal to -1
 */

/* throws Exception */
void StatementTest::testGetUpdateCount02()
{
  createStandardTable(TABLE_CTSTABLE2);
  int updCountVal=0;
  String sSqlStmt=sqlProps[ "SelCoffeeNull" ];

  logMsg(String("Query String : ") + sSqlStmt);
  stmt->execute(sSqlStmt);

  logMsg("Calling getMoreResults on Statement");
  updCountVal=static_cast<int> (stmt->getUpdateCount());

  if (updCountVal == -1) {
    logMsg("getUpdateCount method returns : -1");
  } else {
    logErr(" getUpdateCount method returns a invalid value");
    FAIL("Call to getUpdateCount is Failed!");
  }
}

/*
 * @testName:         testGetWarnings
 *
 * @assertion:        A driver must provide support for Statement and
 *            ResultSet.  This implies that the methods in the
 *            Statement interface must be implemented and must behave as
 *            specified in the JDBC 1.0 and 2.0 specifications.        (See
 *            Section :40.3 Statement Methods JDBC 2.0 API Tutorial
 *            & Reference).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The getWarnings() method should return a SQLWarning object;
 *            or null if there are no warnings (See JDK 1.2.2 API
 *            Documentation)
 *
 * @test_Strategy:    Get a Statement object and call getWarnings() method
 *            should return an SQLWarning object
 */

/* throws Exception */
void StatementTest::testGetWarnings()
{
  createStandardTable(TABLE_INTEGERTAB);
  if (hasSps) {
    initTable("Integer_Tab", sqlProps, conn);

    //TODO: Need to invent somth to generate warnings instead of this
#ifdef  WE_GOT_SMTH2DEAL_WITH_SP

    CallableStatement cstmt=conn->prepareCall(
                                              "{call Integer_Proc(?,?,?)}");
    logMsg(String("The Callable Statement ") + cstmt);
    cstmt->registerOutParameter(1, "cppconn/datatype.h".INTEGER);
    cstmt->registerOutParameter(2, "cppconn/datatype.h".INTEGER);
    cstmt->registerOutParameter(3, "cppconn/datatype.h".INTEGER);
    cstmt->execute();

    int nRetVal=cstmt->getInt(1);

    Statement state(cstmt);
#else
    sql::Statement * state=stmt.get();
#endif

    logMsg("Calling getWarnings method");
    const sql::SQLWarning * sWarning=state->getWarnings();

    if (sWarning != NULL)
    {
      logMsg("getWarnings method returns a SQLWarning object");
    } else if (sWarning == NULL)
    {
      logErr("getWarnings() method returns a NULL SQLWarning Object");
    }

    clearTable("Integer_Tab", conn);
  }
}


#ifdef MISSING_METHODS_INCLUDED2STATEMENT
/*
 * @testName:         testSetFetchDirection04
 * @assertion:        A driver must provide support for Statement and
 *            ResultSet.  This implies that the methods in the
 *            Statement interface must be implemented and must behave as
 *            specified in the JDBC 1.0 and 2.0 specifications.        (See
 *            Section :40.3 Statement Methods JDBC 2.0 API Tutorial
 *            & Reference).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The setFetchDirection(int dir) method sets the statement
 *            object's fetch direction.  The setFetchDirection method does
 *            not return any value. (See JDK 1.2.2 API Documentation)
 *
 * @test_Strategy:    Get a Statement object and call the setFetchDirection(int direction)
 *            method with an invalid value and it should throw an SQLException
 */
/* throws Exception */
void StatementTest::testSetFetchDirection04()
{
  bool sqlExceptFlag=false;

  logMsg("Calling setFetchDirection method ");

  try
  {
    stmt->setFetchDirection(-1);
  } catch (sql::SQLException * sqe) {
    sqlExceptFlag=true;
  }

  if (sqlExceptFlag) {
    logMsg(
           "setFetchDirection method does not sets the invalid direction for the ResultSet ");
  } else {
    logErr(
           "setFetchDirection method sets the invalid direction for ResultSet");
    FAIL("Call to setFetchDirection is Failed");
  }
}
#endif


#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
/*
 * @testName:         testSetFetchSize02
 * @assertion:        A driver must provide support for Statement and
 *            ResultSet.  This implies that the methods in the
 *            Statement interface must be implemented and must behave as
 *            specified in the JDBC 1.0 and 2.0 specifications.        (See
 *            Section :40.3 Statement Methods JDBC 2.0 API Tutorial
 *            & Reference).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The setFetchSize(int rowsize) method sets the number of rows
 *            to fetch from the database specified by the value of rowsize.
 *            The setFetchSize does not return any value. (See JDK 1.2.2
 *            API Documentation)
 *
 * @test_Strategy:    Get a Statement object and call the setFetchSize(int rows)
 *            method with the value of Statement.getMaxRows and call
 *            getFetchSize() method and it should return a int value
 *            that is been set
 */
/* throws Exception */
void StatementTest::testSetFetchSize02()
{
  int maxFetchSizeVal=50;
  int maxRowsVal=0;
  int retVal=0;

  stmt->setMaxRows(maxFetchSizeVal);
  maxRowsVal=static_cast<int> (stmt->getMaxRows());

  TestsListener::messagesLog()
          << "Maximum Rows that Statement can contain " << maxRowsVal << std::endl;

  logMsg("Calling the setFetchSize method");
  stmt->setFetchSize(maxRowsVal);
  retVal=stmt->getFetchSize();

  if (maxFetchSizeVal == retVal) {
    logMsg(
           "setFetchSize method sets the value as FetchSize for ResultSet");
  } else {
    logErr(
           "setFetchSize method does not set the value as Fetch Size for ResultSet");
    FAIL("Call to setFetchSize is Failed");
  }
}


/*
 * @testName:         testSetFetchSize05
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The setFetchSize(int size) method sets the number of rows
 *            to fetch from the database specified by the value size.
 *            The method does not return any value and throws SQLException
 *            if a database access error occurs or the condition
 *            0 <= size <= this.getMaxRows is not satisfied.
 *            (See JDK 1.2.2 API Documentation)
 *
 * @test_Strategy:    Get a Statement object and call the setFetchSize(int rows)
 *            method with the negative value and it should throw
 *            SQLException
 *
 */
/* throws Exception */
void StatementTest::testSetFetchSize05()
{
  int maxFetchSizeVal=0;
  String sMaxFetchSizeVal;
  bool sqlExceptFlag=false;

  sMaxFetchSizeVal=sqlProps[ "Max_Set_Val" ];
  maxFetchSizeVal=StringUtils::toInt(sMaxFetchSizeVal);
  maxFetchSizeVal=maxFetchSizeVal * (-1);

  TestsListener::messagesLog()
          << "Maximum Value to be set as Fetch Size " << maxFetchSizeVal << std::endl;

  logMsg("Calling setFetchSize method");

  try
  {
    stmt->setFetchSize(maxFetchSizeVal);
  } catch (sql::SQLException &) {
    sqlExceptFlag=true;
  }

  if (sqlExceptFlag) {
    logMsg("setFetchSize method does not set the invalid value ");
  } else {
    logErr("setFetchSize method sets the Invalid value ");
    FAIL("Call to setFetchSize is Failed");
  }
}


/*
 * @testName:         testSetMaxFieldSize01
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The setMaxFieldSize(int maxsize) method sets the maximum size
 *            for a column in a result set specified by the value maxsize (in
 *            bytes). For maximum portability, the maximum field size should
 *            be set to a value greater than 256. If the value maxsize is 0
 *            then it means that there is no limit to the size of a column.
 *            The setMaxFieldSize(int maxsize) does not return any value.
 *            (See JDK 1.2.2 API Documentation)
 *
 * @test_Strategy:    Get a Statement object and call the setMaxFieldSize(int max)
 *            method and call getMaxFieldSize() method and it should return
 *            an integer value that is been set
 *
 */
/* throws Exception */
void StatementTest::testSetMaxFieldSize01()
{
  int maxFieldSizeVal=0;
  String sMaxFieldSizeVal;
  int retVal=0;

  sMaxFieldSizeVal=sqlProps[ "Max_Set_Val" ];
  maxFieldSizeVal=StringUtils::toInt(sMaxFieldSizeVal);
  maxFieldSizeVal=maxFieldSizeVal * 256;

  TestsListener::messagesLog()
          << "Maximum Field Size Value to be set " << maxFieldSizeVal << std::endl;

  logMsg("Calling maxFieldSize method ");
  stmt->setMaxFieldSize(maxFieldSizeVal);
  retVal=stmt->getMaxFieldSize();

  if (maxFieldSizeVal == retVal) {
    logMsg(
           "setMaxFieldSize method sets the value for Maximum Field Size");
  } else {
    logErr(
           "setMaxFieldSize method does not set the value for Maximum Field Size");
    FAIL("Call to setMaxFieldSize is Failed");
  }
}


/*
 * @testName:         testSetMaxFieldSize02
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The setMaxFieldSize(int maxsize) method sets the maximum size
 *            for a column in a result set specified by the value maxsize (in
 *            bytes). For maximum portability, the maximum field size should
 *            be set to a value greater than 256. If the value maxsize is 0
 *            then it means that there is no limit to the size of a column.
 *            The setMaxFieldSize(int maxsize) does not return any value.
 *            (See JDK 1.2.2 API Documentation)
 *
 * @test_Strategy:    Get a Statement object and call the setMaxFieldSize(int max)
 *            method with an invalid value (negative value) and It should
 *            throw a SQLException
 *
 */

/* throws Exception */
void StatementTest::testSetMaxFieldSize02()
{
  int maxFieldSizeVal=0;
  String sMaxFieldSizeVal;
  bool sqlExceptFlag=false;

  sMaxFieldSizeVal=sqlProps[ "Max_Set_Val" ];
  maxFieldSizeVal=StringUtils::toInt(sMaxFieldSizeVal);
  maxFieldSizeVal=maxFieldSizeVal * (-1);

  TestsListener::messagesLog()
          << "Rows Value to be set " << maxFieldSizeVal << std::endl;
  logMsg("Calling the setMaxFieldSize method");

  try
  {
    stmt->setMaxFieldSize(maxFieldSizeVal);
  } catch (sql::SQLException &) {
    sqlExceptFlag=true;
  }

  if (sqlExceptFlag) {
    logMsg("setMaxFieldSize method does not set the Invalid value ");
  } else {
    logErr("setMaxFieldSize method sets the Invalid value");
    FAIL("Call to setMaxFieldSize is Failed");
  }
}


/*
 * @testName:         testSetMaxRows01
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The setMaxRows(int maxsize) method sets the maximum number
 *            of rows that any ResultSet object can contain is specified
 *            by the value maxsize. If the value maxsize is 0 then it means
 *            that there is no limit. The setMaxRows(int maxsize) does not
 *            return any value.(See JDK 1.2.2 API Documentation)
 *
 * @test_Strategy:    Get a Statement object and call the setMaxRows(int rows)
 *            method and call getMaxRows() method and it should return a
 *            integer value that is been set
 *
 */
/* throws Exception */
void StatementTest::testSetMaxRows01()
{
  int maxRowsVal=0;
  String sMaxRowsVal;
  int retVal=0;

  sMaxRowsVal=sqlProps[ "Max_Set_Val" ];
  maxRowsVal=StringUtils::toInt(sMaxRowsVal);
  TestsListener::messagesLog()
          << "Maximum Rows Value to be set " << maxRowsVal << std::endl;

  logMsg("Calling maxRowsVal method");
  stmt->setMaxRows(maxRowsVal);

  retVal=static_cast<int> (stmt->getMaxRows());

  if (maxRowsVal == retVal) {
    logMsg("setMaxRows method sets the value for Maximum Rows");
  } else {
    logErr(
           "setMaxRows method does not set the value for Maximum Rows");
    FAIL("Call to setMaxRows is Failed");
  }
}


/*
 * @testName:         testSetMaxRows02
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The setMaxRows(int maxsize) method sets the maximum number
 *            of rows that any ResultSet object can contain is specified
 *            by the value maxsize. If the value maxsize is 0 then it means
 *            that there is no limit. The setMaxRows(int maxsize) does not
 *            return any value.(See JDK 1.2.2 API Documentation)
 *
 * @test_Strategy:    Get a Statement object and call the setMaxRows(int rows)
 *            method with an invalid value (negative value) and It should
 *            throw an SQLException
 *
 */

/* throws Exception */
void StatementTest::testSetMaxRows02()
{
  int maxRowsVal=0;
  String sMaxRowsVal;
  bool sqlExceptFlag=false;

  sMaxRowsVal=sqlProps[ "Max_Set_Val" ];
  maxRowsVal=StringUtils::toInt(sMaxRowsVal);
  maxRowsVal=maxRowsVal * (-1);
  TestsListener::messagesLog()
          << "Rows Value to be set " << maxRowsVal << std::endl;
  logMsg("Calling setMaxRows method");

  try
  {
    stmt->setMaxRows(maxRowsVal);
  } catch (sql::SQLException &) {
    sqlExceptFlag=true;
  }

  if (sqlExceptFlag) {
    logMsg("setMaxRows method does not set the Invalid value ");
  } else {
    logErr("setMaxRows method sets the Invalid value");
    FAIL("Call to setMaxRows is Failed");
  }
}


/*
 * @testName:         testSetQueryTimeout02
 * @assertion:        The Statement object provides methods for executing SQL
 *            statements and retrieving results.(See section 40.1 of
 *            JDBC 2.0 API Reference & Tutorial second edition).
 *
 *            The driver must provide full support for Statement methods.
 *            The driver must also support all the methods for executing
 *            SQL Statements in a single batch (Batch Updates). (See
 *            section 6.2.2.3 of Java2 Platform Enterprise Edition(J2EE)
 *            Specification v1.2)
 *
 *            The setQueryTimeout(int secval) method sets the time limit
 *            for the number of secval seconds a driver will wait for a
 *            statement object to be executed. If the value secval is 0
 *            then it means that there is no limit. The setQueryTimeout
 *            method does not return any value. (See JDK 1.2.2 API
 *            Documentation)
 *
 * @test_Strategy:    Get a Statement object and call the setQueryTimeout(int secval)
 *            method with an invalid value (negative value)and It should
 *            throw an SQLException
 *
 */

/* throws Exception */
void StatementTest::testSetQueryTimeout02()
{
  int maxQueryTimeVal=0;
  String sMaxQueryTimeVal;
  bool sqlExceptFlag=false;

  sMaxQueryTimeVal=sqlProps[ "Max_Set_Val" ];
  maxQueryTimeVal=StringUtils::toInt(sMaxQueryTimeVal);
  maxQueryTimeVal=maxQueryTimeVal * (-1);
  TestsListener::messagesLog()
          << "Seconds Value to be set as QueryTimeout " << maxQueryTimeVal
          << std::endl;
  logMsg("Calling maxQueryTimeout method");

  try
  {
    stmt->setQueryTimeout(maxQueryTimeVal);
  } catch (sql::SQLException &) {
    sqlExceptFlag=true;
  }

  if (sqlExceptFlag) {
    logMsg("setQueryTimeout method does not set the Invalid value ");
  } else {
    logErr("setQueryTimeout method sets the Invalid value");
    FAIL("Call to setQueryTimeout is Failed");
  }
}
#endif

}
}
