/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include <cppconn/prepared_statement.h>
#include <cppconn/connection.h>
#include <cppconn/warning.h>
#include "statement.h"
#include <stdlib.h>

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
  std::stringstream msg;

  stmt.reset(con->createStatement());
  try
  {
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT UNSIGNED)");

    // Lets hope that this will always cause a 1264 or similar warning
    stmt->execute("INSERT INTO test(id) VALUES (-1)");

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
    for (const sql::SQLWarning* warn=stmt->getWarnings(); warn; warn=warn->getNextWarning()) {
      FAIL("There should be no more warnings!");
  }

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
  std::map<std::string, sql::ConnectPropertyVal> connection_properties;

  try
  {
    std::map<std::string, sql::ConnectPropertyVal> connection_properties;

    {
      sql::ConnectPropertyVal tmp;
      /* url comes from the unit testing framework */
      tmp.str.val=url.c_str();
      tmp.str.len=url.length();
      connection_properties[std::string("hostName")]=tmp;
    }

    {
      sql::ConnectPropertyVal tmp;
      /* user comes from the unit testing framework */
      tmp.str.val=user.c_str();
      tmp.str.len=user.length();
      connection_properties[std::string("userName")]=tmp;
    }

    {
      sql::ConnectPropertyVal tmp;
      tmp.str.val=passwd.c_str();
      tmp.str.len=passwd.length();
      connection_properties[std::string("password")]=tmp;
    }

    {
      sql::ConnectPropertyVal tmp;
      tmp.bval= !TestsRunner::getStartOptions()->getBool("dont-use-is");
      connection_properties[std::string("metadataUseInfoSchema")]=tmp;
    }

    connection_properties.erase("CLIENT_MULTI_RESULTS");
    {
      sql::ConnectPropertyVal tmp;
      tmp.bval=true;
      connection_properties[std::string("CLIENT_MULTI_RESULTS")]=tmp;
    }
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

    ASSERT(!stmt->execute("CREATE PROCEDURE p(OUT ver_param VARCHAR(25)) BEGIN SELECT VERSION() INTO ver_param; END;"));
    ASSERT(!stmt->execute("CALL p(@version)"));
    ASSERT(stmt->execute("SELECT @version AS _version"));
    res.reset(stmt->getResultSet());
    ASSERT(res->next());
    ASSERT_EQUALS(dbmeta->getDatabaseProductVersion(), res->getString("_version"));

    stmt->execute("DROP PROCEDURE IF EXISTS p");
    ASSERT(!stmt->execute("CREATE PROCEDURE p(IN ver_in VARCHAR(25), OUT ver_out VARCHAR(25)) BEGIN SELECT ver_in INTO ver_out; END;"));
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
    } while (stmt->getMoreResults());
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
    } while (stmt->getMoreResults());
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

  std::map<std::string, sql::ConnectPropertyVal> connection_properties;
  int id=0;

  try
  {

    /*
     This is the first way to do an unbuffered fetch...
     */
    {
      sql::ConnectPropertyVal tmp;
      /* url comes from the unit testing framework */
      tmp.str.val=url.c_str();
      tmp.str.len=url.length();
      connection_properties[std::string("hostName")]=tmp;
    }

    {
      sql::ConnectPropertyVal tmp;
      /* user comes from the unit testing framework */
      tmp.str.val=user.c_str();
      tmp.str.len=user.length();
      connection_properties[std::string("userName")]=tmp;
    }

    {
      sql::ConnectPropertyVal tmp;
      tmp.str.val=passwd.c_str();
      tmp.str.len=passwd.length();
      connection_properties[std::string("password")]=tmp;
    }

    {
      sql::ConnectPropertyVal tmp;
      tmp.bval= !TestsRunner::getStartOptions()->getBool("dont-use-is");
      connection_properties[std::string("metadataUseInfoSchema")]=tmp;
    }

    logMsg("... setting TYPE_FORWARD_ONLY through connection map");
    connection_properties.erase("defaultStatementResultType");
    {
      sql::ConnectPropertyVal tmp;
      tmp.lval=sql::ResultSet::TYPE_FORWARD_ONLY;
      connection_properties[std::string("defaultStatementResultType")]=tmp;

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
      stmt->execute("INSERT INTO test(id) VALUES (1), (2), (3), (4), (5)");

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

} /* namespace statement */
} /* namespace testsuite */
