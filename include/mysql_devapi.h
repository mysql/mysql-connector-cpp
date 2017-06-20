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

#include "devapi/common.h"
#include "devapi/result.h"
#include "devapi/statement.h"
#include "devapi/crud.h"
#include "devapi/collection_crud.h"
#include "devapi/table_crud.h"

#include <bitset>

namespace cdk {

class Session;

}  // cdk


namespace mysqlx {

class Session;
class Schema;
class Collection;
class Table;


/**
  Represents a database object

  Inherited by Schema, Table and Collection. Can't be used alone.
*/

class PUBLIC_API DatabaseObject
{

protected:

  Session *m_sess;

  DLL_WARNINGS_PUSH
  string m_name;
  DLL_WARNINGS_POP

  DatabaseObject(Session& sess, const string& name = string())
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

  Session& getSession() { return *m_sess; }


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
  Check options for an updatable view.
  @see https://dev.mysql.com/doc/refman/en/view-check-option.html
*/

enum class CheckOption
{
  CASCADED, //!< cascaded
  LOCAL     //!< local
};

/**
  Algorithms used to process views.
  @see https://dev.mysql.com/doc/refman/en/view-algorithms.html
*/

enum class Algorithm
{
  UNDEFINED,  //!< undefined
  MERGE,      //!< merge
  TEMPTABLE   //!< temptable
};

/**
  View security settings.
  @see https://dev.mysql.com/doc/refman/en/stored-programs-security.html
*/

enum class SQLSecurity
{
  DEFINER,  //!< definer
  INVOKER   //!< invoker
};


class ViewCreate;
class ViewAlter;

namespace internal {

/*
   Create/Alter View classes
*/

struct View_impl
  : public Executable_impl
{
  virtual void add_columns(const string&) = 0;
  virtual void algorithm(Algorithm) = 0;
  virtual void security(SQLSecurity) = 0;
  virtual void definer(const string&) = 0;
  virtual void defined_as(TableSelect&&) = 0;
  virtual void with_check_option(CheckOption) = 0;
};


template <class Op>
class ViewCheckOpt
: public Executable<Result,Op>
{
protected:

  using Executable<Result, Op>::check_if_valid;
  using Executable<Result, Op>::m_impl;

  View_impl* get_impl()
  {
    check_if_valid();
    return static_cast<View_impl*>(m_impl.get());
  }

public:

  /**
    Specify checks that are done upon insertion of rows into an updatable
    view.

    @see https://dev.mysql.com/doc/refman/en/view-check-option.html
  */

  Executable<Result,Op> withCheckOption(CheckOption option)
  {
    get_impl()->with_check_option(option);
    return std::move(*this);
  }
};


template <class Op>
class ViewDefinedAs
: public ViewCheckOpt<Op>
{
protected:

  using ViewCheckOpt<Op>::get_impl;

public:

  ///@{
  // TODO: How to copy documentation here?
  ViewCheckOpt<Op> definedAs(TableSelect&& table)
  {
    get_impl()->defined_as(std::move(table));
    return std::move(*this);
  }

  /**
     Specify table select operation for which the view is created.

     @note In situations where select statement is modified after
     passing it to definedAs() method, later changes do not affect
     view definition which uses the state of the statement at the time
     of definedAs() call.
  */

  ViewCheckOpt<Op> definedAs(const TableSelect& table)
  {
    TableSelect table_tmp(table);
    get_impl()->defined_as(std::move(table_tmp));
    return std::move(*this);
  }

  ///@}
};


template <class Op>
class ViewDefiner
: public ViewDefinedAs<Op>
{
protected:

  using ViewDefinedAs<Op>::get_impl;

public:

  /**
    Specify definer of a view.

    The definer is used to determine access rights for the view. It is specified
    as a valid MySQL account name of the form "user@host".

    @see https://dev.mysql.com/doc/refman/en/stored-programs-security.html
  */
 ViewDefinedAs<Op> definer(const string &user)
 {
  get_impl()->definer(user);
  return std::move(*this);
 }
};


template <class Op>
class ViewSecurity
  : public ViewDefiner<Op>
{
protected:

  using ViewDefiner<Op>::get_impl;

public:

  /**
    Specify security characteristics of a view.

    @see https://dev.mysql.com/doc/refman/en/stored-programs-security.html
  */

  ViewDefiner<Op> security(SQLSecurity sec)
  {
    get_impl()->security(sec);
    return std::move(*this);
  }
};


template <class Op>
class ViewAlgorithm
  : public ViewSecurity<Op>
{
protected:

  using ViewSecurity<Op>::get_impl;

public:

  /**
    Specify algorithm used to process the view.

    @see https://dev.mysql.com/doc/refman/en/view-algorithms.html
  */

  ViewSecurity<Op> algorithm(Algorithm alg)
  {
    get_impl()->algorithm(alg);
    return std::move(*this);
  }

};


/*
  Base class for Create/Alter View
*/

template <class Op>
class View_base
  : public ViewAlgorithm<Op>
{
protected:

  using ViewAlgorithm<Op>::get_impl;

  void add_columns(const char *name)
  {
    get_impl()->add_columns(name);
  }

  void add_columns(const string &name)
  {
    get_impl()->add_columns(name);
  }

  template <typename C>
  void add_columns(const C& col)
  {
    for (auto el : col)
    {
      get_impl()->add_columns(el);
    }
  }

  template <typename C, typename...R>
  void add_columns(const C &name,const R&...rest)
  {
    add_columns(name);
    add_columns(rest...);
  }

public:

  /**
    Define the column names of the created/altered View.
  */

  template<typename...T>
  ViewAlgorithm<Op> columns(const T&...names)
  {
    add_columns(names...);
    return std::move(*this);
  }

  /// @cond IGNORED
  friend Schema;
  friend ViewCreate;
  friend ViewAlter;
  /// @endcond

};


} // namespace internal

/**
  Represents an operation which creates a view.

  The query for which the view is created must be specified with
  `definedAs()` method. Other methods can specify different view creation
  options. When operation is fully specified, it can be executed with
  a call to `execute()`.
*/

class PUBLIC_API ViewCreate
  : public internal::View_base<ViewCreate>
{

  ViewCreate(Schema &sch, const string& name, bool replace);

  /// @cond IGNORED
  friend Schema;
  /// @endcond

};


/**
  Represents an operation which modifies an existing view.

  ViewAlter operation must specify new query for the view with
  `definedAs()` method (it is not possible to change other characteristics
  of a view without changing its query).
*/

class PUBLIC_API ViewAlter
  : public internal::View_base<ViewAlter>
{

  ViewAlter(Schema &sch, const string& name);

  /// @cond IGNORED
  friend Schema;
  /// @endcond

};


namespace internal {


