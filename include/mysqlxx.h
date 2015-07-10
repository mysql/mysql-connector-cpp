#ifndef MYSQLXX_H
#define MYSQLXX_H

#include <mysql/cdk.h>

#include <string>
#include <ostream>

namespace mysqlx {

using cdk::string;
using std::ostream;

class Session
{
  class Impl;
  Impl *m_impl;

public:

  typedef cdk::string string;

  Session(unsigned short port,
          const string  &user,
          const char    *pwd =NULL);
  ~Session();
};


class Error
{};

ostream& operator<<(ostream&, const Error&);

}  // mysqlx

#endif
