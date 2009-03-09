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
  const sql::SQLWarning* warn;
  std::stringstream msg;

  stmt.reset(con->createStatement());
  try
  {
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT UNSIGNED)");
    // Lets hope that this will always cause a 1264 or similar warning
    stmt->execute("INSERT INTO test(id) VALUES (-1)");
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

} /* namespace statement */
} /* namespace testsuite */


