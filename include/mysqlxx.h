#ifndef MYSQLXX_H
#define MYSQLXX_H

#include <mysql/cdk.h>

#include <vector>
#include <string>
#include <ostream>
#include <memory>

namespace mysqlx {

using cdk::foundation::nocopy;

using cdk::byte;
using cdk::string;
using std::ostream;
typedef unsigned col_count_t;

class Session;
class Schema;
class Collection;
class Result;
class Row;


class Executable : nocopy
{
  typedef cdk::Reply* Result_init;

  class Impl;
  Impl  *m_impl;

  Executable(Impl *impl) : m_impl(impl)
  {}

  class Op_collection_add;

public:

  ~Executable();
  Result_init execute();

  friend class Impl;
  friend class Session;
  friend class NodeSession;
  friend class Result;
  friend class Collection;
  friend class Schema;
};


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
  friend class Executable::Op_collection_add;
};


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

  friend class Executable::Op_collection_add;
};


class Session : nocopy
{
protected:

  cdk::ds::TCPIP   m_ds;
  std::string      m_pwd;
  cdk::ds::Options m_opt;
  cdk::Session     m_sess;

public:

  /*
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
  */

  Session(unsigned short port,
          const string  &user,
          const char    *pwd =NULL)
  : m_ds("localhost", port)
  , m_pwd(pwd ? pwd : "")
  , m_opt(user, pwd ? &m_pwd : NULL)
  , m_sess(m_ds, m_opt)
  {
    m_sess.wait();
    if (!m_sess.is_valid())
      throw m_sess.get_error();
  }

  virtual ~Session()
  {}

  Schema getSchema(const string&);

private:

  typedef Executable::Result_init Result_init;

  Result_init create_collection(const string &schema,
                                const string &name,
                                bool  reuse);

  Result_init add_document(const string &schema,
                           const string &name,
                           const string &json);

  friend class Schema;
  friend class Collection;
  friend class Result;
  friend class Executable::Op_collection_add;
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

  Executable::Result_init executeSql(const string &query)
  {
    cdk::Reply *r= new cdk::Reply(m_sess.sql(query));
    r->wait();
    if (0 < r->entry_count())
      r->get_error().rethrow();
    return Executable::Result_init(r);
  }
};



class Row : nocopy
{
public:

  virtual ~Row() {}

  virtual const string getString(col_count_t pos) =0;
  const string operator[](col_count_t pos)
  { return getString(pos); }
};


class Result : nocopy
{
  cdk::Reply  *m_reply;
  cdk::Cursor *m_cursor;
  col_count_t  m_pos;

  class Impl;
  Impl  *m_impl;

public:

  // TODO: use const references as initializers

  Result(cdk::Reply *init);
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