  struct ViewDrop_impl
  : public Executable_impl
{
  virtual void if_exists() = 0;
};


template <class Op>
class ViewDropIfExists
    : public Executable<Result,Op>
{
protected:

  using Executable<Result, Op>::check_if_valid;
  using Executable<Result, Op>::m_impl;

  ViewDrop_impl* get_impl()
  {
    check_if_valid();
    return static_cast<ViewDrop_impl*>(m_impl.get());
  }

public:

  /**
    Modify drop view operation so that it checks existence of the view
    before dropping it.
  */

  Executable<Result,Op> ifExists()
  {
    get_impl()->if_exists();
    return std::move(*this);
  }
};

} // namespace internal


/**
  Represents an operation which drops a view.
*/

class PUBLIC_API ViewDrop
  : public internal::ViewDropIfExists<ViewDrop>
{

  ViewDrop(Schema &sch, const string& name);

  /// @cond IGNORED
  friend Schema;
  /// @endcond

};


/**
  Represents a database schema.

  A `Schema` instance  can be obtained from `Session::getSchema()`
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
  : public DatabaseObject
{

public:

  /**
     Construct named schema object.
  */

  Schema(Session &sess, const string &name)
    : DatabaseObject(sess, name)
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
    collection does not exists, the returned object will refer
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

  internal::List_init<Collection> getCollections();

  /**
    Return list of names of collections in the schema.
  */

