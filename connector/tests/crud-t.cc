/*
* Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
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

using std::cout;
using std::endl;
using namespace mysqlx;

class Crud : public mysqlx::test::Xplugin
{
};


TEST_F(Crud, basic)
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

  {
    Result add;

    DbDoc doc("{ \"name\": \"foo\", \"age\": 1 }");

    add = coll.add(doc, doc).execute();
    cout << "- added doc with id: " << add.getLastDocumentId() << endl;

    add = coll.add("{ \"name\": \"bar\", \"age\": 2 }")
      .add("{ \"name\": \"baz\", \"age\": 3, \"date\": { \"day\": 20, \"month\": \"Apr\" }}").execute();
    cout << "- added 2 docs, last id: " << add.getLastDocumentId() << endl;

    add = coll.add("{ \"_id\": \"myuuid-1\", \"name\": \"foo\", \"age\": 7 }",
      "{ \"name\": \"buz\", \"age\": 17 }").execute();
    cout << "- added 2 docs, last id: " << add.getLastDocumentId() << endl;
  }

  {
    RowResult res = sql("select count(*) from test.c1");
    unsigned  cnt = res.fetchOne()[0];
    EXPECT_EQ(6, cnt);
  }

  cout << "Fetching documents..." << endl;

  DocResult docs = coll.find("name like 'ba%'").execute();

  DbDoc doc = docs.fetchOne();

  unsigned i = 0;
  for (; doc; ++i, doc = docs.fetchOne())
  {
    cout << "doc#" << i << ": " << doc << endl;

    for (Field fld : doc)
    {
      cout << " field `" << fld << "`: " << doc[fld] << endl;
    }

    string name = doc["name"];
    cout << " name: " << name << endl;

    if (doc.hasField("date") && Value::DOCUMENT == doc.fieldType("date"))
    {
      cout << "- date field" << endl;
      DbDoc date = doc["date"];
      for (Field fld : date)
      {
        cout << "  date `" << fld << "`: " << date[fld] << endl;
      }
      string month = doc["date"]["month"];
      int day = date["day"];
      cout << "  month: " << month << endl;
      cout << "  day: " << day << endl;
    }

    cout << endl;
  }

  EXPECT_EQ(2, i);

  cout << "querying collection with SQL ..." << endl;

  {
    SqlResult res = sql(L"SELECT * FROM test.c1");

    cout << "Query sent, reading rows..." << endl;
    cout << "There are " << res.getColumnCount() << " columns in the result" << endl;

    EXPECT_EQ(2, res.getColumnCount());

    Row row;
    unsigned row_count = 0;
    while ((row = res.fetchOne()))
    {
      row_count++;
      cout << "== next row ==" << endl;
      for (unsigned i = 0; i < res.getColumnCount(); ++i)
      {
        cout << "col#" << i << ": " << row[i] << endl;
      }
    }

    EXPECT_EQ(6, row_count);
  }

  cout << "Done!" << endl;
}


TEST_F(Crud, life_time)
{
  SKIP_IF_NO_XPLUGIN;

  /*
    Create collection table with a document used for testing
    below.
  */

  {
    Collection coll = getSchema("test").createCollection("life_time", true);
    coll.remove().execute();
    coll.add("{ \"name\": \"bar\", \"age\": 2 }").execute();
  }

  /*
    Check that rows returned from RowResult and fields of a row
    each have its own, independent life-time.
  */

  {
    DbDoc doc;
    Value field;
    unsigned value;

    {
      Row row;

      {
        RowResult res = sql(L"SELECT 7,doc FROM test.life_time");
        row = res.fetchOne();
        value = row[0];

        // Note: we use group to make sure that the tmp RowResult instance
        // is deleted when we acces the row below.
      }

      field = row[0];
      doc = row[1];
    }

    // Simialr, row is now deleted when we access field and doc.

    cout << "field value: " << field << endl;
    EXPECT_EQ(value, (unsigned)field);

    cout << "document: " << doc << endl;

    /*
      FIXME: code below throws error
      "CDK Error: Unknown character at 68"
    */
    //string name = doc["name"];
    //EXPECT_EQ(2, (unsigned)doc["age"]);
    //EXPECT_EQ(string("bar"), (string)doc["name"]);
  }

}
