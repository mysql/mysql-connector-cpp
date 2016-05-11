/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
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
#include "mysqlx/task.h"
#include "mysqlx/crud.h"
#include "mysqlx/collection_crud.h"
#include "mysqlx/table_crud.h"

#include <list>

namespace cdk {

class Session;

}  // cdk


namespace mysqlx {

class XSession;
class Schema;
class Collection;
class Table;

/**
  List_initializer class is used to initialize user std::vector, std::list or
  own list imlpementations, as long as initialized by iterators of defied type
  */

template <typename T>
struct List_init
{
   std::forward_list<T> m_data;

   template <typename I>
   List_init(const I& i)
     : m_data(std::begin(i), std::end(i))
   {}

   template<typename U>
   operator U()
   {
     return U(m_data.begin(), m_data.end());
   }
};


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

  /// Construct named schema object.
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

  std::list<Collection> getCollections();

  /**
    Return list of names of collections in the schema.
  */

  List_init<string> getCollectionNames();

  /**
    Return list of `Table` object representing tables and views in
    the shcema.
  */

  List_init<Table> getTables();

  /**
    Return list of tables and views in the shcema.
  */
  List_init<string> getTableNames();

  friend class Collection;
  friend class Task;
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
  : public internal::CollectionAddBase
  , public internal::CollectionRemoveBase
  , public internal::CollectionFindBase
  , public internal::CollectionModifyBase
{
  Schema m_schema;
  const string m_name;

public:

  Collection(const Collection &other)
    : CollectionOpBase(*this)
    , m_schema(other.m_schema), m_name(other.m_name)
  {}

  Collection(const Schema &sch, const string &name)
    : CollectionOpBase(*this)
    , m_schema(sch), m_name(name)
  {}

  const string& getName() const { return m_name; }
  const Schema& getSchema() const { return m_schema; }


  friend class Task;
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
    : public internal::TableInsertBase
    , public internal::TableSelectBase
    , public internal::TableUpdateBase
    , public internal::TableRemoveBase
{
  Schema m_schema;
  const string m_name;
  enum { YES, NO, UNDEFINED} m_isview = UNDEFINED;

public:

  Table(const Schema &sch, const string &name)
    : internal::TableOpBase(*this)
    , m_schema(sch), m_name(name)
  {}

  Table(const Schema &sch, const string &name, bool isView)
    : Table(sch, name)
  {
    m_isview = isView ? YES : NO;
  }

  const string& getName() const { return m_name; }
  const Schema& getSchema() const { return m_schema; }

  bool isView();

  friend class Task;
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

  Schema createSchema(const string &name, bool reuse = false);

  /**
    Get named schema object in a given session.

    Errors will be thrown if one tries to use non-existing
    schema with check_existence = true.
  */

  Schema getSchema(const string&, bool check_existence = false);

  /**
    Get list of schema objects in a given session.
  */

  std::list<Schema> getSchemas();

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

  /// Create a single node session.
  NodeSession(const char* host, unsigned short port,
              const string  &user,
              const char    *pwd =NULL)
   : XSession(host, port, user, pwd)
  {}

  /// Create a single node session on localhost.
  NodeSession(unsigned short port,
              const string  &user,
              const char    *pwd =NULL)
   : NodeSession("localhost", port, user, pwd)
  {}

  /// Operation that runs arbitrary SQL query on the node.
  Executable& sql(const string &query);
};


}  // mysqlx

#endif
