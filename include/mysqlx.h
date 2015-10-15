/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#ifndef MYSQLXX_H
#define MYSQLXX_H

/**
  @file
  Main Connector/C++ header.

  This header should be included by code that wants to use Connector/C++.
  It defines classes that form public API implemented by the connector.

  @sa result.h
*/


#include "mysqlx/common.h"
#include "mysqlx/result.h"
#include "mysqlx/task.h"


namespace cdk {

class Session;

}  // cdk


namespace mysqlx {

class XSession;
class Schema;
class Collection;


/**
  Represents a schema in a given XSession.

  A `Schema` instance  can be obtained from `XSession::getSchema()`
  method:

  ~~~~~~
  XSession session;
  Schema   mySchema;

  mySchema= session.getSchema("My Schema");
  ~~~~~~

  or it can be directly constructed as follows:

  ~~~~~~
  XSession   session;
  Schema     mySchema(session, "My Schema");
  ~~~~~~

  Each `Schema` instance is tied to a particular session and all
  the operations on the schema and its objects are performed using
  that session. If session is deleted, attempt to use schema of
  that session would yield an error.

  When creating Schema object, by default no checks are made that
  it actually exists in the database. Operation that is executed
  on the server and involves such non-existent schema will throw
  an error at execution time.
*/

class Schema
{
  XSession &m_sess;
  const string m_name;

public:

  /// @costructor
  Schema(XSession &sess, const string &name)
    : m_sess(sess), m_name(name)
  {}

  /**
    Construct schema object representing the default schema
    of the session.

    @todo Clarify what "default schema" is.
  */
  Schema(XSession&);

  /// Get schema name
  const string& getName() const { return m_name; }

  /**
    Create new collection in the schema. If `reuse` is false
    and collection already exists, an error is thrown. Otherwise,
    if collection with given name already exists, it is returned
    as if `getCollection()` was called.
  */

  Collection createCollection(const string&, bool reuse= false);

  /**
    Return `Collection` object representing named collection in
    the shcema. If `check_exists` is true and named collection
    does not exist, an error will be thrown. Otherwise, if named
    collection does not exists, the returned object will refer
    to non-existent collection.

    @note Checking existence of the collection involves
    communication with the server. If `check_exists` is false,
    on the other hand, no I/O is involved when creating a
    `Collection` instance.
  */

  Collection getCollection(const string&, bool check_exists= false);

  friend class Collection;
  friend class Task;
};


/*
  Collection and its operations
  =============================
*/


/**
  Represents an operation that can be executed.

  Creating an operation does not involve any communication
  with the server. Only when `execute()` method is called
  operation is sent to the server for execution.
*/

class Executable
{
protected:

  Task m_task;

public:

  /// Execute given operation and wait for its result.

  virtual BaseResult execute()
  {
    return m_task.wait();
  }
};


/**
  Operation which adds documents to a collection.

  Operation stores a list of documents that will be added
  to a given collection when this operation is executed.

  Documents are added to the list with `add()` method.
*/

class CollectionAdd
  : virtual public Executable
{
  virtual void prepare_add() = 0;
  virtual void do_add(const string&) = 0;

public:

  /**
    Add document(s) to a collection.

    Documents are described by JSON strings.
  */

  CollectionAdd& add(const string &json)
  {
    try {
      prepare_add();
      do_add(json);
      return *this;
    }
    CATCH_AND_WRAP
  }

  /**
    @copydoc add(const string&)
    Several documents can be passed to single `add()` call.
  */

  template<typename... Types>
  CollectionAdd& add(const string &json, Types... rest)
  {
    try {
      add(json);
      return add(rest...);
    }
    CATCH_AND_WRAP
  }

};


/**
  Operation which removes documents from a collection.

  @todo Sorting and limiting the range of deleted documents.
  @todo Binding values for operation parameters.
*/

class CollectionRemove
  : virtual public Executable
{
public:

  /// Remove all documents from the collection.
  virtual Executable& remove() =0;

  /// Remove documents satisfying given expression.
  virtual Executable& remove(const string&) =0;
};


/**
  Operation which finds documents satisfying given criteria.

  @todo Sorting and limiting the result.
  @todo Binding values for operation parameters.
  @todo Grouping of returned documents.
*/

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

  ~~~~~~
  Schema db;
  Collection myColl;

  myColl= db.getCollection("My Collection");
  ~~~~~~

  or directly constructed as follows:

  ~~~~~~
  Schema db;
  Collection myColl(db, "My Collection");
  ~~~~~~

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

  BaseResult execute()
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


/**
  Represents a session which gives access to data stored
  in the data store.

  When creating new session a host name, TCP/IP port,
  user name and password are specified. Once created,
  session is ready to be used. Session destructor closes
  session and cleans up after it.

  If it is not possible to create a valid session for some
  reason, errors are thrown from session constructor.

  @todo Add all `XSession` methods defined by DevAPI.
*/

class XSession : nocopy
{
protected:

  class Impl;
  Impl  *m_impl;

  cdk::Session& get_cdk_session();

public:

  /// @constructor
  XSession(const char *host, unsigned short port,
           const string  &user,
           const char    *pwd =NULL);

  /// Create session for database on localhost.
  XSession(unsigned short port,
           const string  &user,
           const char    *pwd = NULL)
    : XSession("localhost", port, user, pwd)
  {}

  virtual ~XSession();

  /**
    Get named schema object in a given session.

    The object does not have to exist in the database.
    Errors will be thrown if one tries to use non-existing
    schema.
  */

  Schema getSchema(const string&);

private:

  friend class Schema;
  friend class Collection;
  friend class Result;
  friend class Task;
};


/**
  A session which is always connected to a single
  MySQL node.

  In addition to `XSession` functionality, `NodeSession`
  allows for execution of arbitrary SQL queries.
*/

class NodeSession
  : public XSession
  , public Executable
{
public:

  /// @constructor
  NodeSession(const char* host, unsigned short port,
              const string  &user,
              const char    *pwd =NULL)
   : XSession(host, port, user, pwd)
  {}

  /// @constructor
  NodeSession(unsigned short port,
              const string  &user,
              const char    *pwd =NULL)
   : NodeSession("localhost", port, user, pwd)
  {}

  /// @member
  Executable& sql(const string &query);
};


}  // mysqlx

#endif
