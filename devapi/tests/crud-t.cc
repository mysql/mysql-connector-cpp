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
#include <iostream>
#include <list>
#include <algorithm>

using std::cout;
using std::endl;
using namespace mysqlx;

class Crud : public mysqlx::test::Xplugin
{
public:

  void SetUp()
  {
    Xplugin::SetUp();

    /*
      Clear sql_mode to work around problems with how
      xplugin handles group_by queries (the "only_full_group_by"
      mode which is enabled by default).
    */
    try {
      get_sess().sql("set sql_mode=''").execute();
    }
    catch (...)
    {}
  }

  void add_data(Collection &coll);
};

void output_id_list(Result& res)
{
  std::vector<mysqlx::GUID> ids = res.getDocumentIds();
  for (auto id : ids)
  {
    cout << "- added doc with id: " << id  << endl;
  }

}

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
    output_id_list(add);
    EXPECT_EQ(2U, add.getAffectedItemsCount());

    add = coll.add("{ \"name\": \"bar\", \"age\": 2 }")
      .add("{ \"name\": \"baz\", \"age\": 3, \"date\": { \"day\": 20, \"month\": \"Apr\" }}").execute();
    output_id_list(add);
    EXPECT_EQ(2U, add.getAffectedItemsCount());

    add = coll.add("{ \"_id\": \"myuuid-1\", \"name\": \"foo\", \"age\": 7 }",
      "{ \"name\": \"buz\", \"age\": 17 }").execute();
    output_id_list(add);
    EXPECT_EQ(2U, add.getAffectedItemsCount());
    EXPECT_EQ(0U, add.getAutoIncrementValue());
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

    // Similar, row is now deleted when we access field and doc.

    cout << "field value: " << field << endl;
    EXPECT_EQ(value, (unsigned)field);

    cout << "document: " << doc << endl;

    string name = doc["name"];
    EXPECT_EQ(2, (unsigned)doc["age"]);
    EXPECT_EQ(string("bar"), (string)doc["name"]);
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
  output_id_list(add);

  add = coll.add("{ \"name\": \"baz\", \"age\": 3,\
                  \"birth\": { \"day\": 20, \"month\": \"Apr\" } }")
        .add("{ \"name\": \"bar\", \"age\": 2, \
                    \"food\": [\"Milk\", \"Soup\"] }")

        .execute();
  output_id_list(add);

  add = coll.add("{ \"_id\": \"myuuid-1\", \"name\": \"foo\", \"age\": 7 }",
                 "{ \"name\": \"buz\", \"age\": 17 }").execute();
  output_id_list(add);

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


  std::map<string, Value> args;

  args["name"] = "ba%";
  args["age"] = 3;

  CollectionRemove remove(coll, "name like :name and age < :age");

  remove.bind(args).execute();

  CollectionFind find(coll, "name like :name and age < :age");

  docs = find.bind(args).execute();

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
    Result res;
    auto op = coll.modify("name like :name and age < :age");
    op.set(string("name"), Value("boo"));
    op.set("$.age", expr("age+1"));
    op.arrayAppend("food", "Popcorn");
    res = op.arrayAppend("food", "Coke")
      .bind("name", "ba%")
      .bind("age", 3)
      .execute();

    EXPECT_EQ(1U, res.getAffectedItemsCount());
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
      CollectionModify op(coll, "name like :name");
      op.unset("food").bind("name", "bo%").execute();
    }

    docs = coll.find("name like :name")
           .bind("name", "bo%")
           .execute();

    doc = docs.fetchOne();

    EXPECT_THROW(doc["food"], Error);

    cout << endl;
  }

}