  internal::List_init<string> getCollectionNames();

  /**
    Return list of `Table` object representing tables and views in
    the schema.
  */

  internal::List_init<Table> getTables();

  /**
    Return list of tables and views in the schema.
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


  ViewDrop dropView(const mysqlx::string& view_name)
  {
    return ViewDrop(*this, view_name);
  }


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


  const Schema& getSchema() const override { return m_schema; }

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

  @ingroup devapi
  @todo Other CRUD operations on a table.
*/

class PUBLIC_API Table
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


  const Schema& getSchema() const override { return m_schema; }

  bool existsInDatabase() const override;

  /**
    Get number of rows
  */
  uint64_t count();

};


/**
  Represents session options to be passed at Session object
  creation.

  SessionSettings can be constructed using URL string, common connect options
  (host, port, user, password, database) or with a list
  of `SessionSettings::Options` constants followed by option value.

  Examples:
  ~~~~~~

    SessionSettings from_url("mysqlx://user:pwd@host:port/db?ssl-mode=required");

    SessionSettings from_options("host", port, "user", "pwd", "db");

    SessionSettings from_option_list(
      SessionSettings::USER, "user",
      SessionSettings::PWD,  "pwd",
      SessionSettings::HOST, "host",
      SessionSettings::PORT, port,
      SessionSettings::DB,   "db",
      SessionSettings::SSL_MODE, SessionSettings::SSLMode::REQUIRED
    );
  ~~~~~~

  Some settings, such as @ref HOST, can be specified several times to build
  a list of hosts to be used by connection fail-over logic.

  @ingroup devapi
*/

class PUBLIC_API SessionSettings
{
public:

#ifndef _WIN32
#define ADD_SOCKET(x) x(SOCKET)        /*!< path to unix domain socket*/
#else
#define ADD_SOCKET(x)
#endif //_WIN32

#define SETTINGS_OPTIONS(x)                                                      \
  x(URI)          /*!< connection URI or string */                               \
  /*! DNS name of the host, IPv4 address or IPv6 address */                      \
  x(HOST)                                                                        \
  x(PORT)          /*!< X Plugin port to connect to */                           \
  x(PRIORITY)      /*!< define priority on a multiple host connection */         \
  x(USER)          /*!< user name */                                             \
  x(PWD)           /*!< password */                                              \
  x(DB)            /*!< default database */                                      \
  x(SSL_MODE)      /*!< define `SSLMode` option to be used */                    \
  x(SSL_CA)        /*!< path to a PEM file specifying trusted root certificates*/\
  ADD_SOCKET(x)


  #define OPTIONS_ENUM(x) x,

  /**
    Session creation options

    @note `PRIORITY` should be defined after a HOST (PORT) definition

    @note Specifying `SSL_CA` option requires `SSL_MODE` value of `VERIFY_CA`
    or `VERIFY_IDENTITY`. If `SSL_MODE` is not explicitly given then
    setting `SSL_CA` implies `VERIFY_CA`.
  */

  enum Options
  {
    SETTINGS_OPTIONS(OPTIONS_ENUM)
    LAST
  };


#define SSL_MODE_TYPES(x)\
  x(DISABLED)        /*!< Establish an unencrypted connection.  */ \
  x(REQUIRED)        /*!< Establish a secure connection if the server supports
                          secure connections. The connection attempt fails if a
                          secure connection cannot be established. This is the
                          default if @ref SSL_MODE is not specified. */ \
  x(VERIFY_CA)       /*!< Like `REQUIRED`, but additionally verify the server
                          TLS certificate against the configured Certificate
                          Authority (CA) certificates (defined by @ref SSL_CA
                          Option). The connection attempt fails if no valid
                          matching CA certificates are found.*/ \
  x(VERIFY_IDENTITY) /*!< Like `VERIFY_CA`, but additionally verify that the
                          server certificate matches the host to which the
                          connection is attempted.*/\

#define SSL_ENUM(x) x,

  /**
     Modes to be used by @ref SSL_MODE option
   */
  enum class SSLMode
  {
    SSL_MODE_TYPES(SSL_ENUM)
  };

