#ifndef MYSQLXX_H
#define MYSQLXX_H

#include <mysql/cdk.h>

#include <string>
#include <ostream>

namespace mysqlx {

using cdk::string;
using std::ostream;
typedef unsigned col_count_t;

class Session
{
  cdk::ds::TCPIP  m_ds;
  std::string     m_pwd;
  cdk::Session    m_sess;

public:

  Session(unsigned short port,
          const string  &user,
          const char    *pwd =NULL)
  : m_ds("localhost", port)
  , m_pwd(pwd)
/*
  TODO: Consider refactoring cdk::Session so that constructor
  accepts const reference to ds object. Then instead of m_ds,
  a temporary cdk::ds::TCPIP() instance could be used.
*/
  , m_sess(m_ds, cdk::ds::Options(user, pwd ? &m_pwd : NULL))
  {}

  virtual ~Session()
  {}
};


class NodeSession: public Session
{
public:

  NodeSession(unsigned short port,
              const string  &user,
              const char    *pwd =NULL)
   : Session(port, user, pwd)
  {}

  const NodeSession& executeSql(const string&);
};


class Row;

class SqlResult
{
public:

  SqlResult(const NodeSession&);

  col_count_t getColumnCount() const;
  Row* next();
};


class Row
{
public:

  const string& getString(unsigned pos);
  const string& operator[](unsigned pos)
  { return getString(pos); }
};


class Error
{};

ostream& operator<<(ostream&, const Error&);

}  // mysqlx

#endif
