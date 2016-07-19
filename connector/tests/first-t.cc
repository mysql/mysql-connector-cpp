/*
* Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
*
* The MySQL Connector/C++ is licensed under the terms of the GPLv2
* <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
* MySQL Connectors. There are special exceptions to the terms and
* conditions of the GPLv2 as it is applied to this software, see the
* FLOSS License Exception
* <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published
* by the Free Software Foundation; version 2 of the License.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <test.h>
#include <mysqlx.h>
#include <iostream>
#include <boost/format.hpp>

using std::cout;
using std::endl;
using namespace mysqlx;

class First : public mysqlx::test::Xplugin
{
};


TEST_F(First, first)
{
  SKIP_IF_NO_XPLUGIN;

  SqlResult res = get_sess().sql("SELECT @@version").execute();
  cout << "Talking to MySQL Server: " << res.fetchOne()[0] << endl;
  cout << "Done!" << endl;
}


TEST_F(First, sql)
{
  SKIP_IF_NO_XPLUGIN;

  sql("DROP TABLE IF EXISTS test.t");
  sql("CREATE TABLE test.t(c0 INT, c1 TEXT)");

  get_sess().sql(L"INSERT INTO test.t VALUES (?,?)")
    .bind(33, L"foo")
    .execute();
  get_sess().sql(L"INSERT INTO test.t VALUES (?,?)")
    .bind(13)
    .bind(L"bar")
    .execute();

  int args[] = { 7, 30 };

  RowResult res = get_sess().sql(L"SELECT *,? FROM test.t WHERE c0 > ?")
                            .bind(args)
                            .execute();

  Row row = res.fetchOne();

  cout << "col#0: " << row[0] << endl;
  cout << "col#1: " << row[1] << endl;
  cout << "col#2: " << row[2] << endl;

  EXPECT_EQ(args[0], (int)row[2]);
  EXPECT_LT(args[1], (int)row[0]);

  cout << "Done!" << endl;
}


TEST_F(First, sql_multi)
{
  // Testing multi result sets
  SKIP_IF_NO_XPLUGIN;

  sql(L"DROP PROCEDURE IF EXISTS test.test");
  sql(L"CREATE PROCEDURE test.test() BEGIN SELECT 1; SELECT 2, 'foo'; END");

  {
    SqlResult res = get_sess().sql(L"CALL test.test()").execute();

    EXPECT_TRUE(res.hasData());

    cout << "-- first rset --" << endl;

    EXPECT_EQ(1U, res.getColumnCount());

    Row row = res.fetchOne();
    EXPECT_TRUE(row);

    for (unsigned i = 0; i < res.getColumnCount(); ++i)
      cout << "- col#" << i << ": " << row[i] << endl;

    EXPECT_TRUE(res.nextResult());
    EXPECT_TRUE(res.hasData());

    cout << "-- second rset --" << endl;

    EXPECT_EQ(2U, res.getColumnCount());

    row = res.fetchOne();
    EXPECT_TRUE(row);

    for (unsigned i = 0; i < res.getColumnCount(); ++i)
      cout << "- col#" << i << ": " << row[i] << endl;

    EXPECT_FALSE(res.nextResult());
  }

  // with buffering

  {
    SqlResult res = get_sess().sql(L"CALL test.test()").execute();

    cout << "-- first rset --" << endl;

    std::vector<Row> rows = res.fetchAll();
    EXPECT_EQ(1U, rows.size());

    EXPECT_TRUE(res.nextResult());

    cout << "-- second rset --" << endl;
    EXPECT_EQ(2U, res.getColumnCount());

    rows = res.fetchAll();
    EXPECT_EQ(1U, rows.size());

    EXPECT_FALSE(res.nextResult());
  }

  // check discarding of multi-rset

  {
    SqlResult res = get_sess().sql(L"CALL test.test()").execute();
    EXPECT_TRUE(res.fetchOne());
  }

  {
    SqlResult res = get_sess().sql(L"CALL test.test()").execute();
  }

  cout << "Done!" << endl;
}


TEST_F(First, api)
{
  // Check that assignment works for database objects.

  SKIP_IF_NO_XPLUGIN;

  Schema s = get_sess().getSchema("foo");
  s = get_sess().getSchema("test");

  Table t = s.getTable("t1");
  t = s.getTable("t");

  Collection c = s.getCollection("c1");
  c = s.getCollection("c");

  sql("DROP TABLE IF EXISTS test.t");
  sql("CREATE TABLE test.t(c0 INT, c1 TEXT)");
  s.createCollection("c", true);

  {
    RowResult res;
    EXPECT_THROW(res.fetchOne(),Error);
    res = t.select().execute();
  }

  {
    DocResult res;
    EXPECT_THROW(res.fetchOne(),Error);
    res = c.find().execute();
  }
}


/*
  Test different forms of session constructor.

  The goal of this test is to check that session can be constructed
  given session parameters of appropriate types. Different forms of
  constructors are tested as well as whether implicit conversions for
  parameter types work as expected.

  The S_ctor_test<> template defines static method test() which calls
  t0<T>::test() for different types T of the first session parameter.
  The t0<T>::test() is defined in similar way, testing different possible
  types of the second session parameter and so on.

  Tests create a session for invalid IP address 0.0.0.0 expecting session
  constructor to throw error.
*/

