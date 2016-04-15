/*
* Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
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
#include <list>

using std::cout;
using std::endl;
using namespace mysqlx;

class Batch : public mysqlx::test::Xplugin
{
};


/*
  Test CRUD multi operations such as inserting several documents
  or performing several modifications by a single CRUD operation.
*/

TEST_F(Batch, crud)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("c1", true);

  coll.remove().execute();

  {
    RowResult res = sql("select count(*) from test.c1");
    unsigned  cnt = res.fetchOne()[0];
    EXPECT_EQ(0, cnt);
  }

  cout << "Inserting documents..." << endl;

  // Add several documents in single add operation.

  static string docs[] = {
    L"{ \"_id\": \"id1\", \"name\": \"foo\", \"age\": 1 }",
    L"{ \"_id\": \"id2\", \"name\": \"bar\", \"age\": 2 }",
    L"{ \"_id\": \"id3\", \"name\": \"baz\", \"age\": 3, \"date\": { \"day\": 20, \"month\": \"Apr\" }}",
    L"{ \"_id\": \"id4\", \"name\": \"foo\", \"age\": 7 }",
    L"{ \"_id\": \"id5\", \"name\": \"buz\", \"age\": 17 }"
  };

  {
    CollectionAdd add = coll.add(docs[0]);

    for (unsigned pos = 1; pos < sizeof(docs) / sizeof(string); ++pos)
    {
      add.add(docs[pos]);
    }

    add.execute();
  }

  {
    RowResult res = sql("select count(*) from test.c1");
    unsigned  cnt = res.fetchOne()[0];
    EXPECT_EQ(5, cnt);
    cout << "Documents added" << endl;
  }

  cout << "Atomicity check..." << endl;

  /*
    Check that multi-add operation is atomic.

    None of the documents below should be added because the third
    document has non-unique id.
  */

  static string docs1[] = {
    L"{ \"_id\": \"id6\", \"name\": \"atomic test\" }",
    L"{ \"_id\": \"id7\", \"name\": \"atomic test\" }",
    L"{ \"_id\": \"id1\", \"name\": \"atomic test\" }",
    L"{ \"_id\": \"id8\", \"name\": \"atomic test\" }",
    L"{ \"_id\": \"id6\", \"name\": \"atomic test\" }",
  };

  {
    CollectionAdd add = coll.add(docs1[0]);

    for (unsigned pos = 1; pos < sizeof(docs1) / sizeof(string); ++pos)
    {
      add.add(docs[pos]);
    }

    EXPECT_THROW(add.execute(),Error);
  }

  {
    DocResult find = coll.find("name = 'atomic test'").execute();
    EXPECT_FALSE(find.fetchOne());
    RowResult res = sql("select count(*) from test.c1");
    unsigned  cnt = res.fetchOne()[0];
    EXPECT_EQ(5, cnt);
  }

  cout << "Modifying documents..." << endl;

  // Perform several modifications in single modify operation.

  {
    CollectionModify modify = coll.modify();

    modify.set("age", expr("2*age"));
    modify.unset("date");
    modify.set("food", expr("[]"));
    modify.arrayAppend("food", "milk");
    modify.arrayAppend("food", "soup");
    modify.arrayAppend("food", "potatoes").execute();
  }

  {
    DocResult find = coll.find().execute();
    unsigned pos = 0;


    for (DbDoc doc; (doc = find.fetchOne()); ++pos)
    {
      cout << " -doc#" << pos <<": " << doc << endl;
      EXPECT_NO_THROW(doc["food"]);
      EXPECT_THROW(doc["date"], Error);
    }
    EXPECT_EQ(5, pos);
  }

  cout << "Done!" << endl;
}

