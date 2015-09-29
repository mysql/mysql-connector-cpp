#ifndef MYSQLXX_H
#define MYSQLXX_H

#include "common.h"
#include "result.h"
#include "task.h"


namespace cdk {

class Session;

}  // cdk


namespace mysqlx {

class Session;
class Schema;
class Collection;


class Schema
{
  Session &m_sess;
  const string m_name;

public:

  Schema(Session &sess, const string &name)
    : m_sess(sess), m_name(name)
  {}

  Schema(Session&); // default schema of the session

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
  Executable find();
  Executable find(const string&);

  friend class Task;
};


/*
  Session
  =======
*/

class Session : nocopy
{
protected:

  class Impl;
  Impl  *m_impl;

  cdk::Session& get_cdk_session();

public:

  /*
  Session(const char *host, unsigned short port,
          const string  &user,
          const char    *pwd =NULL);
  */

  Session(unsigned short port,
          const string  &user,
          const char    *pwd =NULL);
  virtual ~Session();

  Schema getSchema(const string&);

private:

  friend class Schema;
  friend class Collection;
  friend class Result;
  friend class Task;
};


class NodeSession: public Session
{
public:

/*
  NodeSession(const char* host, unsigned short port,
              const string  &user,
              const char    *pwd =NULL)
   : Session(host, port, user, pwd)
  {}
*/

  NodeSession(unsigned short port,
              const string  &user,
              const char    *pwd =NULL)
   : Session(port, user, pwd)
  {}

  Result executeSql(const string &query);
};


}  // mysqlx

#endif
