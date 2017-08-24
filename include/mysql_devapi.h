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

#ifndef MYSQL_DEVAPI_H
#define MYSQL_DEVAPI_H

#ifndef __cplusplus
#error This header can be only used with C++ code
#endif

/**
  @defgroup devapi  X DevAPI Classes

  X DevAPI Classes and types. See @ref devapi_ref for introduction.

  @defgroup devapi_op     Database operations
  @ingroup devapi

  Classes representing yet-to-be-executed database operations.

  Such operations are created by various methods of @link mysqlx::Collection
  `Collection`@endlink or @link mysqlx::Table `Table`@endlink classes. Database
  operation classes define methods that specify additional operation
  characteristics before it gets executed with `execute()` method. The latter
  returns a @link mysqlx::Result `Result`@endlink, @link mysqlx::DocResult
  `DocResult`@endlink or @link mysqlx::RowResult `RowResult`@endlink object,
  depending on the type of operation.

  @defgroup devapi_res    Classes for result processing
  @ingroup devapi

  Classes used to examine results of a statement and documents or
  rows contained in a result.

  @defgroup devapi_aux    Auxiliary types
  @ingroup devapi
*/


/**
  @file
  The main header for MySQL Connector/C++ DevAPI.

  This header should be included by C++ code which uses the DevAPI implemented
  by MySQL Connector/C++.

  @sa result.h, document.h

  @ingroup devapi
*/

/*
  X DevAPI public classes are declared in this and other headers included from
  devapi/ folder. The main public API classes, such as Session below, contain
  declarations of public interface methods. Any obscure details of the public
  API, which must be defined in the public header, are factored out
  to Session_detail class from which the main Session class inherits.
  Among other things, Session_detail declares the implementation class for
  Session. This implementation class is opaque and its details are not defined
  in the public headers - only in the implementation part. Definitions of
  XXX_detail classes can be found in devapi/detail/ sub-folder.
*/

#include "devapi/common.h"
#include "devapi/result.h"
#include "devapi/collection_crud.h"
#include "devapi/table_crud.h"
#include "devapi/view_ddl.h"
#include "devapi/settings.h"
#include "devapi/detail/session.h"


