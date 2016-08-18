/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C++ is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FLOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef MYSQLXX_H
#define MYSQLXX_H

/**
  @file
  Main Connector/C++ header.

  This header should be included by code that wants to use Connector/C++.
  It defines classes that form public API implemented by the connector.

  @sa result.h, document.h
*/


#include "mysqlx/common.h"
#include "mysqlx/result.h"
#include "mysqlx/statement.h"
#include "mysqlx/crud.h"
#include "mysqlx/collection_crud.h"
#include "mysqlx/table_crud.h"


#define DEFAULT_MYSQLX_PORT 33060


/*
  On Windows, dependency on the sockets library can be handled using
  #pragma comment directive.
*/

#ifdef _WIN32
#pragma comment(lib,"ws2_32")
#endif


namespace cdk {

class Session;

}  // cdk


namespace mysqlx {

class XSession;
class Schema;
class Collection;
class Table;


/**
  Represents a database object

  Inherited by Schema, Table and Collection. Can't be used alone.
*/

class DatabaseObject
{

protected:

  XSession_base *m_sess;
  string m_name;

  DatabaseObject(XSession_base& sess, const string& name = string())
    : m_sess(&sess), m_name(name)
  {}

  virtual ~DatabaseObject()
  {}

public:


  /**
     Get database object name
  */

  const string& getName() const { return m_name; }


  /**
    Get Session object
  */

  XSession_base& getSession() { return *m_sess; }


  /**
     Get schema object
  */

  virtual const Schema& getSchema() const = 0;


  /**
     Check if database object exists

     Every check will contact server.
  */

  virtual bool existsInDatabase() const = 0;


  friend Schema;
  friend Table;
  friend Collection;

};


/**
  Represents a database schema.

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
  : public DatabaseObject
{

public:

  /**
     Construct named schema object.
  */

  Schema(XSession_base &sess, const string &name)
    : DatabaseObject(sess, name)
  {}

  /**
    Construct schema object representing the default schema
    of the session.

    @todo Clarify what "default schema" is.
  */

  Schema(XSession_base&);


  /**
     Get schema object
  */

  const Schema& getSchema() const override { return *this; }


  /**
     Check if database object exists
  */

  bool existsInDatabase() const override;


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

  /**
    Return `Table` object representing table or view in
    the shcema. If `check_exists` is true and table
    does not exist, an error will be thrown.
    Otherwise, if table does not exists,
    the returned object will refer
    to non-existent table.

    @note Checking existence of the table involves
    communication with the server. If `check_exists` is false,
    on the other hand, no I/O is involved when creating a
    `Table` instance.
  */

  Table getTable(const string&, bool check_existence = false);

  /**
    Return list of `Collection` object representing collections in
    the shcema.
  */

  internal::List_init<Collection> getCollections();

  /**
    Return list of names of collections in the schema.
  */

  internal::List_init<string> getCollectionNames();

  /**
    Return list of `Table` object representing tables and views in
    the shcema.
  */

  internal::List_init<Table> getTables();

  /**
    Return list of tables and views in the shcema.
  */

  internal::List_init<string> getTableNames();

  /**
    Return Collection as a Table object.

    @note Checking existence of the collection involves
    communication with the server. If `check_exists` is false,
    on the other hand, no I/O is involved when creating a
    `Table` instance.
  */
  Table getCollectionAsTable(const string&, bool check_exists = true);

