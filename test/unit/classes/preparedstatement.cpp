
/*
   Copyright 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include <cppconn/connection.h>


#include <cppconn/warning.h>

#include "preparedstatement.h"
#include <stdlib.h>

namespace testsuite
{
namespace classes
{

void preparedstatement::getSavepointId()
{
  logMsg("preparedstatement::getSavepointId() - MySQL_Savepoint::getSavepointId()");
  sql::Savepoint* sp;

  try
  {
    con->setAutoCommit(true);
    sp=con->setSavepoint("mypreparedstatement");
    FAIL("You should not be able to set a preparedstatement in autoCommit mode");
  }
  catch (sql::SQLException &)
  {
  }

  try
  {
    con->setAutoCommit(false);
    sp=con->setSavepoint("mypreparedstatement");
    try
    {
      sp->getSavepointId();
      FAIL("Anonymous preparedstatements are not supported");
    }
    catch (sql::InvalidArgumentException &)
    {
    }
    con->releaseSavepoint(sp);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void preparedstatement::getSavepointName()
{
  logMsg("preparedstatement::getSavepointName() - MySQL_Savepoint::getSavepointName()");
  sql::Savepoint* sp;
  try
  {
    con->setAutoCommit(false);
    sp=con->setSavepoint("mypreparedstatement");
    ASSERT_EQUALS("mypreparedstatement", sp->getSavepointName());
    con->releaseSavepoint(sp);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

} /* namespace preparedstatement */
} /* namespace testsuite */