TEST_F(Crud, order_limit)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("c1", true);

  add_data(coll);

  DocResult docs = coll.find()
                       .sort("age ASC")
                       .limit(2)
                       .offset(4)
                       .execute();

  DbDoc doc = docs.fetchOne();

  //with the offset=4 first row is age = 7
  int prev_val = 6;

  int i = 0;
  for (; doc; ++i, doc = docs.fetchOne())
  {
    cout << "doc#" << i << ": " << doc << endl;

    EXPECT_LT(prev_val, (int)doc["age"]);
    prev_val = doc["age"];

  }


  EXPECT_EQ(2, i);

  // Modify the first line (ordered by age) incrementing 1 to the age.

  coll.modify()
      .set("age",expr("age+1"))
      .sort("age ASC")
      .limit(1)
      .execute();


  /*
    Check if modify is ok.
    name DESC because now there are 2 documents with same age,
    checking the "foo" ones and ages 1 and 2
  */

  docs = coll.find().sort("age ASC", "name DESC")
                    .limit(2)
                    .execute();

  doc = docs.fetchOne();

  i = 0;
  for (; doc; ++i, doc = docs.fetchOne())
  {
    cout << "doc#" << i << ": " << doc << endl;

    // age 1 and 2
    EXPECT_EQ(i+1, (int)doc["age"]);
    EXPECT_EQ(string("foo"), (string)doc["name"] );

  }

  // Remove the two lines

  coll.remove().sort("age ASC", "name DESC")
               .limit(2)
               .execute();

  docs = coll.find().sort("age ASC", "name DESC")
                    .limit(1)
                    .execute();

  EXPECT_NE(string("foo"), (string)docs.fetchOne()["name"]);
  EXPECT_TRUE(docs.fetchOne().isNull());

}

