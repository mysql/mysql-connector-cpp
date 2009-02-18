/*
Copyright 2008 - 2009 Sun Microsystems, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

There are special exceptions to the terms and conditions of the GPL
as it is applied to this software. View the full text of the
exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
software distribution.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cppconn/warning.h>

#include "resultset.h"
#include <sstream>
#include <stdlib.h>
#include <cppconn/resultset.h>
#include <cppconn/datatype.h>
#include <cppconn/connection.h>

namespace testsuite
{
namespace classes
{

void resultset::andrey1()
{
  // Message for --verbose output
  logMsg("resultset::andrey1 - MySQL_ResultSet::getInt*");
  try
  {
    /*
     Only FYI, Andrey - remove this - this comes from the framework test/unit/unit_fixture*!
    typedef std::auto_ptr<sql::Connection> Connection;
    typedef std::auto_ptr<sql::PreparedStatement> PreparedStatement;
    typedef std::auto_ptr<sql::Statement> Statement;
    typedef std::auto_ptr<sql::ResultSet> ResultSet;
    typedef sql::Driver Driver;
    typedef std::auto_ptr<sql::ResultSetMetaData> ResultSetMetaData;
    typedef std::auto_ptr<sql::DatabaseMetaData> DatabaseMetaData;
     */

    // Use connection from the framework
    stmt.reset(con->createStatement());

    /*
     Only FYI, Andrey - remove this - those public auto_ptr vars are initialized by the framework
     Connection con
     PreparedStatement pstmt
     Statement stmt
     ResultSet res
     */

    /* Only FYI, Andrey - remove this -
     There's some abstraction in the
     framework for creating SQL objects, I don't find it handy.
     Its used in the JDBC compliance tests, however.
     */
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT)");

    // Ocasionally write a log message which appears when you run the test using --verbose
    logMsg("... creating table for INT test");

    // You probabaly want to use PS for this...
    stmt->execute("INSERT INTO test(id) VALUES (9999)");

    res.reset(stmt->executeQuery("SELECT id FROM test"));
    // ASSERT(<expression> == true)
    ASSERT(res->next());
    // ASSERT(expected, retrieved) is not overloaded for all types!
    ASSERT_EQUALS(9999, res->getInt(1));
    ASSERT_EQUALS(res->getInt(1), res->getInt("id"));

    // And there is ASSERT_LT and ASSERT_GT
    ASSERT_LT(100, res->getInt(1));
    
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
  
}

} /* namespace resultset */
} /* namespace testsuite */
