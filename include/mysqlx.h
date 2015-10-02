#ifndef MYSQLXX_H
#define MYSQLXX_H

#include "common.h"
#include "result.h"
#include "task.h"


namespace cdk {

class Session;

}  // cdk


namespace mysqlx {

class XSession;
class Schema;
class Collection;


class Schema
{
  XSession &m_sess;
  const string m_name;

public:

  Schema(XSession &sess, const string &name)
    : m_sess(sess), m_name(name)
  {}

  Schema(XSession&); // default schema of the session

  const string& getName() const { return m_name; }

  Collection createCollection(const string&, bool reuse= false);
  Collection getCollection(const string&, bool check_exists= false);

  friend class Collection;
  friend class Task;
};


/*
  Database objects
  ================
*/


class Collection
{
  Schema &m_schema;
  const string m_name;

public:

  Collection(Schema &sch, const string &name)
    : m_schema(sch), m_name(name)
  {}

  const string& getName() const { return m_name; }
  const Schema& getSchema() const { return m_schema; }

  Executable add(const string&); // add document given by json string
  Executable remove();
  Executable remove(const string&);
  Executable find();
  Executable find(const string&);

  friend class Task;
};


/*
  Session
  =======
*/

class XSession : nocopy
{
protected:

  class Impl;
  Impl  *m_impl;

  cdk::Session& get_cdk_session();

public:

  XSession(const char *host, unsigned short port,
           const string  &user,
           const char    *pwd =NULL);

  XSession(unsigned short port,
           const string  &user,
           const char    *pwd = NULL)
    : XSession("localhost", port, user, pwd)
  {}

  virtual ~XSession();

  Schema getSchema(const string&);

private:

  friend class Schema;
  friend class Collection;
  friend class Result;
  friend class Task;
};


class NodeSession: public XSession
{
public:


  NodeSession(const char* host, unsigned short port,
              const string  &user,
              const char    *pwd =NULL)
   : XSession(host, port, user, pwd)
  {}

  NodeSession(unsigned short port,
              const string  &user,
              const char    *pwd =NULL)
   : NodeSession("localhost", port, user, pwd)
  {}

  Result executeSql(const string &query);
};


}  // mysqlx

#endif
