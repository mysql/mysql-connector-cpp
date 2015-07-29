#ifndef MYSQLXX_H
#define MYSQLXX_H

#include <mysql/cdk.h>

#include <vector>
#include <string>
#include <ostream>

namespace mysqlx {

using cdk::foundation::nocopy;

using cdk::byte;
using cdk::string;
using std::ostream;
typedef unsigned col_count_t;

class Result;
class Row;

class Session : nocopy
{
protected:

  cdk::ds::TCPIP  m_ds;
  std::string     m_pwd;
  cdk::Session    m_sess;

  class Access : private cdk::Reply
  {
    typedef cdk::Reply::Initializer Initializer;
    friend class Session;
  };

  typedef Access::Initializer Result_init;

public:

  Session(const char *host, unsigned short port,
          const string  &user,
          const char    *pwd =NULL)
  : m_ds(host, port)
  , m_pwd(pwd ? pwd : "")
  , m_sess(m_ds, cdk::ds::Options(user, pwd ? &m_pwd : NULL))
  {
    if (!m_sess.is_valid())
      throw m_sess.get_error();
  }

  Session(unsigned short port,
          const string  &user,
          const char    *pwd =NULL)
  : m_ds("localhost", port)
  , m_pwd(pwd ? pwd : "")
/*
  TODO: Consider refactoring cdk::Session so that constructor
  accepts const reference to ds object. Then instead of m_ds,
  a temporary cdk::ds::TCPIP() instance could be used.
*/
  , m_sess(m_ds, cdk::ds::Options(user, pwd ? &m_pwd : NULL))
  {}

  virtual ~Session()
  {}

  friend class Result;
};


class NodeSession: public Session
{
public:

  NodeSession(const char* host, unsigned short port,
              const string  &user,
              const char    *pwd =NULL)
   : Session(host, port, user, pwd)
  {}

  NodeSession(unsigned short port,
              const string  &user,
              const char    *pwd =NULL)
   : Session(port, user, pwd)
  {}

  Result_init executeSql(const string &query)
  {
    return m_sess.sql(query);
  }
};


class Row
{
public:

  virtual ~Row() {}

  virtual const string getString(col_count_t pos) =0;
  const string operator[](col_count_t pos)
  { return getString(pos); }
};


class Result : nocopy
{
  cdk::Reply   m_reply;
  cdk::Cursor *m_cursor;
  col_count_t  m_pos;

  class Impl;
  Impl  *m_impl;

public:

  // TODO: use const references as initializers

  Result(Session::Result_init init);
  ~Result();

  col_count_t getColumnCount() const
  {
    if (!m_cursor)
      throw "No result set";
    return m_cursor->col_count();
  }

  Row* next();

};


class Error
{};

ostream& operator<<(ostream&, const Error&);

}  // mysqlx

#endif
