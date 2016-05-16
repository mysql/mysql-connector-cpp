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
  Class which acts as query parameter source, passing an arbitrary list of
  string parameters
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

  Args() = default;

  template <typename... A>
  Args(const string& arg, A... args)
  {
    add_arg(arg, args...);
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


// ---------------------------------------------------------------------


/*
  Code to fetch list of SCHEMA, TABLE and COLLECTION.

  Checks are done by querying INFORMATION_SCHEMA database, or using SQL or admin
  commands. Sending appropriate query is implemented by list_query<T> class
  which derives from cdk::Reply (T is the type of object to check).
  Each list_query object implements a static method add_data() which receives
  data from cdk::Reply processor
*/

enum obj_type { TABLE, SCHEMA, COLLECTION };

template <obj_type> struct List_query;

template <typename E>
struct List_query_base
    : public cdk::Row_processor
{
  /*
     It will be called for each row with the defined data for storing it on the
     next item to be added to the list.
     If class implementing method returns false, the rest of row data is skipped
     and the element not added to the list.
   */
  virtual bool field_data(size_t col, cdk::string&&) = 0;


  template <typename I>
  List_query_base(I& init)
    : m_reply(init)
  {
    m_reply.wait();
    m_cursor.reset(new cdk::Cursor(m_reply));

    m_list_it = m_list.before_begin();
  }

  cdk::Reply m_reply;
  cdk::scoped_ptr<cdk::Cursor> m_cursor;

  bool m_skip_line = false;
  E m_elem;
  std::forward_list<E> m_list;
  typename std::forward_list<E>::iterator m_list_it;


  bool row_begin(row_count_t)
  {
    m_skip_line = false;
    return true;
  }

  void row_end(row_count_t)
  {
    if (!m_skip_line)
      m_list_it = m_list.emplace_after(m_list_it, m_elem);
  }

  size_t field_begin(col_count_t, size_t s) { return s; }

  void field_end(col_count_t) {}

  void field_null(col_count_t) {}

  size_t field_data(col_count_t col, bytes b)
  {
    // TODO: the data should be saved here and only processed on field_end()
    cdk::Codec<cdk::TYPE_STRING> codec(m_cursor->format(col));

    cdk::string data;
    codec.from_bytes(b, data);

    if (!m_skip_line)
      m_skip_line = !field_data(col, std::move(data));

    return 1024;
  }
  void end_of_data() {}

  std::forward_list<E> execute()
  {
    m_cursor->get_rows(*this);
    m_cursor->wait();
    return std::move(m_list);
  }

};


template<>
struct List_query<obj_type::SCHEMA>
    : Args
    , List_query_base<mysqlx::string>
{

  List_query(cdk::Session &sess)
    : List_query_base<mysqlx::string>(sess.sql(L"SHOW SCHEMAS"))
  {
  }

  List_query(cdk::Session &sess, const string& schema)
    : Args(schema)
    , List_query_base<mysqlx::string>(
        sess.sql(L"SHOW SCHEMAS LIKE ?", this))
  {
  }

  //if returns false, skip current row
  bool field_data(size_t col, cdk::string&& data) override
  {
    if (0 == col)
      m_elem = std::move(data);
    return true;
  }
};


/*
  COLLECTION and TABLE list_query will use list_objects admin command.

  list_objects return a list of TABLE, VIEW or COLLECTION.

  As arguments, we can pass the schema and optionally can pass a filter to the
  objects names.

  The retrieved data contains 2 columns
  - Object name
  - Type (TABLE/VIEW/COLLECTION)

  */

template<>
struct List_query<obj_type::COLLECTION>
    : Args
    , List_query_base<mysqlx::string>
{


  List_query(cdk::Session &sess, const string& schema)
    : Args(schema)
    , List_query_base<mysqlx::string>(sess.admin("list_objects", *this))
  {
  }

  List_query(cdk::Session &sess, const string& schema, const string& collection)
    : Args(schema, collection)
    , List_query_base<mysqlx::string>(sess.admin("list_objects", *this))
  {
  }

  //if returns false, skip current row
  bool field_data(size_t col, cdk::string&& data) override
  {
    switch (col)
    {
      case 0: m_elem = std::move(data); break;
      case 1: return data.compare(L"COLLECTION") == 0;
    }
    return true;
  }

};


template<>
struct List_query<obj_type::TABLE>
    : Args
    , List_query_base<std::pair<mysqlx::string,bool>>
{

  List_query(cdk::Session &sess, const string& schema, const string& table = string())
    : Args(schema, table)
    , List_query_base<std::pair<mysqlx::string,bool>>(sess.admin("list_objects", *this))
  {
  }

  // if returns false, skip current row
  bool field_data(size_t col, cdk::string&& data) override
  {
    switch (col)
    {
      // col 0 = name
      case 0:
        m_elem.first = std::move(data);
        break;
      // col 1 = type (view/table)
      case 1:
        m_elem.second = (data == (L"VIEW"));
        return (data.compare(L"TABLE") == 0) ||
               (data.compare(L"VIEW") == 0);
        break;
    }
    return true;
  }

};



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


// helper function to wait on reply and throw errors
void check_reply_skip_error_throw(cdk::Reply&& r, int skip_server_error)
{
  r.wait();

  if (0 < r.entry_count())
  {
    const cdk::Error &err= r.get_error();
    if (err.code() != cdk::server_error(skip_server_error))
      err.rethrow();
  }
}


void XSession::dropSchema(const string &name)
try{
    std::stringstream qry;
    qry << "Drop Schema `" << name << "`";
    //skip server error 1008 = schema doesn't exist
    check_reply_skip_error_throw(get_cdk_session().sql(qry.str()),
                                 1008);
}
CATCH_AND_WRAP


//TODO: better implementation: check if drop_collection can drop views also
void XSession::dropTable(const mysqlx::string& schema, const string& table)
try{
  Args args(schema, table);
  // Doesn't throw if table doesn't exit (server error 1051)
  check_reply_skip_error_throw(get_cdk_session().admin("drop_collection", args),
                               1051);
}
CATCH_AND_WRAP

void XSession::dropCollection(const mysqlx::string& schema,
                              const mysqlx::string& collection)
try{
  Args args(schema, collection);
  // Doesn't throw if collection doesn't exit (server error 1051)
  check_reply_skip_error_throw(get_cdk_session().admin("drop_collection", args),
                               1051);
}
CATCH_AND_WRAP


Schema XSession::getSchema(const string &name, bool check)
try {

  if (check)
  {
    if (List_query<SCHEMA>(get_cdk_session(), name).execute().empty())
      // TODO: Better error (schema name)
      throw Error("No such schema");
  }

  return Schema(*this, name);
}
CATCH_AND_WRAP

List_init<Schema> XSession::getSchemas()
try {


  auto schemas_names = List_query<SCHEMA>(get_cdk_session()).execute();

  std::forward_list<Schema> schemas_list;
  std::forward_list<Schema>::iterator schema_it =  schemas_list.before_begin();

  for (auto el : schemas_names)
  {
    schema_it = schemas_list.emplace_after(schema_it, Schema(*this, el));
  }

  return std::move(schemas_list);
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

    if (List_query<COLLECTION>(m_sess.get_cdk_session(),
                               m_name,
                               name )
        .execute().empty())
    {
      // TODO: Better error (collection name)
      throw Error("No such collection");
    }
  }
  return Collection(*this, name);
}
CATCH_AND_WRAP

