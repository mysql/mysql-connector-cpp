#include <iostream>
#include <mysqlxx.h>
//#include <mysql/cdk.h>

namespace mysqlx {

}

using namespace ::std;
//using namespace ::cdk;
using namespace ::mysqlx;

int main()
try {

  cout <<"Creating session on localhost..." <<endl;

#if 0

  ds::TCPIP ds("localhost", 13010);
  ds::Options opt("root", NULL);
  //connection::TCPIP conn("localhost", 13010);
  //conn.connect();

  Session sess(ds, opt);

  sess.wait();
  if (!sess.is_valid())
    throw sess.get_error();

  cout <<"Session accepted, sending query..." <<endl;

  //Reply r= sess.sql(L"SELECT 1, 'ala'");
  //Cursor c(r);

#else

  NodeSession sess(13010, "root");

  cout <<"Session accepted, creating collection..." <<endl;

  Schema sch= sess.getSchema("test");
  Collection coll= sch.createCollection("c1", true);

  cout <<"inserting document..." <<endl;

  //coll.add("{ \"_id\": \"uuid-5\", \"name\": \"foo\", \"age\": 7 }").execute();

  /*
    TODO:

    On Linux and OSX, doing it this way invokes copy constructor:

    Result res= sess.executeSql(...);

    Perhaps using const reference as result initializer will avoid this.
  */

  cout <<"querying collection..." <<endl;

  Result res(sess.executeSql(L"SELECT * FROM test.c1"));

  cout <<"Query sent, reading rows..." <<endl;
  cout <<"There are " <<res.getColumnCount() <<" columns in the result" <<endl;
  Row *row;

  while (NULL != (row= res.next()))
  {
    cout <<"== next row ==" <<endl;
    for (unsigned i=0; i < res.getColumnCount(); ++i)
    {
      cout <<"col#" <<i <<": " <<(*row)[i] <<endl;
    }
  }

#endif

  cout <<"Done!" <<endl;
}
catch (const cdk::Error &err)
{
  // TODO: why these errors are not caught as std::exception?
  cout <<"CDK ERROR: " <<err <<endl;
}
//catch (const mysqlx::Error &err)
//{
//  cout <<"ERROR: " <<err <<endl;
//}
catch (std::exception &ex)
{
  cout <<"STD EXCEPTION: " <<ex.what() <<endl;
}
catch (const char *ex)
{
  cout <<"EXCEPTION: " <<ex <<endl;
}