TEST_F(Crud, projections)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("c1", true);

  add_data(coll);

  for (unsigned round = 0; round < 2; ++round)
  {
    cout << "== round " << round << " ==" << endl;

    DocResult docs;

    switch (round)
    {
    case 0:
    {
      std::vector<string> fields;
      fields.push_back("age AS Age1");
      fields.push_back("age AS Age2");

      docs = coll.find()
        .fields("age AS age", "2016-age AS birthYear", fields)
        .execute();

      break;
    }

    case 1:
    {
      docs =  coll.find()
                  .fields(expr(
                    "{"
                    "  \"age\": age,"
                    "  \"birthYear\": 2016-age,"
                    "  \"Age1\": age,"
                    "  \"Age2\": age"
                    "}"
                   ))
                  .execute();
      break;
    }
    }

    for (DbDoc doc = docs.fetchOne();
         !doc.isNull();
         doc = docs.fetchOne())
    {
      int rows = 0;
      for (auto col : doc)
      {
        ++rows;
        cout << col << endl;
      }
      EXPECT_EQ(4, rows);
      EXPECT_EQ(2016 - (int)doc["age"], (int)doc["birthYear"]);
    }
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


TEST_F(Crud, table)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  sql("DROP TABLE IF EXISTS test.crud_table");
  sql(
    "CREATE TABLE test.crud_table("
    "  _id VARCHAR(32),"
    "  name VARCHAR(32),"
    "  age INT"
    ")");

  Schema sch = sess.getSchema("test");
  Table tbl = sch.getTable("crud_table");
  Result res;

  //Insert values on table

  std::vector<string> cols = {"_id"};

  //Inserting empty list

  //Bug #25515964
  //Adding empty list shouldn't do anything
  std::list<Row> rList;
  tbl.insert(cols, "age", string("name")).rows(rList).rows(rList).execute();

  //Using containers (vectors, const char* and string)

  auto insert = tbl.insert(cols, "age", string("name"));
  insert.values("ID#1", 10, "Foo");
  insert.values("ID#2", 5 , "Bar" );
  insert.values("ID#3", 3 , "Baz");
  res = insert.execute();

  EXPECT_EQ(3U, res.getAffectedItemsCount());

  //test inserting with 1 param only
  tbl.insert("_id").values("ID#99").execute();

  //Check if values inserted are ok

  {
    auto op_select = tbl.select();
    RowResult result =  op_select.where("name like :name")
                        .bind("name", "Fo%")
                        .execute();

    //FIXME: Fix when Row::Setter is fixed
    const Row r = result.fetchOne();

    EXPECT_EQ(string("Foo"),(string)r[1]);
    EXPECT_EQ(10, (int)r[2]);
    EXPECT_EQ(true, result.fetchOne().isNull());
  }

  // Testing insert data without specifying columns

  tbl.insert().values("ID#98","MasterZ","10").execute();

  //Check if values inserted are ok

  {
    auto op_select = tbl.select();
    RowResult result =  op_select.where("name like :name")
    .bind("name", "Ma%")
    .execute();

    //FIXME: Fix when Row::Setter is fixed
    const Row r = result.fetchOne();

    EXPECT_EQ(string("MasterZ"),(string)r[1]);
    EXPECT_EQ(10,(int)r[2]);
    EXPECT_EQ(true, result.fetchOne().isNull());
  }

  // Update values (name and age) where name = Fo%

  auto upd = tbl.update();
  upd.set("name","Qux");
  upd.set("age",expr("age+1"));
  upd.where("name like :name");
  upd.bind("name", "Fo%");
  res = upd.execute();

  EXPECT_EQ(1U, res.getAffectedItemsCount());

  // Check if its ok

  {
    auto op_select = tbl.select();
    op_select.where("name like :name");
    op_select.bind("name", "Qu%");
    RowResult result = op_select.execute();

    //FIXME: Fix when Row::Setter is fixed
    const Row r = result.fetchOne();


    EXPECT_EQ(string("Qux"), (string)r[1]);
    EXPECT_EQ(11,(int)r[2]);
    EXPECT_EQ(true, result.fetchOne().isNull());
  }


  // Delete rows where name = Qu%

  auto rm = tbl.remove();
  rm.where("name like :name");
  rm.bind("name", "Qu%");
  res = rm.execute();

  EXPECT_EQ(1U, res.getAffectedItemsCount());

  {
    auto op_select = tbl.select();
    op_select.where("name like :name");
    op_select.bind("name", "Qu%");
    RowResult result = op_select.execute();

    Row r = result.fetchOne();

    EXPECT_EQ(true, r.isNull());

  }

  {

    sql("DROP TABLE IF EXISTS test.crud_table");
    sql(
          "CREATE TABLE test.crud_table("
          "c0 JSON,"
          "c1 INT"
          ")");

    Schema sch = sess.getSchema("test");
    Table tbl = sch.getTable("crud_table");

    res = tbl.insert("c0","c1")
             .values("{\"foo\": 1}", 1 )
             .values("{\"foo\": 2}", 2 )
             .values("{\"foo\": 3}", 3 ).execute();

    EXPECT_EQ(3U, res.getAffectedItemsCount());

    RowResult res = tbl.select("c0->$.foo", "c1").where("c0->$.foo > 1 AND c1 < 3").execute();

    Row r = res.fetchOne();

    EXPECT_EQ(2, static_cast<int>(r[0]));
    EXPECT_EQ(2, static_cast<int>(r[1]));

  }

  // Check generated auto-increment values

  {

    sql("DROP TABLE IF EXISTS test.crud_table");
    sql(
      "CREATE TABLE test.crud_table("
      "c0 JSON,"
      "c1 INT AUTO_INCREMENT,"
      "PRIMARY KEY (c1)"
      ")");

    Schema sch = sess.getSchema("test");
    Table tbl = sch.getTable("crud_table");

    res = tbl.insert("c0")
      .values("{\"foo\": 1}")
      .values("{\"foo\": 2}")
      .values("{\"foo\": 3}").execute();

    EXPECT_EQ(1U, res.getAutoIncrementValue());
    EXPECT_EQ(3U, res.getAffectedItemsCount());

    res = tbl.insert("c0")
      .values("{\"foo\": 4}")
      .values("{\"foo\": 5}").execute();

    EXPECT_EQ(4U, res.getAutoIncrementValue());
    EXPECT_EQ(2U, res.getAffectedItemsCount());

    RowResult res = tbl.select("c0->$.foo", "c1").execute();

    for (Row r; (r = res.fetchOne());)
      EXPECT_EQ((int)r[0], (int)r[1]);

  }

}


TEST_F(Crud, table_order_limit)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  sql("DROP TABLE IF EXISTS test.crud_table");
  sql(
    "CREATE TABLE test.crud_table("
    "  _id VARCHAR(32),"
    "  name VARCHAR(32),"
    "  age INT"
    ")");

  Schema sch = sess.getSchema("test");
  Table tbl = sch.getTable("crud_table");


  //Insert values on table

  std::vector<string> cols = {"_id"};
  //Using containers (vectors, const char* and string)
  auto insert = tbl.insert(cols, "age", string("name"));
  insert.values("ID#1", 10, "Foo");
  insert.values("ID#2", 5 , "Bar" );
  insert.values("ID#3", 3 , "Baz");
  insert.execute();

  {
    RowResult result = tbl.select().orderBy("age ASC")
                                .limit(1)
                                .offset(1)
                                .execute();

    Row r = result.fetchOne();

    EXPECT_EQ(5, (int)r[2]);
    EXPECT_TRUE(result.fetchOne().isNull());
  }

  tbl.update().set("age", expr("age+1"))
              .orderBy("age ASC")
              .limit(1)
              .execute();

  {
    RowResult result = tbl.select().orderBy("age ASC")
                                   .limit(1)
                                   .execute();

    Row r = result.fetchOne();
    EXPECT_EQ(4, (int)r[2]);
    EXPECT_TRUE(result.fetchOne().isNull());
  }

  tbl.remove()
     .where("age > 4")
     .orderBy("age DESC")
     .limit(1)
     .execute();


  {
    RowResult result = tbl.select()
                          .where("age > 4")
                          .orderBy("age DESC")
                          .limit(1)
                          .execute();

    Row r = result.fetchOne();
    EXPECT_EQ(5, (int)r[2]);
    EXPECT_TRUE(result.fetchOne().isNull());
  }
}


