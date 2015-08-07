#include <iostream>
#include <mysqlxx.h>
//#include <mysql/cdk.h>

using namespace ::std;
//using namespace ::cdk;
using namespace ::mysqlx;

int main()
try {

  cout <<"Creating session on localhost..." <<endl;

#if 0

  ds::TCPIP ds("nike22.se.oracle.com", 13010);
  ds::Options opt;
  connection::TCPIP conn("localhost", 13010);
  conn.connect();

  mysqlx::Session sess(conn, opt);

  if (!sess.is_valid())
    throw sess.get_error();

  cout <<"Session accepted, sending query..." <<endl;

  //Reply r= sess.sql(L"SELECT 1, 'ala'");
  //Cursor c(r);

#else

  NodeSession sess(13010, "root");

  cout <<"Session accepted, sending query..." <<endl;
  /*
    TODO:

    On Linux and OSX, doing it this way invokes copy constructor:

    Result res= sess.executeSql(...);

    Perhaps using const reference as result initializer will avoid this.
  */

  Result res(sess.executeSql(L"SELECT * FROM mysql.user"));

  cout <<"Query sent, reading rows..." <<endl;

  Row *row;

  while ((row= res.next()))
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
