#include <mysqlxx.h>
#include <mysql/cdk/mysqlx/session.h>

#include <iostream>

using namespace ::std;

namespace mysqlx {


class Session::Impl
{
  typedef cdk::foundation::connection::TCPIP Conn;
  typedef cdk::mysqlx::Session Session;

  Conn m_conn;
  Session *m_sess;

  Impl(unsigned short port, const string &user, const char *pwd)
    : m_conn("localhost", port), m_sess(NULL)
  {
    cout <<"Connecting to port " <<port;
    cout <<" as user: " <<user <<endl;
    m_conn.connect();

    cout <<"Connected, creating session" <<endl;
    std::string password(pwd);
    cdk::ds::Options opt(user, pwd ? &password : NULL);
    m_sess = new Session(m_conn, opt);
  }

  ~Impl()
  {
    delete m_sess;
  }

  friend class mysqlx::Session;
};


Session::Session(unsigned short port,
                 const string  &user,
                 const char    *pwd)
  : m_impl(NULL)
{
  m_impl = new Impl(port, user, pwd);
}

Session::~Session()
{
  delete m_impl;
}


ostream& operator<<(ostream &out, const Error&)
{
  out <<"MYSQLX Error!";
  return out;
}

}  // mysqlx