TEST_F(Crud, table_projections)
{

  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  sql("DROP TABLE IF EXISTS test.crud_table");
  sql(
    "CREATE TABLE test.crud_table("
    "  _id VARCHAR(32),"
    "  name VARCHAR(32),"
    "  age INT"
    ")");

  Schema sch = sess.getSchema("test");
  Table tbl = sch.getTable("crud_table");


  //Insert values on table

  std::vector<string> cols = {"_id"};
  //Using containers (vectors, const char* and string)
  auto insert = tbl.insert(cols, "age", string("name"));
  insert.values("ID#1", 10, "Foo");
  insert.values("ID#2", 5 , "Bar" );
  insert.values("ID#3", 3 , "Baz");
  insert.execute();

  std::vector<string> fields;
  fields.push_back("age");
  fields.push_back("2016-age AS birth_year");

  RowResult result = tbl.select(fields, "age AS dummy")
                     .orderBy("age ASC")
                     .execute();

  for (Row r = result.fetchOne(); !r.isNull(); r = result.fetchOne())
  {
    EXPECT_EQ(3, r.colCount());
    EXPECT_EQ(2016-static_cast<int>(r[0]), static_cast<int>(r[1]));
  }

}


/*
  Test move semantics for CRUD operation objects.
*/
#if 0
TEST_F(Crud, move)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("coll",true);

  cout << endl;
  cout << "Collection.add 1" << endl;

  {
    auto a = coll.add("{\"foo\" : 7}");
    auto b = a;

    EXPECT_THROW(a.add(""), Error);
    EXPECT_NO_THROW(b.add(""));
  }

  cout << "Collection.add 2" << endl;

  {
    auto a = coll.add("{\"foo\" : 7}");
    auto b = a.add("");

    EXPECT_THROW(a.add(""), Error);
    EXPECT_NO_THROW(b.add(""));
  }

  cout << "Collection.add 3" << endl;

  {
    CollectionAdd a = coll.add("{\"foo\" : 7}");
    CollectionAdd b = a.add("");

    EXPECT_THROW(a.add(""), Error);
    EXPECT_NO_THROW(b.add(""));
  }

  cout << endl;
  cout << "Collection.find 1" << endl;

  {
    auto a = coll.find();
    auto b = a;

    EXPECT_THROW(a.execute(), Error);
    EXPECT_NO_THROW(b.execute());
  }

  cout << "Collection.find 2" << endl;

  {
    CollectionFind a = coll.find();
    CollectionFind b = a;

    EXPECT_THROW(a.execute(), Error);
    EXPECT_NO_THROW(b.execute());
  }

  cout << "Collection.find 3" << endl;

  {
    CollectionFind a = coll.find("foo = 7");
    CollectionFind b = a;

    EXPECT_THROW(a.execute(), Error);
    EXPECT_NO_THROW(b.execute());
  }

  cout << endl;
  cout << "Collection.modify 1" << endl;

  {
    auto a = coll.modify();
    auto b = a;

    EXPECT_THROW(a.set("",7), Error);
    EXPECT_NO_THROW(b.set("",7));
  }

  cout << "Collection.modify 2" << endl;

  {
    auto a = coll.modify();
    auto b = a.unset("");

    EXPECT_THROW(a.set("", 7), Error);
    EXPECT_NO_THROW(b.set("", 7));
  }

  cout << "Collection.modify 3" << endl;

  {
    CollectionModify a = coll.modify();
    CollectionModify b = a.unset("");

    EXPECT_THROW(a.set("", 7), Error);
    EXPECT_NO_THROW(b.set("", 7));
  }

  cout << endl;
  cout << "Collection.remove 1" << endl;

  {
    auto a = coll.remove();
    auto b = a;

    EXPECT_THROW(a.execute(), Error);
    EXPECT_NO_THROW(b.execute());
  }

  cout << "Collection.remove 2" << endl;

  {
    CollectionRemove a = coll.remove();
    CollectionRemove b = a;

    EXPECT_THROW(a.execute(), Error);
    EXPECT_NO_THROW(b.execute());
  }

  cout << endl;
}

