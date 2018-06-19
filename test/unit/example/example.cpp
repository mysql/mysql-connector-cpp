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

void example_test_class::test_hello_world2()
{
  logMsg("Hello world without framework magic");
  try
  {
    /*
     Connection, Statement and ResultSet are typedefs from unit_fixture.h:

     typedef boost::scoped_ptr<sql::Connection> Connection;
     typedef boost::scoped_ptr<sql::PreparedStatement> PreparedStatement;
     typedef boost::scoped_ptr<sql::ParameterMetaData> ParameterMetaData;
     typedef boost::scoped_ptr<sql::Statement> Statement;
     typedef boost::scoped_ptr<sql::Savepoint> Savepoint;
     typedef boost::scoped_ptr<sql::ResultSet> ResultSet;
     typedef sql::Driver Driver;
     typedef boost::scoped_ptr<sql::ResultSetMetaData> ResultSetMetaData;
     typedef boost::scoped_ptr<sql::DatabaseMetaData> DatabaseMetaData;

     Do yourself a favour and use boost::scoped_ptr in tests!
     */
    Connection my_con(getConnection());
    Statement my_stmt(my_con->createStatement());

    /*
     The framework defines a couple of public members (from unit_fixture.h):

     Connection con;
     PreparedStatement pstmt;
     Statement stmt;
     ResultSet res;

     The members are managed in unit_fixture.cpp setUp() and tearDown().
     You don't need to clean up them as long as setUp() and tearDown()
     are called and are not overwritten by your own method.

     However note, if you declare a local variable of the same type
     and name of any of the above members, for example ResultSet res, some
     compilers will spawn warnings that you are hiding members.
     */
    res.reset(my_stmt->executeQuery("SELECT 'Hello world!' AS _world"));

    res->next();
    logMsg(res->getString("_world"));
    res->close();

    ResultSet res2(my_stmt->executeQuery("SELECT 'What a boring example' AS _complain"));
    res2->next();
    logMsg(res2->getString("_complain"));
    res2->close();
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void example_test_class::test_assert_equals()
{
  logMsg("ASSERT_EQUALS() macro demo");
  /*
   Be careful: ASSERT_EQUALS() is not available for each and every type!
   You can find more details in test_asserts.h.
   */
  int a=-1;
  ASSERT_EQUALS(a, a);

  unsigned int h=1;
  ASSERT_EQUALS(h, h);

  int64_t i=-10;
  ASSERT_EQUALS(i, i);

  uint64_t j=10;
  ASSERT_EQUALS(j, j);

  bool b=false;
  ASSERT_EQUALS(b, b);

  float c= -1.23f;
  ASSERT_EQUALS(c, c);

  double d=1.23;
  ASSERT_EQUALS(d, d);

  /*
   For fuzzy comparisons e.g. 1.01 vs. 1.01 fetched from MySQL
   */
  double k = 1.01;
  double l = 1.0999992;
  fuzzyEquals(k, l, 0.001);

  long double e=1.23e12;
  ASSERT_EQUALS(e, e);

  const char f[]="foo";
  ASSERT_EQUALS(f, f);

  std::string g("good idea");
  ASSERT_EQUALS(g, g);
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
  ASSERT_GT(0, 2);
}

void example_test_class::test_assert_equals_fail()
{
  logMsg("ASSERT_EQUALS failure - EXPECTED failure");
  ASSERT_EQUALS(true, false);
}

void example_test_class::test_assert_message()
{
  logMsg("ASSERT_MESSAGE example");
  /*
   Wrapper of
  void assertTrue(const char * msg, bool expression
                , const char* file, int line);
   */
  ASSERT_MESSAGE(1 < 2, "ASSERT_MESSAGE example (true)");
  ASSERT_MESSAGE(1 > 2, "ASSERT_MESSAGE example (fail) - EXPECTED failure");
}

void example_test_class::test_skip()
{
  logMsg("SKIP() example");
  /* Test if MySQL has a certain feature, e.g. if Procedures are supported */
  if (true)
    SKIP("EXAMPLE: the server does not support stored procedures");
  /*
   If MySQL does not suppoer feature xyz, call SKIP() and test execution ends.
   You get here only if you feature is available
   */
  logMsg("Feature is available, use it.");
}

void example_test_class::test_todo()
{
  logMsg("TODO() example");
  /* Test if MySQL has a certain feature, e.g. if Procedures are supported */
  TODO("If you know that a test mail fail and you intentionally want to let it fail temporarily, for example, as long as a bug has not been fixed, mark it as TODO");
  //FAIL("Expected failure, known failure - its on your TODO");
}

} /* namespace example */
} /* namespace testsuite */
