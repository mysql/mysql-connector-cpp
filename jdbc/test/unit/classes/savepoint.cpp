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



#include <cppconn/connection.h>


#include <cppconn/warning.h>

#include "savepoint.h"
#include <stdlib.h>

#include <boost/scoped_ptr.hpp>

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
    boost::scoped_ptr< sql::Savepoint > sp(con->setSavepoint("mysavepoint"));
    FAIL("You should not be able to set a savepoint in autoCommit mode");
  }
  catch (sql::SQLException &)
  {
  }

  try
  {
    con->setAutoCommit(false);
    boost::scoped_ptr< sql::Savepoint > sp(con->setSavepoint("mysavepoint"));
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
    boost::scoped_ptr< sql::Savepoint > sp(con->setSavepoint("mysavepoint"));
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
