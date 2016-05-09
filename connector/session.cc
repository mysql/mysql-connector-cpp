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

#include <mysqlx.h>
#include <mysql/cdk.h>

#include <iostream>
#include <sstream>

#include "impl.h"

using namespace ::mysqlx;

class XSession::Impl
{
  cdk::ds::TCPIP   m_ds;
  std::string      m_pwd;
  cdk::ds::Options m_opt;
  cdk::Session     m_sess;

  Impl(const char *host, unsigned short port,
       const string &user, const char *pwd =NULL)
    : m_ds(host, port)
    , m_pwd(pwd ? pwd : "")
    , m_opt(user, pwd ? &m_pwd : NULL)
    , m_sess(m_ds, m_opt)
  {
    if (!m_sess.is_valid())
      m_sess.get_error().rethrow();
  }

  friend class XSession;
};


XSession::XSession(const char *host, unsigned short port,
                   const string  &user,
                   const char    *pwd)
try {
  m_impl= new Impl(host, port, user, pwd);
}
CATCH_AND_WRAP


XSession::~XSession()
try {
  delete m_impl;
}
CATCH_AND_WRAP

cdk::Session& XSession::get_cdk_session()
{
  return m_impl->m_sess;
}


// ---------------------------------------------------------------------


/*
  Class which acts as query parameter source, passing two parameters:
  1. schema name
  2. schema object name
*/

class Args
    : public cdk::Any_list
{
  typedef cdk::Any_list Any_list;

  std::vector<string> m_args;

  void add_arg(const string& arg)
  {
    m_args.push_back(arg);
  }

  template <typename... A>
  void add_arg(const string& arg, const A&... rest)
  {
    add_arg(arg);
    add_arg(rest...);
  }

public:

  template <typename... A>
  Args(A... args)
  {
    add_arg(args...);
  }

  void process(Any_list::Processor &lp) const
  {
    cdk::Safe_prc<Any_list::Processor> sp(lp);
    sp->list_begin();
    // NOTE: uses utf8
    for (auto arg : m_args)
    {
      sp->list_el()->scalar()->str(arg);
    }
    sp->list_end();
  }
};

class Create_args
  : public Args
{

public:

  Create_args(const string &schema, const string &name)
    : Args(schema, name)
  {}

};


// ---------------------------------------------------------------------


/*
  Code to check existence of data store objects.

  Checks are done by querying INFORMATION_SCHEMA database. Sending
  appropriate query is implemented by check_query<T> class which derives
  from cdk::Reply (T is the type of object to check).
*/

enum obj_type { TABLE, SCHEMA };

template <obj_type> struct check_query;

template <>
struct check_query<SCHEMA>
  : public Args
  , public cdk::Reply
{

  typedef const string& Args_t;

  check_query(cdk::Session &sess, Args_t name)
    : Args(name)
    , cdk::Reply(sess.sql(
        L"SELECT 1 FROM INFORMATION_SCHEMA.SCHEMATA"
        L" WHERE SCHEMA_NAME LIKE ?", this))
  {
    wait();
    if (entry_count() > 0)
    {
      // TODO: Better error
      THROW("Could not check existence of a schema (query failed)");
    }
  }

};

template <>
struct check_query<TABLE>
  : public cdk::Reply
{
  typedef Create_args& Args_t;

  check_query(cdk::Session &sess, Args_t args)
    : cdk::Reply(sess.sql(
        L"SELECT 1 FROM INFORMATION_SCHEMA.TABLES"
        L" WHERE TABLE_SCHEMA LIKE ?"
        L"   AND TABLE_NAME LIKE ?", &args))
  {
    wait();
    if (entry_count() > 0)
    {
      // TODO: Better error
      THROW("Could not check existence of a table (query failed)");
    }
  }
};


/*
  Function which checks existence of an object of type T in the data store.

  The arguments which describe object to check are different for different
  types of objects, as defined by type check_query<T>::Args_t.
*/

template <obj_type T>
bool check_existence(cdk::Session &sess, typename check_query<T>::Args_t args)
{
  check_query<T> check(sess, args);

  // Row procesor which checks if reply has at least one row.

  struct : public cdk::Row_processor
  {
    bool m_has_row;

    bool row_begin(row_count_t)
    {
      m_has_row = true;
      return false;
    }

    void row_end(row_count_t) {}
    size_t field_begin(col_count_t, size_t) { return 0; }
    void field_end(col_count_t) {}
    void field_null(col_count_t) {}
    size_t field_data(col_count_t, bytes) { return 0; }
    void end_of_data() {}
  }
  rp;
  rp.m_has_row = false;

  cdk::Cursor c(check);
  c.get_row(rp);
  c.wait();

  return rp.m_has_row;
}

/*
   Function gets list of Schemas / Tables / Collections

   The argument depends on
 */

enum list_type { SCHEMAS, TABLES, COLLECTIONS };

template <list_type> struct list_query;