  friend Collection;
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
  : public DatabaseObject
  , public internal::CollectionAddBase
  , public internal::CollectionRemoveBase
  , public internal::CollectionFindBase
  , public internal::CollectionModifyBase
{
  Schema m_schema;

public:

  Collection(const Collection &other)
    : CollectionOpBase(*this)
    , DatabaseObject(*other.m_schema.m_sess, other.m_name)
    , m_schema(other.m_schema)
  {}

  Collection(Collection&& other)
    : CollectionOpBase(*this)
    , DatabaseObject(*other.m_sess, std::move(other.m_name))
    , m_schema(std::move(other.m_schema))
  {}

  Collection(const Schema &sch, const string &name)
    : CollectionOpBase(*this)
    , DatabaseObject(*sch.m_sess, name)
    , m_schema(sch)
  {}


  Collection& operator=(const Collection &other)
  {
    DatabaseObject::operator=(other);
    m_schema = other.m_schema;
    return *this;
  }


  /**
     Get schema object
  */

  const Schema& getSchema() const override { return m_schema; }

  /**
     Check if database object exists
  */

  bool existsInDatabase() const override;


  /**
    Get number of documents
  */
  uint64_t count();

};


/*
  Table object
  ============
*/


/**
  Represents a table in a schema.

  Collection object can be obtained from `Schema::getTable()`
  method:

  ~~~~~~
  Schema db;
  Table  myTable;

  myTable= db.getTable("My Table");
  ~~~~~~

  or directly constructed as follows:

  ~~~~~~
  Schema db;
  Table myTable(db, "My Table");
  ~~~~~~

  Rows can be added to a table using `insert()` method followed
  by `values()` calls, which prepare insert operation.

  @todo Other CRUD operations on a table.
*/

class Table
    : public DatabaseObject
    , public internal::TableInsertBase
    , public internal::TableSelectBase
    , public internal::TableUpdateBase
    , public internal::TableRemoveBase
{
  Schema m_schema;
  enum { YES, NO, UNDEFINED} m_isview = UNDEFINED;

public:

  DIAGNOSTIC_PUSH

  #if _MSC_VER && _MSC_VER < 1900
    /*
      MSVC 2013 has problems with delegating constructors for classes which
      use virtual inheritance.
      See: https://www.daniweb.com/programming/software-development/threads/494204/visual-c-compiler-warning-initvbases
    */
    DISABLE_WARNING(4100)
  #endif

  Table(const Table& other)
    : internal::TableOpBase(*this)
    , DatabaseObject(*other.m_sess, other.m_name)
    , m_schema(other.m_schema)
  {
    m_isview = other.m_isview;
  }

  Table(Table&& other)
    : internal::TableOpBase(*this)
    , DatabaseObject(*other.m_sess, std::move(other.m_name))
    , m_schema(std::move(other.m_schema))
  {
    m_isview = other.m_isview;
  }

  Table(const Schema &sch, const string &name)
    : internal::TableOpBase(*this)
    , DatabaseObject(*sch.m_sess, name)
    , m_schema(sch)
  {}

  Table(const Schema &sch, const string &name, bool isView_)
    : Table(sch, name)
  {
    m_isview = isView_ ? YES : NO;
  }

  DIAGNOSTIC_POP


  Table& operator=(const Table &other)
  {
    DatabaseObject::operator=(other);
    m_schema = other.m_schema;
    m_isview = other.m_isview;
    return *this;
  }


  bool isView();


  /**
     Get schema object
  */

  const Schema& getSchema() const override { return m_schema; }


  /**
     Check if Table exists
  */

  bool existsInDatabase() const override;

  /**
    Get number of rows
  */
  uint64_t count();

};


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

class XSession_base : internal::nocopy
{
protected:

  class Impl;
  Impl  *m_impl;

  void register_result(internal::BaseResult *result);
  void deregister_result(internal::BaseResult *result);

  cdk::Session& get_cdk_session();

  struct Options;

  XSession_base(const Options&);

public:

  /**
    @constructor
    Create session specified by mysqlx connection string.

    Connection string can be either an utf8 encoded single-byte
    string or a wide string (which is converted to utf8 before
    parsing).
  */

  XSession_base(const std::string &url);

  XSession_base(const char *url)
    : XSession_base(std::string(url))
  {}

  XSession_base(const string &url)
    : XSession_base(std::string(url))
  {}


  /**
    Create session explicitly specifying session parameters.
  */

  XSession_base(const std::string &host, unsigned port,
           const string  &user,
           const char *pwd = NULL,
           const string &db = string());

  XSession_base(const std::string &host, unsigned port,
           const string  &user,
           const std::string &pwd,
           const string &db = string())
    : XSession_base(host, port, user, pwd.c_str(), db)
  {}

  /**
    Create session using the default port
  */