#endif


TEST_F(Crud, doc_path)
{

  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);
  sess.dropCollection("test", "coll");

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("coll",false);

  coll.add( "{\"date\": {\"monthName\":\"December\", \"days\":[1,2,3]}}").execute();

  coll.modify().set("date.monthName", "February" ).execute();
  coll.modify().set("$.date.days[0]", 4 ).execute();

  DocResult docs = coll.find().execute();

  DbDoc doc = docs.fetchOne();

  EXPECT_EQ(string("February"), static_cast<string>(doc["date"]["monthName"]));
  EXPECT_EQ(4, static_cast<int>(doc["date"]["days"][0]));

  coll.modify().arrayDelete("date.days[0]").execute();
  docs = coll.find().execute();
  doc = docs.fetchOne();
  EXPECT_EQ(2, static_cast<int>(doc["date"]["days"][0]));

  coll.modify().unset("date.days").execute();
  docs = coll.find().execute();
  doc = docs.fetchOne();
  EXPECT_THROW(static_cast<int>(doc["date"]["days"][0]), Error);

}


TEST_F(Crud, row_error)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating table..." << endl;

  sql("DROP TABLE IF EXISTS test.row_error");
  sql(
    "CREATE TABLE test.row_error("
    "  _id VARCHAR(32),"
    "  age BIGINT"
    ")");

  Schema sch = sess.getSchema("test");
  Table tbl = sch.getTable("row_error");


  //Insert values on table


  auto insert = tbl.insert("_id", "age");
  insert.values("ID#1", (int64_t)-9223372036854775807LL);
  insert.values("ID#3", (int64_t)9223372036854775805LL);
  insert.values("ID#4", (int64_t)9223372036854775806LL);
  insert.execute();

  //Overflow on second line

  {
    auto op_select = tbl.select("100000+age AS newAge");
    RowResult result =  op_select.execute();

    try {

      Row r = result.fetchOne();

      for ( ;
            !r.isNull();
            r = result.fetchOne())
      {
        std::cout << (int64_t)r.get(0) << std::endl;
      }

    }
    catch (mysqlx::Error& e)
    {
      FAIL() << e;
    }
  }

  //Everything should work as expected if dropped
  {
    auto op_select = tbl.select("100000+age");
    RowResult result =  op_select.execute();
  }
}


TEST_F(Crud, coll_as_table)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("coll", true);

  // Clean up
  coll.remove().execute();

  // Add Doc to collection
  DbDoc doc("{ \"name\": \"foo\", \"age\": 1 }");

  coll.add(doc, doc).execute();

  // Get Collectionas Table
  Table tbl = sch.getCollectionAsTable("coll");

  // Check if we can get result from collection using table
  RowResult tblResult = tbl.select("doc->$.name").execute();
  Row r = tblResult.fetchOne();
  EXPECT_EQ(string("foo"), static_cast<string>(r[0]));

  // Update Collection using Table
  tbl.update().set("doc->$.name", "bar").execute();

  // Check if it was successful
  tblResult = tbl.select("doc->$.name").execute();

  r = tblResult.fetchOne();

  EXPECT_EQ(string("bar"), static_cast<string>(r[0]));

  // Check same result with Collection obj
  DocResult docres = coll.find().fields(expr("{\"name\": name, \"age\":age+1}")).execute();

  doc = docres.fetchOne();

  EXPECT_EQ(string("bar"), static_cast<string>(doc["name"]));
  EXPECT_EQ(2, static_cast<int>(doc["age"]));

  sql("DROP TABLE IF EXISTS test.not_collection");
  sql(
    "CREATE TABLE test.not_collection("
    "  _id VARCHAR(32),"
    "  age BIGINT"
    ")");

  // Should throw exception if its not a collection
  try {
    sch.getCollectionAsTable("not_collection");
    FAIL() << "Should throw error because this is not a collection";
  } catch (Error &) {
  }

  // Should NOT exception if its not a collection
  try {
    sch.getCollectionAsTable("not_collection", false);
  } catch (Error &) {
    FAIL() << "Should throw error because this is not a collection";
  }


}


