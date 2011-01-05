/*
Copyright (c) 2009, 2011, Oracle and/or its affiliates. All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPLv2
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPLv2 as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/



#include <cppconn/connection.h>


#include <cppconn/warning.h>

#include "savepoint.h"
#include <stdlib.h>

namespace testsuite
{
namespace classes
{

void savepoint::getSavepointId()
{
  logMsg("savepoint::getSavepointId() - MySQL_Savepoint::getSavepointId()");

  try
  {
    con->setAutoCommit(true);
    std::auto_ptr< sql::Savepoint > sp(con->setSavepoint("mysavepoint"));
    FAIL("You should not be able to set a savepoint in autoCommit mode");
  }
  catch (sql::SQLException &)
  {
  }

  try
  {
    con->setAutoCommit(false);
    std::auto_ptr< sql::Savepoint > sp(con->setSavepoint("mysavepoint"));
    try
    {
      sp->getSavepointId();
      FAIL("Anonymous savepoints are not supported");
    }
    catch (sql::InvalidArgumentException &)
    {
    }
    con->releaseSavepoint(sp.get());
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void savepoint::getSavepointName()
{
  logMsg("savepoint::getSavepointName() - MySQL_Savepoint::getSavepointName()");
  try
  {
    con->setAutoCommit(false);
    std::auto_ptr< sql::Savepoint > sp(con->setSavepoint("mysavepoint"));
    ASSERT_EQUALS("mysavepoint", sp->getSavepointName());
    con->releaseSavepoint(sp.get());
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

} /* namespace savepoint */
} /* namespace testsuite */
