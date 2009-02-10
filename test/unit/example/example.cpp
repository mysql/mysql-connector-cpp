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

#include "example.h"

namespace testsuite
{
namespace example
{

void example_test_class::test_hello_world1()
{
  logMsg("Hello world using framework magic");
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

  } catch (sql::SQLException &e)
  {
    /* If anything goes wrong, write some info to the log... */
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());

    /*
     ... and let the test fail. FAIL() is a macro.
     FAIL calls fail(const char* reason, const char* file, int line)
     */
    FAIL(e.what());
  }
  /* If all goes fine, there is no need to call PASS() or something.	 */
}

void example_test_class::test_hello_world2()
{
  logMsg("Hello world without framework magic");
  try
  {
    /*
     Connection, Statement and ResultSet are typedefs from unit_fixture.h:

     typedef std::auto_ptr<sql::Connection> Connection
     typedef std::auto_ptr<sql::Statement> Statement;
     typedef std::auto_ptr<sql::ResultSet> ResultSet;
     typedef std::auto_ptr<sql::ResultSetMetaData> ResultSetMetaData;
     typedef std::auto_ptr<sql::DatabaseMetaData> DatabaseMetaData;

     Do yourself a favour and use auto_ptr in tests!
     */
    Connection con(getConnection());
    Statement stmt(con->createStatement());
    ResultSet res(stmt->executeQuery("SELECT 'Hello world!' AS _world"));

    res->next();
    logMsg(res->getString("_world"));
    res->close();

    ResultSet res2(stmt->executeQuery("SELECT 'What a boring example' AS _complain"));
    logMsg(res2->getString("_world"));
    res2->close();

  } catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    FAIL(e.what());
  }
}

void example_test_class::test_assert_equals()
{
  logMsg("ASSERT_EQUALS() macro demo");
  /*
   Be careful: ASSERT_EQUALS() is not available for each and every type!
   */
  int a=1;
  ASSERT_EQUALS(a, a);

  bool b=false;
  ASSERT_EQUALS(b, b);

  float c= -1.23f;
  ASSERT_EQUALS(c, c);

  double d=1.23;
  ASSERT_EQUALS(d, d);

  long double e=1.23e12;
  ASSERT_EQUALS(e, e);

  const char f[]="foo";
  ASSERT_EQUALS(f, f);
}

void example_test_class::test_assert_lessthan()
{
  logMsg("ASSERT_LT() macro demo");
  /*
   Be careful: ASSERT_LT(expected, value) is not available for each and every type!
   In fact at the time of writing its only available for int and unsigned int
   */
  int a=1, b=2;

  ASSERT_LT(b, a);
  ASSERT_LT((unsigned int) b, (unsigned int) a);

  /* There is no "less or equal than", use LT instead.
  Example: 1 <= 2 */
  ASSERT_LT(3, 1);
}

void example_test_class::test_assert_greaterthan()
{
  logMsg("ASSERT_GT() macro demo");
  /*
   Be careful: ASSERT_GT(expected, value) is not available for each and every type!
   In fact at the time of writing its only available for int and unsigned int
   */
  int a=1, b=2;

  ASSERT_GT(a, b);
  ASSERT_GT((unsigned int) a, (unsigned int) b);

  /* There is no "greater or equal than", use GT instead.
  Example: 2 >= 1 */
  ASSERT_GT(2, 0);
}

void example_test_class::test_assert_equals_fail()
{
  logMsg("ASSERT_EQUALS failure");
  ASSERT_EQUALS(true, false);
}

void example_test_class::test_assert_message()
{
  logMsg("ASSERT_MESSAGE example");
  /*
   Wrapper of
  void assertTrue(const char * msg, bool expression
                , const char* file, int line);

  ASSERT_MESSAGE(1 < 2, "ASSERT_MESSAGE example (true)");
  ASSERT_MESSAGE(1 > 2, "ASSERT_MESSAGE example (fail)");
   */
}

} /* namespace example */
} /* namespace testsuite */
