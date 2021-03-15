/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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



#include <cppconn/prepared_statement.h>
#include <cppconn/connection.h>
#include <cppconn/warning.h>
#include "statement.h"
#include <stdlib.h>
#include <time.h>

namespace testsuite
{
namespace classes
{

void statement::anonymousSelect()
{
  logMsg("statement::anonymousSelect() - MySQL_Statement::*, MYSQL_Resultset::*");

  stmt.reset(con->createStatement());
  try
  {
    res.reset(stmt->executeQuery("SELECT ' ', NULL"));
    ASSERT(res->next());
    ASSERT_EQUALS(" ", res->getString(1));

    std::string mynull(res->getString(2));
    ASSERT(res->isNull(2));
    ASSERT(res->wasNull());

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void statement::getWarnings()
{
  logMsg("statement::getWarnings() - MySQL_Statement::get|clearWarnings()");

  //TODO: Enable it after fixing
  SKIP("Removed untill fixed");

  std::stringstream msg;
  unsigned int count= 0;

  stmt.reset(con->createStatement());
  try
  {
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT UNSIGNED)");

    // Generating 2  warnings to make sure we get only the last 1 - won't hurt
    stmt->execute("INSERT INTO test(id) VALUES (-2)");
    // Lets hope that this will always cause a 1264 or similar warning
    stmt->execute("INSERT INTO test(id) VALUES (-1)");

    for (const sql::SQLWarning* warn=stmt->getWarnings(); warn; warn=warn->getNextWarning())
    {
      ++count;
      msg.str("");
      msg << "... ErrorCode = '" << warn->getErrorCode() << "', ";
      msg << "SQLState = '" << warn->getSQLState() << "', ";
      msg << "ErrorMessage = '" << warn->getMessage() << "'";
      logMsg(msg.str());

      ASSERT((0 != warn->getErrorCode()));
      if (1264 == warn->getErrorCode())
      {
        ASSERT_EQUALS("22003", warn->getSQLState());
      }
      else
      {
        ASSERT(("" != warn->getSQLState()));
      }
      ASSERT(("" != warn->getMessage()));
    }

    ASSERT_EQUALS(1, count);

    for (const sql::SQLWarning* warn=stmt->getWarnings(); warn; warn=warn->getNextWarning())
    {
      msg.str("");
      msg << "... ErrorCode = '" << warn->getErrorCode() << "', ";
      msg << "SQLState = '" << warn->getSQLState() << "', ";
      msg << "ErrorMessage = '" << warn->getMessage() << "'";
      logMsg(msg.str());

      ASSERT((0 != warn->getErrorCode()));
      if (1264 == warn->getErrorCode())
      {
        ASSERT_EQUALS("22003", warn->getSQLState());
      }
      else
      {
        ASSERT(("" != warn->getSQLState()));
      }
      ASSERT(("" != warn->getMessage()));
    }

    stmt->clearWarnings();
    for (const sql::SQLWarning* warn=stmt->getWarnings(); warn; warn=warn->getNextWarning())
    {
      FAIL("There should be no more warnings!");
    }

    // New warning
    stmt->execute("INSERT INTO test(id) VALUES (-3)");
    // Verifying we have warnings now
    ASSERT(stmt->getWarnings() != NULL);

    // Statement without tables access does not reset warnings.
    stmt->execute("SELECT 1");
    ASSERT(stmt->getWarnings() == NULL);
    res.reset(stmt->getResultSet());
    res->next();

    // TODO - how to use getNextWarning() ?
    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void statement::clearWarnings()
{
  logMsg("statement::clearWarnings() - MySQL_Statement::clearWarnings");
  // const sql::SQLWarning* warn;
  // std::stringstream msg;

  stmt.reset(con->createStatement());
  try
  {
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(col1 DATETIME, col2 DATE)");
    stmt->execute("INSERT INTO test SET col1 = NOW()");
    // Lets hope that this will always cause a 1264 or similar warning
    ASSERT(!stmt->execute("UPDATE test SET col2 = col1"));
    stmt->clearWarnings();
    // TODO - how to verify?
    // TODO - how to use getNextWarning() ?
    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void statement::callSP()
{
  logMsg("statement::callSP() - MySQL_Statement::*");
  std::stringstream msg;

  try
  {
    sql::ConnectOptionsMap connection_properties;

    /* url comes from the unit testing framework */
    connection_properties["hostName"]=url;
    /* user comes from the unit testing framework */
    connection_properties["userName"]=user;
    connection_properties["password"]=passwd;

    bool bval= !TestsRunner::getStartOptions()->getBool("dont-use-is");
    connection_properties["metadataUseInfoSchema"]=bval;

    connection_properties.erase("CLIENT_MULTI_RESULTS");
    connection_properties["CLIENT_MULTI_RESULTS"]=true;

    con.reset(driver->connect(connection_properties));
    con->setSchema(db);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

  try
  {
    stmt.reset(con->createStatement());

    try
    {
      stmt->execute("DROP PROCEDURE IF EXISTS p");
    }
    catch (sql::SQLException &e)
    {
      logMsg("... skipping:");
      logMsg(e.what());
      return;
    }

    sql::DatabaseMetaData * dbmeta=con->getMetaData();

    ASSERT(!stmt->execute("DROP TABLE IF EXISTS test"));
    ASSERT(!stmt->execute("CREATE TABLE test(id INT)"));
    ASSERT(!stmt->execute("CREATE PROCEDURE p() BEGIN INSERT INTO test(id) VALUES (123), (456); END;"));
    ASSERT(!stmt->execute("CALL p()"));
    ASSERT_EQUALS(2, (int) stmt->getUpdateCount());
    ASSERT(stmt->execute("SELECT id FROM test ORDER BY id ASC"));
    res.reset(stmt->getResultSet());
    ASSERT(res->next());
    ASSERT_EQUALS(123, res->getInt("id"));
    ASSERT(!stmt->execute("DROP TABLE IF EXISTS test"));
    stmt->execute("DROP PROCEDURE IF EXISTS p");

    ASSERT(!stmt->execute("CREATE PROCEDURE p(OUT ver_param VARCHAR(250)) BEGIN SELECT VERSION() INTO ver_param; END;"));
    ASSERT(!stmt->execute("CALL p(@version)"));
    ASSERT(stmt->execute("SELECT @version AS _version"));
    res.reset(stmt->getResultSet());
    ASSERT(res->next());
    ASSERT_EQUALS(dbmeta->getDatabaseProductVersion(), res->getString("_version"));

    stmt->execute("DROP PROCEDURE IF EXISTS p");
    ASSERT(!stmt->execute("CREATE PROCEDURE p(IN ver_in VARCHAR(250), OUT ver_out VARCHAR(250)) BEGIN SELECT ver_in INTO ver_out; END;"));
    ASSERT(!stmt->execute("CALL p('myver', @version)"));
    ASSERT(stmt->execute("SELECT @version AS _version"));
    res.reset(stmt->getResultSet());
    ASSERT(res->next());
    ASSERT_EQUALS("myver", res->getString("_version"));

    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT, label CHAR(1))");
    ASSERT_EQUALS(3, stmt->executeUpdate("INSERT INTO test(id, label) VALUES (1, 'a'), (2, 'b'), (3, 'c')"));

    stmt->execute("DROP PROCEDURE IF EXISTS p");
    ASSERT(!stmt->execute("CREATE PROCEDURE p() READS SQL DATA BEGIN SELECT id, label FROM test ORDER BY id ASC; END;"));
    ASSERT(stmt->execute("CALL p()"));
    msg.str("");
    do
    {
      res.reset(stmt->getResultSet());
      while (res->next())
      {
        msg << res->getInt("id") << res->getString(2);
      }
    }
    while (stmt->getMoreResults());
    ASSERT_EQUALS("1a2b3c", msg.str());

    stmt->execute("DROP PROCEDURE IF EXISTS p");
    ASSERT(!stmt->execute("CREATE PROCEDURE p() READS SQL DATA BEGIN SELECT id, label FROM test ORDER BY id ASC; SELECT id, label FROM test ORDER BY id DESC; END;"));
    ASSERT(stmt->execute("CALL p()"));
    msg.str("");
    do
    {
      res.reset(stmt->getResultSet());
      while (res->next())
      {
        msg << res->getInt("id") << res->getString(2);
      }
    }
    while (stmt->getMoreResults());
    ASSERT_EQUALS("1a2b3c3c2b1a", msg.str());

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void statement::selectZero()
{
  logMsg("statement::selectZero() - MySQL_Statement::*");

  stmt.reset(con->createStatement());
  try
  {
    res.reset(stmt->executeQuery("SELECT 1, -1, 0"));
    ASSERT(res->next());
    ASSERT_EQUALS("1", res->getString(1));
    ASSERT_EQUALS("-1", res->getString(2));
    ASSERT_EQUALS("0", res->getString(3));
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

}

void statement::unbufferedFetch()
{
  logMsg("statement::unbufferedFetch() - MySQL_Resultset::*");

  sql::ConnectOptionsMap connection_properties;
  int id=0;

  try
  {

    /*
     This is the first way to do an unbuffered fetch...
     */
    /* url comes from the unit testing framework */
    connection_properties["hostName"]=url;
    /* user comes from the unit testing framework */
    connection_properties["userName"]=user;
    connection_properties["password"]=passwd;

    bool bval= !TestsRunner::getStartOptions()->getBool("dont-use-is");
    connection_properties["metadataUseInfoSchema"]=bval;

    logMsg("... setting TYPE_FORWARD_ONLY through connection map");
    connection_properties.erase("defaultStatementResultType");
    {
      connection_properties["defaultStatementResultType"]=sql::ResultSet::TYPE_FORWARD_ONLY;

      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }
      con->setSchema(db);
      stmt.reset(con->createStatement());
      ASSERT_EQUALS(stmt->getResultSetType(), sql::ResultSet::TYPE_FORWARD_ONLY);
      stmt->execute("DROP TABLE IF EXISTS test");

      stmt->execute("CREATE TABLE test(id INT)");
      stmt->execute("INSERT INTO test(id) VALUES (0), (2), (3), (4), (5)");
      ASSERT_EQUALS(5, (int) stmt->getUpdateCount());
      stmt->execute("UPDATE test SET id = 1 WHERE id = 0");
      ASSERT_EQUALS(1, (int) stmt->getUpdateCount());
      stmt->execute("DELETE FROM test WHERE id = 1");
      ASSERT_EQUALS(1, (int) stmt->getUpdateCount());
      stmt->execute("INSERT INTO test(id) VALUES (1)");
      ASSERT_EQUALS(1, (int) stmt->getUpdateCount());

      logMsg("... simple forward reading");
      res.reset(stmt->executeQuery("SELECT id FROM test ORDER BY id ASC"));
      id=0;
      while (res->next())
      {
        id++;
        ASSERT_EQUALS(res->getInt(1), res->getInt("id"));
        ASSERT_EQUALS(id, res->getInt("id"));
      }
      checkUnbufferedScrolling();

      logMsg("... simple forward reading again");
      res.reset(stmt->executeQuery("SELECT id FROM test ORDER BY id DESC"));
      try
      {
        res->beforeFirst();
        FAIL("Nonscrollable result set not detected");
      }
      catch (sql::SQLException &)
      {
      }
      id=5;
      while (res->next())
      {
        ASSERT_EQUALS(res->getInt(1), res->getInt("id"));
        ASSERT_EQUALS(id, res->getInt("id"));
        id--;
      }
    }
    connection_properties.erase("defaultStatementResultType");


    logMsg("... setting TYPE_FORWARD_ONLY through setClientOption()");
    try
    {
      created_objects.clear();
      con.reset(getConnection());
    }
    catch (sql::SQLException &e)
    {
      fail(e.what(), __FILE__, __LINE__);
    }
    int option=sql::ResultSet::TYPE_FORWARD_ONLY;
    con->setSchema(db);
    con->setClientOption("defaultStatementResultType", (static_cast<void *> (&option)));
    stmt.reset(con->createStatement());

    logMsg("... simple forward reading");
    res.reset(stmt->executeQuery("SELECT id FROM test ORDER BY id ASC"));
    id=0;
    while (res->next())
    {
      id++;
      ASSERT_EQUALS(res->getInt(1), res->getInt("id"));
      ASSERT_EQUALS(id, res->getInt("id"));
    }
    checkUnbufferedScrolling();
    logMsg("... simple forward reading again");
    res.reset(stmt->executeQuery("SELECT id FROM test ORDER BY id DESC"));
    try
    {
      res->beforeFirst();
      FAIL("Nonscrollable result set not detected");
    }
    catch (sql::SQLException &)
    {
    }
    id=5;
    while (res->next())
    {
      ASSERT_EQUALS(res->getInt(1), res->getInt("id"));
      ASSERT_EQUALS(id, res->getInt("id"));
      id--;
    }

    logMsg("... setting TYPE_FORWARD_ONLY pointer magic");
    try
    {
      created_objects.clear();
      con.reset(getConnection());
    }
    catch (sql::SQLException &e)
    {
      fail(e.what(), __FILE__, __LINE__);
    }
    con->setSchema(db);
    stmt.reset(con->createStatement());
    res.reset((stmt->setResultSetType(sql::ResultSet::TYPE_FORWARD_ONLY)->executeQuery("SELECT id FROM test ORDER BY id ASC")));
    logMsg("... simple forward reading");

    id=0;
    while (res->next())
    {
      id++;
      ASSERT_EQUALS(res->getInt(1), res->getInt("id"));
      ASSERT_EQUALS(id, res->getInt("id"));
    }
    checkUnbufferedScrolling();

    logMsg("... simple forward reading again");
    res.reset((stmt->setResultSetType(sql::ResultSet::TYPE_FORWARD_ONLY)->executeQuery("SELECT id FROM test ORDER BY id DESC")));
    ASSERT_EQUALS(stmt->getResultSetType(), sql::ResultSet::TYPE_FORWARD_ONLY);
    try
    {
      res->beforeFirst();
      FAIL("Nonscrollable result set not detected");
    }
    catch (sql::SQLException &)
    {
    }
    id=5;
    while (res->next())
    {
      ASSERT_EQUALS(res->getInt(1), res->getInt("id"));
      ASSERT_EQUALS(id, res->getInt("id"));
      id--;
    }

    stmt->execute("DROP TABLE IF EXISTS test");

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void statement::unbufferedOutOfSync()
{
  logMsg("statement::unbufferedOutOfSync() - MySQL_Statement::*");

  try
  {
    stmt.reset(con->createStatement());
    res.reset((stmt->setResultSetType(sql::ResultSet::TYPE_FORWARD_ONLY)->executeQuery("SELECT 1")));
    ASSERT_EQUALS(stmt->getResultSetType(), sql::ResultSet::TYPE_FORWARD_ONLY);

    res.reset((stmt->setResultSetType(sql::ResultSet::TYPE_FORWARD_ONLY)->executeQuery("SELECT 1")));
    FAIL("Commands should be out of sync");
  }
  catch (sql::SQLException &e)
  {
    logMsg("... expecting Out-of-sync exception");
    logMsg(e.what());
    logMsg("SQLState: " + std::string(e.getSQLState()));
  }
}

void statement::checkUnbufferedScrolling()
{
  logMsg("... checkUnbufferedScrolling");
  try
  {
    res->previous();
    FAIL("Nonscrollable result set not detected");
  }
  catch (sql::SQLException &)
  {
  }

  try
  {
    res->absolute(1);
    FAIL("Nonscrollable result set not detected");
  }
  catch (sql::SQLException &)
  {
  }

  try
  {
    res->afterLast();
    logMsg("... a bit odd, but OK, its forward");
  }
  catch (sql::SQLException &e)
  {
    fail(e.what(), __FILE__, __LINE__);
  }

  try
  {
    res->beforeFirst();
    FAIL("Nonscrollable result set not detected");
  }
  catch (sql::SQLException &)
  {
  }

  try
  {
    res->last();
    FAIL("Nonscrollable result set not detected");
  }
  catch (sql::SQLException &)
  {
  }
}


void statement::queryTimeout()
{
  logMsg("statement::queryTimeout() - MySQL_Statement::setQueryTimeout");

  int serverVersion= getMySQLVersion(con);
  int timeout= 3;
  if ( serverVersion < 57004 )
  {
    SKIP("Server version >= 5.7.4 needed to run this test");
  }

  stmt.reset(con->createStatement());

  try
  {
    stmt->setQueryTimeout(timeout);
    ASSERT_EQUALS(timeout, stmt->getQueryTimeout());
    time_t t1= time(NULL);
    res.reset(stmt->executeQuery("select sleep(5)"));
    time_t t2= time(NULL);
    ASSERT((t2-t1) >= timeout);
    ASSERT((t2-t1) < 5);

    while(res->next());

    timeout = 1;
    pstmt.reset(con->prepareStatement("select sleep(5) "));
    pstmt->setQueryTimeout(timeout);
    t1= time(NULL);
    res.reset(pstmt->executeQuery());
    t2= time(NULL);
    ASSERT((t2-t1) >= timeout);
    ASSERT((t2-t1) < 3);

    while(res->next());
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void statement::queryAttributes()
{
  stmt.reset(con->createStatement());

  if(stmt->setQueryAttrNull("dummy") == 0)
    SKIP("Server doesn't support query attributes");

  try {
    stmt->execute("UNINSTALL COMPONENT 'file://component_query_attributes'");
  }  catch (...)
  {}

  stmt->execute("INSTALL COMPONENT 'file://component_query_attributes'");

// Scenario 1

  stmt->setQueryAttrInt("attr1", 200);
  stmt->setQueryAttrString("attr2", "string value");
  stmt->setQueryAttrBoolean("attr3", true);

  res.reset(stmt->executeQuery("SELECT 1,"
                               "mysql_query_attribute_string('attr1'), "
                               "mysql_query_attribute_string('attr2'), "
                               "mysql_query_attribute_string('attr3')"));

  ASSERT(res->next());
  ASSERT_EQUALS(1, res->getInt(1));
  ASSERT_EQUALS(200, res->getInt(2));
  ASSERT_EQUALS("string value", res->getString(3));
  ASSERT_EQUALS(true, res->getBoolean(4));

  // Scenario 2
  stmt->setQueryAttrInt("attr1", 200);
  res.reset(stmt->executeQuery("SELECT 1,"
                               "mysql_query_attribute_string('attr1'), "
                               "mysql_query_attribute_string('attr2'), "
                               "mysql_query_attribute_string('attr3')"));

  ASSERT(res->next());
  ASSERT_EQUALS(1, res->getInt(1));
  ASSERT_EQUALS(200, res->getInt(2));
  ASSERT_EQUALS("string value", res->getString(3));
  ASSERT_EQUALS(true, res->getBoolean(4));

  // Scenario 3
  stmt->setQueryAttrInt("attr1", 100);
  res.reset(stmt->executeQuery("SELECT 1,"
                               "mysql_query_attribute_string('attr1'), "
                               "mysql_query_attribute_string('attr2'), "
                               "mysql_query_attribute_string('attr3')"));

  ASSERT(res->next());
  ASSERT_EQUALS(1, res->getInt(1));
  ASSERT_EQUALS(100, res->getInt(2));
  ASSERT_EQUALS("string value", res->getString(3));
  ASSERT_EQUALS(true, res->getBoolean(4));

  //Scenario 4
  stmt->clearAttributes();
  res.reset(stmt->executeQuery("SELECT 1,"
                               "mysql_query_attribute_string('attr1'), "
                               "mysql_query_attribute_string('attr2'), "
                               "mysql_query_attribute_string('attr3')"));

  ASSERT(res->next());
  ASSERT_EQUALS(1, res->getInt(1));
  ASSERT(res->isNull(2));
  ASSERT(res->isNull(3));
  ASSERT(res->isNull(4));

  stmt->execute("UNINSTALL COMPONENT 'file://component_query_attributes'");

}


} /* namespace statement */
} /* namespace testsuite */
