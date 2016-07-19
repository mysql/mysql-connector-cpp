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