  XSession_base(const std::string &host,
           const string  &user,
           const char    *pwd = NULL,
           const string  &db = string())
    : XSession_base(host, DEFAULT_MYSQLX_PORT, user, pwd, db)
  {}

  XSession_base(const std::string &host,
           const string  &user,
           const std::string &pwd,
           const string  &db = string())
    : XSession_base(host, DEFAULT_MYSQLX_PORT, user, pwd, db)
  {}

  /**
    Create session on localhost.
  */

  XSession_base(unsigned port,
           const string  &user,
           const char    *pwd = NULL,
           const string  &db = string())
    : XSession_base("localhost", port, user, pwd, db)
  {}

  XSession_base(unsigned port,
           const string  &user,
           const std::string &pwd,
           const string  &db = string())
    : XSession_base("localhost", port, user, pwd.c_str(), db)
  {}

  virtual ~XSession_base();

  /**
    Get named schema object in a given session.

    The object does not have to exist in the database.
    Errors will be thrown if one tries to use non-existing
    schema.
  */

  Schema createSchema(const string &name, bool reuse = false);

  /**
    Get named schema object in a given session.

    Errors will be thrown if one tries to use non-existing
    schema with check_existence = true.
  */

  Schema getSchema(const string&, bool check_existence = false);

  Schema getDefaultSchema();

  /**
    Get list of schema objects in a given session.
  */

  internal::List_init<Schema> getSchemas();

  /**
    Drop the schema.

    Errors will be thrown if schema doesn't exist,
  */

  void   dropSchema(const string &name);

  /**
    Drop a table from a schema.

    Errors will be thrown if table doesn't exist,
  */

  void   dropTable(const string& schema, const string& table);

  /**
    Drop a collection from a schema.

    Errors will be thrown if collection doesn't exist,
  */

  void   dropCollection(const string& schema, const string& collection);

  /**
    Start a new transaction.

    Throws error if previously opened transaction is not closed.
  */

  void startTransaction();

  /**
    Commit opened transaction, if any.

    Does nothing if no transaction was opened. After commiting the
    transaction is closed.
  */

  void commit();

  /**
    Rollback opened transaction, if any.

    Does nothing if no transaction was opened. Transaction which was
    rolled back is closed. To start a new transaction a call to
    `startTransaction()` is needed.
  */

  void rollback();


public:

  struct Access;
  friend Access;

  friend Schema;
  friend Collection;
  friend Table;
  friend Result;
  friend RowResult;

  template <typename A>
  friend class Op_base;

  friend internal::BaseResult;
};


class XSession
    : public XSession_base
{

  using XSession_base::XSession_base;

};


/**
  A session which offers SQL query execution.

  In addition to `XSession` functionality, `NodeSession`
  allows for execution of arbitrary SQL queries.
*/

class NodeSession
  : public XSession_base
{
public:

  /**
    NodeSession constructors accept the same parameters
    as XSession constructors.
  */

  template <
    typename... T,
    typename = typename std::enable_if<
      std::is_constructible<XSession_base, T...>::value
    >::type
  >
  NodeSession(T... args)
    : XSession_base(args...)
  {}

  /*
    Templates below are here to take care of the optional password
    parameter of type const char* (which can be either 2-nd or 3-rd in
    the parameter list). Without these templates passing
    NULL as password does not work, because NULL is defined as 0
    which has type int.
  */

  template <
    typename    A,
    typename    B,
    typename... T,
    typename = typename std::enable_if<
      std::is_constructible<XSession_base, A, B, const char*, T...>::value
    >::type
  >
  NodeSession(A a, B b, void* p, T... args)
    : XSession_base(a, b, (const char*)p, args...)
  {}

  template <
    typename    A,
    typename    B,
    typename    C,
    typename... T,
    typename = typename std::enable_if<
    std::is_constructible<XSession_base, A, B, C, const char*, T...>::value
    >::type
  >
    NodeSession(A a, B b, C c, void* p, T... args)
    : XSession_base(a, b, c, (const char*)p, args...)
  {}


  /**
    Operation that runs arbitrary SQL query on the node.
  */

  SqlStatement& sql(const string &query);

private:

  SqlStatement m_stmt;

  friend XSession;
};


}  // mysqlx

#endif
