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
#include <list>

using std::cout;
using std::endl;
using namespace mysqlx;

class Crud : public mysqlx::test::Xplugin
{
public:
  void add_data(Collection &coll);
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
      for (i = 0; i < res.getColumnCount(); ++i)
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


TEST_F(Crud, add_doc_negative)
{
  SKIP_IF_NO_XPLUGIN;

  Collection coll = getSchema("test").createCollection("c1", true);

  coll.remove().execute();

  EXPECT_THROW(coll.add("").execute(), mysqlx::Error);
  EXPECT_THROW(coll.add("invaliddata").execute(), mysqlx::Error);
}



TEST_F(Crud, arrays)
{
  SKIP_IF_NO_XPLUGIN;

  Collection coll = getSchema("test").createCollection("c1", true);

  coll.remove().execute();

  coll.add("{ \"arr\": [ 1, 2, \"foo\", [ 3, { \"bar\" : 123 } ] ] }")
      .execute();

  cout << "Document added" << endl;

  DocResult find = coll.find().execute();
  DbDoc     doc = find.fetchOne();

  cout << "Document fetched" << endl;

  EXPECT_EQ(Value::ARRAY, doc.fieldType("arr"));

  auto arr = doc["arr"];

  cout << "arr: " << arr << endl;

  EXPECT_EQ(4, arr.elementCount());

  unsigned pos = 0;
  for (Value val : doc["arr"])
    cout << "arr[" << pos++ << "]: " << val << endl;

  EXPECT_EQ(1, (int)arr[0]);
  EXPECT_EQ(2, (int)arr[1]);
  EXPECT_EQ(string("foo"), (string)arr[2]);
  EXPECT_EQ(Value::ARRAY, arr[3].getType());

  cout << endl << "sub array arr[3]: " << arr[3] << endl;
  pos = 0;
  for (Value val : arr[3])
    cout << "sub[" << pos++ << "]: " << val << endl;

  EXPECT_EQ(3, (int)arr[3][0]);
  EXPECT_EQ(Value::DOCUMENT, arr[3][1].getType());
  EXPECT_EQ(123, (int)arr[3][1]["bar"]);
}


void Crud::add_data(Collection &coll)
{
  coll.remove().execute();

  {
    RowResult res = sql("select count(*) from test.c1");
    unsigned  cnt = res.fetchOne()[0];
    EXPECT_EQ(0, cnt);
  }

  cout << "Inserting documents..." << endl;

  Result add;

  DbDoc doc("{ \"name\": \"foo\", \"age\": 1 }");

  add = coll.add(doc, doc).execute();
  cout << "- added doc with id: " << add.getLastDocumentId() << endl;

  add = coll.add("{ \"name\": \"bar\", \"age\": 2, \
                    \"food\": [\"Milk\", \"Soup\"] }")
        .add("{ \"name\": \"baz\", \"age\": 3,\
                \"birth\": { \"day\": 20, \"month\": \"Apr\" } }").execute();
  cout << "- added 2 docs, last id: " << add.getLastDocumentId() << endl;

  add = coll.add("{ \"_id\": \"myuuid-1\", \"name\": \"foo\", \"age\": 7 }",
                 "{ \"name\": \"buz\", \"age\": 17 }").execute();
  cout << "- added 2 docs, last id: " << add.getLastDocumentId() << endl;

  {
    RowResult res = sql("select count(*) from test.c1");
    unsigned  cnt = res.fetchOne()[0];
    EXPECT_EQ(6, cnt);
  }


}

TEST_F(Crud, bind)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("c1", true);

  add_data(coll);


  cout << "Fetching documents..." << endl;

  DocResult docs = coll.find("name like :name and age < :age")
         .bind("name", "ba%")
         .bind("age", 3)
         .execute();

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

    EXPECT_EQ(string("bar"), (string)doc["name"]);

    cout << "  age: " << doc["age"] << endl;

    EXPECT_EQ(2, (int)doc["age"]);