template<>
struct list_query<list_type::SCHEMAS>
    : cdk::Reply
{

  typedef string Elem_t;

  list_query(cdk::Session &sess)
    : cdk::Reply(sess.sql(L"SHOW SCHEMAS"))
  {
    wait();
  }

  //if returns false, skip current row
  static bool add_data(string& elem, size_t col, string&& data)
  {
    if (0 == col)
      elem = std::move(data);
    return true;
  }
};


template<>
struct list_query<list_type::COLLECTIONS>
    : Args
    , public cdk::Reply
{

  typedef string Elem_t;

  list_query(cdk::Session &sess, const string& schema)
    : Args(schema)
    , cdk::Reply(sess.admin("list_objects", *this))
  {
    wait();
  }

  //if returns false, skip current row
  static bool add_data(string& elem, size_t col, string&& data)
  {
    if (0 == col)
      elem = std::move(data);
    if (1 == col)
      return data.compare(L"COLLECTION") == 0;
    return true;
  }

};


template<>
struct list_query<list_type::TABLES>
    : Args
    , public cdk::Reply
{

  typedef std::pair<string,bool> Elem_t;

  list_query(cdk::Session &sess, const string& schema, const string& table = string())
    : Args(schema, table)
    , cdk::Reply(sess.admin("list_objects", *this))
  {
    wait();
  }

  // if returns false, skip current row
  static bool add_data(std::pair<string,bool>& elem, size_t col, string&& data)
  {
    switch (col)
    {
      // col 0 = name
      case 0:
        elem.first = std::move(data);
        break;
      // col 1 = type (view/table)
      case 1:
        elem.second = (data == (L"VIEW"));
        return (data.compare(L"TABLE") == 0) ||
               (data.compare(L"VIEW") == 0);
        break;
    }
    return true;
  }

};







template<list_type T>
struct List_process : public cdk::Row_processor
{
  cdk::Cursor& m_cursor;
  std::list<typename list_query<T>::Elem_t> &m_list;
  typename list_query<T>::Elem_t elem;
  bool m_add_line = true;

  List_process(cdk::Cursor& cursor,
               std::list<typename list_query<T>::Elem_t>& list)
    : m_cursor(cursor)
    , m_list(list)
  {}

  bool row_begin(row_count_t)
  {
    return true;
  }

  void row_end(row_count_t)
  {
    if (m_add_line)
      m_list.push_back(elem);
    m_add_line = true;
  }
  size_t field_begin(col_count_t, size_t s) { return m_add_line ? s : 0; }
  void field_end(col_count_t) {}
  void field_null(col_count_t) {}
  size_t field_data(col_count_t col, bytes b)
  {
    cdk::Codec<cdk::TYPE_STRING> codec(m_cursor.format(col));

    cdk::string data;
    codec.from_bytes(b, data);

    if (m_add_line)
      m_add_line = list_query<T>::add_data(elem, col, std::move(data));

    return b.size();
  }
  void end_of_data() {}
};

template <list_type T>
std::list<typename list_query<T>::Elem_t> get_list(list_query<T>&& qry)
{
  cdk::Cursor c(qry);

  std::list<typename list_query<T>::Elem_t> list;

  List_process<T> prc(c, list);
  c.get_rows(prc);

  c.wait();

  return list;

}


// ---------------------------------------------------------------------


Schema XSession::createSchema(const string &name, bool reuse)
try {
  std::stringstream query;
  query << "Create Schema `" << name << "`";
  cdk::Reply r(get_cdk_session().sql(query.str()));

  r.wait();

  if (0 < r.entry_count())
  {
    const cdk::Error &err= r.get_error();
    // 1007 = schema already exists
    if (!reuse || cdk::server_error(1007) != err.code())
      err.rethrow();
  }
  return Schema(*this, name);
}
CATCH_AND_WRAP

void XSession::dropSchema(const string &name)
try{
  std::stringstream query;
  query << "Drop Schema `" << name << "`";
  cdk::Reply r(get_cdk_session().sql(query.str()));

  r.wait();

  if (0 < r.entry_count())
  {
    const cdk::Error &err= r.get_error();
    err.rethrow();
  }
}
CATCH_AND_WRAP

void XSession::dropTable(const mysqlx::string& schema, const string& table)
try{
  std::stringstream query;
  query << "Drop Table `" << schema << "`.`" << table << "`";
  cdk::Reply r(get_cdk_session().sql(query.str()));

  r.wait();

  if (0 < r.entry_count())
  {
    const cdk::Error &err= r.get_error();
    err.rethrow();
  }
}
CATCH_AND_WRAP

void XSession::dropCollection(const mysqlx::string& schema,
                              const mysqlx::string& collection)
try{
  Create_args args(schema, collection);
  cdk::Reply r(get_cdk_session().admin("drop_collection", args));

  r.wait();

  if (0 < r.entry_count())
  {
    const cdk::Error &err= r.get_error();
    err.rethrow();
  }
}
CATCH_AND_WRAP

Schema XSession::getSchema(const string &name, bool check)
try {

  if (check)
  {
    if (!check_existence<SCHEMA>(get_cdk_session(), name))
      // TODO: Better error (schema name)
      throw Error("No such schema");
  }

  return Schema(*this, name);
}
CATCH_AND_WRAP