TEST_F(Crud, get_ids)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("coll", true);

  // Clean up
  coll.remove().execute();


  // Add Doc to collection
  DbDoc doc1("{ \"name\": \"foo\", \"age\": 1 }");
  DbDoc doc2("{ \"_id\":\"ABCDEFGHIJKLMNOPQRTSUVWXYZ012345\","
             " \"name\": \"bar\", \"age\": 2 }");

  Result res;
  res = coll.add(doc2).execute();

  EXPECT_EQ(string("ABCDEFGHIJKLMNOPQRTSUVWXYZ012345"), string(res.getDocumentId()));

  res = coll.remove().execute();

  // This functions can only be used on add() operations
  EXPECT_THROW(res.getDocumentId(), Error);
  EXPECT_THROW(res.getDocumentIds(), Error);

  res = coll.add(doc1).add(doc2).execute();

  EXPECT_ANY_THROW(res.getDocumentId());

  std::vector<mysqlx::GUID> list = res.getDocumentIds();

  EXPECT_EQ(2, list.size());
  EXPECT_NE(string("ABCDEFGHIJKLMNOPQRTSUVWXYZ012345"), string(list[0]));
  EXPECT_EQ(string("ABCDEFGHIJKLMNOPQRTSUVWXYZ012345"), string(list[1]));

}


TEST_F(Crud, count)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("coll", true);

  //Remove all rows
  coll.remove().execute();

  {
    CollectionAdd add(coll);

    for (int i = 0; i < 1000; ++i)
    {
      std::stringstream json;
      json << "{ \"name\": \"foo\", \"age\":" << i << " }";

      add.add(json.str());
    }

    add.execute();
  }

  EXPECT_EQ(1000, coll.count());

  coll.remove().limit(500).execute();

  Table tbl = sch.getCollectionAsTable("coll");

  EXPECT_EQ(500, tbl.count());

}


TEST_F(Crud, buffered)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("coll", true);

  coll.remove().execute();

  for (int j = 0; j < 10; ++j)
  {
    CollectionAdd add(coll);
    for (int i = 0; i < 1000; ++i)
    {
      std::stringstream json;
      json << "{ \"name\": \"foo\", \"age\": " << 1000*j + i << " }";
      add.add(json.str());
    }
    add.execute();
  }

  {
    DocResult res = coll.find().sort("age").execute();

    //Get first directly
    DbDoc r = res.fetchOne();
    EXPECT_EQ(0, static_cast<int>(r["age"]));

    EXPECT_EQ(9999, res.count());

    //Get second from cache, after count()
    EXPECT_EQ(1, static_cast<int>(res.fetchOne()["age"]));

    //Get the rest of it
    std::vector<DbDoc> rows = res.fetchAll();

    EXPECT_EQ(9998, rows.size());

    auto row = rows.begin();
    int i = 2;
    for( ; row != rows.end() ; ++row, ++i)
    {
      EXPECT_EQ(i, static_cast<int>((*row)["age"]));
    }

    EXPECT_EQ(0, res.count());

    std::vector<DbDoc> rows_empty = res.fetchAll();

    EXPECT_EQ(0, rows_empty.size());

  }

  {
    Table tbl = sch.getCollectionAsTable("coll");

    RowResult res = tbl.select("doc->$.age AS age")
                    .orderBy("doc->$.age")
                    .execute();

    //Get first directly
    Row r = res.fetchOne();

    EXPECT_EQ(0, static_cast<int>(r[0]));

    EXPECT_EQ(9999, res.count());

    //Get second from cache, after count()
    EXPECT_EQ(1, static_cast<int>(res.fetchOne()[0]));

    //Get the rest of it
    std::vector<Row> rows = res.fetchAll();

    EXPECT_EQ(9998, rows.size());

    auto row = rows.begin();
    int i = 2;
    for( ; row != rows.end() ; ++row, ++i)
    {
      EXPECT_EQ(i, static_cast<int>((*row)[0]));
    }

    EXPECT_EQ(0, res.count());

    std::vector<Row> rows_empty = res.fetchAll();

    EXPECT_EQ(0, rows_empty.size());

  }


}


