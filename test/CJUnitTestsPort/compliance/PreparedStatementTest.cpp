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




#include <jdbc.h>

#include "PreparedStatementTest.h"


namespace testsuite
{
namespace compliance
{
/*
 * @testName:         testGetMetaData
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods.  (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The getMetadata() method returns a valid ResultSetMetaData Object
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Execute the getMetaData() method and get the number of columns using
 *		      getColumnCount() method of ResultSetMetaData.Execute a query using
 *		      executeQuery() method and get the number of columns. Both the values
 *		      should be equal or it should throw an SQL exception.
 *
 */

/* throws Exception */
void PreparedStatementTest::testGetMetaData()
{
  ResultSet res;
  ResultSetMetaData rsmd;
  ResultSetMetaData rsmdPrep;
  String sPrepStmt=sqlProps[ "SelCoffeeAll" ];
  createStandardTable(TABLE_CTSTABLE2);
  pstmt.reset(conn->prepareStatement(sPrepStmt));

  try
  {
    logMsg("Getting MetaData from PreparedStatement");

    /*
    prepared statement class currently doesn't contain (get) metadata
    so, probably test doesn't make sense. Changing to compare result sets metadata
     */
    //rsmdPrep= pstmt->getMetaData();
    res.reset(pstmt->executeQuery());
    rsmdPrep=res->getMetaData();

    logMsg(String("Executing Query : ") + sPrepStmt);

    rs.reset(stmt->executeQuery(sPrepStmt));
    logMsg("Getting MetaData from ResultSet");
    rsmd=rs->getMetaData();

    if (rsmdPrep->getColumnCount() == rsmd->getColumnCount())
      logMsg("Call to getMetaData Method is Passed");
    else
    {
      logErr("getMetaData Method does not return a valid ResultSetMetaData");
      FAIL("Call to getMetaData Method is Failed!");
    }

  }
  catch (sql::SQLException & sqe)
  {
    logErr(String("SQL std::exception * ") + sqe.what());
  }

}

/*
 * @testName:         testClearParameters
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0
 *                    API Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for Prepared Statement
 *                    methods.  (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The method clearParameters() clears the values set for the
 *                    PreparedStatement object's IN parameters and releases the
 *                    resources used by those values.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Set the value for the IN parameter of the Prepared Statement object.
 *                    Call the clearParameters() method.Call the executeQuery() method
 *                    to check if the call to clearParameters() clears the IN parameter
 *                    set by the Prepared Statement object.
 */

/* throws Exception */
void PreparedStatementTest::testClearParameters()

{
  ResultSet reSet;
  bool sqlexcflag=false;
  String sPrepStmt=sqlProps[ "CoffeeTab_Query" ];
  createStandardTable(TABLE_CTSTABLE2);
  logMsg(String("Prepared Statement String :") + sPrepStmt);
  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setInt(1, 1);
  pstmt->clearParameters();

  try
  {
    reSet.reset(pstmt->executeQuery());
  }
  catch (sql::SQLException &)
  {
    sqlexcflag=true;
  }

  if (sqlexcflag)
  {
    logMsg("clearParameters Method clears the current Parameters ");
  }
  else
  {
    logErr("clearParameters Method does not clear the current Parameters");
    FAIL("Call to clearParameters Method is Failed!");
  }
}

/*
 * @testName:         testExecute01
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The execute()method executes the SQL statement contained in
 *                    the Prepared Statement object and indicates whether the first
 *                    result is a resultset,an update count,or there are no results.
 *                    (See JDK 1.2.2 API Documentation)
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Set the value for the IN parameter of the Prepared Statement
 *                    object. Execute the precompiled SQL Statement of deleting a row.
 *                    It should return a boolean value and the value should be equal to
 *                    false.  (See JDK 1.2.2 API documentation)
 *
 */

/* throws Exception */
void PreparedStatementTest::testExecute01()
{
  bool retValue;
  String sPrepStmt=sqlProps[ "CoffeeTab_Delete" ];
  createStandardTable(TABLE_CTSTABLE2);

  logMsg(String("Prepared Statement String :") + sPrepStmt);
  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setInt(1, 1);
  retValue=pstmt->execute();

  if (!retValue)
  {
    logMsg("execute Method executes the SQL Statement ");
  }
  else
  {
    logErr("execute Method does not execute the SQL Statement");
    FAIL("Call to execute Method is Failed!");
  }
}

/*
 * @testName:         testExecute02
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0
 *                    API Reference & Tutorial,Second Edition)
 *                    The drivers must provide full support for Prepared Statement
 *                    methods.  (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The execute()method executes the SQL statement contained in the
 *                    Prepared Statement object and indicates whether the first result
 *                    is a resultset,an update count,or there are no results.
 *                    (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Set the value for the IN parameter of the Prepared Statement object.
 *                    Execute the precompiled SQL Statement by calling executeQuery()
 *                    method with a non-existent row.Call ResultSet.next() method.
 *                    It should return a boolean value and the value should be equal
 *                    to false. (See JDK 1.2.2 API documentation)
 *
 */

/* throws Exception */
void PreparedStatementTest::testExecute02()
{
  bool retValue;
  String sPrepStmt=sqlProps[ "CoffeeTab_Delete" ];
  createStandardTable(TABLE_CTSTABLE2);

  logMsg(String("Prepared Statement String :") + sPrepStmt);
  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setInt(1, 0);
  retValue=pstmt->execute();
  if (!retValue)
  {
    logMsg("execute Method executes the SQL Statement ");
  }
  else
  {
    logErr("execute Method does not execute the SQL Statment");
    FAIL("Call to execute Method is Failed!");
  }
}

/*
 * @testName:         testExecute03
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    edition(J2EE) specification v 1.2).
 *
 *                    The execute() method executes the SQL statement contained in
 *                    the Prepared Statement object and indicates whether the first
 *                    result is a resultset,an update count,or there are no results.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Execute the precompiled SQL Statement by calling execute() method
 *                    without setting the parameters.An SQL Exception must be thrown.
 *                    (See JDK 1.2.2 API documentation)
 *
 */

/* throws Exception */
void PreparedStatementTest::testExecute03()

{
  bool sqlexcflag=false;
  String sPrepStmt=sqlProps[ "CoffeeTab_Delete" ];
  createStandardTable(TABLE_CTSTABLE2);

  logMsg(String("Prepared Statement String :") + sPrepStmt);
  pstmt.reset(conn->prepareStatement(sPrepStmt));

  try
  {
    pstmt->execute();
  }
  catch (sql::SQLException &)
  {
    sqlexcflag=true;
  }

  if (sqlexcflag)
  {
    logMsg("execute Method executes the SQL Statement ");
  }
  else
  {
    logErr("execute Method does not execute the SQL Statment");
    FAIL("Call to execute Method is Failed!");
  }
}

/*
 * @testName:         testExecuteQuery01
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0
 *                    API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The executeQuery() method executes SQL query in this
 *                    PreparedStatement object and returns the result set generated
 *                    by the query.(See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Set the value for the IN parameter of the Prepared Statement object.
 *                    Execute the precompiled SQL Statement by calling executeQuery()
 *                    method. It should return a ResultSet object.
 *
 */

/* throws Exception */
void PreparedStatementTest::testExecuteQuery01()
{
  ResultSet reSet;
  const String & sPrepStmt=sqlProps[ "CoffeeTab_Query" ];
  createStandardTable(TABLE_CTSTABLE2);

  logMsg(String("Prepared Statement String :") + sPrepStmt);
  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setInt(1, 1);
  reSet.reset(pstmt->executeQuery());

  if (reSet.get() != NULL)
  {
    logMsg("executeQuery Method executes the SQL Statement ");
  }
  else
  {
    logErr("executeQuery Method does not execute the SQL Statment");
    FAIL("Call to executeQuery Method is Failed!");
  }
}


/*
 * @testName:         testExecuteQuery02
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The executeQuery() method executes SQL query in this
 *                    PreparedStatement object and returns the result set generated
 *                    by the query. (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Set the value for the IN parameter of the Prepared Statement object.
 *                    Execute the precompiled SQL Statement by calling executeQuery()
 *                    method with a non existent row.  A call to ResultSet.next()
 *                    should return a false value.
 */

/* throws Exception */
void PreparedStatementTest::testExecuteQuery02()
{
  ResultSet reSet;
  String sPrepStmt=sqlProps[ "CoffeeTab_Query" ];
  createStandardTable(TABLE_CTSTABLE2);

  logMsg(String("Prepared Statement String :") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setInt(1, 0);
  reSet.reset(pstmt->executeQuery());

  if (!reSet->next())
  {
    logMsg("executeQuery Method executes the SQL Statement ");
  }
  else
  {
    logErr("executeQuery Method does not execute the SQL Statment");
    FAIL("Call to executeQuery Method is Failed!");
  }
}

/*
 * @testName:         testExecuteQuery03
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters).(See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The executeQuery() method executes SQL query in this
 *                    PreparedStatement object and returns the result set generated
 *                    by the query. (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the
 *                    database. Execute the precompiled SQL Statement by calling
 *                    executeQuery() method without setting the parameters.
 *                    It should throw a SQL Exception.
 */

/* throws Exception */
void PreparedStatementTest::testExecuteQuery03()
{
  ResultSet reSet;
  bool sqlexcflag=false;
  String sPrepStmt=sqlProps[ "CoffeeTab_Query" ];
  createStandardTable(TABLE_CTSTABLE2);

  logMsg(String("Prepared Statement String :") + sPrepStmt);
  pstmt.reset(conn->prepareStatement(sPrepStmt));

  try
  {
    reSet.reset(pstmt->executeQuery());
  }
  catch (sql::SQLException &)
  {
    sqlexcflag=true;
  }

  if (sqlexcflag)
  {
    logMsg("executeQuery Method executes the SQL Statement ");
  }
  else
  {
    logErr("executeQuery Method does not execute the SQL Statment");
    FAIL("Call to executeQuery Method is Failed!");
  }
}

/*
 * @testName:         testExecuteUpdate01
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.  It can take one or more parameters as input
 *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The executeUpdate(String sql) method executes an SQL Insert,
 *                    Update or Delete Statement and returns the number of rows that
 *                    were affected.It can also be used to execute SQL statements
 *                    that have no return value such as DDL statements that create
 *                    or drop tables.  (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the
 *                    database. Set the value for the IN parameter of the PreparedStatement
 *                    object. Execute the precompiled SQL Statement by calling
 *                    executeUpdate() method. It should return an integer
 *                    value indicating the number of rows that were affected.
 *                    (The value could be zero if zero rows are affected).
 */

/* throws Exception */
void PreparedStatementTest::testExecuteUpdate01()
{
  int retValue;
  const String & sPrepStmt=sqlProps[ "CoffeeTab_Delete" ];
  createStandardTable(TABLE_CTSTABLE2);

  logMsg(String("Prepared Statement String :") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setInt(1, 1);
  retValue=pstmt->executeUpdate();

  if (retValue >= 0)
  {
    logMsg("executeUpdate Method executes the SQL Statement ");
  }
  else
  {
    logErr("executeUpdate Method does not execute the SQL Statment");
    FAIL("Call to executeUpdate Method is Failed!");
  }
}

/*
 * @testName:         testExecuteUpdate02
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters) (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The executeUpdate() method executes an SQL Insert,
 *                    Update or Delete Statement and returns the number of rows that
 *                    were affected.It can also be used to execute SQL statements
 *                    that have no return value such as DDL statements that create
 *                    or drop tables.(The value could be zero if zero rows are affected).
 *                    (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Set the value for the IN parameter of the Prepared Statement
 *                    object. Execute the precompiled SQL Statement by calling
 *                    executeUpdate() method with a non existent row.
 *                    It should return an int value.
 */

/* throws Exception */
void PreparedStatementTest::testExecuteUpdate02()
{
  int retValue;
  String sPrepStmt=sqlProps[ "CoffeeTab_Delete" ];
  createStandardTable(TABLE_CTSTABLE2);

  logMsg(String("Prepared Statement String :") + sPrepStmt);
  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setInt(1, 0);
  retValue=pstmt->executeUpdate();

  if (retValue >= 0)
  {
    logMsg("executeUpdate Method executes the SQL Statement ");
  }
  else
  {
    logErr("executeUpdate Method does not execute the SQL Statment");
    FAIL("Call to executeUpdate Method is Failed!");
  }
}

/*
 * @testName:         testExecuteUpdate03
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods.  (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The executeUpdate() method executes an SQL Insert,Update or
 *                    Delete Statement and returns the number of rows that were affected.
 *                    It can also be used to execute SQL statements that have no
 *                    return value such as DDL statements that create or drop tables.
 *                    (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Execute the precompiled SQL Statement without setting the IN parameter.
 *                    It should throw an SQL exception.
 */

/* throws Exception */
void PreparedStatementTest::testExecuteUpdate03()
{
  bool sqlexcflag=false;
  String sPrepStmt=sqlProps[ "CoffeeTab_Delete" ];
  createStandardTable(TABLE_CTSTABLE2);

  logMsg(String("Prepared Statement String :") + sPrepStmt);
  pstmt.reset(conn->prepareStatement(sPrepStmt));
  try
  {
    pstmt->executeUpdate();
  }
  catch (sql::SQLException &)
  {
    sqlexcflag=true;
  }
  if (sqlexcflag)
  {
    logMsg("executeUpdate Method executes the SQL Statement ");
  }
  else
  {
    logErr("executeUpdate Method does not execute the SQL Statment");
    FAIL("Call to executeUpdate Method is Failed!");
  }
}

/*
 * @testName:         testSetBigDecimal01
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition).
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods.  (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setBigDecimal(int parameterIndex, BigDecimal x) method
 *                    sets the parameterIndex to x which is an instance of
 *                    java.math.BigDecimal class. The Driver converts this  to a
 *                    JDBC Numeric Value when it sends it to the database. (See
 *                    section 24.3.2 of JDBC 2.0 API Tutorial & Reference,2nd Edition).
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Execute the precompiled.SQL Statement by calling the
 *                    setBigDecimal(int parameterindex, BigDecimal x) method for updating
 *                    the value of column MIN_VAL in Numeric_Tab.Check first
 *                    the return value of executeUpdate() method used is equal to 1.
 *                    Call the ResultSet.getBigDecimal(int columnIndex)method.
 *                    Check if returns the BigDecimal Value that has been set.
 */


/* throws Exception */
/* setBigDecimal is not included into current interface */
#ifdef __SET_BIG_DECIMAL_ADDED_

void PreparedStatementTest::testSetBigDecimal01()

{
  BigDecimal rBigDecimalVal;
  BigDecimal minBigDecimalVal;

  initTable("Numeric_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Numeric_Tab_Min_Update" ];
  logMsg(String("Prepared Statement String :") + sPrepStmt);
  String sminBigDecimalVal=extractVal("Numeric_Tab", 2, sqlProps, conn);
  minBigDecimalVal=new BigDecimal(sminBigDecimalVal);
  logMsg(String("BigDecimal Value :") + minBigDecimalVal);
  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setBigDecimal(1, minBigDecimalVal);
  int retVal=pstmt->executeUpdate();

  TestsListener::messagesLog() << "Return Value "
          << retVal << std::endl;

  if (retVal != 1)
  {
    logErr("Minimum Value not being updated in the Min_Val column");
    FAIL("Call to setBigDecimal Method is Failed!");
  }
  String Min_Val_Query=sqlProps[ "Numeric_Query_Min" ];
  logMsg(Min_Val_Query);
  rs.reset(stmt->executeQuery(Min_Val_Query));
  rs->next();
  rBigDecimalVal=rs->getBigDecimal(1);
  logMsg(String("Returned BigDecimal Value after Updation:") + rBigDecimalVal);
  if (rBigDecimalVal.compareTo(minBigDecimalVal) == 0)
  {
    logMsg("setBigDecimal Method sets the designated parameter to a BigDecimal value ");
  }
  else
  {
    logErr("setBigDecimal Method does not set the designated parameter to a BigDecimal value ");
    FAIL("Call to setBigDecimal Method is Failed!");
  }

}


/*
 * @testName:         testSetBigDecimal02
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters) (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setBigDecimal(int parameterIndex, BigDecimal x) method
 *                    sets the parameterIndex to x which is an instance of
 *                    java.math.BigDecimal class.The Driver converts this to a JDBC
 *                    Numeric Value when it sends it to the database.(See section
 *                    24.3.2 of JDBC 2.0 API Tutorial & Reference, Second Edition).
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Execute the precompiled SQL Statement by calling the
 *                    setBigDecimal(int parameterindex, BigDecimal x) method for updating
 *                    the value of column NULL_VAL in Numeric_Tab.  Call the
 *                    ResultSet.getBigDecimal(int columnIndex) method. Check if
 *                    returns the BigDecimal Value that has been set.
 */

/* throws Exception */
void PreparedStatementTest::testSetBigDecimal02()
{
  BigDecimal maxBigDecimalVal / *=NULL* /;
  BigDecimal rBigDecimalVal / *=NULL* /;

  initTable("Numeric_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Numeric_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String :") + sPrepStmt);
  String smaxBigDecimalVal=extractVal("Numeric_Tab", 1, sqlProps, conn);
  maxBigDecimalVal=new BigDecimal(smaxBigDecimalVal);
  logMsg(String("BigDecimal Value :") + maxBigDecimalVal);
  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setBigDecimal(1, maxBigDecimalVal);
  pstmt->executeUpdate();
  String Null_Val_Query=sqlProps[ "Numeric_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rBigDecimalVal=rs->getBigDecimal(1);
  logMsg(String("Returned BigDecimal Value after Updation:") + rBigDecimalVal);
  if (rBigDecimalVal.compareTo(maxBigDecimalVal) == 0)
  {
    logMsg("setBigDecimal Method sets the designated parameter to a BigDecimal value ");
  }
  else
  {
    logErr("setBigDecimal Method does not set the designated parameter to a BigDecimal value ");
    FAIL("Call to setBigDecimal Method is Failed!");
  }
}
#endif


/*
 * @testName:         testSetBoolean01
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition).
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setBoolean(int parameterIndex, boolean x) method sets the
 *                    parameterIndex to x, a Java boolean value.The driver converts
 *                    this to JDBC BIT value when it sends it to the database. (See
 *                    section 24.3.2 of JDBC 2.0 API Tutorial & Reference, 2nd Edition).
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Execute the precompiled SQL Statement by calling the
 *                    setBoolean(int parameterIndex, boolean x) to set MAX_VAL
 *                    column of Bit_tab to the boolean value x.
 *                    Call the ResultSet.getBoolean(int columnIndex) method to check
 *                    if it returns the boolean Value that has been set.
 *
 */

/* throws Exception */
void PreparedStatementTest::testSetBoolean01()
{
  createStandardTable(TABLE_BITTAB);
  bool bMinBooleanVal;
  bool rBooleanVal;

  initTable("Bit_Tab", sqlProps, conn);

  const String & sPrepStmt=sqlProps[ "Bit_Tab_Max_Update" ];
  logMsg(String("Prepared Statement String :") + sPrepStmt);

  String sminBooleanVal=extractVal("Bit_Tab", 2, sqlProps, conn);

  bMinBooleanVal=(sminBooleanVal == "true" ? true : false);

  TestsListener::messagesLog()
          << "Boolean Value :" << bMinBooleanVal << std::endl;

  ResultSet tmp_res(stmt->executeQuery("select * from Bit_Tab"));

  TestsListener::messagesLog() << "Row count = " << tmp_res->rowsCount() << std::endl;

  TestsListener::messagesLog() << sPrepStmt << std::endl;

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setBoolean(1, bMinBooleanVal);
  pstmt->executeUpdate();
  String Max_Val_Query=sqlProps[ "Bit_Query_Max" ];
  logMsg(Max_Val_Query);
  rs.reset(stmt->executeQuery(Max_Val_Query));
  rs->next();
  rBooleanVal=rs->getBoolean(1);

  TestsListener::messagesLog()
          << "Returned Boolean Value after Updation:" << rBooleanVal << std::endl;

  if (rBooleanVal == bMinBooleanVal)
  {
    logMsg("setBoolean Method sets the designated parameter to a Boolean value ");
  }
  else
  {
    logErr("setBoolean Method does not set the designated parameter to a Boolean value ");
    FAIL("Call to setBoolean Method is Failed!");
  }
}

/*
 * @testName:         testSetBoolean02
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setBoolean(int parameterIndex, boolean x) method sets the
 *                    parameterIndex to x, a Java boolean value.The driver converts this
 *                    to JDBC BIT value when it sends it to the database. (See section
 *                    24.3.2 of JDBC 2.0 API Tutorial & Reference, Second Edition).
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Execute the precompiled SQL Statement by calling the method
 *                    setBoolean(int parameterIndex, boolean x) to set NULL_VAL
 *                    column of Bit_tab to the boolean value x.
 *                    Call the ResultSet.getBoolean(int columnIndex) method to
 *                    check if it returns the boolean Value that has been set.
 */

/* throws Exception */
void PreparedStatementTest::testSetBoolean02()
{
  createStandardTable(TABLE_BITTAB);
  bool bMaxBooleanVal;
  bool rBooleanVal;

  initTable("Bit_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Bit_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String :") + sPrepStmt);
  String smaxBooleanVal=extractVal("Bit_Tab", 1, sqlProps, conn);
  bMaxBooleanVal=StringUtils::toBoolean(smaxBooleanVal);

  TestsListener::messagesLog()
          << "Boolean Value :" << bMaxBooleanVal << std::endl;

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setBoolean(1, bMaxBooleanVal);
  pstmt->executeUpdate();
  String Null_Val_Query=sqlProps[ "Bit_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rBooleanVal=rs->getBoolean(1);

  TestsListener::messagesLog()
          << "Returned Boolean Value after Updation:" << rBooleanVal << std::endl;

  if (rBooleanVal == bMaxBooleanVal)
  {
    logMsg("setBoolean Method sets the designated parameter to a Boolean value ");
  }
  else
  {
    logErr("setBoolean Method does not set the designated parameter to a Boolean value ");
    FAIL("Call to setBoolean Method is Failed!");
  }
}

/*
 * @testName:         testSetByte01
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setByte(int parameterIndex, byte x) method sets the parameter
 *                    number parameterIndex to x,a Java byte value.The Driver converts
 *                    this to a JDBC Tinyint value when it sends it to the database. (See
 *                    section 24.3.2 of JDBC 2.0 API Tutorial & Reference, 2nd Edition).
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Execute the precompiled SQL Statement by calling the
 *                    setByte(int parameterindex, byte x) method for updating MAX_VAL
 *                    column of Tinyint_Tab.  Call the ResultSet.getByte(int columnIndex)
 *                    method to check if it returns the byte Value that has been set.
 */

/* throws Exception */
/* setByte is not included into current interface */
#ifdef __SET_BYTE_ADDED_

void PreparedStatementTest::testSetByte01()

{
  byte bminByteVal=0;
  byte rByteVal=0;
  try
  {
    initTable("Tinyint_Tab", sqlProps, conn);
    const String & sPrepStmt=sqlProps[ "Tinyint_Tab_Max_Update" ];
    logMsg(String("Prepared Statement String :") + sPrepStmt);
    String sminByteVal=extractVal("Tinyint_Tab", 2, sqlProps, conn);
    bminByteVal=Byte.parseByte(sminByteVal);
    logMsg(String("Byte Value :") + bminByteVal);
    pstmt.reset(conn->prepareStatement(sPrepStmt));
    pstmt->setByte(1, bminByteVal);
    pstmt->executeUpdate();
    String Max_Val_Query=sqlProps[ "Tinyint_Query_Max" ];
    logMsg(Max_Val_Query);
    rs.reset(stmt->executeQuery(Max_Val_Query));
    rs->next();
    rByteVal=rs->getByte(1);
    logMsg(String("Returned Byte Value after Updation:") + rByteVal);
    if (rByteVal == bminByteVal)
    {
      logMsg("setByte Method sets the designated parameter to a Byte value ");
    }
    else
    {
      logErr("setByte Method does not set the designated parameter to a Byte value ");
      FAIL("Call to setByte Method is Failed!");
    }
  }
  catch (sql::SQLException & sqle)
  {
    logErr(String("SQL std::exception * ") + sqle->what());
    FAIL("Call to setByte is Failed!");
  }
  catch (std::exception * e)
  {
    logErr(String("Unexpected std::exception * ") + e->what());
    FAIL("Call to setByte Failed!", e);
  }

  {
    try
    {
      clearTable("Tinyint_Tab", conn);
    }
    catch (std::exception * e)
    {
    }
  }
}


/*
 * @testName:         testSetByte02
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setByte(int parameterIndex, byte x) method sets the parameter
 *                    number parameterIndex to x,a Java byte value.  The Driver converts
 *                    this to a JDBC Tinyint value when it sends it to the database.
 *                    (See section 24.3.2 of JDBC 2.0 API Tutorial & Reference,2nd Edition)
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Execute the precompiled SQL Statement by calling the
 *                    setByte(int parameterindex, byte x) method for updating NULL_VAL
 *                    column of Tinyint_Tab.  Call the ResultSet.getByte(int columnIndex)
 *                    method,to check if it returns the byte Value that has been set.
 */


/* throws Exception */

/* setByte is not included into current interface */

void PreparedStatementTest::testSetByte02()

{
  byte bmaxByteVal=0;
  byte rByteVal=0;
  try
  {
    initTable("Tinyint_Tab", sqlProps, conn);
    const String & sPrepStmt=sqlProps[ "Tinyint_Tab_Null_Update" ];
    logMsg(String("Prepared Statement String :") + sPrepStmt);
    String smaxByteVal=extractVal("Tinyint_Tab", 1, sqlProps, conn);
    bmaxByteVal=Byte.parseByte(smaxByteVal);
    logMsg(String("Byte Value :") + bmaxByteVal);
    pstmt.reset(conn->prepareStatement(sPrepStmt));
    pstmt->setByte(1, bmaxByteVal);
    pstmt->executeUpdate();
    String Null_Val_Query=sqlProps[ "Tinyint_Query_Null" ];
    logMsg(Null_Val_Query);
    rs.reset(stmt->executeQuery(Null_Val_Query));
    rs->next();
    rByteVal=rs->getByte(1);
    logMsg(String("Returned Byte Value after Updation:") + rByteVal);
    if (rByteVal == bmaxByteVal)
    {
      logMsg("setByte Method sets the designated parameter to a Byte value ");
    }
    else
    {
      logErr("setByte Method does not set the designated parameter to a Byte value ");
      FAIL("Call to setByte Method is Failed!");
    }
  }
  catch (sql::SQLException & sqle)
  {
    logErr(String("SQL std::exception * ") + sqle->what());
    FAIL("Call to setByte is Failed!");
  }
  catch (std::exception * e)
  {
    logErr(String("Unexpected std::exception * ") + e->what());
    FAIL("Call to setByte Failed!", e);
  }

  {
    try
    {
      clearTable("Tinyint_Tab", conn);
    }
    catch (std::exception * e)
    {
    }
  }
}
#endif


/*
 * @testName:         testSetFloat01
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods.  (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setFloat(int parameterIndex, float x) method sets the parameter
 *                    number parameterIndex to x ,a Java float value.  The Driver converts
 *                    this to a JDBC REAL when it sends it to the database. (See section
 *                    24.3.2 of JDBC 2.0 API Tutorial & Reference,2nd Edition).
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database
 *                    execute the precompiled SQL Statement by calling the
 *                    setFloat(int parameterindex, float x) method for updating
 *                    the MAX_VAL column of Float_Tab.  Call the
 *                    ResultSet.getFloat(int columnIndex)
 *                    method to check if it returns the float Value that has been set.
 *
 */


/* throws Exception */
/* setFloat is not included into current interface */
#ifdef __SET_FLOAT_ADDED_

void PreparedStatementTest::testSetFloat010101()
{
  float rFloatVal=0;
  float minFloatVal=0;

  initTable("Float_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Float_Tab_Max_Update" ];
  logMsg(String("Prepared Statement String :") + sPrepStmt);
  String sminFloatVal=extractVal("Float_Tab", 2, sqlProps, conn);
  minFloatVal=float.parseFloat(sminFloatVal);
  logMsg(String("float Value :") + minFloatVal);
  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setFloat(1, minFloatVal);
  pstmt->executeUpdate();
  String Max_Val_Query=sqlProps[ "Float_Query_Max" ];
  logMsg(Max_Val_Query);
  rs.reset(stmt->executeQuery(Max_Val_Query));
  rs->next();
  rFloatVal=rs->getFloat(1);
  logMsg(String("Returned float Value after Updation:") + rFloatVal);
  if (rFloatVal == minFloatVal)
  {
    logMsg("setFloat Method sets the designated parameter to a float value ");
  }
  else
  {
    logErr("setFloat Method does not set the designated parameter to a float value ");
    FAIL("Call to setFloat Method is Failed!");
  }
}


/*
 * @testName:         testSetFloat02
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements.It can take one or more parameters as input
 *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setFloat(int parameterIndex, float x) method sets the
 *                    parameter number parameterIndex to x ,a Java float value.
 *                    The Driver converts this to a JDBC REAL when it sends it to
 *                    the database. (See section 24.3.2 of JDBC 2.0 API Tutorial &
 *                    Reference, Second Edition).
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database
 *                    execute the precompiled SQL Statement by calling the
 *                    setFloat(int parameterindex, float x)  method for updating the
 *                    NULL_VAL column of Float_Tab.  Call the
 *                    ResultSet.getFloat(int columnIndex) method to check if it returns
 *                    the float Value that has been set.
 *
 */

/* throws Exception */

/* setFloat is not included into current interface */

void PreparedStatementTest::testSetFloat020202()

{
  float maxFloatVal=0;
  float rFloatVal=0;
  try
  {
    initTable("Float_Tab", sqlProps, conn);
    const String & sPrepStmt=sqlProps[ "Float_Tab_Null_Update" ];
    logMsg(String("Prepared Statement String :") + sPrepStmt);
    String smaxFloatVal=extractVal("Float_Tab", 1, sqlProps, conn);
    maxFloatVal=float.parseFloat(smaxFloatVal);
    logMsg(String("float Value :") + maxFloatVal);
    pstmt.reset(conn->prepareStatement(sPrepStmt));
    pstmt->setFloat(1, maxFloatVal);
    pstmt->executeUpdate();
    String Null_Val_Query=sqlProps[ "Float_Query_Null" ];
    logMsg(Null_Val_Query);
    rs.reset(stmt->executeQuery(Null_Val_Query));
    rs->next();
    rFloatVal=rs->getFloat(1);
    logMsg(String("Returned float Value after Updation:") + rFloatVal);
    if (rFloatVal == maxFloatVal)
    {
      logMsg("setFloat Method sets the designated parameter to a float value ");
    }
    else
    {
      logErr("setFloat Method does not set the designated parameter to a float value ");
      FAIL("Call to setFloat   Method is Failed!");
    }
  }
  catch (sql::SQLException & sqle)
  {
    logErr(String("SQL std::exception * ") + sqle->what());
    FAIL("Call to setFloat is Failed!");
  }
  catch (std::exception * e)
  {
    logErr(String("Unexpected std::exception * ") + e->what());
    FAIL("Call to setFloat Failed!", e);
  }

  {
    try
    {
      clearTable("Float_Tab", conn);
    }
    catch (std::exception * e)
    {
    }
  }
}
#endif

/*
 * @testName:         testSetInt01
 * @assertion:        A Prepared Statement object provides a way of calling precompiled
 *                    SQL statements. It can take one or more parameters as input
 *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
 *                    Reference & Tutorial,Second Edition)
 *
 *                    The JDBC drivers must provide full support for PreparedStatement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setInt(int parameterIndex, int x) method sets the parameter
 *                    number parameterIndex to x ,a Java int value.  The Driver converts
 *                    this to a JDBC INTEGER when it sends it to the database. (See
 *                    section 24.3.2 of JDBC 2.0 API Tutorial & Reference, 2nd Edition)
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database
 *                    execute the precompiled SQL Statement by calling the
 *                    setInt(int parameterindex, int x) method for updating the MAX_VAL
 *                    column of Integer_Tab.  Call the ResultSet.getInt(int columnIndex)
 *                    method to check if it returns the integer Value that has been set.
 *
 */

/* throws Exception */
void PreparedStatementTest::testSetInt01()
{
  createStandardTable(TABLE_INTEGERTAB);
  int rIntegerVal=0;
  int minIntegerVal=0;

  initTable("Integer_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Integer_Tab_Max_Update" ];
  logMsg(String("Prepared Statement String :") + sPrepStmt);

  String sminIntegerVal=extractVal("Integer_Tab", 2, sqlProps, conn);
  minIntegerVal=StringUtils::toInt(sminIntegerVal);
  TestsListener::messagesLog()
          << "int Value :" << minIntegerVal << std::endl;

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setInt(1, minIntegerVal);
  pstmt->executeUpdate();

  String Max_Val_Query=sqlProps[ "Integer_Query_Max" ];
  logMsg(Max_Val_Query);
  rs.reset(stmt->executeQuery(Max_Val_Query));
  rs->next();
  rIntegerVal=rs->getInt(1);
  TestsListener::messagesLog()
          << "Returned int Value after Updation:" << rIntegerVal << std::endl;

  if (rIntegerVal == minIntegerVal)
  {
    logMsg("setInt Method sets the designated parameter to a int value ");
  }
  else
  {
    logErr("setInteger Method does not set the designated parameter to a int value ");
    FAIL("Call to setInt Method is Failed!");
  }
}


/*
 * @testName:         testSetInt02
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setInt(int parameterIndex,int x) method sets parameter number
 *                    parameterIndex to Java int value x. The driver converts this to an
 *                    SQL INTEGER value when it sends it to the Database
 *                    (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database
 *                    Using setInt(int parameterIndex,int x),update the column with the maximum
 *                    value of Integer_Tab. Now execute a query to get the maximum value and
 *                    retrieve the result of the query using the getInt(int columnIndex) method
 *                    Compare the returned value, with the maximum value extracted from the
 *                    ctssql.stmt file. Both of them should be equal
 */

/* throws Exception */
void PreparedStatementTest::testSetInt02()
{
  createStandardTable(TABLE_INTEGERTAB);
  int maxIntegerVal=0;
  int rIntegerVal=0;

  initTable("Integer_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Integer_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  String smaxIntegerVal=extractVal("Integer_Tab", 1, sqlProps, conn);
  maxIntegerVal=StringUtils::toInt(smaxIntegerVal);

  TestsListener::messagesLog()
          << "int Value: " << maxIntegerVal << std::endl;

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setInt(1, maxIntegerVal);
  pstmt->executeUpdate();

  const String & Null_Val_Query=sqlProps[ "Integer_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rIntegerVal=rs->getInt(1);

  TestsListener::messagesLog()
          << "Returned int Value after Updation: " << rIntegerVal << std::endl;
  TestsListener::messagesLog()
          << "Value returned from ctssql.stmt: " << maxIntegerVal << std::endl;

  if (rIntegerVal == maxIntegerVal)
  {
    logMsg("setInt Method sets the designated parameter to a int value ");
  }
  else
  {
    logErr("setInteger Method does not set the designated parameter to a int value ");
    FAIL("Call to setInt   Method is Failed!");
  }
}

/*
 * @testName:         testSetDate01
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setDate(int parameterIndex,Date x) method sets parameter
 *                    number parameterIndex to sql.Date value x. The driver
 *                    converts this to an SQL DATE value when it sends it to the database
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database
 *                    execute the precompiled SQL Statement by calling setDate(int parameterIndex,Date x)
 *                    method and call the ResultSet.getDate(int) method to check and
 *                    it should return a String Value that it is been set
 */


/* throws Exception */
/* setDate is not included into current interface. Need to look if I should to rework it
 * for setDateTime
 */
#ifdef __SET_DATE_ADDED_

void PreparedStatementTest::testSetDate01()
{
  Date mfgDateVal /*= NULL*/;
  Date rDateVal /*= NULL*/;
  String smfgDateVal;

  initTable("Date_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Date_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  smfgDateVal=extractVal("Date_Tab", 1, sqlProps, conn);
  smfgDateVal=smfgDateVal.substr(smfgDateVal.indexOf('\'') + 1, smfgDateVal.lastIndexOf('\''));
  mfgDateVal=sql.Date.valueOf(smfgDateVal);
  logMsg(String("Date Value: ") + mfgDateVal);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setDate(1, mfgDateVal);
  pstmt->executeUpdate();
  String Null_Val_Query=sqlProps[ "Date_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rDateVal=rs->getDate(1);
  logMsg(String("Returned Date Value after Updation: ") + rDateVal);
  logMsg(String("Value returned from ctssql.stmt: ") + mfgDateVal);
  if (rDateVal == mfgDateVal)
  {
    logMsg("setDate Method sets the designated parameter to a Date value ");
  }
  else
  {
    logErr("setDate Method does not set the designated parameter to a Date value ");
    FAIL("Call to setDate Method is Failed!");
  }
}


/*
 * @testName:         testSetDate02
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setDate(int parameterIndex,Date x,Calendar cal) method
 *                    sets the designated parameter to the given sql.Date value,
 *                    using the given Calendar object. The driver uses the Calendar
 *                    object to construct an SQL DATE value, which the driver then sends
 *                    to the database. With a a Calendar object, the driver can calculate
 *                    the date taking into account a custom timezone. If no Calendar
 *                    object is specified, the driver uses the default timezone, which
 *                    is that of the virtual machine running the application.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database
 *                    execute the precompiled SQL Statement by calling
 *                    setDate(int parameterIndex,Date x,Calendar cal) method
 *                    and call the ResultSet.getDate(int) method to check and
 *                    it should return a String Value that it is been set
 */

/* throws Exception */

/* setDate is not included into current interface. Need to look if I should to rework it
 * for setDateTime
 */

void PreparedStatementTest::testSetDate02()
{
  Date mfgDateVal /*= NULL*/;
  Date rDateVal /*= NULL*/;
  String smfgDateVal;
  Calendar cal /*= NULL*/;

  initTable("Date_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Date_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  smfgDateVal=extractVal("Date_Tab", 1, sqlProps, conn);
  smfgDateVal=smfgDateVal.substr(smfgDateVal.indexOf('\'') + 1, smfgDateVal.lastIndexOf('\''));
  mfgDateVal=sql.Date.valueOf(smfgDateVal);
  logMsg(String("Date Value :") + mfgDateVal);
  cal=Calendar.getInstance();
  pstmt=conn->prepareStatement(sPrepStmt);
  logMsg(String("** Setting mfgDateVal") + mfgDateVal);
  pstmt->setDate(1, mfgDateVal, cal);
  pstmt->executeUpdate();
  logMsg("Update query succeeeds");
  const String & Null_Val_Query=sqlProps[ "Date_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  logMsg("Select query succeeds");
  rDateVal=rs->getDate(1, cal);
  logMsg("getDate() succeeds");
  logMsg(String("Returned Date Value after Updation: ") + rDateVal);
  logMsg(String("Value returned from ctssql.stmt: ") + mfgDateVal);
  if (rDateVal == mfgDateVal)
  {
    logMsg("setDate Method sets the designated parameter to a Date value ");
  }
  else
  {
    logErr(String("setDate Method does not set the designated parameter to a Date value ") + rDateVal + "!=" + mfgDateVal);
    FAIL("Call to setDate Method is Failed!");
  }
}
#endif

/*
 * @testName:         testSetDouble01
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setDouble(int parameterIndex, double x) method Sets the
 *                    designated parameter to a Java double value. The driver converts
 *                    this to an SQL DOUBLE value when it sends it to the database.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database
 *                    Using setDouble(int parameterIndex,double x),update the column the
 *                    minimum value of Double_Tab.
 *                    Now execute a query to get the minimum value and retrieve the result
 *                    of the query using the getDouble(int columnIndex) method.Compare the
 *                    returned value, with the minimum value extracted from the ctssql.stmt file.
 *                    Both of them should be equal.
 */

/* throws Exception */
void PreparedStatementTest::testSetDouble01()
{
  createStandardTable(TABLE_DOUBLETAB);
  double rDoubleVal=0;
  double minDoubleVal=0;

  initTable("Double_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Double_Tab_Max_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  String sminDoubleVal=extractVal("Double_Tab", 2, sqlProps, conn);
  minDoubleVal=StringUtils::toDouble(sminDoubleVal);

  TestsListener::messagesLog()
          << "Double Value: " << minDoubleVal << std::endl;

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setDouble(1, minDoubleVal);
  pstmt->executeUpdate();
  const String & Max_Val_Query=sqlProps[ "Double_Query_Max" ];
  logMsg(Max_Val_Query);
  rs.reset(stmt->executeQuery(Max_Val_Query));
  rs->next();
  rDoubleVal=rs->getDouble(1);

  TestsListener::messagesLog()
          << "Returned Double Value after Updation:" << rDoubleVal << std::endl;
  TestsListener::messagesLog()
          << "Value returned from ctssql.stmt: " << minDoubleVal << std::endl;

  if (rDoubleVal == minDoubleVal)
  {
    logMsg("setDouble Method sets the designated parameter to a Double value ");
  }
  else
  {
    logErr("setDouble Method does not set the designated parameter to a Double value ");
    FAIL("Call to setDouble Method is Failed!");
  }
}

/*
 * @testName:         testSetDouble02
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setDouble(int parameterIndex, double x) method Sets the
 *                    designated parameter to a Java double value. The driver converts
 *                    this to an SQL DOUBLE value when it sends it to the database.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Using setDouble(int parameterIndex,double x),update the column
 *                    the maximum value of Double_Tab.
 *                    Now execute a query to get the maximum value and retrieve the result
 *                    of the query using the getDouble(int columnIndex) method.Compare the
 *                    returned value, with the maximum value extracted from the ctssql.stmt file.
 *                    Both of them should be equal.
 */

/* throws Exception */
void PreparedStatementTest::testSetDouble02()
{
  createStandardTable(TABLE_DOUBLETAB);
  double maxDoubleVal=0;
  double rDoubleVal=0;

  initTable("Double_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Double_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  String smaxDoubleVal=extractVal("Double_Tab", 1, sqlProps, conn);
  maxDoubleVal=StringUtils::toDouble(smaxDoubleVal);
  TestsListener::messagesLog()
          << "Double Value: " << maxDoubleVal << std::endl;

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setDouble(1, maxDoubleVal);
  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Double_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rDoubleVal=rs->getDouble(1);

  TestsListener::messagesLog()
          << "Returned Double Value after Updation: " << rDoubleVal << std::endl;
  TestsListener::messagesLog()
          << "Value returned from ctssql.stmt: " << maxDoubleVal << std::endl;

  if (rDoubleVal == maxDoubleVal)
  {
    logMsg("setDouble Method sets the designated parameter to a Double value ");
  }
  else
  {
    logErr("setDouble Method does not set the designated parameter to a Double value ");
    FAIL("Call to setDouble Method is Failed!");
  }
}

/*
 * @testName:         testSetLong01
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setInt64(int parameterIndex, long x) method Sets the
 *                    designated parameter to a Java long value. The driver converts
 *                    this to an SQL BIGINT value when it sends it to the database.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Using setInt64(int parameterIndex,long x),update the column the
 *                    minimum value of Bigint_Tab.
 *                    Now execute a query to get the minimum value and retrieve the result
 *                    of the query using the getInt64(int columnIndex) method.Compare the
 *                    returned value, with the minimum value extracted from the ctssql.stmt file.
 *                    Both of them should be equal.
 */

/* throws Exception */
void PreparedStatementTest::testSetLong01()
{
  createStandardTable(TABLE_BIGINTTAB);
  long long rLongVal=0;
  long long minLongVal=0;

  initTable("Bigint_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Bigint_Tab_Max_Update" ];

  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  String sminLongVal=extractVal("Bigint_Tab", 2, sqlProps, conn);
  minLongVal=StringUtils::toLong(sminLongVal);
  TestsListener::messagesLog()
          << "Long Value: " << minLongVal << std::endl;

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setInt64(1, minLongVal);
  pstmt->executeUpdate();

  const String & Max_Val_Query=sqlProps[ "Bigint_Query_Max" ];

  logMsg(Max_Val_Query);

  rs.reset(stmt->executeQuery(Max_Val_Query));
  rs->next();
  rLongVal=rs->getInt64(1);

  TestsListener::messagesLog()
          << "Returned Long Value after Updation:" << rLongVal << std::endl;
  TestsListener::messagesLog()
          << "Value returned from ctssql.stmt: " << minLongVal << std::endl;

  if (rLongVal == minLongVal)
  {
    logMsg("setInt64 Method sets the designated parameter to a long value ");
  }
  else
  {
    logErr("setInt64 Method does not set the designated parameter to a long value ");
    FAIL("Call to setInt64 Method is Failed!");
  }
}

/*
 * @testName:         testSetLong02
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setInt64(int parameterIndex, long x) method Sets the
 *                    designated parameter to a Java long value. The driver converts
 *                    this to an SQL BIGINT value when it sends it to the database.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Using setInt64(int parameterIndex,long x),update the column the
 *                    maximum value of Bigint_Tab.
 *                    Now execute a query to get the maximum value and retrieve the result
 *                    of the query using the getInt64(int columnIndex) method.Compare the
 *                    returned value, with the maximum value extracted from the ctssql.stmt file.
 *                    Both of them should be equal.
 */

/* throws Exception */
void PreparedStatementTest::testSetLong02()
{
  createStandardTable(TABLE_BIGINTTAB);
  long long maxLongVal=0;
  long long rLongVal=0;

  initTable("Bigint_Tab", sqlProps, conn);

  const String & sPrepStmt=sqlProps[ "Bigint_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  String smaxLongVal=extractVal("Bigint_Tab", 1, sqlProps, conn);
  maxLongVal=StringUtils::toLong(smaxLongVal);

  TestsListener::messagesLog()
          << "Long Value: " << maxLongVal << std::endl;

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setInt64(1, maxLongVal);
  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Bigint_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rLongVal=rs->getInt64(1);

  TestsListener::messagesLog()
          << "Returned Long Value after Updation:" << rLongVal << std::endl;
  TestsListener::messagesLog()
          << "Value returned from ctssql.stmt: " << maxLongVal << std::endl;

  if (rLongVal == maxLongVal)
  {
    logMsg("setInt64 Method sets the designated parameter to a long value ");
  }
  else
  {
    logErr("setInt64 Method does not set the designated parameter to a long value ");
    FAIL("Call to setInt64 Method is Failed!");
  }
}


/*
 * @testName:         testSetShort01
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setShort(int parameterIndex, short x) method Sets the
 *                    designated parameter to a Java short value. The driver converts
 *                    this to an SQL SMALLINT value when it sends it to the database.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Using setShort(int parameterIndex,short x),update the column the
 *                    minimum value of Smallint_Tab.
 *                    Now execute a query to get the minimum value and retrieve the result
 *                    of the query using the getShort(int columnIndex) method.Compare the
 *                    returned value, with the minimum value extracted from the ctssql.stmt file.
 *                    Both of them should be equal.
 */

/* throws Exception */
#ifdef __SET_SHORT_ADDED_

void PreparedStatementTest::testSetShort01()
{
  short rShortVal=0;
  short minShortVal=0;

  initTable("Smallint_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Smallint_Tab_Max_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  String sminShortVal=extractVal("Smallint_Tab", 2, sqlProps, conn);
  minShortVal=StringUtils::toShort(sminShortVal);
  logMsg(String("Short Value: ") + minShortVal);
  pstmt=conn->prepareStatement(sPrepStmt);
  pstmt->setShort(1, minShortVal);
  pstmt->executeUpdate();
  const String & Max_Val_Query=sqlProps[ "Smallint_Query_Max" ];
  logMsg(Max_Val_Query);
  rs.reset(stmt->executeQuery(Max_Val_Query));
  rs->next();
  rShortVal=rs->getShort(1);
  logMsg(String("Returned Short Value after Updation: ") + rShortVal);
  logMsg(String("Value returned from ctssql.stmt: ") + minShortVal);
  if (rShortVal == minShortVal)
  {
    logMsg("setShort Method sets the designated parameter to a Short value ");
  }
  else
  {
    logErr("setShort Method does not set the designated parameter to a Short value ");
    FAIL("Call to setShort Method is Failed!");
  }
}

/*
 * @testName:         testSetShort02
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setShort(int parameterIndex, short x) method Sets the
 *                    designated parameter to a Java short value. The driver converts
 *                    this to an SQL SMALLINT value when it sends it to the database.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Using setShort(int parameterIndex,short x),update the column the
 *                    maximum value of Smallint_Tab.
 *                    Now execute a query to get the maximum value and retrieve the result
 *                    of the query using the getShort(int columnIndex) method.Compare the
 *                    returned value, with the maximum value extracted from the ctssql.stmt file.
 *                    Both of them should be equal.
 */

/* throws Exception */
void PreparedStatementTest::testSetShort02()

{
  short maxShortVal=0;
  short rShortVal=0;

  initTable("Smallint_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Smallint_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  String smaxShortVal=extractVal("Smallint_Tab", 1, sqlProps, conn);
  maxShortVal=StringUtils::toShort(smaxShortVal);
  logMsg(String("Short Value: ") + maxShortVal);
  pstmt=conn->prepareStatement(sPrepStmt);
  pstmt->setShort(1, maxShortVal);
  pstmt->executeUpdate();
  String Null_Val_Query=sqlProps[ "Smallint_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rShortVal=rs->getShort(1);
  logMsg(String("Returned Short Value after Updation:") + rShortVal);
  logMsg(String("Value returned from ctssql.stmt: ") + maxShortVal);

  if (rShortVal == maxShortVal)
  {
    logMsg("setShort Method sets the designated parameter to a Short value ");
  }
  else
  {
    logErr("setShort Method does not set the designated parameter to a Short value ");
    FAIL("Call to setShort Method is Failed!");
  }
}
#endif


/*
 * @testName:         testSetNull01
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for INTEGER Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 *
 */


/* throws Exception */
/* setNull is not included into current interface */
#ifdef __SET_NULL_ADDED_

void PreparedStatementTest::testSetNull01()
{
  createStandardTable(TABLE_INTEGERTAB);
  bool NULLFlag;

  initTable("Integer_Tab", sqlProps, conn);

  const String & sPrepStmt=sqlProps[ "Integer_Tab_Null_Update" ];

  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  // Second parameter to setNull is ignored
  pstmt->setNull(1, sql::DataType::INTEGER);
  pstmt->executeUpdate();

  String Null_Val_Query=sqlProps[ "Integer_Query_Null" ];

  logMsg(Null_Val_Query);

  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();

  rs->getInt(1);
  NULLFlag=rs->wasNull();

  TestsListener::messagesLog()
          << "Boolean Value After Updation: " << NULLFlag << std::endl;

  if (NULLFlag)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}


/*
 * @testName:         testSetNull02
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for FLOAT Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 */

/* throws Exception */
void PreparedStatementTest::testSetNull02()
{
  createStandardTable(TABLE_FLOATTAB);
  bool NULLFlag;

  initTable("Float_Tab", sqlProps, conn);

  const String & sPrepStmt=sqlProps[ "Float_Tab_Null_Update" ];

  logMsg(String("Prepared Statement String :") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setNull(1, sql::DataType::REAL);

  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Float_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rs->getDouble(1);
  NULLFlag=rs->wasNull();

  TestsListener::messagesLog()
          << "Boolean Value After Updation: " << NULLFlag << std::endl;

  clearTable("Float_Tab", conn);

  if (NULLFlag)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}

/*
 * @testName:         testSetNull03
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for SMALLINT Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 */

/* throws Exception */
void PreparedStatementTest::testSetNull03()
{
  createStandardTable(TABLE_SMALLINTTAB);
  bool NULLFlag;

  initTable("Smallint_Tab", sqlProps, conn);

  const String & sPrepStmt=sqlProps[ "Smallint_Tab_Null_Update" ];

  logMsg(String("Prepared Statement String :") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));

  pstmt->setNull(1, sql::DataType::SMALLINT);
  pstmt->executeUpdate();

  String Null_Val_Query=sqlProps[ "Smallint_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));

  rs->next();
  // it's easier to hack the test, than comment it
  rs->getInt(1);
  NULLFlag=rs->wasNull();

  logMsg(String("Boolean Value After Updation: ") + (NULLFlag ? "true" : "false"));

  if (NULLFlag)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}

/*
 * @testName:         testSetNull04
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for CHAR Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 */

/* throws Exception */
void PreparedStatementTest::testSetNull04()
{
  createStandardTable(TABLE_CHARTAB);
  bool NULLFlag;
  String rStringVal;

  initTable("Char_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Char_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));

  pstmt->setNull(1, sql::DataType::CHAR);
  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Char_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rStringVal=rs->getString(1);
  NULLFlag=rs->wasNull();
  logMsg(String("Boolean Value After Updation: ") + (NULLFlag ? "true" : "false"));

  if (NULLFlag)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}

/*
 * @testName:         testSetNull05
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2)
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for TIME Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 */

/* throws Exception */
// Assuming that set/getTiem and Timestamp will be added along w/ setDate
#ifdef __SET_DATE_ADDED_

void PreparedStatementTest::testSetNull05()
{
  bool NULLFlag;
  Time rTimeVal /*= NULL*/;

  initTable("Time_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Time_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setNull(1, sql::Types::TIME);
  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Time_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rTimeVal=rs->getTime(1);
  NULLFlag=rs->wasNull();
  logMsg(String("Time Value After Updation: ") + NULLFlag);
  if (NULLFlag)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}


/*
 * @testName:         testSetNull06
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for TIMESTAMP Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 */

/* throws Exception */
void PreparedStatementTest::testSetNull06()
{
  bool NULLFlag;
  Timestamp rTimestampVal/*= NULL*/;

  initTable("Timestamp_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Timestamp_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setNull(1, sql::Types::TIMESTAMP);
  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Timestamp_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rTimestampVal=rs->getTimestamp(1);
  NULLFlag=rs->wasNull();
  logMsg(String("Boolean Value After Updation: ") + NULLFlag);
  if (NULLFlag)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}

/*
 * @testName:         testSetNull07
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for DATE Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 */

/* throws Exception */
void PreparedStatementTest::testSetNull07()

{
  bool NULLFlag;
  Date rDateVal /*= NULL*/;

  initTable("Date_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Date_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setNull(1, sql::Types::DATE);
  logMsg("Null set");
  pstmt->executeUpdate();
  logMsg("Updated");
  String Null_Val_Query=sqlProps[ "Date_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rDateVal=rs->getDate(1);
  NULLFlag=rs->wasNull();
  logMsg(String("Boolean Value After Updation: ") + NULLFlag);
  if (NULLFlag)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}
#endif


#ifdef __SET_BIG_DECIMAL_ADDED_
/*
 * @testName:         testSetNull08
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for NUMERIC Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 */

/* throws Exception */
void PreparedStatementTest::testSetNull08()
{
  bool NULLFlag;
  BigDecimal rBigDecimalVal /*= NULL*/;

  initTable("Numeric_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Numeric_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setNull(1, sql::Types::NUMERIC);
  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Numeric_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rBigDecimalVal=rs->getBigDecimal(1);
  NULLFlag=rs->wasNull();
  logMsg(String("Boolean Value After Updation: ") + NULLFlag);
  if (NULLFlag)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}
#endif


#ifdef __SET_BYTE_ADDED_
/*
 * @testName:         testSetNull09
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for TINYINT Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 */

/* throws Exception */
void PreparedStatementTest::testSetNull09()
{
  bool NULLFlag;
  byte rByteVal=0;

  initTable("Tinyint_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Tinyint_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setNull(1, sql::Types::TINYINT);
  pstmt->executeUpdate();
  String Null_Val_Query=sqlProps[ "Tinyint_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rByteVal=rs->getByte(1);
  NULLFlag=rs->wasNull();
  logMsg(String("Boolean Value After Updation: ") + NULLFlag);
  if (NULLFlag)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}
#endif


/*
 * @testName:         testSetNull10
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for DOUBLE Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 */

/* throws Exception */
void PreparedStatementTest::testSetNull10()
{
  createStandardTable(TABLE_DOUBLETAB);
  bool NULLFlag;

  initTable("Double_Tab", sqlProps, conn);

  const String & sPrepStmt=sqlProps[ "Double_Tab_Null_Update" ];

  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setNull(1, sql::DataType::DOUBLE);
  pstmt->executeUpdate();

  String Null_Val_Query=sqlProps[ "Double_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rs->getDouble(1);

  NULLFlag=rs->wasNull();

  logMsg(String("Boolean Value After Updation: ") + (NULLFlag ? "true" : "false"));

  if (NULLFlag)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}

/*
 * @testName:         testSetNull11
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for BIGINT Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 */

/* throws Exception */
void PreparedStatementTest::testSetNull11()
{
  createStandardTable(TABLE_BIGINTTAB);
  bool NULLFlag;

  initTable("Bigint_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Bigint_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setNull(1, sql::DataType::INTEGER);
  pstmt->executeUpdate();

  String Null_Val_Query=sqlProps[ "Bigint_Query_Null" ];

  logMsg(Null_Val_Query);

  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rs->getInt64(1);
  NULLFlag=rs->wasNull();

  logMsg(String("Boolean Value After Updation: ") + (NULLFlag ? "true" : "false"));

  if (NULLFlag)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}
#endif

/*
 * @testName:         testSetString01
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setString(int parameterIndex, String x) method sets the designated
 *                    parameter to a Java String value. The driver converts this to an
 *                    SQL VARCHAR or LONGVARCHAR value (depending on the argument's size
 *                    relative to the driver's limits on VARCHAR values) when it sends
 *                    it to the database.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Using the setString(int parameterIndex, String x) method,update the
 *                    column value with the maximum value of Char_Tab. Call the
 *                    getString(String columnName) method to retrieve this value. Extract
 *                    the maximum value from the ctssql.stmt file. Compare this value
 *                    with the value returned by the getString(String columnName) method.
 *                    Both the values should be equal.
 */

/* throws Exception */
void PreparedStatementTest::testSetString01()
{
  createStandardTable(TABLE_CHARTAB);
  String maxStringVal;
  String rStringVal;

  initTable("Char_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Char_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  maxStringVal=extractVal("Char_Tab", 1, sqlProps, conn);
  maxStringVal=maxStringVal.substr(1, maxStringVal.length() - 1);
  logMsg(String("String Value: ") + maxStringVal);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setString(1, maxStringVal);
  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Char_Query_Null" ];
  logMsg(Null_Val_Query);

  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rStringVal=rs->getString(1);
  rStringVal=StringUtils::trim(rStringVal);
  maxStringVal=StringUtils::trim(maxStringVal);
  logMsg(String("Returned String Value after Updation: ") + rStringVal);
  logMsg(String("Value returned from ctssql.stmt: ") + maxStringVal);

  if (rStringVal == maxStringVal)
  {
    logMsg("setString Method sets the designated parameter to a String value ");
  }
  else
  {
    logErr("setString Method does not set the designated parameter to a String value ");
    FAIL("Call to setString Method is Failed!");
  }
}


#ifdef __SET_TIME_ADDED_
/*
 * @testName:         testSetTime01
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setTime(int parameterIndex, Time x) method sets the
 *                    designated parameter to a sql.Time value. The driver
 *                    converts this to an SQL TIME value when it sends it to the database.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Using the setTime(int parameterIndex, Time x) method,update the
 *                    column value with the Non-Null Time value. Call the getTime(int columnno)
 *                    method to retrieve this value. Extract the Time value
 *                    from the ctssql.stmt file. Compare this value with the value returned
 *                    by the getTime(int columnno)  method. Both the values should be equal
 */

/* throws Exception */
void PreparedStatementTest::testSetTime01()
{
  Time brkTimeVal /*= NULL*/;
  Time rTimeVal /*= NULL*/;
  String sbrkTimeVal;

  initTable("Time_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Time_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  sbrkTimeVal=extractVal("Time_Tab", 1, sqlProps, conn);
  sbrkTimeVal=sbrkTimeVal.substr(sbrkTimeVal.indexOf('\'') + 1, sbrkTimeVal.lastIndexOf('\''));
  sbrkTimeVal=StringUtils::trim(sbrkTimeVal);
  brkTimeVal=Time.valueOf(sbrkTimeVal);
  logMsg(String("Time Value: ") + brkTimeVal);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setTime(1, brkTimeVal);
  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Time_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rTimeVal=rs->getTime(1);
  logMsg(String("Returned Time Value after Updation: ") + rTimeVal);
  logMsg(String("Value returned from ctssql.stmt: ") + brkTimeVal);
  if (rTimeVal == brkTimeVal)
  {
    logMsg("setTime Method sets the designated parameter to a Time value ");
  }
  else
  {
    logErr("setTime Method does not set the designated parameter to a Time value ");
    FAIL(String("Call to setTime Method is Failed!, expected '") + brkTimeVal + "', value returned was '" + rTimeVal + "'");
  }
}


/*
 * @testName:         testSetTime02
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setTime(int parameterIndex, String x, Calendar cal) method sets the
 *                    designated parameter to the given sql.Time value, using the given
 *                    Calendar object. The driver uses the Calendar object to construct an
 *                    SQL TIME value, which the driver then sends to the database. With a Calendar
 *                    object, the driver can calculate the time taking into account a custom
 *                    timezone. If no Calendar object is specified, the driver uses the default
 *                    timezone, which is that of the virtual machine running the application.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Using the setTime(int parameterIndex, Time x, Calendar cal) method,update the
 *                    column value with the Non-Null Time value using the Calendar Object. Call the
 *                    getTime(int columnno) method to retrieve this value. Extract the Time value
 *                    from the ctssql.stmt file. Compare this value with the value returned
 *                    by the getTime(int columnno)  method. Both the values should be equal.
 */

/* throws Exception */
void PreparedStatementTest::testSetTime02()
{
  Time brkTimeVal /*= NULL*/;
  Time rTimeVal /*= NULL*/;
  String sbrkTimeVal;
  Calendar cal /*= NULL*/;

  initTable("Time_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Time_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  sbrkTimeVal=extractVal("Time_Tab", 1, sqlProps, conn);
  sbrkTimeVal=sbrkTimeVal.substr(sbrkTimeVal.indexOf('\'') + 1, sbrkTimeVal.lastIndexOf('\''));
  sbrkTimeVal=StringUtils::trim(sbrkTimeVal);
  brkTimeVal=Time.valueOf(sbrkTimeVal);
  logMsg(String("Time Value: ") + brkTimeVal);
  cal=Calendar.getInstance();

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setTime(1, brkTimeVal, cal);
  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Time_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rTimeVal=rs->getTime(1, cal);
  logMsg(String("Returned Time Value after Updation: ") + rTimeVal);
  logMsg(String("Value returned from ctssql.stmt:") + brkTimeVal);
  if ((rTimeVal).trim().equals((brkTimeVal).trim()))
  {
    logMsg("setTime Method sets the designated parameter to a Time value ");
  }
  else
  {
    logErr("setTime Method does not set the designated parameter to a Time value ");
    FAIL(String("Call to setTime Method is Failed!, expected '") + brkTimeVal + "', value returned was '" + rTimeVal + "'");
  }
}
#endif


#ifdef __SET_TIMESTAMP_ADDED_
/*
 * @testName:         testSetTimestamp01
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setTimestamp(int parameterIndex,Timestamp x) method sets
 *                    the designated parameter to a sql.Timestamp value.
 *                    The driver converts this to an SQL TIMESTAMP value when it
 *                    sends it to the database.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Using the setTimestamp(int parameterIndex, Timestamp x) method,update the
 *                    column value with the Non-Null Timestamp value. Call the getTimestamp(int columnno)
 *                    method to retrieve this value. Extract the Timestamp value
 *                    from the ctssql.stmt file. Compare this value with the value returned
 *                    by the getTimestamp(int columnno)  method. Both the values should be equal.
 */

/* throws Exception */
void PreparedStatementTest::testSetTimestamp01()
{
  Timestamp inTimeVal /*= NULL*/;
  Timestamp rTimestampVal /*= NULL*/;
  String sinTimeVal;

  initTable("Timestamp_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Timestamp_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  sinTimeVal=extractVal("Timestamp_Tab", 1, sqlProps, conn);
  sinTimeVal=sinTimeVal.substr(sinTimeVal.indexOf('\'') + 1, sinTimeVal.lastIndexOf('\''));
  inTimeVal=Timestamp.valueOf(sinTimeVal);
  logMsg(String("Timestamp Value: ") + inTimeVal);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setTimestamp(1, inTimeVal);
  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Timestamp_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rTimestampVal=rs->getTimestamp(1);
  logMsg(String("Returned Timestamp Value after Updation: ") + rTimestampVal);
  logMsg(String("Value returned from ctssql.stmt  :") + inTimeVal);

  if (rTimestampVal == inTimeVal)
  {
    logMsg("setTimestamp Method sets the designated parameter to a Timestamp value ");
  }
  else
  {
    logErr("setTimestamp Method does not set the designated parameter to a Timestamp value ");
    FAIL("Call to setTimestamp Method is Failed!");
  }
}


/*
 * @testName:         testSetTimestamp02
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setTimestamp(int parameterIndex, String x, Calendar cal) method sets the
 *                    designated parameter to the given sql.Timestamp value, using the given
 *                    Calendar object. The driver uses the Calendar object to construct an
 *                    SQL TIMESTAMP value, which the driver then sends to the database. With a Calendar
 *                    object, the driver can calculate the time taking into account a custom
 *                    timezone. If no Calendar object is specified, the driver uses the default
 *                    timezone, which is that of the virtual machine running the application.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Using the setTimestamp(int parameterIndex, Time x, Calendar cal) method,update the
 *                    column value with the Non-Null Timestamp value using the Calendar Object. Call the
 *                    getTimestamp(int columnno) method to retrieve this value. Extract the Timestamp value
 *                    from the ctssql.stmt file. Compare this value with the value returned
 *                    by the getTimestamp(int columnno) method. Both the values should be equal.
 */

/* throws Exception */
void PreparedStatementTest::testSetTimestamp02()
{
  Timestamp inTimeVal /*= NULL*/;
  Timestamp rTimestampVal /*= NULL*/;
  String sinTimeVal;
  Calendar cal /*= NULL*/;

  initTable("Timestamp_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Timestamp_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  sinTimeVal=extractVal("Timestamp_Tab", 1, sqlProps, conn);
  sinTimeVal=sinTimeVal.substr(sinTimeVal.indexOf('\'') + 1, sinTimeVal.lastIndexOf('\''));
  inTimeVal=Timestamp.valueOf(sinTimeVal);
  logMsg(String("Timestamp Value: ") + inTimeVal);
  cal=Calendar.getInstance();

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setTimestamp(1, inTimeVal, cal);
  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Timestamp_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rTimestampVal=rs->getTimestamp(1, cal);
  logMsg(String("Returned Timestamp Value after Updation: ") + rTimestampVal);
  logMsg(String("Value returned from ctssql.stmt  :") + inTimeVal);
  if (rTimestampVal == inTimeVal)
  {
    logMsg("setTimestamp Method sets the designated parameter to a Timestamp value ");
  }
  else
  {
    logErr("setTimestamp Method does not set the designated parameter to a Timestamp value ");
    FAIL("Call to setTimestamp Method is Failed!");
  }
}
#endif


/*
 * @testName:         testSetString02
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setString(int parameterIndex, String x) method sets the designated
 *                    parameter to a Java String value. The driver converts this to an
 *                    SQL VARCHAR or LONGVARCHAR value (depending on the argument's size
 *                    relative to the driver's limits on VARCHAR values) when it sends
 *                    it to the database.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Using setString(int parameterIndex, String x),update the column
 *                    with the maximum value which is a SQL VARCHAR. Call the
 *                    getString(int ColumnIndex) method to retrieve this value. Extract
 *                    the maximum value as a String from the ctssql.stmt file.
 *                    Compare this value with the value returned by the getString method.
 *                    Both the values should be equal.
 */

/* throws Exception */
void PreparedStatementTest::testSetString02()
{
  createStandardTable(TABLE_VARCHARTAB);
  String maxStringVal;
  String rStringVal;

  initTable("Varchar_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Varchar_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  maxStringVal=extractVal("Varchar_Tab", 1, sqlProps, conn);
  maxStringVal=maxStringVal.substr(1, maxStringVal.length() - 1);
  logMsg(String("String Value: ") + maxStringVal);
  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setString(1, maxStringVal);
  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Varchar_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();

  rStringVal=rs->getString(1);
  rStringVal=StringUtils::trim(rStringVal);
  maxStringVal=StringUtils::trim(maxStringVal);

  logMsg(String("Returned String Value after Updation: ") + rStringVal);
  logMsg(String("Value returned from ctssql.stmt: ") + maxStringVal);

  if (rStringVal == maxStringVal)
  {
    logMsg("setString Method sets the designated parameter to a String value ");
  }
  else
  {
    logErr("setString Method does not set the designated parameter to a String value ");
    FAIL("Call to setString Method is Failed!");
  }
}

/*
 * @testName:         testSetFloat01
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setFloat(int parameterIndex, float x) sets the designated parameter
 *                    to a Java float value. The driver converts this to an SQL FLOAT value
 *                    when it sends it to the database.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Using setFloat(int parameterIndex,float x),update the column with the
 *                    minimum value of Real_Tab.
 *                    Now execute a query to get the minimum value and retrieve the result
 *                    of the query using the getFloat(int columnIndex) method.Compare the
 *                    returned value, with the minimum value extracted from the ctssql.stmt file.
 *                    Both of them should be equal.
 */


/* throws Exception */
#ifdef __SET_FLOAT_ADDED_

void PreparedStatementTest::testSetFloat01()
{
  float rFloatVal=0;
  float minFloatVal=0;

  initTable("Real_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Real_Tab_Max_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  String sminFloatVal=extractVal("Real_Tab", 2, sqlProps, conn);
  minFloatVal=float.parseFloat(sminFloatVal);
  logMsg(String("float Value: ") + minFloatVal);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setFloat(1, minFloatVal);
  pstmt->executeUpdate();
  const String & Max_Val_Query=sqlProps[ "Real_Query_Max" ];
  logMsg(Max_Val_Query);
  rs.reset(stmt->executeQuery(Max_Val_Query));
  rs->next();
  rFloatVal=rs->getFloat(1);
  logMsg(String("Returned float Value after Updation: ") + rFloatVal);
  logMsg(String("Value returned from ctssql.stmt: ") + minFloatVal);
  if (rFloatVal == minFloatVal)
  {
    logMsg("setFloat Method sets the designated parameter to a float value ");
  }
  else
  {
    logErr("setFloat Method does not set the designated parameter to a float value ");
    FAIL("Call to setFloat Method is Failed!");
  }
}

/*
 * @testName:         testSetFloat02
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setFloat(int parameterIndex, float x) sets the designated parameter
 *                    to a Java float value. The driver converts this to an SQL FLOAT value
 *                    when it sends it to the database.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    Using setFloat(int parameterIndex,float x),update the column with the
 *                    maximum value of Real_Tab.
 *                    Now execute a query to get the maximum value and retrieve the result
 *                    of the query using the getFloat(int columnIndex) method.Compare the
 *                    returned value, with the maximum value extracted from the ctssql.stmt file.
 *                    Both of them should be equal.
 */

/* throws Exception */
void PreparedStatementTest::testSetFloat02()
{
  float maxFloatVal=0;
  float rFloatVal=0;

  initTable("Real_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Real_Tab_Null_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  String smaxFloatVal=extractVal("Real_Tab", 1, sqlProps, conn);
  maxFloatVal=float.parseFloat(smaxFloatVal);
  logMsg(String("float Value: ") + maxFloatVal);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setFloat(1, maxFloatVal);
  pstmt->executeUpdate();
  const String & Null_Val_Query=sqlProps[ "Real_Query_Null" ];
  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rFloatVal=rs->getFloat(1);
  logMsg(String("Returned float Value after Updation: ") + rFloatVal);
  logMsg(String("Value returned from ctssql.stmt: ") + maxFloatVal);
  if (rFloatVal == maxFloatVal)
  {
    logMsg("setFloat Method sets the designated parameter to a float value ");
  }
  else
  {
    logErr("setFloat Method does not set the designated parameter to a float value ");
    FAIL("Call to setFloat Method is Failed!");
  }
}
#endif


#ifdef __SET_NULL_ADDED_
/*
 * @testName:         testSetNull12
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for Varchar Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 *
 */

/* throws Exception */
void PreparedStatementTest::testSetNull12()
{
  createStandardTable(TABLE_VARCHARTAB);
  bool NULLFlag;
  String rStringVal;

  initTable("Varchar_Tab", sqlProps, conn);

  const String & sPrepStmt=sqlProps[ "Varchar_Tab_Null_Update" ];

  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setNull(1, sql::DataType::VARCHAR);
  pstmt->executeUpdate();

  const String & Null_Val_Query=sqlProps[ "Varchar_Query_Null" ];

  logMsg(Null_Val_Query);

  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rStringVal=rs->getString(1);
  NULLFlag=rs->wasNull();

  logMsg(String("Boolean Value After Updation: ") + (NULLFlag ? "true" : "false"));

  if (NULLFlag)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}


/*
 * @testName:         testSetNull13
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for LONGVARCHAR Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 *
 */

/* throws Exception */
void PreparedStatementTest::testSetNull13()
{
  createStandardTable(TABLE_LONGVARCHARNULLTAB);
  bool NULLFlag;
  String rStringVal;

  initTable("Longvarcharnull_Tab", sqlProps, conn);

  const String & sPrepStmt=sqlProps[ "Longvarchar_Tab_Null_Update" ];

  logMsg(String("Prepared Statement String: ") + sPrepStmt);

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setNull(1, sql::DataType::LONGVARBINARY);
  pstmt->executeUpdate();

  const String & Null_Val_Query=sqlProps[ "Longvarchar_Query_Null" ];

  logMsg(Null_Val_Query);
  rs.reset(stmt->executeQuery(Null_Val_Query));
  rs->next();
  rStringVal=rs->getString(1);
  NULLFlag=rs->wasNull();

  // TODO: add toString(bool) in the StringUtils
  logMsg(String("Boolean Value After Updation: ") + (NULLFlag ? "true" : "false"));

  if (NULLFlag)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}


#ifdef _WE_HAVE_SOME_REAL_TYPE
/*
 * @testName:         testSetNull14
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for REAL Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 *
 */

/* throws Exception */
void PreparedStatementTest::testSetNull14()
{
  bool NULLFlag;
  float rFloatVal=0;
  try
  {
    initTable("Real_Tab", sqlProps, conn);
    const String & sPrepStmt=sqlProps[ "Real_Tab_Null_Update" ];
    logMsg(String("Prepared Statement String: ") + sPrepStmt);

    pstmt.reset(conn->prepareStatement(sPrepStmt));
    pstmt->setNull(1, sql::Types::REAL);
    pstmt->executeUpdate();
    const String & Null_Val_Query=sqlProps[ "Real_Query_Null" ];
    logMsg(Null_Val_Query);
    rs.reset(stmt->executeQuery(Null_Val_Query));
    rs->next();
    rFloatVal=rs->getFloat(1);
    NULLFlag=rs->wasNull();
    logMsg(String("Boolean Value After Updation: ") + NULLFlag);
    if (NULLFlag)
    {
      logMsg("setNull Method sets the designated parameter to a SQL Null");
    }
    else
    {
      logErr("setNull Method does not set the designated parameter to a SQL Null ");
      FAIL("Call to setNull Method is Failed!");
    }
  }
  catch (sql::SQLException & sqle)
  {
    logErr(String("SQL std::exception *: ") + sqle->what());
    FAIL("Call to setNull is Failed!");
  }
  catch (std::exception * e)
  {
    logErr(String("Unexpected std::exception *: ") + e->what());
    FAIL("Call to setNull is Failed!", e);
  }
}
#endif

//getBigDecimal will be probably added along w/ its "set" counterpart
#ifdef __SET_BIG_DECIMAL_ADDED_
/*
 * @testName:         testSetNull15
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for DECIMAL Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 *
 */

/* throws Exception */
void PreparedStatementTest::testSetNull15()
{
  bool NULLFlag;
  BigDecimal rBigDecimalVal /*= NULL*/;
  try
  {
    initTable("Decimal_Tab", sqlProps, conn);
    const String & sPrepStmt=sqlProps[ "Decimal_Tab_Null_Update" ];
    logMsg(String("Prepared Statement String: ") + sPrepStmt);

    pstmt.reset(conn->prepareStatement(sPrepStmt));
    pstmt->setNull(1, sql::Types::DECIMAL);
    pstmt->executeUpdate();
    const String & Null_Val_Query=sqlProps[ "Decimal_Query_Null" ];
    logMsg(Null_Val_Query);
    rs.reset(stmt->executeQuery(Null_Val_Query));
    rs->next();
    rBigDecimalVal=rs->getBigDecimal(1);
    NULLFlag=rs->wasNull();
    logMsg(String("Boolean Value After Updation ") + NULLFlag);
    if (NULLFlag)
    {
      logMsg("setNull Method sets the designated parameter to a SQL Null");
    }
    else
    {
      logErr("setNull Method does not set the designated parameter to a SQL Null ");
      FAIL("Call to setNull Method is Failed!");
    }
  }
  catch (sql::SQLException & sqle)
  {
    logErr(String("SQL std::exception *: ") + sqle->what());
    FAIL("Call to setNull is Failed!");
  }
  catch (std::exception * e)
  {
    logErr(String("Unexpected std::exception *: ") + e->what());
    FAIL("Call to setNull is Failed!", e);
  }
}
#endif


#ifdef __SET_BYTE_ADDED_
/*
 * @testName:         testSetNull16
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for BINARY Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 *
 */

/* throws Exception */
void PreparedStatementTest::testSetNull16()
{
  byte retByteArr[] /*= NULL*/;
  String binarySize;

  initTable("Binary_Tab", sqlProps, conn);
  binarySize=sqlProps["binarySize"];
  logMsg(String("Binary Table Size : ") + binarySize);
  const String & sPrepStmt=sqlProps[ "Binary_Tab_Val_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  int bytearrsize=StringUtils::toInt(binarySize);
  logMsg(String("Binary Size : ") + bytearrsize);
  byte[] bytearr=new byte[bytearrsize];
  String sbyteval;
  for (int count=0; count < bytearrsize; count++)
  {
    sbyteval=int.toString(count % 255);
    bytearr[count]=Byte.parseByte(sbyteval);
  }

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setBytes(1, bytearr);
  pstmt->executeUpdate();
  pstmt->setNull(1, sql::Types::BINARY);
  pstmt->executeUpdate();
  const String & Binary_Val_Query=sqlProps[ "Binary_Query_Val" ];
  logMsg(Binary_Val_Query);
  rs.reset(stmt->executeQuery(Binary_Val_Query));
  rs->next();
  retByteArr=(byte[]) rs->getBytes(1);
  if (retByteArr == NULL)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null ");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}


/*
 * @testName:         testSetNull17
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for VARBINARY Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 *
 */

/* throws Exception */
void PreparedStatementTest::testSetNull17()

{
  byte retByteArr[] /*= NULL*/;
  String varbinarySize;

  initTable("Varbinary_Tab", sqlProps, conn);
  varbinarySize=sqlProps["varbinarySize"];
  logMsg(String("Varbinary Table Size : ") + varbinarySize);
  const String & sPrepStmt=sqlProps[ "Varbinary_Tab_Val_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  int bytearrsize=StringUtils::toInt(varbinarySize);
  logMsg(String("Varbinary Size : ") + bytearrsize);
  byte[] bytearr=new byte[bytearrsize];
  String sbyteval;
  for (int count=0; count < bytearrsize; count++)
  {
    sbyteval=int.toString(count % 255);
    bytearr[count]=Byte.parseByte(sbyteval);
  }

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setBytes(1, bytearr);
  pstmt->executeUpdate();
  pstmt->setNull(1, sql::Types::VARBINARY);
  pstmt->executeUpdate();
  const String & Varbinary_Val_Query=sqlProps[ "Varbinary_Query_Val" ];
  logMsg(Varbinary_Val_Query);
  rs.reset(stmt->executeQuery(Varbinary_Val_Query));
  rs->next();
  retByteArr=(byte[]) rs->getBytes(1);
  if (retByteArr == NULL)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null ");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}


/*
 * @testName:         testSetNull18
 * @assertion:        A Prepared Statement object provides a way of calling
 *                    precompiled SQL statements.It can take one or more parameters
 *                    as input arguments(IN parameters). (See section 11.1.6 of
 *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
 *
 *                    The drivers must provide full support for Prepared Statement
 *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
 *                    Edition(J2EE) specification v 1.2).
 *
 *                    The setNull(int parameterIndex, int sqlType) method sets the
 *                    designated parameter to SQL NULL(the generic SQL NULL defined in
 *                    sql.Types). Note that the JDBC type of the parameter to be
 *                    set to JDBC NULL must be specified.
 *
 * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
 *                    execute the precompiled SQL Statement to set the value as SQL Null
 *                    for LONGVARBINARY Type and retrieve the same value by executing a query. Call
 *                    the ResultSet.wasNull() method to check it. It should return a true value.
 *
 */

/* throws Exception */
void PreparedStatementTest::testSetNull18()
{
  byte retByteArr[] /*= NULL*/;
  bool byteArrFlag=false;

  initTable("Longvarbinary_Tab", sqlProps, conn);
  const String & sPrepStmt=sqlProps[ "Longvarbinary_Tab_Val_Update" ];
  logMsg(String("Prepared Statement String: ") + sPrepStmt);
  const String & binsize=sqlProps["longvarbinarySize"];
  int bytearrsize=StringUtils::toInt(binsize);
  logMsg(String("Longvarbinary Size : ") + bytearrsize);
  byte[] bytearr=new byte[bytearrsize];
  String sbyteval;
  for (int count=0; count < bytearrsize; count++)
  {
    sbyteval=int.toString(count % 255);
    bytearr[count]=Byte.parseByte(sbyteval);
  }

  pstmt.reset(conn->prepareStatement(sPrepStmt));
  pstmt->setBytes(1, bytearr);
  pstmt->executeUpdate();
  pstmt->setNull(1, sql::Types::LONGVARBINARY);
  pstmt->executeUpdate();
  const String & Longvarbinary_Val_Query=sqlProps[ "Longvarbinary_Query_Val" ];
  logMsg(Longvarbinary_Val_Query);
  rs.reset(stmt->executeQuery(Longvarbinary_Val_Query));
  rs->next();
  retByteArr=rs->getBytes(1);
  if (retByteArr == NULL)
  {
    logMsg("setNull Method sets the designated parameter to a SQL Null ");
  }
  else
  {
    logErr("setNull Method does not set the designated parameter to a SQL Null ");
    FAIL("Call to setNull Method is Failed!");
  }
}
#endif    // __SET_BYTE_ADDED_
#endif    // __SET_NULL_ADDED_


/**
 * @see junit.framework.TestCase#setUp()
 */

/* throws Exception */
void PreparedStatementTest::setUp()
{
  super::setUp();

  dbmd=conn->getMetaData();
}

}
}
