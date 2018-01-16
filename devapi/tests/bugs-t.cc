/*
* Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
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
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
using namespace mysqlx;

class Bugs : public mysqlx::test::Xplugin
{
};



TEST_F(Bugs, bug25505482)
{
  SKIP_IF_NO_XPLUGIN;

  const vector<const char*> good =
  {
    "CHARSET(CHAR(X'65'))",
    "'abc' NOT LIKE 'ABC1'",
    "'a' RLIKE '^[a-d]'",
    "'a' REGEXP '^[a-d]'",
    "POSITION('bar' IN 'foobarbar')",
  };

  const vector<const char*> not_supported =
  {
    "CHARSET(CHAR(X'65' USING utf8))",
    "TRIM(BOTH 'x' FROM 'xxxbarxxx')",
    "TRIM(LEADING 'x' FROM 'xxxbarxxx')",
    "TRIM(TRAILING 'xyz' FROM 'barxxyz')",
    "'Heoko' SOUNDS LIKE 'h1aso'",
  };

  get_sess().createSchema("test", true);
  get_sess().sql("DROP TABLE IF EXISTS test.bug25505482").execute();
  get_sess().sql("CREATE TABLE test.bug25505482 (a int)").execute();

  Table t = get_sess().getSchema("test").getTable("bug25505482");

  cout << "== testing supported expressions ==" << endl << endl;

  for (const char *expr : good)
  {
    cout << "- testing expression: " << expr << endl;
    EXPECT_NO_THROW(t.select().where(expr).execute());
  }

  cout << endl << "== testing not supported expressions ==" << endl << endl;

  for (const char *expr : not_supported)
  {
    cout << "- testing not supported expression: " << expr << endl;
    try {
      t.select().where(expr).execute();
      FAIL() << "Expected error when parsing expression";
    }
    catch (const Error &e)
    {
      std::string msg(e.what());
      cout << "-- got error: " << msg << endl;
      EXPECT_NE(msg.find("not supported yet"), std::string::npos);
    }
  }

  cout << "Done!" << endl;
}

TEST_F(Bugs, bug26130226_crash_update)
{
  SKIP_IF_NO_XPLUGIN;

  get_sess().dropSchema("crash_update");
  get_sess().createSchema("crash_update");
  Schema sch = get_sess().getSchema("crash_update");
  Collection coll = sch.createCollection("c1", true);

  coll.add("{ \"name\": \"abc\", \"age\": 1 , \"misc\": 1.2}").execute();
  Table tabNew = sch.getCollectionAsTable("c1");

  EXPECT_THROW(
    tabNew.update().set((char *)0, expr("")).execute(), // SegFault
    Error);
}

TEST_F(Bugs, bug_26962725_double_bind)
{
  get_sess().dropSchema("bug_26962725_double_bind");
  Schema db = get_sess().createSchema("bug_26962725_double_bind");
  /// Collection.find() function with fixed values

  db.dropCollection("my_collection");
  Collection myColl = db.createCollection("my_collection");

  myColl.add(R"({"name":"mike", "age":39})")
        .add(R"({"name":"johannes", "age":28})")
        .execute();

  EXPECT_EQ(2, myColl.find().execute().count());

  // Create Collection.remove() operation, but do not run it yet
  auto myRemove = myColl.remove("name = :param1 AND age = :param2");

  // Binding parameters to the prepared function and .execute()
  myRemove.bind("param1", "mike").bind("param2", 39).execute();
  myRemove.bind("param1", "johannes").bind("param2", 28).execute();

  EXPECT_EQ(0, myColl.find().execute().count());
}
