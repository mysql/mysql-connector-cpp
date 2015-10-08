#ifndef MYSQLXX_H
#define MYSQLXX_H

/**
  @file
  Main Connector/C++ 2.0 header.

  This is more text about this header.
*/


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

/**
  Represents a schema in a given XSession.

  All operations on the schema are performed using XSession
  instance for which this Schema object was created.

  @note When creating Schema object no checks are made that
  it actually exists in the database.
*/

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
  Collection
  ==========
*/


/**
  Base class for operations that can be executed.
*/

class Executable
{
protected:

  Task m_task;

public:

  /// Execute given operation and wait for its result.

  virtual Result execute()
  {
    return m_task.wait();
  }
};


/// Operation which adds documents to a collection.

class CollectionAdd
  : virtual public Executable
{
  virtual void prepare_add() = 0;
  virtual void do_add(const string&) = 0;

public:

  /**
    @name add
    Add document(s) to a collection.

    Documents are described by JSON strings. Several documents
    can be passed to single `add()` call.
  */
  //@{

  CollectionAdd& add(const string &json)
  {
    try {
      prepare_add();
      do_add(json);
      return *this;
    }
    CATCH_AND_WRAP
  }


  template<typename... Types>
  CollectionAdd& add(const string &json, Types... rest)
  {
    try {
      add(json);
      return add(rest...);
    }
    CATCH_AND_WRAP
  }

  //@}
};


class CollectionRemove
  : virtual public Executable
{
public:

  /// Remove all documents from the collection.
  virtual Executable& remove() =0;

  /// Remove documents satisfying given expression.
  virtual Executable& remove(const string&) =0;
};


class CollectionFind
  : virtual public Executable
{
public:

  /// Return all the documents in the collection.
  virtual Executable& find() =0;

  /// Find documents that satisfy given expression.
  virtual Executable& find(const string&) =0;
};


/**
  Represents collection of documents in a schema.

  Collection object can be obtained from `Schema::getCollection()`
  method:

      Schema db;
      Collection myColl;

      myColl= db.getCollection("My Collection");

  Collection instance can be also directly constructed as follows:

      Schema db;
      Collection myColl(db, "My Collection");

  Documents can be added and removed using @link add `add()`@endlink
  and `remove()` method, respectively. Method `find()`
  is used to query documents that satisfy given criteria.

  @todo `update()` method.
  @todo Support for parameterized collection operations.
  @todo Sorting and limiting returned results.
*/

class Collection
  : public CollectionAdd
  , public CollectionRemove
  , public CollectionFind
{
  Schema &m_schema;
  const string m_name;

  enum { NONE, ADD } m_op;

  Result execute()
  {
    m_op = NONE;
    return Executable::execute();
  }

  void prepare_add();
  void do_add(const string&);

public:

  Collection(Schema &sch, const string &name)
    : m_schema(sch), m_name(name), m_op(NONE)
  {}

  const string& getName() const { return m_name; }
  const Schema& getSchema() const { return m_schema; }

  Executable& remove();
  Executable& remove(const string&);
  Executable& find();
  Executable& find(const string&);

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
