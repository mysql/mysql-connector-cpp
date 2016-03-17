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

class Create_args
  : public cdk::Any_list
{
  typedef cdk::Any_list Any_list;

  const string &m_schema;
  const string &m_name;

public:

  Create_args(const string &schema, const string &name)
    : m_schema(schema), m_name(name)
  {}

  void process(Any_list::Processor &lp) const
  {
    cdk::Safe_prc<Any_list::Processor> sp(lp);
    sp->list_begin();
    // NOTE: uses utf8
    sp->list_el()->scalar()->str(m_schema);
    sp->list_el()->scalar()->str(m_name);
    sp->list_end();
  }
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
  : public cdk::Reply
  , public cdk::Any_list
{
  const string &m_name;
  typedef const string& Args_t;

  check_query(cdk::Session &sess, Args_t name)
    : m_name(name)
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

  // Pass schema name as the value of an SQL placeholder.

  void process(Processor &prc) const
  {
    prc.list_begin();
    safe_prc(prc)->list_el()->scalar()->str(m_name);
    prc.list_end();
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


// ---------------------------------------------------------------------


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
    Create_args args(m_name, name);
    if (!check_existence<TABLE>(m_sess.get_cdk_session(), args))
      // TODO: Better error (collection name)
      throw Error("No such table");
  }
  return Table(*this, name);
}
CATCH_AND_WRAP


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

BaseResult Task::wait()
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
