/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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



#include "bug456.h"

namespace testsuite
{
namespace regression
{

void template_bug456_class::template_bug456_method()
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
