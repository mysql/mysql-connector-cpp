/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include <cppconn/warning.h>
#include "perf_statement.h"
#include <stdlib.h>

namespace testsuite
{
namespace performance
{

void perf_statement::anonymousSelect()
{
  logMsg("perf_statement::anonymousSelect() - MySQL_Statement::*, MYSQL_Resultset::*");

  stmt.reset(con->createStatement());
  try
  {
    sleep(1);
    double i = 0;
    Timer::startTimer("loop");
    for (int j = 1; j < INT_MAX / 30; j++)
      i = i + 0.27823873787832;
    Timer::stopTimer("loop");
    
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

} /* namespace perf_statement */
} /* namespace testsuite */
