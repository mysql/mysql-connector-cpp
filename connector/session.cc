#include <mysqlxx.h>
#include <mysql/cdk/mysqlx/session.h>

#include <iostream>

using namespace ::std;

namespace mysqlx {


ostream& operator<<(ostream &out, const Error&)
{
  out <<"MYSQLX Error!";
  return out;
}

}  // mysqlx