template <class Session>
struct S_ctor_test
{
  template <typename A>
  struct t0
  {
    template <typename B>
    struct t1
    {
      template <typename C>
      struct t2
      {
        template <typename D>
        struct t3
        {
          template <typename E>
          struct t4
          {
            static void test(A host, B port, C user, D pwd, E db)
            {
              try {
                Session s(host, port, user, pwd, db);
              }
              catch (const Error&)
              {}

              try {
                Session s(host, port, user, NULL, db);
              }
              catch (const Error&)
              {}

              try {
                Session s(port, user, pwd, db);
              }
              catch (const Error&)
              {
              }

              try {
                Session s(port, user, NULL, db);
              }
              catch (const Error&)
              {
              }

              try {
                Session s(host, user, pwd, db);
              }
              catch (const Error&)
              {
              }

              try {
                Session s(host, user, NULL, db);
              }
              catch (const Error&)
              {
              }
            }
          };

          static void test(A host, B port, C user, D pwd)
          {
            t4<string>::test(host, port, user, pwd, "db");
            t4<std::string>::test(host, port, user, pwd, "db");
            t4<const char*>::test(host, port, user, pwd, "db");
            t4<const wchar_t*>::test(host, port, user, pwd, L"db");

            try {
              Session s(host, port, user, pwd);
            }
            catch (const Error&)
            {}

            try {
              Session s(host, port, user, NULL);
            }
            catch (const Error&)
            {}

            try {
              Session s(port, user, pwd);
            }
            catch (const Error&)
            {
            }

            try {
              Session s(port, user, NULL);
            }
            catch (const Error&)
            {
            }

            try {
              Session s(host, user, pwd);
            }
            catch (const Error&)
            {
            }

            try {
              Session s(host, user, NULL);
            }
            catch (const Error&)
            {
            }
          }
        };

        static void test(A host, B port, C user)
        {
          t3<const char*>::test(host, port, user, "pwd");
          t3<const char*>::test(host, port, user, NULL);
          t3<std::string>::test(host, port, user, "pwd");

          try {
            Session s(host, port, user);
          }
          catch (const Error&)
          {
          }

          try {
            Session s(port, user);
          }
          catch (const Error&)
          {
          }

          try {
            Session s(host, user);
          }
          catch (const Error&)
          {
          }
        }
      };

      static void test(A host, B port)
      {
        t2<string>::test(host, port, "user");
        t2<std::string>::test(host, port, "user");
        t2<const char*>::test(host, port, "user");
        t2<const wchar_t*>::test(host, port, L"user");
      }
    };

    static void test(A host)
    {
      t1<unsigned>::test(host, 0);
      t1<unsigned short>::test(host, 0);
      t1<int>::test(host, 0);

      // Treat argument as URL

      try {
        Session s(host);
      }
      catch (const Error&)
      {
      }
    }

  };

  static void test()
  {
    /*
      Note: using invalid host name so that session constructor
      fails early (preferably before doing any real i/o).
    */
    t0<string>::test("");
    t0<std::string>::test("");
    t0<const char*>::test("");
  }
};


TEST_F(First, api_session)
{
  S_ctor_test<mysqlx::XSession>::test();
  S_ctor_test<mysqlx::NodeSession>::test();
}
