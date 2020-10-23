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



/* Ported JDBC compliance connection tests
 * Most of them don't really make sense in c++
 */

#include <cppconn/statement.h>
#include <cppconn/metadata.h>

#include "ConnectionTest.h"

namespace testsuite
{
namespace compliance
{

/*
 * @testName:         testClose
 *
 * @assertion:        A Connection object represents a Connection in a database.
 *                    A Connection session includes the SQL Statements that are
 *                    executed and the results that are returned over that connection.
 *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
 *                    edition).
 *
 *                    The JDBC drivers must provide accurate and complete
 *                    metadata through the Connection.getMetaData() method. (See
 *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
 *                    specification v1.2).
 *
 *                    The Close method closes the connection object and the close()
 *                    method does not return any value. (See JDK 1.2.2 API
 *                    documentation)
 *
 * @test_Strategy:    Get a Connection object and call close() method
 *                    and call isClosed() method and it should return a true value
 *
 */

/* throws Exception */
void ConnectionTest::testClose()
{
  bool closeFlag=false;
  Connection con /*= NULL*/;
  con.reset(getConnection());
  logMsg("Calling Close() method ");
  con->close();
  closeFlag=con->isClosed();
  if (closeFlag)
  {
    logMsg("close method closes the Connection object ");
  } else
  {
    logErr("close method does not close the Connection object");
    FAIL("Call to Close is Failed!");
  }
}

/*
 * @testName:         testCreateStatement01
 *
 * @assertion:        A Connection object represents a Connection in a database.
 *                    A Connection session includes the SQL Statements that are
 *                    executed and the results that are returned over that connection.
 *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
 *                    edition).
 *
 *                    The JDBC drivers must provide accurate and complete
 *                    metadata through the Connection.getMetaData() method. (See
 *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
 *                    specification v1.2).
 *
 *                    The createStatement() method returns  a Statement object that
 *                    will produce non-scrollable and non-updatable result set.
 *                    (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Connection object and call createStatement()
 *                    method and call instanceof to check
 *                    It should return a Statement object
 */

/* throws Exception */
void ConnectionTest::testCreateStatement01()
{
  Statement statemt /*= NULL*/;
  logMsg("Calling createStatement() method ");

  statemt.reset(conn->createStatement());
  if (statemt.get() != nullptr)
  {
    logMsg("createStatement method creates a Statement object");
  } else
  {
    logErr(
           "createStatement method does not create a Statement object");
    FAIL("Call to createStatement is Failed!");
  }
  statemt->close();
}

/*
 * @testName:         testGetCatalog
 *
 * @assertion:        A Connection object represents a Connection in a database.
 *                    A Connection session includes the SQL Statements that are
 *                    executed and the results that are returned over that connection.
 *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
 *                    edition)
 *
 *                    The JDBC drivers must provide accurate and complete
 *                    metadata through the Connection.getMetaData() method. (See
 *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
 *                    specification v1.2)
 *
 *                    The getCatalog() method returns a String object; the string
 *                    represents the connection object's catalog name and null if there
 *                    is none.  (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Connection object and call getCatalog() method
 *                    It should return a String value The getCatalogs() method in
 *                    Databasemeta data object will return a Resultset object that contains
 *                    the catalog name in the column TABLE_CAT .The String returned by
 *                    Connection.getCatalog() method will be checked against these
 *                    column values.
 */

/**
Changing it to used getSchema()/getSchemas, that have been just introduced. Reverting is possible.
Probably should have tests both for getSchema and getCatalog
 */

/* throws Exception */
void ConnectionTest::testGetCatalog()
{
  String catalogName;
  String url;
  String retValue;
  String extValue;
  bool flag=false;

  logMsg("Calling getSchema() method ");

  //retValue = conn->getCatalog();
  retValue=conn->getSchema();
  logMsg(String("Catalog Name   ") + retValue);

  if (retValue.size() > 0)
  {
    rs.reset(dbmd->getSchemas());

    while (rs->next())
    {
      //extValue = rs->getString("TABLE_CAT");
      extValue=rs->getString("TABLE_SCHEM");
      logMsg(String("Catalog Name   ") + extValue);
      if (retValue == extValue)
      {
        flag=true;
        break;
      }
    }

    if (flag)
    {
      logMsg(String("getCatalog returns the Catalog name") + retValue);
    } else
    {
      logErr("getCatalog does not return the catalog name");
      FAIL("Call to getCatalog is Failed!");
    }
  } else
  {
    logMsg("getCatalog returns a empty String Object");
  }
}

/*
 * @testName:         testGetMetaData
 *
 * @assertion:        A Connection object represents a Connection in a database.
 *                    A Connection session includes the SQL Statements that are
 *                    executed and the results that are returned over that connection.
 *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
 *                    edition)
 *
 *                    The JDBC drivers must provide accurate and complete
 *                    metadata through the Connection.getMetaData() method. (See
 *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
 *                    specification v1.2)
 *
 *                    The getMetaData method returns a DatabaseMetaData object.
 *                    (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Connection object and call getMetaData()
 *                    method and call instanceof method to check
 *                    It should return a DatabaseMetaData object
 *
 */

/* throws Exception
 * As many others here - doesn't make much sense in c++. changed it
 * to cast to implementation class
 */

void ConnectionTest::testGetMetaData()
{
  logMsg("Calling getMetaData() method ");
  DatabaseMetaData rsmd(conn->getMetaData());

  if (rsmd != NULL)
  {
    logMsg("getMetaData returns the DatabaseMetaData object ");
  } else
  {
    logErr(
           "getMetaData does not return the DatabaseMetaData object");
    FAIL("Call to getMetaData is Failed!");
  }

}

/*
 * @testName:         testGetTransactionIsolation
 *
 * @assertion:        A Connection object represents a Connection in a database.
 *                    A Connection session includes the SQL Statements that are
 *                    executed and the results that are returned over that connection.
 *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
 *                    edition).
 *
 *                    The JDBC drivers must provide accurate and complete
 *                    metadata through the Connection.getMetaData() method. (See
 *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
 *                    specification v1.2)
 *
 *                    The getTransactionIsolation method returns an int value and must
 *                    be equal to the value of  TRANSACTION_NONE or
 *                    TRANSACTION_READ_COMMITTED or TRANSACTION_READ_UNCOMMITTED
 *                    or TRANSACTION_REPEATABLE_READ or TRANSACTION_SERIALIZABLE.
 *                    (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Connection object and call getTransactionIsolation() method
 *                    It should return a Integer value and must be equal to the value of
 *                    TRANSACTION_NONE or TRANSACTION_READ_COMMITTED or
 *                    TRANSACTION_READ_UNCOMMITTED or TRANSACTION_REPEATABLE_READ or
 *                    TRANSACTION_SERIALIZABLE which is default set by the driver
 *
 *
 */

/* throws Exception */
void ConnectionTest::testGetTransactionIsolation()
{
  int transIsolateVal=0;
  logMsg("Calling getTransactionIsolation() method ");
  transIsolateVal=conn->getTransactionIsolation();
  if (transIsolateVal == sql::TRANSACTION_NONE)
  {
    TestsListener::messagesLog()
            << "getTransactionIsolation method returns Transaction Isolation mode as "
            << transIsolateVal << std::endl;
  } else if (transIsolateVal == sql::TRANSACTION_READ_COMMITTED)
  {
    TestsListener::messagesLog()
            << "getTransactionIsolation method returns Transaction Isolation mode as "
            << transIsolateVal << std::endl;
  } else if (transIsolateVal == sql::TRANSACTION_READ_UNCOMMITTED)
  {
    TestsListener::messagesLog()
            << "getTransactionIsolation method returns Transaction Isolation mode as "
            << transIsolateVal << std::endl;
  } else if (transIsolateVal == sql::TRANSACTION_REPEATABLE_READ)
  {
    TestsListener::messagesLog()
            << "getTransactionIsolation method returns Transaction Isolation mode as "
            << transIsolateVal << std::endl;
  } else if (transIsolateVal == sql::TRANSACTION_SERIALIZABLE)
  {
    TestsListener::messagesLog()
            << "getTransactionIsolation method returns Transaction Isolation mode as "
            << transIsolateVal << std::endl;
  } else
  {
    logErr("getTransactionIsolation method returns an invalid value");
    FAIL("Call to getTransactionIsolation is Failed!");
  }
}

/*
 * @testName:         testIsClosed01
 *
 * @assertion:        A Connection object represents a Connection in a database.
 *                    A Connection session includes the SQL Statements that are
 *                    executed and the results that are returned over that connection.
 *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
 *                    edition).
 *
 *                    The JDBC drivers must provide accurate and complete
 *                    metadata through the Connection.getMetaData() method. (See
 *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
 *                    specification v1.2).
 *
 *                    The isClosed method returns a boolean value; true if the
 *                    connection is closed or false if it is still open.
 *                    (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Connection object and call isClosed() method
 *                    It should return a boolean value and the value should be
 *                    equal to false
 *
 */

/* throws Exception */
void ConnectionTest::testIsClosed01()
{
  bool closeFlag=false;
  Connection con /*= NULL*/;
  con.reset(getConnection());
  logMsg("Calling isClosed Method");
  closeFlag=con->isClosed();
  if (!closeFlag)
  {
    logMsg(String("isClosed method returns  ") + (closeFlag ? "true" : "false"));
  } else
  {
    logErr("isClosed method returns an invalid value");
    FAIL("Call to isClosed is Failed!");
  }
  con->close();
}

/*
 * @testName:         testIsClosed02
 *
 * @assertion:        A Connection object represents a Connection in a database.
 *                    A Connection session includes the SQL Statements that are
 *                    executed and the results that are returned over that connection.
 *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
 *                    edition).
 *
 *                    The JDBC drivers must provide accurate and complete
 *                    metadata through the Connection.getMetaData() method. (See
 *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
 *                    specification v1.2)
 *
 *                    The isClosed method returns a boolean value; true if the
 *                    connection is closed or false if it is still open.
 *                    (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Connection object and call close() method
 *                    and call isClosed() method
 *                    It should return a boolean value and the value should be
 *                    equal to true
 *
 */

/* throws Exception */
void ConnectionTest::testIsClosed02()
{
  bool closeFlag=false;
  Connection con /*= NULL*/;
  con.reset(getConnection());
  con->close();
  logMsg("Calling isClosed() method ");
  closeFlag=con->isClosed();
  if (closeFlag)
  {
    logMsg(String("isClosed method returns  ")
           + (closeFlag ? "true" : "false"));
  } else
  {
    logErr("isClosed method returns an invalid value");
    FAIL("Call to isClosed is Failed!");
  }
}


#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
/*
 * @testName:         testIsReadOnly
 *
 * @assertion:        A Connection object represents a Connection in a database.
 *                    A Connection session includes the SQL Statements that are
 *                    executed and the results that are returned over that connection.
 *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
 *                    edition).
 *
 *                    The JDBC drivers must provide accurate and complete
 *                    metadata through the Connection.getMetaData() method. (See
 *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
 *                    specification v1.2).
 *
 *                    The isReadOnly method returns a boolean value; true if the
 *                    connection is in read-only mode and false otherwise.
 *                    (See JDK 1.2.2 API documentation)
 *
 * @test_Strategy:    Get a Connection object and call setReadOnly(boolean b)
 *                    method and call isReadOnly() method
 *                    It should return a boolean value that is been set
 *
 */
/* throws Exception */
void ConnectionTest::testIsReadOnly()
{
  bool errorFlag=true;
  logMsg("invoking the setReadOnly method");

  conn->setReadOnly(false);

  logMsg("Calling isReadOnly() method ");

  if (conn->isReadOnly())
  {
    errorFlag=true;
  } else
  {
    errorFlag=false;
  }

  if (!errorFlag)
  {
    logMsg("isReadOnly method is Successful");
  } else
  {
    logErr("isReadOnly method returns an invalid value");
    FAIL("Call to isReadOnly is Failed!");
  }
}
#endif


/*
 * @testName:         testNativeSQL
 *
 * @assertion:        A Connection object represents a Connection in a database.
 *                    A Connection session includes the SQL Statements that are
 *                    executed and the results that are returned over that connection.
 *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
 *                    edition)
 *
 *                    The JDBC drivers must provide accurate and complete
 *                    metadata through the Connection.getMetaData() method. (See
 *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
 *                    specification v1.2).
 *
 *                    The nativeSQL(String sql) method returns a String object
 *                    representing the native form of a sql.  (See JDK 1.2.2 API
 *                    documentation)
 *
 * @test_Strategy:    Get a Connection object and call nativeSQL(String sql) method
 *                    It should return a String value which represents native SQL
 *                    grammar implementation of the SQL statement if the driver supports
 *                    else it returns the actual SQL statement as a String.This is checked
 *                   by using instanceof method
 */

/* throws Exception */
void ConnectionTest::testNativeSQL()
{
  String sSqlStmt;
  String nativeSql;
  sSqlStmt=sqlProps["Escape_Seq_Query"];
  logMsg(String("SQL Statement with Escape Syntax") + sSqlStmt);
  logMsg("Calling nativeSQL method ");
  nativeSql=conn->nativeSQL(sSqlStmt);
  if (nativeSql.size() > 0)
  {
    logMsg(String("nativeSQL method returns :  ") + nativeSql);
  } else
  {
    logErr(
           "nativeSQL method does not return the System native SQL grammar");
    FAIL("Call to nativeSQL is Failed!");
  }
}

/**
 * @see junit.framework.TestCase#setUp()
 */

/* throws Exception */
void ConnectionTest::setUp()
{
  super::setUp();
  dbmd= conn->getMetaData();
}

}
}