TEST_F(Crud, iterators)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("coll", true);

  coll.remove().execute();

  {
    CollectionAdd add(coll);

    for (int i = 0; i < 1000; ++i)
    {
      std::stringstream json;
      json << "{ \"name\": \"foo\", \"age\":" << i << " }";

      add.add(json.str());
    }

    add.execute();
  }

  {
    DocResult res = coll.find().sort("age").execute();

    int age = 0;
    for( DbDoc doc : res)
    {
      EXPECT_EQ(age, static_cast<int>(doc["age"]));

      ++age;

      //break the loop
      if (age == 500)
        break;
    }

    EXPECT_EQ(500, age);

    //get the other 500
    for( DbDoc doc : res.fetchAll())
    {
      EXPECT_EQ(age, static_cast<int>(doc["age"]));

      ++age;
    }

    EXPECT_EQ(1000, age);

  }

  {
    Table tbl = sch.getCollectionAsTable("coll");

    RowResult res = tbl.select("doc->$.age AS age")
                    .orderBy("doc->$.age")
                    .execute();

    int age = 0;
    for( Row row : res)
    {
      EXPECT_EQ(age, static_cast<int>(row[0]));

      ++age;

      //break the loop
      if (age == 500)
        break;
    }

    EXPECT_EQ(500, age);

    //get the other 500
    for( Row row : res.fetchAll())
    {
      EXPECT_EQ(age, static_cast<int>(row[0]));

      ++age;

    }

    EXPECT_EQ(1000, age);

  }
}


TEST_F(Crud, diagnostic)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Preparing table..." << endl;

  NodeSession &sess = get_sess();

  sess.sql("DROP TABLE IF EXISTS test.t").execute();
  sess.sql("CREATE TABLE test.t (a TINYINT NOT NULL, b CHAR(4))").execute();

  Table t = sess.getSchema("test").getTable("t");

  cout << "Table ready..." << endl;

  /*
    The following statement clears the default SQL mode in
    which all warnings are upgraded to errors.
  */

  sess.sql("SET SESSION sql_mode=''").execute();

  cout << "Inserting rows into the table..." << endl;

  // This insert statement should generate warnings

  Result res = t.insert().values(10, "mysql").values(300, "xyz").execute();

  for (Warning w : res.getWarnings())
  {
    cout << w << endl;
  }

  EXPECT_EQ(2U, res.getWarningCount());

  std::vector<Warning> warnings = res.getWarnings();

  for (unsigned i = 0; i < res.getWarningCount(); ++i)
  {
    EXPECT_EQ(warnings[i].getCode(), res.getWarning(i).getCode());
  }
}

TEST_F(Crud, cached_results)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Preparing table..." << endl;

  XSession sess(this);

  Collection coll = sess.createSchema("test", true)
                        .createCollection("test", true);

  coll.remove().execute();

  coll.add("{\"user\":\"Foo\"}").execute();
  coll.add("{\"user\":\"Bar\"}").execute();
  coll.add("{\"user\":\"Baz\"}").execute();

  auto coll_op = coll.find();
  auto coll_op2 = coll.find();

  DocResult coll_res = coll_op.execute();
  DocResult coll_res2 = coll_op2.execute();

  DbDoc coll_row = coll_res.fetchOne();
  DbDoc coll_row2 = coll_res2.fetchOne();

  for (; coll_row && coll_row2;
       coll_row = coll_res.fetchOne(),
       coll_row2 = coll_res2.fetchOne())
  {
    EXPECT_EQ(static_cast<string>(coll_row["user"]),
              static_cast<string>(coll_row2["user"]));

    std::cout << "User: " << coll_row["user"] << std::endl;
  }

}

TEST_F(Crud, add_empty)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("c1", true);

  coll.remove().execute();

  //Check bug when Result was created uninitialized
  Result add;

  //Adding Empty docs throws Error
  EXPECT_THROW(add = coll.add(static_cast<wchar_t*>(NULL)).execute(),
               mysqlx::Error);
  EXPECT_THROW(add = coll.add(static_cast<char*>(NULL)).execute(),
               mysqlx::Error);
}