  SessionSettings(){}

  SessionSettings(SessionSettings &settings)
    : m_options(settings.m_options)
    , m_option_used(settings.m_option_used)
  {}

  SessionSettings(SessionSettings &&settings)
    : m_options(std::move(settings.m_options))
    , m_option_used(std::move(settings.m_option_used))
  {}


  /**
    Get settings from a connection string or URI.

    Connection sting has the form `"user:pass\@host:port/?option&option"`,
    valid URI is like a connection string with a `mysqlx://` prefix. Host is
    specified as either DNS name, IPv4 address of the form "nn.nn.nn.nn" or
    IPv6 address of the form "[nn:nn:nn:...]".

    Possible connection options are:

    - `ssl-mode` : define @ref SSLMode option to be used
    - `ssl-ca=`path : path to a PEM file specifying trusted root certificates
  */

  SessionSettings(const string &uri)
  {
    do_set(true, URI, uri);
  }


  /**
    Explicitly specify basic connection settings.

    @note Session settings constructed this way request an SSL connection
    by default.
  */

  SessionSettings(const std::string &host, unsigned port,
                  const string  &user,
                  const char *pwd = NULL,
                  const string &db = string())
  {
    do_set(true, HOST, host,
        PORT, port,
        USER, user,
        DB, db);

    if (pwd)
      do_set(true, PWD, pwd);

  }

  SessionSettings(const std::string &host, unsigned port,
                  const string  &user,
                  const std::string &pwd,
                  const string &db = string())
    : SessionSettings(host, port, user, pwd.c_str(), db)
  {}

  /**
    Basic settings with the default port

    @note Session settings constructed this way request an SSL connection
    by default.
  */

  SessionSettings(const std::string &host,
                  const string  &user,
                  const char    *pwd = NULL,
                  const string  &db = string())
    : SessionSettings(host, DEFAULT_MYSQLX_PORT, user, pwd, db)
  {}

  SessionSettings(const std::string &host,
                  const string  &user,
                  const std::string &pwd,
                  const string  &db = string())
    : SessionSettings(host, DEFAULT_MYSQLX_PORT, user, pwd, db)
  {}

  /**
    Basic settings for a session on the localhost.

    @note Session settings constructed this way request an SSL connection
    by default.
  */

  SessionSettings(unsigned port,
                  const string  &user,
                  const char    *pwd = NULL,
                  const string  &db = string())
    : SessionSettings("localhost", port, user, pwd, db)
  {}

  SessionSettings(unsigned port,
                  const string  &user,
                  const std::string &pwd,
                  const string  &db = string())
    : SessionSettings("localhost", port, user, pwd.c_str(), db)
  {}

  /*
    Templates below are here to take care of the optional password
    parameter of type const char* (which can be either 3-rd or 4-th in
    the parameter list). Without these templates passing
    NULL as password does not work, because NULL is defined as 0
    which has type int.
  */

  template <
    typename    HOST,
    typename    PORT,
    typename    USER,
    typename... T,
    typename std::enable_if<
      std::is_constructible<SessionSettings, HOST, PORT, USER, const char*, T...>::value
    >::type* = nullptr
  >
  SessionSettings(HOST h, PORT p, USER u ,long , T... args)
    : SessionSettings(h, p, u, nullptr, args...)
  {}


  template <
    typename    PORT,
    typename    USER,
    typename... T,
    typename std::enable_if<
      std::is_constructible<SessionSettings, PORT, USER, const char*, T...>::value
    >::type* = nullptr
  >
  SessionSettings(PORT p, USER u ,long , T... args)
    : SessionSettings(p, u, nullptr, args...)
  {}


  /**
    Specify settings as a list of session options.

    The list of options consist of `SessionSettings::Options` constant
    identifying the option to set, followed by option value.

    Example:
    ~~~~~~
      SessionSettings from_option_list(
        SessionSettings::USER, "user",
        SessionSettings::PWD,  "pwd",
        SessionSettings::HOST, "host",
        SessionSettings::PORT, port,
        SessionSettings::DB,   "db",
        SessionSettings::SSL_MODE, SessionSettings::SSLMode::REQUIRED
      );
    ~~~~~~

    @see `SessionSettings::Options`.
  */