namespace mysqlx {

/**
  Represents a database schema.

  A `Schema` instance can be obtained from `Session::getSchema()`
  method:

  ~~~~~~
  Session session;
  Schema   mySchema;

  mySchema= session.getSchema("My Schema");
  ~~~~~~

  or it can be directly constructed as follows:

  ~~~~~~
  Session   session;
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

  @ingroup devapi
*/

class PUBLIC_API Schema
  : public internal::Db_object
{
  using CollectionList = internal::List_init<Collection>;
  using TableList      = internal::List_init<Table>;
  using StringList     = internal::List_init<string>;

public:

  /**
     Construct named schema object.
  */

  Schema(Session &sess, const string &name)
    : internal::Db_object(sess, name)
  {}

  /**
    Construct schema object representing the default schema
    of the session.

    @todo Clarify what "default schema" is.
  */

  Schema(Session&);


  const Schema& getSchema() const override { return *this; }

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
    the schema. If `check_exists` is true and named collection
    does not exist, an error will be thrown. Otherwise, if named
    collection does not exist, the returned object will refer
    to non-existent collection.

    @note Checking existence of the collection involves
    communication with the server. If `check_exists` is false,
    on the other hand, no I/O is involved when creating a
    `Collection` instance.
  */

  Collection getCollection(const string&, bool check_exists= false);

  /**
    Return `Table` object representing table or view in the schema.
    If `check_exists` is true and table does not exist, an error will be thrown.
    Otherwise, if table does not exists, the returned object will refer
    to non-existent table.

    @note Checking existence of the table involves
    communication with the server. If `check_exists` is false,
    on the other hand, no I/O is involved when creating a
    `Table` instance.
  */

  Table getTable(const string&, bool check_existence = false);

  /**
    Return list of `Collection` object representing collections in
    the schema.
  */

  CollectionList getCollections();

  /**
    Return list of names of collections in the schema.
  */

  StringList getCollectionNames();

  /**
    Return list of `Table` object representing tables and views in
    the schema.
  */

  TableList getTables();

  /**
    Return list of tables and views in the schema.
  */

  StringList getTableNames();

  /**
    Return Collection as a Table object.

    @note Checking existence of the collection involves
    communication with the server. If `check_exists` is false,
    on the other hand, no I/O is involved when creating a
    `Table` instance.
  */
  Table getCollectionAsTable(const string&, bool check_exists = true);

  /**
    Create new View in the schema.

    If replace is true, the view should exist, otherwise Error is thrown.
  */
  ViewCreate createView(const mysqlx::string& view_name, bool replace = false)
  {
    return ViewCreate(*this, view_name, replace);
  }

  /**
    Change created View in the schema.
  */
  ViewAlter alterView(const mysqlx::string& view_name)
  {
    return ViewAlter(*this, view_name);
  }


  /**
    Drop given collection from the schema.

    This method will silently succeed if given collection does not exist.

    @note If table name is passed to the method, it will behave like
    dropTable().
  */

  void dropCollection(const mysqlx::string& name);

  /**
    Drop given table from the schema.

    This method will silently succeed if given table does not exist. If given
    table is a view (isView() returns true) then it will not be dropped (and no
    error is reported) - use dropView() instead.

    @note If collection name is passed to the method, it will behave like
    dropCollection().
  */

  void dropTable(const mysqlx::string& name);

  /**
    Drop given view from the schema.

    This method will silently succeed if given view does not exist. This is
    also the case when a name of non-view object, such as table or collection
    was given (as a view with the given name does not exist).
  */

  void dropView(const mysqlx::string& name);


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

  @ingroup devapi

  @todo `update()` method.
  @todo Support for parameterized collection operations.
  @todo Sorting and limiting returned results.
*/

class PUBLIC_API Collection
  : public internal::Db_object
{
  Schema m_schema;

public:

  Collection(const Schema &sch, const string &name)
    : internal::Db_object(*sch.m_sess, name)
    , m_schema(sch)
  {}


  /**
    Get schema of the collection.
  */

  const Schema& getSchema() const override { return m_schema; }

  /**
    Check if this collection existis in the database.

    @note This check involves communication with the server.
  */

  bool existsInDatabase() const override;


  /**
    Get number of documents in the collection.
  */
  uint64_t count();

  /*
    CRUD operations on a collection
    -------------------------------
  */

  /**
    Return operation which fetches all the documents in the collection.
  */

  CollectionFind find()
  {
    try {
      return CollectionFind(*this);
    }
    CATCH_AND_WRAP;
  }

  /**
    Return operation which finds documents that satisfy given expression.
  */

  CollectionFind find(const string &cond)
  {
    try {
      return CollectionFind(*this, cond);
    }
    CATCH_AND_WRAP;
  }

  /**
    Return operation which adds documents to the collection. Documents can be
    specified as JSON strings or `DbDoc` instances. One or more documents can
    be specified in single `add()` call. Further documents  can be
    added with more `add()` calls. Method `add()` also accepts a container with
    documents or a range of documents given by two iterators.
  */

  template <typename... Types>
  CollectionAdd add(Types... args)
  {
    try {

      CollectionAdd add(*this);
      return add.add(args...);
    }
    CATCH_AND_WRAP;
  }

  /**
    Return operation which removes documents satisfying given expression.
  */

  CollectionRemove remove(const string &cond)
  {
    try {
      return CollectionRemove(*this, cond);
    }
    CATCH_AND_WRAP;
  }

  /**
    Return operation which modifies documents that satisfy given expression.
  */

  CollectionModify modify(const string &expr)
  {
    try {
      return CollectionModify(*this, expr);
    }
    CATCH_AND_WRAP;
  }

  /**
    Returns Document with the giver id.
    Returns empty document if not found.
  */

  DbDoc getOne(const string &id)
  {
    return find("_id = :id").bind("id", id).execute().fetchOne();
  }

  /**
    Removes the document with the given id.
   */

  Result removeOne(const string &id)
  {
    return remove("_id = :id").bind("id", id).execute();
  }

  /**
    Replaces the document identified by id if it exists and returns true.
    Otherwise returns false. Parameter document can be either DbDoc object,
    or JSON string, or expr(docexpr) where docexpr is like JSON
    string but field values are expressions.
    It is not possible to bind values of named parameters with .bind()
    because the statement gets executed upon calling of this function.
  */

  bool replaceOne(string id, internal::ExprValue &&document)
  {
      return
          internal::CollectionReplace(*this, id, std::move(document)).execute().getAffectedItemsCount() == 1;
  }

  /**
    Adds a new document identified by id if it does not exist and 
    returns true. Otherwise replaces the existing document
    with that id and returns false.
    Parameter document can be either DbDoc object,
    or JSON string, or expr(docexpr) where docexpr is like JSON
    string but field values are expressions.
    It is not possible to bind values of named parameters with .bind()
    because the statement is executed upon calling of this function.
  */

  bool addOrReplace(string id, internal::ExprValue &&document)
  {
      return internal::CollectionReplace(*this, id, std::move(document), true).execute().getAffectedItemsCount() == 1;
  }

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

  @ingroup devapi
  @todo Other CRUD operations on a table.
*/

class PUBLIC_API Table
    : public internal::Db_object
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

  Table(const Schema &sch, const string &name)
    : internal::Db_object(*sch.m_sess, name)
    , m_schema(sch)
  {}

  Table(const Schema &sch, const string &name, bool isView_)
    : Table(sch, name)
  {
    m_isview = isView_ ? YES : NO;
  }

  DIAGNOSTIC_POP


  bool isView();

  const Schema& getSchema() const override { return m_schema; }

  bool existsInDatabase() const override;

  /**
    Get number of rows
  */
  uint64_t count();


  /*
    CRUD operations
    ---------------
  */

  /**
    Return operation which inserts rows into the full table without
    restricting the columns.

    Each row added by the operation must have the same number of values as
    the number of columns of the table. However, this check is done only
    after sending the insert command to the server. If value count does not
    match table column count server reports error.
  */

  TableInsert insert()
  {
    try {
      return TableInsert(*this);
    }
    CATCH_AND_WRAP;
  }

  /**
    Return operation which inserts row into the table restricting the columns.

    Each row added by the operation must have the same number of values
    as the columns specified here. However, this check is done only
    after sending the insert command to the server. If value count does not
    match table column count server reports error.
  */

  template <class... T>
  TableInsert insert(const T&... t)
  {
    try {
      return TableInsert(*this, t...);
    }
    CATCH_AND_WRAP;
  }

  /**
    Select rows from table.

    Optional list of expressions defines projection with transforms
    rows found by this operation.
  */

  template<typename ...PROJ>
  TableSelect select(const PROJ&...proj)
  {
    try {
      return TableSelect(*this, proj...);
    }
    CATCH_AND_WRAP;
  }

  /**
    Return operation which removes rows from the table.
  */

  TableRemove remove()
  {
    try {
      return TableRemove(*this);
    }
    CATCH_AND_WRAP;
  }

  /**
    Return operation which updates rows in the table.
  */

  TableUpdate update()
  {
    try {
      return TableUpdate(*this);
    }
    CATCH_AND_WRAP;
  }

};


using SqlStatement = internal::SQL_statement;


DLL_WARNINGS_PUSH

/**
  Represents a session which gives access to data stored
  in the data store.

  Session can be created from connection string, given `SessionSettings` object
  or a host name, TCP/IP port, user name and password can be specified directly.
  Once created, session is ready to be used. Session destructor closes session
  and cleans up after it.

  If it is not possible to create a valid session for some reason, errors
  are thrown from session constructor.

  It is possible to specify several hosts when creating a session. In that
  case failed connection to one of the hosts will trigger fail-over attempt
  to connect to a different host in the list. Only if none of the hosts could
  be contacted, session creation will fail. It is also possible to specify
  priorities for the hosts in the list which determine the order in which
  hosts are tried (see `SessionOption::PRIORITY`).

  @ingroup devapi
*/

class PUBLIC_API Session
  : internal::nocopy
  , internal::Session_detail
{

  DLL_WARNINGS_POP

  using SchemaList = internal::List_init<Schema>;

protected:

  /*
    This constructor constructs a child session of a parent session.
  */

  INTERNAL Session(Session*);

public:


  /**
    Create session specified by `SessionSettings` object.
  */

  Session(SessionSettings settings);


  /**
    Create session using given session settings.

    This constructor forwards arguments to a `SessionSettings` constructor.
    Thus all forms of specifying session options are also directly available
    in `Session` constructor.

    Examples:
    ~~~~~~

      Session from_uri("mysqlx://user:pwd@host:port/db?ssl-mode=disabled");


      Session from_options("host", port, "user", "pwd", "db");

      Session from_option_list(
        SessionOption::USER, "user",
        SessionOption::PWD,  "pwd",
        SessionOption::HOST, "host",
        SessionOption::PORT, port,
        SessionOption::DB,   "db",
        SessionOption::SSL_MODE, SSLMode::DISABLED
      );
    ~~~~~~

    @see `SessionSettings`
  */

  template<typename...T>
  Session(T...options)
    : Session(SessionSettings(options...))
  {}


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
    Get the default schema specified when session was created.
  */

  Schema getDefaultSchema();

  /**
    Get the name of the default schema specified when session was created.
  */

  string getDefaultSchemaName();

  /**
    Get list of schema objects in a given session.
  */

  SchemaList getSchemas();

  /**
    Drop the schema.

    Errors will be thrown if schema doesn't exist,
  */

  void   dropSchema(const string &name);


  /**
    Operation that runs arbitrary SQL query.
  */

  SqlStatement sql(const string &query)
  {
    try {
      return SqlStatement(this, query);
    }
    CATCH_AND_WRAP
  }

  /**
    Start a new transaction.

    Throws error if previously opened transaction is not closed.
  */

  void startTransaction();

  /**
    Commit opened transaction, if any.

    Does nothing if no transaction was opened. After committing the
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

  /**
    Closes current session.

    After a session is closed, any call to other method will throw Error.
  */

  void close()
  {
    try {
      Session_detail::close();
    }
    CATCH_AND_WRAP
  }


public:

  struct INTERNAL Access;
  friend Access;

  friend Schema;
  friend Collection;
  friend Table;
  friend Result;
  friend RowResult;

  ///@cond IGNORE
  friend internal::Result_detail;
  ///@endcond
};


}  // mysqlx

#endif
