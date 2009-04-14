/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "bug123.h"

namespace testsuite
{
namespace regression
{

void template_bug123_class::template_bug123_method()
{
  logMsg("Regression test for #[TODO - add bug numer]");
  try
  {
    /*
     By default the framework will establish a connection in setUp() and connect
     to the configured MySQL Server and select the configured schema.
     No other members will be initialized, however, there are several
     auto-ptr members which you can make use of: this->stmt, this->pstmt, this->res.
     The good thing about the auto-ptr members is that you don't need to care much
     about them. tearDown() will reset them and auto-ptr will ensure proper
     memory management.
     */
    stmt.reset(con->createStatement());

    /* Running a SELECT and storing the returned result set in this->res */
    res.reset(stmt->executeQuery("SELECT 'Hello world!'"));

    /* Move result set cursor to first rowm, fetch result, write result to log  */
    res->next();
    logMsg(res->getString(1));

  }
  catch (sql::SQLException &e)
  {
    /* If anything goes wrong, write some info to the log... */
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));

    /*
     ... and let the test fail. FAIL() is a macro.
     FAIL calls fail(const char* reason, const char* file, int line)
     */
    fail(e.what(), __FILE__, __LINE__);
  }
  /* If all goes fine, there is no need to call PASS() or something.	 */
}

} /* namespace regression */
} /* namespace testsuite */
