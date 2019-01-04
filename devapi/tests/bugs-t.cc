/*
 * Copyright (c) 2017, 2018, Oracle and/or its affiliates. All rights reserved.
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
  SKIP_IF_NO_XPLUGIN;

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

TEST_F(Bugs, bug_27727505_multiple_results)
{
  mysqlx::Session &sess = get_sess();
  sess.dropSchema("bug_27727505_multiple_results");
  sess.createSchema("bug_27727505_multiple_results");

  /* ddl */
  std::string strValue = "";
  sess.sql("use bug_27727505_multiple_results").execute();
  sess.sql("drop table if exists bug_27727505_multiple_results").execute();
  sess.sql("create table newtable(f0 int, f1 varchar(1024))").execute();
  for(int i=0;i<100;i++)
  {
    strValue.resize(1024, 'a');
    sess.sql("insert into newtable values(?,?)")
        .bind(i)
        .bind(strValue.c_str())
        .execute();
  }
  sess.sql("drop procedure if exists test").execute();
  sess.sql("CREATE PROCEDURE test() BEGIN select f0, f1 from newtable where"
           " f0 <= 33; select f0, f1 from newtable where f0 <= 10; END")
      .execute();
  SqlResult res = sess.sql("call test").execute();

  Row row;
  int setNo = 0;
  do
  {
    std::vector<Row> rowAll = res.fetchAll();
    unsigned int j=0;
    for(j = 0;j < rowAll.size();j++)
    {
      string data = (string)rowAll[j][1];
      int num = rowAll[j][0];
      if((unsigned int)num!=j || strValue.compare(data))
      {
        std::stringstream ss;
        ss << "Fetch fail in set : "<<setNo<<" row : "<<num ;
        throw ss.str();
      }
      else
      {
        cout << "Fetch pass in set : "<<setNo<<" row : "<<num << endl;
      }
    }
    if((setNo == 0 && j != 34) || (setNo == 1 && j != 11))
    {
      throw "Not all results fetched";
    }
    std::vector<Type> expcType;
    expcType.push_back (Type::INT);
    expcType.push_back (Type::STRING);
    std::vector<string> expcName;
    expcName.push_back ("f0");
    expcName.push_back ("f1");

    const Columns &cc = res.getColumns();
    for(unsigned int i=0;i < res.getColumnCount();i++)
    {
      if(expcName[i].compare(cc[i].getColumnName()))
      {
        throw "Column Name mismatch";
      }
      if(expcType[i] != cc[i].getType())
      {
        throw "Column Type mismatch";
      }
      if(0 != cc[i].getFractionalDigits())
      {
        throw "getFractionalDigits is not zero";
      }
      cout << cc[i].getColumnName() << endl;
      cout << cc[i].getType() << endl;
      cout << cc[i].isNumberSigned() << endl;
      cout << cc[i].getFractionalDigits() << endl;
    }

    setNo++;
  }
  while(res.nextResult());
  sess.sql("drop procedure if exists test").execute();
  sess.sql("CREATE PROCEDURE test() BEGIN select f0, f1 from newtable "
           "where f0 > 1000; select f0, f1  from newtable where f0 <= 10;"
           " END").execute();
  res = sess.sql("call test").execute();
  setNo = 0;
  do
  {
    unsigned int j=0;
    std::vector<Row> rowAll = res.fetchAll();
    for(j = 0;j < rowAll.size();j++)
    {
      string data = (string)rowAll[j][1];
      int num = rowAll[j][0];
      if((unsigned int)num!=j || strValue.compare(data))
      {
        std::stringstream ss;
        ss << "Fetch fail in set : "<<setNo<<" row : "<<num ;
        throw ss.str();
      }
      else
      {
        cout << "Fetch pass in set : "<<setNo<<" row : "<<num << endl;
      }
    }
    if((setNo == 0 && j != 0) || (setNo == 1 && j != 11))
    {
      throw "Not all results fetched";
    }

    setNo++;
  }
  while(res.nextResult());
}


TEST_F(Bugs, bug_hang_send_maxpacket)
{
  SKIP_IF_NO_XPLUGIN;

  auto schema = get_sess().createSchema("bug_hang_maxpacket",true);
  schema.dropCollection("test");
  auto coll = schema.createCollection("max_packet",true);

  auto query_max_packet = sql("show variables like '%mysqlx_max_allowed_packet%'");

  size_t maxpacket = std::stoul(query_max_packet.fetchOne()[1].get<std::string>());

  std::string name(maxpacket,L'A');

  std::stringstream buffer;
  buffer << R"({ "name": ")" << name << R"("})";

  try{
    coll.add(buffer.str()).execute();
    FAIL() << "Should have thrown error!";
  }
  catch (Error &e)
  {
    std::cout << "Expected: " << e << std::endl;
  }

}

TEST_F(Bugs, modify_clone)
{
  SKIP_IF_NO_XPLUGIN;

  auto coll = get_sess().getSchema("test", true).createCollection("modify_clone");
  CollectionModify cModify = coll.modify("true").set("$.name", "Data_New");
  //Should not crash
  Result mod = cModify.execute();
}
