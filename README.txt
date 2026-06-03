# MySQL Connector/C++

Copyright (c) 2008, 2026, Oracle and/or its affiliates.

This is a release of MySQL Connector/C++, the C++ interface for communicating
with MySQL servers.

License information can be found in the LICENSE.txt file.
This distribution may include materials developed by third parties. For license
and attribution notices for these materials, please refer to the LICENSE.txt file.

For more information on MySQL Connector/C++ visit https://dev.mysql.com/doc/connector-cpp/en/
For additional downloads and the source of MySQL Connector/C++ visit http://dev.mysql.com/downloads

Contributing information for code and non-code submissions can be found in the CONTRIBUTING.md file.

MySQL Connector/C++ is brought to you by the MySQL team at Oracle.

## Description

This is a release of MySQL Connector/C++, [the C++ interface](https://dev.mysql.com/doc/dev/connector-cpp/8.0/) for communicating with MySQL servers.

For detailed information please visit the official [MySQL Connector/C++ documentation](https://dev.mysql.com/doc/dev/connector-cpp/8.0/).

## Download & Install

MySQL Connector/C++ can be installed from pre-compiled packages that can be downloaded from the [MySQL downloads page](https://dev.mysql.com/downloads/connector/cpp/).
The process of installing of Connector/C++ from a binary distribution is described in [MySQL online manuals](https://dev.mysql.com/doc/connector-cpp/8.0/en/connector-cpp-installation-binary.html)

### Building from sources

MySQL Connector/C++ can be installed from the source. Please check [MySQL online manuals](https://dev.mysql.com/doc/connector-cpp/8.0/en/connector-cpp-installation-source.html)

### GitHub Repository

This repository contains the MySQL Connector/C++ source code as per latest released version. You should expect to see the same contents here and within the latest released Connector/C++ package.

## Sample Code

```
#include <iostream>
#include <mysqlx/xdevapi.h>

using ::std::cout;
using ::std::endl;
using namespace ::mysqlx;


int main(int argc, const char* argv[])
try {

  const char   *url = (argc > 1 ? argv[1] : "mysqlx://root@127.0.0.1");

  cout << "Creating session on " << url
       << " ..." << endl;

  Session sess(url);

  cout <<"Session accepted, creating collection..." <<endl;

  Schema sch= sess.getSchema("test");
  Collection coll= sch.createCollection("c1", true);

  cout <<"Inserting documents..." <<endl;

  coll.remove("true").execute();

  {
    DbDoc doc(R"({ "name": "foo", "age": 1 })");

    Result add =
      coll.add(doc)
          .add(R"({ "name": "bar", "age": 2, "toys": [ "car", "ball" ] })")
          .add(R"({ "name": "bar", "age": 2, "toys": [ "car", "ball" ] })")
          .add(R"({
                 "name": "baz",
                  "age": 3,
                 "date": { "day": 20, "month": "Apr" }
              })")
          .add(R"({ "_id": "myuuid-1", "name": "foo", "age": 7 })")
          .execute();

    std::list<string> ids = add.getGeneratedIds();
    for (string id : ids)
      cout <<"- added doc with id: " << id <<endl;
  }

  cout <<"Fetching documents..." <<endl;

  DocResult docs = coll.find("age > 1 and name like 'ba%'").execute();

  int i = 0;
  for (DbDoc doc : docs)
  {
    cout <<"doc#" <<i++ <<": " <<doc <<endl;

    for (Field fld : doc)
    {
      cout << " field `" << fld << "`: " <<doc[fld] << endl;
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

    if (doc.hasField("toys") && Value::ARRAY == doc.fieldType("toys"))
    {
      cout << "- toys:" << endl;
      for (auto toy : doc["toys"])
      {
        cout << "  " << toy << endl;
      }
    }

    cout << endl;
  }
  cout <<"Done!" <<endl;
}
catch (const mysqlx::Error &err)
{
  cout <<"ERROR: " <<err <<endl;
  return 1;
}
catch (std::exception &ex)
{
  cout <<"STD EXCEPTION: " <<ex.what() <<endl;
  return 1;
}
catch (const char *ex)
{
  cout <<"EXCEPTION: " <<ex <<endl;
  return 1;
}

```

## Documentation

You can find the documentation on the MySQL website at
<http://dev.mysql.com/doc/dev/connector-cpp/>

For the new features/bugfix history, see release notes at
<https://dev.mysql.com/doc/relnotes/connector-cpp/en/news-8-0.html>.
Note that the initial releases used major version 2.0.

* [MySQL](http://www.mysql.com/)
* [Connector/C++ API Reference](https://dev.mysql.com/doc/dev/connector-cpp/8.0/)

## Contact

For general discussion of the MySQL Connector/C++ please use the C/C++
community forum at <http://forums.mysql.com/list.php?167> or join
the MySQL Connector/C++ mailing list at <http://lists.mysql.com>.

Bugs can be reported at <http://bugs.mysql.com/report.php>. Please
use the "Connector / C++" or "Connector / C++ Documentation" bug
category.

* [Discussion Forum](https://forums.mysql.com/list.php?167)
* [Slack](https://mysqlcommunity.slack.com)
* [Bugs](https://bugs.mysql.com)

