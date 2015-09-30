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
  cout <<"first doc: " <<docs.first() <<endl;

  DbDoc *doc= docs.next();

  for(int i=0; doc; ++i, doc= docs.next())
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
      string month = (*doc)["date"][Field("month")];
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
