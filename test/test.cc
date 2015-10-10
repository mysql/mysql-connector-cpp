/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#include <iostream>
#include <mysqlx.h>


using ::std::cout;
using ::std::endl;
using namespace ::mysqlx;


int main()
try {

  cout <<"Creating session on localhost..." <<endl;

  XSession sess(13010, "root");

  cout <<"Session accepted, creating collection..." <<endl;

  Schema sch= sess.getSchema("test");
  Collection coll= sch.createCollection("c1", true);

  cout <<"inserting document..." <<endl;

  coll.remove().execute();

  {
    Result add;

    add= coll.add("{ \"name\": \"foo\", \"age\": 1 }").execute();
    cout <<"- added doc with id: " <<add.getLastDocumentId() <<endl;

    add= coll.add("{ \"name\": \"bar\", \"age\": 2 }").execute();
    cout <<"- added doc with id: " <<add.getLastDocumentId() <<endl;

    add= coll.add("{ \"name\": \"baz\", \"age\": 3, \"date\": { \"day\": 20, \"month\": \"Apr\" }}").execute();
    cout <<"- added doc with id: " <<add.getLastDocumentId() <<endl;

    add= coll.add("{ \"_id\": \"myuuid-1\", \"name\": \"foo\", \"age\": 7 }").execute();
    cout <<"- added doc with id: " <<add.getLastDocumentId() <<endl;
  }

  cout <<"Fetching documents..." <<endl;

  DocResult docs = coll.find().execute(); // "age > 1 and name like 'ba%'").execute();

  DbDoc *doc = docs.fetchOne();

  for (int i = 0; doc; ++i, doc = docs.fetchOne())
  {
    cout <<"doc#" <<i <<": " <<*doc <<endl;

    for (Field fld : *doc)
    {
      cout << " field `" << fld << "`: " <<(*doc)[fld] << endl;
    }

    string name = (*doc)["name"];
    cout << " name: " << name << endl;

    if (doc->hasField("date") && Value::DOCUMENT == doc->fieldType("date"))
    {
      cout << "- date field" << endl;
      DbDoc date = (*doc)["date"];
      for (Field fld : date)
      {
        cout << "  date `" << fld << "`: " << date[fld] << endl;
      }
      string month = (*doc)["date"]["month"];
      int day = date["day"];
      cout << "  month: " << month << endl;
      cout << "  day: " << day << endl;
    }

    cout << endl;
  }

  cout <<"Done!" <<endl;
}
catch (const mysqlx::Error &err)
{
  cout <<"ERROR: " <<err <<endl;
}
catch (std::exception &ex)
{
  cout <<"STD EXCEPTION: " <<ex.what() <<endl;
}
catch (const char *ex)
{
  cout <<"EXCEPTION: " <<ex <<endl;
}
