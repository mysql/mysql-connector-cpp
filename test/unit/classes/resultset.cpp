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
    stmt->execute("DROP TABLE IF EXISTS test_function_int");
    stmt->execute("CREATE TABLE test_function_int(i integer, i_uns integer unsigned, b bigint, b_uns bigint unsigned)");

		int64_t	r1_c1 = L64(2147483646),
				r1_c2 = L64(2147483650),
				r1_c3 = L64(9223372036854775806),
				r2_c1 = L64(2147483647),
				r2_c2 = L64(2147483649),
				r2_c3 = L64(9223372036854775807);

		uint64_t r1_c4 = UL64(9223372036854775810),
				 r2_c4 = UL64(18446744073709551615);
				
		pstmt.reset(con->prepareStatement("INSERT INTO test_function_int (i, i_uns, b, b_uns) VALUES(?,?,?,?)"));

		ASSERT(pstmt.get() != NULL);
		pstmt->clearParameters();
		pstmt->setInt(1, r1_c1);
		pstmt->setInt64(2, r1_c2);
		pstmt->setInt64(3, r1_c3);
		pstmt->setUInt64(4, r1_c4);
		ASSERT_EQUALS(false, pstmt->execute());

		pstmt->clearParameters();
		pstmt->setInt(1, r2_c1);
		pstmt->setInt64(2, r2_c2);
		pstmt->setInt64(3, r2_c3);
		pstmt->setUInt64(4, r2_c4);
		ASSERT_EQUALS(false, pstmt->execute());
	
		pstmt.reset(con->prepareStatement("SELECT i, i_uns, b, b_uns FROM test_function_int"));
		ASSERT(pstmt.get() != NULL);
		ASSERT(pstmt->execute());

		res.reset(pstmt->getResultSet());

		ASSERT(res->next());

		ASSERT_EQUALS((int64_t) res->getInt("i"), r1_c1);
		ASSERT_EQUALS((int64_t) res->getInt(1), r1_c1);

		ASSERT_EQUALS(res->getInt64("i_uns"), r1_c2);
		ASSERT_EQUALS(res->getInt64(2), r1_c2);

		ASSERT_EQUALS(res->getInt64("b"), r1_c3);
		ASSERT_EQUALS(res->getInt64(3), r1_c3);

		ASSERT_EQUALS(res->getUInt64("b_uns"), r1_c4);
		ASSERT_EQUALS(res->getUInt64(4), r1_c4);

		ASSERT(res->next());

		ASSERT_EQUALS((int64_t) res->getInt("i"), r2_c1);
		ASSERT_EQUALS((int64_t) res->getInt(1), r2_c1);

		ASSERT_EQUALS(res->getInt64("i_uns"), r2_c2);
		ASSERT_EQUALS(res->getInt64(2), r2_c2);

		ASSERT_EQUALS(res->getInt64("b"), r2_c3);
		ASSERT_EQUALS(res->getInt64(3), r2_c3);

		ASSERT_EQUALS(res->getUInt64("b_uns"), r2_c4);
		ASSERT_EQUALS(res->getUInt64(4), r2_c4);

		ASSERT_EQUALS(res->next(), false);


		res.reset(stmt->executeQuery("SELECT i, i_uns, b, b_uns FROM test_function_int"));

		ASSERT(res->next());

		ASSERT_EQUALS((int64_t) res->getInt("i"), r1_c1);
		ASSERT_EQUALS((int64_t) res->getInt(1), r1_c1);

		ASSERT_EQUALS(res->getInt64("i_uns"), r1_c2);
		ASSERT_EQUALS(res->getInt64(2), r1_c2);

		ASSERT_EQUALS(res->getInt64("b"), r1_c3);
		ASSERT_EQUALS(res->getInt64(3), r1_c3);

		ASSERT_EQUALS(res->getUInt64("b_uns"), r1_c4);
		ASSERT_EQUALS(res->getUInt64(4), r1_c4);

		ASSERT(res->next());

		ASSERT_EQUALS((int64_t) res->getInt("i"), r2_c1);
		ASSERT_EQUALS((int64_t) res->getInt(1), r2_c1);

		ASSERT_EQUALS(res->getInt64("i_uns"), r2_c2);
		ASSERT_EQUALS(res->getInt64(2), r2_c2);

		ASSERT_EQUALS(res->getInt64("b"), r2_c3);
		ASSERT_EQUALS(res->getInt64(3), r2_c3);

		ASSERT_EQUALS(res->getUInt64("b_uns"), r2_c4);
		ASSERT_EQUALS(res->getUInt64(4), r2_c4);

		ASSERT_EQUALS(res->next(), false);

    // Ocasionally write a log message which appears when you run the test using --verbose
    // logMsg("... creating table for INT test");    
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
