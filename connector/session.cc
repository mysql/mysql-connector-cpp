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

#include <boost/format.hpp>
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

Schema XSession::getSchema(const string &name)
try {
  return Schema(*this, name);
}
CATCH_AND_WRAP


/*
  Schema.createCollection()
  =========================
*/

class Create_args
  : public cdk::Any_list
  , public cdk::Any
{
  typedef cdk::Any Any;

  unsigned m_pos;
  const string &m_schema;
  const string &m_name;

public:

  Create_args(const string &schema, const string &name)
    : m_schema(schema), m_name(name)
  {}

  unsigned count() const { return 2; }
  const Any& get(unsigned pos) const
  {
    const_cast<Create_args*>(this)->m_pos= pos;
    return *this;
  }

  void process(Any::Processor &ep) const
  {
    // NOTE: uses utf8
    ep.str((0 == m_pos ? m_schema : m_name));
  }
};


Collection Schema::getCollection(const string &name, bool /*check*/)
try {
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
};


Executable& NodeSession::sql(const string &query)
try {
  Task::Access::reset(m_task, new Op_sql(*this, query));
  return *this;
}
CATCH_AND_WRAP


/*
  Other
  =====
*/


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