  template <typename V,typename...R>
  SessionSettings(Options opt, V val, R...rest)
  {
    do_set(true, opt, val, rest...);
  }


  /*
     SessionSetting operator and methods
   */

  typedef std::vector<std::pair<Options,Value>>::iterator iterator;

  /**
     Returns an iterator pointing to the first element of the SessionSettings.
   */
  iterator begin()
  {
    return m_options.begin();
  }

  /**
     Returns an iterator pointing to the last element of the SessionSettings.
   */
  iterator end()
  {
    return m_options.end();
  }


  /**
    Finds element of specified @p opt and returns its Value.
    Will throw Error if not found.
  */
  Value& find(Options opt);


  /**
    Set list of @ref Options to given values.

    When using @ref HOST, @ref PORT and @ref PRIORITY, all have to be defined
    in the same set() call.
   */

  template<typename V,typename...R>
  void set(Options opt, V v, R...rest)
  {
    m_call_used.reset();
    do_set(false, opt, v,rest...);
  }


  /**
    Clears all settings specified so far.
  */

  void clear()
  {
    m_options.clear();
    m_option_used.reset();
  }

  /**
    Remove all settings for the given option @p opt.

    @note For option such as @ref HOST, which can repeat several times in
    the settings, all occurences are erased.
  */

  void erase(Options opt)
  {
    auto it = m_options.begin();

    while(it != m_options.end())
    {
      if(it->first == opt)
      {
        it = m_options.erase(it);
      }
      else
      {
        ++it;
      }
    }
    m_option_used.reset(opt);
  }


  /**
    Check if option @p opt was defined.
  */

  bool has_option(Options opt)
  {
    return m_option_used.test(opt);
  }


private:

  std::vector<std::pair<Options,Value>> m_options;
  std::bitset<Options::LAST> m_option_used;
  std::bitset<Options::LAST> m_call_used;

  static std::string get_option_name(Options opt);

  void do_add(Options opt, Value &&v);


  /*
    Store option value in Value object (with basic run-time type checks)
    TODO: More precise type checking using per-option types.
  */

  static Value opt_val(Options opt, Value &&val)
  {
    if (opt == SSL_MODE)
      throw Error("SSL_MODE setting requires SessionSettings::SSLMode value.");
    return val;
  }

  /*
    For types which are not convertible to Value, but can be converted to string
    go through string conversion.
  */

  template <
    typename V,
    typename std::enable_if<std::is_convertible<V,string>::value>::type*
    = nullptr
  >
  static Value opt_val(Options opt, V &&val)
  {
    if (opt == SSL_MODE)
      throw Error("SSL_MODE setting requires SessionSettings::SSLMode value.");
    return string(val);
  }

  static Value opt_val(Options opt, SSLMode m)
  {
    if (opt != SSL_MODE)
      throw Error("SessionSettings::SSLMode value can only be used on SSL_MODE setting.");
    return unsigned(m);
  }

  void do_set(bool) {}

  template <typename V, typename...R>
  void do_set(bool host_optional, Options opt, V v, R...rest)
  {
    switch (opt)
    {
    case HOST:
      return do_add_host(opt_val(HOST, v), rest...);

    case PORT:
      if (host_optional)
        return do_add_host("localhost", opt_val(PORT, v), rest...);
      else
        throw Error("Defining PORT without first defining HOST.");

    case PRIORITY:
      if (host_optional)
      {
        do_add_host(
          "localhost", opt_val(PORT, DEFAULT_MYSQLX_PORT), opt_val(PRIORITY, v)
        );
        do_set(false, rest...);
        return;
      }
      else
        throw Error("Defining PRIORITY without first defining HOST.");

    default:

      if (m_call_used.test(opt))
      {
        std::stringstream error;
        error << "SessionSettings option "
              << get_option_name(opt) << " defined twice";

        throw Error(error.str().c_str());
      }

      m_call_used.set(opt);

      do_add(opt, opt_val(opt,v));
      do_set(host_optional, rest...);
    }
  }