TEST_F(Crud, doc_id)
{

  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("c1", true);

  EXPECT_THROW(coll.add("{\"_id\": 127 }").execute(), Error);
  EXPECT_THROW(coll.add("{\"_id\": 12.7 }").execute(), Error);
}

TEST_F(Crud, group_by_having)
{

  SKIP_IF_NO_XPLUGIN;

  cout << "Preparing table..." << endl;

  XSession sess(this);

  sess.dropCollection("test", "coll");

  Collection coll = sess.createSchema("test", true)
                        .createCollection("coll", true);

  Table tbl = sess.createSchema("test", true).getCollectionAsTable("coll", true);

  coll.remove().execute();

  std::vector<string> names = {"Foo", "Baz", "Bar"};

  int i=0;

  for (auto name : names)
  {
    std::stringstream json;
    json <<"{ \"_id\":\""<< i << "\", \"user\":\"" << name << "\", \"age\":" << 20+i << "}";
    coll.add(json.str()).execute();
    ++i;
  }

  // Function to check order of operation
  auto check_order = [&names] (DocResult &coll_res, RowResult &tbl_res)
  {
    DbDoc coll_row = coll_res.fetchOne();
    Row tbl_row = tbl_res.fetchOne();

    for (auto name = names.begin();
         coll_row && tbl_row && name != names.end();
         coll_row = coll_res.fetchOne(),
         tbl_row = tbl_res.fetchOne(),
         ++name)
    {
      EXPECT_EQ(*name, static_cast<string>(coll_row["user"]));
      EXPECT_EQ(*name, static_cast<string>(tbl_row[0]));
    }

    EXPECT_TRUE(coll_row.isNull());
    EXPECT_TRUE(tbl_row.isNull());
  };

  auto coll_res = coll.find().fields("user AS user", "age as age").execute();
  auto tbl_res = tbl.select("doc->$.user as user","doc->$.age as age").execute();

  check_order(coll_res, tbl_res);

  cout << "Check with groupBy" << endl;
  std::sort(names.begin(), names.end());

  std::vector<string> fields = {"user"};
  coll_res = coll.find()
             .fields("user AS user", "age as age")
             .groupBy(fields, "age")
             .execute();

  cout << "and on table" << endl;
  tbl_res = tbl.select("doc->$.user as user", "doc->$.age as age")
               .groupBy(fields,"age")
               .execute();


  check_order(coll_res, tbl_res);


  cout << "Having usage will remove last element of previous groupBy." << endl;
  names.pop_back();

  coll_res = coll.find()
             .fields("user AS user", "age as age")
             .groupBy(fields,"age")
             .having(L"age > 20")
             .execute();

  //and on table
  tbl_res = tbl.select("doc->$.user as user", "doc->$.age as age")
            .groupBy(fields, "age")
            .having(L"age > 20")
            .execute();

  check_order(coll_res, tbl_res);

  cout << "Same test but passing std::string to groupBy" << endl;

  coll_res = coll.find()
             .fields("user AS user", "age as age")
             .groupBy(fields, std::string("age"))
             .having(std::string("age > 20"))
             .execute();

  cout << "and on table" << endl;
  tbl_res = tbl.select("doc->$.user as user", "doc->$.age as age")
            .groupBy(fields, std::string("age"))
            .having(std::string("age > 20"))
            .execute();

  check_order(coll_res, tbl_res);

}

TEST_F(Crud, copy_semantics)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Creating session..." << endl;

  XSession sess(this);

  cout << "Session accepted, creating collection..." << endl;

  Schema sch = sess.getSchema("test");
  Collection coll = sch.createCollection("c1", true);

  add_data(coll);


  cout << "Fetching documents..." << endl;

  CollectionFind find = coll.find("name like :name and age < :age");
  find.bind("name", "ba%");
  find.bind("age", 3);

  CollectionFind find2 = find;

  DocResult docs = find2.execute();

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

  std::map<string, Value> args;

  args["name"] = "ba%";
  args["age"] = 3;

  CollectionRemove remove(coll, "name like :name and age < :age");

  remove.bind(args);

  CollectionRemove remove2 = remove;

  remove2.execute();

  {
    CollectionFind f(coll, "name like :name and age < :age");

    CollectionFind find2 = f;

    find2.bind(args);

    docs = find2.execute();

    doc = docs.fetchOne();
    EXPECT_FALSE((bool)doc);
  }


  cout << "Done!" << endl;
}
