/*
   Copyright 2009 Sun Microsystems, Inc.

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

void statement::getWarnings()
{
  logMsg("statement::getWarnings() - MySQL_Statement::getWarnings()");
  const sql::SQLWarning* warn;
  std::stringstream msg;

  stmt.reset(con->createStatement());
  try
  {
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT UNSIGNED)");

    // Lets hope that this will always cause a 1264 or similar warning
    stmt->execute("INSERT INTO test(id) VALUES (-1)");
    warn=stmt->getWarnings();

    do
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
    while (false);
    //while ((warn=warn->getNextWarning()));

    // TODO - how to use getNextWarning() ?
    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
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
    stmt->execute("CREATE TABLE test(id INT UNSIGNED)");
    // Lets hope that this will always cause a 1264 or similar warning
    ASSERT(!stmt->execute("INSERT INTO test(id) VALUES (-1)"));
    stmt->clearWarnings();
    // TODO - how to verify?
    // TODO - how to use getNextWarning() ?
    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }

}

void statement::callSP()
{
  logMsg("statement::callSP() - MySQL_Statement::*");
  std::stringstream msg;

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

    DatabaseMetaData dbmeta(con->getMetaData());

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
    res.reset(stmt->executeQuery("CALL p()"));
    msg.str("");
    while (res->next())
    {
      msg << res->getInt("id") << res->getString(2);
    }
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
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

} /* namespace statement */
} /* namespace testsuite */