std::list<Schema> XSession::getSchemas()
try {


  auto schemas_names = get_list(list_query<SCHEMAS>(get_cdk_session()));

  std::list<Schema> schemas_list;

  for (auto el : schemas_names)
  {
    schemas_list.push_back(Schema(*this, el));
  }

  return schemas_list;
}
CATCH_AND_WRAP





/*
  Schema.createCollection()
  =========================
*/


Collection Schema::getCollection(const string &name, bool check)
try {

  if (check)
  {
    Create_args args(m_name, name);
    if (!check_existence<TABLE>(m_sess.get_cdk_session(), args))
      // TODO: Better error (collection name)
      throw Error("No such collection");
  }
  return Collection(*this, name);
}
CATCH_AND_WRAP

Collection Schema::createCollection(const string &name, bool reuse)
try {
  Create_args args(m_name, name);
  cdk::Reply r(m_sess.get_cdk_session().admin("create_collection", args));
  r.wait();
  if (0 < r.entry_count())
  {
    const cdk::Error &err= r.get_error();
    // 1050 = table already exists
    if (!reuse || cdk::server_error(1050) != err.code())
      err.rethrow();
  }
  return Collection(*this, name);
}
CATCH_AND_WRAP


Table Schema::getTable(const string &name, bool check)
try {
  if (check)
  {
    auto tb_list = get_list(list_query<TABLES>(m_sess.get_cdk_session(),
                                               m_name,
                                               name ) );

    if (tb_list.empty())
      // TODO: Better error (collection name)
      throw Error("No such table");

    auto el = tb_list.begin();
    return Table(*this, el->first, el->second );
  }
  return Table(*this, name);
}
CATCH_AND_WRAP


std::list<Collection> Schema::getCollections()
try{
  std::list<Collection> list;

  std::list<string> list_name = getCollectionNames();

  for (auto name : list_name)
  {
    list.push_back(Collection(*this, name));
  }

  return list;
}
CATCH_AND_WRAP

std::list<string> Schema::getCollectionNames()
try{
  return get_list(list_query<list_type::COLLECTIONS>(
                    m_sess.get_cdk_session()
                    , m_name)
                  );
}
CATCH_AND_WRAP

std::list<Table> Schema::getTables()
{
  std::list<Table> list;

  auto tables_list = get_list(list_query<list_type::TABLES>(
                                      m_sess.get_cdk_session()
                                      , m_name)
                                    );

  for (auto& prop : tables_list)
  {
    list.push_back(Table(*this, prop.first, prop.second));
  }

  return list;
}

std::list<string> Schema::getTableNames()
{
  std::list<string> list;
  auto tables_list = get_list(list_query<list_type::TABLES>(
                                      m_sess.get_cdk_session()
                                      , m_name)
                                    );

  for (auto& el : tables_list)
  {
    list.push_back(std::move(el.first));
  }

  return list;

}

/*
   Table::isView
   */

  bool Table::isView()
  {
    if (UNDEFINED == m_isview)
    {
      m_isview = m_schema.getTable(m_name, true).isView() ? YES : NO;
    }

    return m_isview == YES ? true : false;
  }

/*
  Executing SQL queries
  =====================
*/


struct Op_sql : public Task::Access::Impl
{
  Op_sql(XSession &sess, const string &query)
    : Impl(sess)
  {
    m_reply = new cdk::Reply(get_cdk_session().sql(query));
  }

  cdk::Reply* send_command()
  {
    return m_reply;
  }
};


Executable& NodeSession::sql(const string &query)
try {
  Task::Access::reset(m_task, new Op_sql(*this, query));
  return *this;
}
CATCH_AND_WRAP


// ---------------------------------------------------------------------


string::string(const std::string &other)
  : std::wstring(cdk::string(other))
{}

string::string(const char *other)
  : std::wstring(cdk::string(other))
{}

string::operator std::string() const
{
  return std::string(cdk::string(*this));
}

/*
string::operator const cdk::foundation::string&() const
{
  return cdk::string(*static_cast<const std::wstring*>(this));
}
*/

ostream& operator<<(ostream &out, const Error&)
{
  out <<"MYSQLX Error!";
  return out;
}


// ---------------------------------------------------------------------


// Implementation of Task API using internal implementation object

Task::~Task() try { delete m_impl; } CATCH_AND_WRAP

bool Task::is_completed()
try { return m_impl ? m_impl->is_completed() : true; } CATCH_AND_WRAP

internal::BaseResult Task::wait()
try {
  if (!m_impl)
    throw Error("Attempt to wait on empty task");
  return m_impl->wait();
} CATCH_AND_WRAP

void Task::cont()
try {
  if (!m_impl)
    throw Error("Attempt to continue an empty task");
  m_impl->cont();
} CATCH_AND_WRAP

void Task::reset(Impl *impl)
{
  delete m_impl;
  m_impl = impl;
}