Collection Schema::createCollection(const string &name, bool reuse)
try {
  Args args(m_name, name);
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
    auto tb_list = List_query<TABLE>(m_sess.get_cdk_session(),
                                     m_name,
                                     name ).execute();
    if (tb_list.empty())
      // TODO: Better error (collection name)
      throw Error("No such table");

    auto el = tb_list.begin();
    return Table(*this, el->first, el->second );
  }
  return Table(*this, name);
}
CATCH_AND_WRAP


List_init<Collection> Schema::getCollections()
try{
  std::forward_list<Collection> list;
  std::forward_list<Collection>::iterator list_it = list.before_begin();

  std::list<string> list_name = getCollectionNames();

  for (auto name : list_name)
  {
    list.emplace_after(list_it,Collection(*this, name));
  }

  return std::move(list);
}
CATCH_AND_WRAP

List_init<string> Schema::getCollectionNames()
try{
  return List_query<COLLECTION>(
                    m_sess.get_cdk_session()
                    , m_name).execute();
}
CATCH_AND_WRAP

List_init<Table> Schema::getTables()
{
  std::forward_list<Table> list;
  std::forward_list<Table>::iterator list_it = list.before_begin();

  auto tables_list = List_query<TABLE>(m_sess.get_cdk_session()
                                       , m_name).execute();

  for (auto& prop : tables_list)
  {
    list_it = list.emplace_after(list_it,
                                 Table(*this, prop.first, prop.second));
  }

  return std::move(list);
}

List_init<string> Schema::getTableNames()
{
  std::forward_list<string> list;
  std::forward_list<string>::iterator list_it = list.before_begin();
  auto tables_list = List_query<TABLE>(m_sess.get_cdk_session()
                                                   , m_name)
                     .execute();

  for (auto& el : tables_list)
  {
    list.emplace_after(list_it, std::move(el.first));
  }

  return std::move(list);

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