    cout << endl;
  }

  EXPECT_EQ(1, i);


  {

    cout << "Fetching documents... using bind Documents" << endl;


    EXPECT_THROW(docs = coll.find("birth like :bday")
                     .bind("bday", DbDoc("{ \"day\": 20, \"month\": \"Apr\" }"))
                     .execute(), mysqlx::Error);

    docs = coll.find("birth like { \"day\": 20, \"month\": \"Apr\" }")
                     .execute();

    doc = docs.fetchOne();

    i = 0;
    for (; doc; ++i, doc = docs.fetchOne())
    {
      cout << "doc#" << i << ": " << doc << endl;

      for (Field fld : doc)
      {
        cout << " field `" << fld << "`: " << doc[fld] << endl;
      }

      string name = doc["name"];
      cout << " name: " << name << endl;

      EXPECT_EQ(string("baz"), (string)doc["name"]);

      cout << "  age: " << doc["age"] << endl;

      EXPECT_EQ(3, (int)doc["age"]);

      cout << endl;
    }


    EXPECT_EQ(1, i);

  }


  {

    cout << "Fetching documents... using bind Arrays" << endl;

    std::list<string> food_list;
    food_list.push_back("Milk");
    food_list.push_back("Soup");

    EXPECT_THROW(
      docs = coll.find("food like :food_list")
                 .bind("food_list", Value(food_list.begin(), food_list.end()))
                 .execute();
        , mysqlx::Error);

    docs = coll.find("food like [\"Milk\", \"Soup\"]")
                     .execute();

    doc = docs.fetchOne();

    i = 0;
    for (; doc; ++i, doc = docs.fetchOne())
    {
      cout << "doc#" << i << ": " << doc << endl;

      for (Field fld : doc)
      {
        cout << " field `" << fld << "`: " << doc[fld] << endl;
      }

      string name = doc["name"];
      cout << " name: " << name << endl;

      EXPECT_EQ(string("bar"), (string)doc["name"]);

      cout << "  age: " << doc["age"] << endl;

      EXPECT_EQ(2, (int)doc["age"]);

      cout << endl;
    }

    EXPECT_EQ(1, i);

  }

  coll.remove("name like :name and age < :age")
                         .bind("name", "ba%")
                         .bind("age", 3)
                         .execute();

  docs = coll.find("name like :name and age < :age")
                       .bind("name", "ba%")
                       .bind("age", 3)
                       .execute();

  doc = docs.fetchOne();
  EXPECT_FALSE((bool)doc);


  cout << "Done!" << endl;
}

TEST_F(Crud, modify)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("c1", true);

  add_data(coll);


  cout << "Fetching documents..." << endl;

  DocResult docs = coll.find("name like :name and age < :age")
         .bind("name", "ba%")
         .bind("age", 3)
         .execute();

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

    EXPECT_EQ(string("bar"), (string)doc["name"]);

    cout << "  age: " << doc["age"] << endl;

    EXPECT_EQ(2, (int)doc["age"]);

    cout << endl;
  }

  EXPECT_EQ(1, i);

  cout << "Modify documents..." << endl;

  {
    auto op = coll.modify("name like :name and age < :age");
    op.set(string("name"), Value("boo"));
    op.set("age", expr("age+1"));
    op.arrayAppend("food", "Popcorn");
    op.arrayAppend("food", "Coke");
    op.bind("name", "ba%");
    op.bind("age", 3);
    op.execute();
  }

  cout << "Fetching documents..." << endl;


  docs = coll.find("name like :name and age < :age")
         .bind("name", "bo%")
         .bind("age", 4)
         .execute();

  doc = docs.fetchOne();

  i = 0;
  for (; doc; ++i, doc = docs.fetchOne())
  {
    cout << "doc#" << i << ": " << doc << endl;

    for (Field fld : doc)
    {
      cout << " field `" << fld << "`: ";

      switch (doc[fld].getType())
      {
        case Value::ARRAY:
          {
            int elem = 0;
            cout << "[";
            for(auto it : doc[fld])
            {
              if (0 != elem)
                cout << ", ";
              cout << it;
              switch (elem)
              {
                case 0: EXPECT_EQ(string("Milk"), (string)it); break;
                case 1: EXPECT_EQ(string("Soup"), (string)it); break;
                case 2: EXPECT_EQ(string("Popcorn"), (string)it); break;
                case 3: EXPECT_EQ(string("Coke"), (string)it); break;
              }

              ++elem;
            }
            cout << "]";
          }
          break;
        default:
          cout << doc[fld];
          break;
      }
      cout << endl;
    }



    string name = doc["name"];
    cout << " name: " << name << endl;

    EXPECT_EQ(string("boo"), (string)doc["name"]);

    cout << "  age: " << doc["age"] << endl;

    EXPECT_EQ(3, (int)doc["age"]);

    {
      auto op = coll.modify("name like :name");
      op.unset("food");
      op.bind("name", "bo%");
      op.execute();
    }

    docs = coll.find("name like :name")
           .bind("name", "bo%")
           .execute();

    doc = docs.fetchOne();

    EXPECT_THROW(doc["food"], Error);

    cout << endl;
  }

}


TEST_F(Crud, existence_checks)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("coll", true);

  cout << "Performing checks..." << endl;

  EXPECT_NO_THROW(sess.getSchema("no_such_schema"));
  EXPECT_THROW(sess.getSchema("no_such_schema", true), Error);
  EXPECT_NO_THROW(sch.getTable("no_such_table"));
  EXPECT_THROW(sch.getTable("no_such_table", true), Error);
  EXPECT_NO_THROW(sch.getCollection("no_such_collection"));
  EXPECT_THROW(sch.getCollection("no_such_collection", true), Error);
  EXPECT_NO_THROW(sch.getCollection("coll", true));
}
