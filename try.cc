#include <iostream>
#include <mysqlxx.h>

using namespace ::std;
using namespace ::mysqlx;

int main()
try {

  cout <<"Creating session on localhost..." <<endl;

  Session sess(33060, "mike", "password");

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