  /*
    Add HOST setting checking valid order of options after it
    (PORT/PRIORITY).
  */

  void do_add_host(Value &&host)
  {
    do_add(Options::HOST, std::move(host));
  }

  void do_add_host(Value &&host, Value &&port)
  {
    do_add(Options::HOST, std::move(host));
    do_add(Options::PORT, std::move(port));
  }

  void do_add_host(Value &&host, Value &&port, Value &&priority)
  {
    do_add(Options::HOST, std::move(host));
    do_add(Options::PORT, std::move(port));
    do_add(Options::PRIORITY, std::move(priority));
  }

  template <typename V, typename...R>
  void do_add_host(Value &&host, Options opt, V v, R...rest)
  {
    if (opt == Options::PORT)
    {
      //we could still have priority
      do_add_host(std::move(host), opt_val(PORT,v), rest...);
      return;
    }
    else if (opt == Options::PRIORITY)
    {
      do_add_host(std::move(host), DEFAULT_MYSQLX_PORT, opt_val(PRIORITY,v));
      do_set(false, rest...);
      return;
    }

    do_add_host(std::move(host));
    do_set(false, opt, v, rest...);
  }

  template <typename V, typename...R>
  void do_add_host(Value &&host, Value &&port, Options opt, V v, R...rest)
  {
    if (opt == Options::PRIORITY)
    {
      do_add_host(std::move(host), std::move(port), opt_val(PRIORITY, v));
      do_set(false, rest...);
      return;
    }
    do_add_host(std::move(host), std::move(port));
    do_set(false, opt, v, rest...);
  }


};



  DLL_WARNINGS_PUSH

/**
    Represents a session which gives access to data stored
    in the data store.

    When creating new session a host name, TCP/IP port,
    user name and password are specified. Once created,
    session is ready to be used. Session destructor closes
    session and cleans up after it.

    If it is not possible to create a valid session for some
    reason, errors are thrown from session constructor.

    It is possible to specify several hosts when creating a session. In that
    case failed connection to one of the hosts will trigger fail-over attempt
    to connect to a different host in the list. Only if none of the hosts could
    be contacted, session creation will fail. It is also possible to specify
    priorities for the hosts in the list which determine the order in which
    hosts are tried (see @ref SessionSettings::PRIORITY).

    @ingroup devapi
    @todo Add all `Session` methods defined by DevAPI.
  */

class PUBLIC_API Session : internal::nocopy
{

  DLL_WARNINGS_POP

protected:

  class INTERNAL Impl;
  Impl  *m_impl;
  bool m_master_session = true;
  SqlStatement m_stmt;

  INTERNAL void register_result(internal::BaseResult *result);
  INTERNAL void deregister_result(internal::BaseResult *result);

  INTERNAL cdk::Session& get_cdk_session();

  struct Options;

  /*
      This constructor constructs a child session of a parent session.
      */
  INTERNAL Session(Session*);

  /*
        This notification is sent from parent session when it is closed.
      */
  void session_closed() { if (!m_master_session) m_impl = NULL; }

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

          Session from_uri("mysqlx://user:pwd@host:port/db?ssl-enable");

          Session from_options("host", port, "user", "pwd", "db");

          Session from_option_list(
            SessionSettings::USER, "user",
            SessionSettings::PWD,  "pwd",
            SessionSettings::HOST, "host",
            SessionSettings::PORT, port,
            SessionSettings::DB,   "db",
            SessionSettings::SSL_ENABLE
          );
        ~~~~~~

        @see `SessionSettings`
      */

  template<typename...T>
  Session(T...options)
    : Session(SessionSettings(options...))
  {}


  virtual ~Session();

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
        Operation that runs arbitrary SQL query.
      */

  SqlStatement& sql(const string &query);

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

  void close();


public:

  struct INTERNAL Access;
  friend Access;

  friend Schema;
  friend Collection;
  friend Table;
  friend Result;
  friend RowResult;

  ///@cond IGNORE
  friend internal::BaseResult;
  ///@endcond
};


}  // mysqlx

#endif
