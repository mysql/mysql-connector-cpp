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

#include <mysql_devapi.h>
#include <mysql/cdk.h>
#include <uri_parser.h>

#include <iostream>
#include <sstream>
#include <list>

#include "impl.h"
#include "../common/settings.h"
#include "../common/result.h"


const unsigned max_priority = 100;

using namespace ::mysqlx;


/*
  Session settings
  ================
*/


template<>
void
internal::Settings_detail<internal::Settings_traits>::do_set(opt_list_t &&opts)
{
  Setter set(*this);

  set.doc_begin();

  for (auto opt_val : opts)
  {
    const Value &val = opt_val.second;
    auto *prc = set.key_val(opt_val.first)->scalar();
    assert(prc);

    switch (val.get_type())
    {
    case Value::STRING:  prc->str(val.get_string()); break;
    case Value::WSTRING: prc->str(val.get_wstring()); break;
    case Value::INT64:   prc->num(val.get_sint());   break;
    case Value::UINT64:  prc->num(val.get_uint());   break;

    default:
      throw_error("Invalid type of session option value");
    }
  }

  set.doc_end();
}

/*
  Session implementation
  ======================
*/


internal::Session_detail::Session_detail(common::Settings_impl &settings)
{
  try {

    cdk::ds::Multi_source source;
    settings.get_data_source(source);
    m_impl = std::make_shared<Impl>(source);

  }
  CATCH_AND_WRAP
}


//Session::Session(Session* master)
//{
//  assert(master);
//  m_impl = master->m_impl;
//  master->add_child(this);
//}


cdk::Session& internal::Session_detail::get_cdk_session()
{
  if (!m_impl)
    throw Error("Session closed");

  return m_impl->m_sess;
}


void internal::Session_detail::prepare_for_cmd()
{
  assert(m_impl);
  m_impl->prepare_for_cmd();
}


void internal::Session_detail::close()
{
  if (m_parent_session)
  {
    m_parent_session->remove_child(this);
  }
  else
  {
    // This is master session, notify child session that it is being
    // closed.

    for (auto sess : m_child_sessions)
    {
      sess->parent_close_notify();
    }

    get_cdk_session().rollback();
  }

  m_impl.reset();
}



// ---------------------------------------------------------------------

/*
  Transactions.
*/

void internal::Session_detail::start_transaction()
{
  try {
    get_cdk_session().begin();
  }
  CATCH_AND_WRAP
}


void internal::Session_detail::commit()
{
  try {
    get_cdk_session().commit();
  }
  CATCH_AND_WRAP
}


void internal::Session_detail::rollback()
{
  try {
    get_cdk_session().rollback();
  }
  CATCH_AND_WRAP
}


// ---------------------------------------------------------------------


using common::Object_type;


void internal::Session_detail::create_schema(const string &name, bool reuse)
{
  Schema_ref schema(name);
  common::create_object<Object_type::SCHEMA>(m_impl, schema, reuse);
}


void internal::Session_detail::drop_schema(const string &name)
{
  Schema_ref schema(name);
  common::drop_object<Object_type::SCHEMA>(m_impl, schema);
}


const std::wstring& internal::Session_detail::get_default_schema_name()
{
  if (m_impl->m_default_db.empty())
    throw Error("No default schema set for the session");
  return m_impl->m_default_db;
}



/*
  Schema list source.
*/

struct internal::Query_src::Res_impl
  : public common::Result_impl<string>
{
  template <typename... Ty>
  Res_impl(Ty&&... args)
    : common::Result_impl<string>(std::forward<Ty>(args)...)
  {};
};


internal::Query_src::~Query_src()
{
  delete m_res;
}


internal::Session_detail::Name_src::Name_src(
  const Session &sess, const string &pattern
)
  : m_sess(sess)
{
  common::Op_list<Object_type::SCHEMA> list_op{ sess.m_impl, pattern };
  m_res = new Res_impl(list_op.execute());
}


auto internal::Session_detail::Schema_src::iterator_get() -> Schema
{
  return { const_cast<Session&>(m_sess), Name_src::iterator_get() };
}




/*
  Schema
  ======
*/


void internal::Schema_detail::create_collection(const string &name, bool reuse)
{
  Object_ref coll(m_name, name);
  common::create_object<Object_type::COLLECTION>(m_sess, coll, reuse);
}


void internal::Schema_detail::drop_collection(const mysqlx::string& name)
{
  Object_ref coll(m_name, name);
  common::drop_object<Object_type::COLLECTION>(m_sess, coll);
}



internal::Schema_detail::Name_src::Name_src(
  const Schema &sch,
  Obj_type type,
  const string &pattern
)
  : m_schema(sch)
{
  Schema_ref obj{ sch.getName() };
  const auto &sess = sch.Schema_detail::m_sess;

  switch (type)
  {
  case COLLECTION:
    {
      common::Op_list<Object_type::COLLECTION> list_op(sess, obj, pattern);
      m_res = new Res_impl(list_op.execute());
    };
    break;

  case TABLE:
    {
      common::Op_list<Object_type::TABLE> list_op(sess, obj, pattern);
      m_res = new Res_impl(list_op.execute());
    };
    break;
  }
}


Collection
internal::Schema_detail::Collection_src::iterator_get()
{
  return Collection(m_schema, Name_src::iterator_get());
}

Table
internal::Schema_detail::Table_src::iterator_get()
{
  auto *row = static_cast<const common::Row_data*>(m_row);
  assert(1 < row->size());
  assert(cdk::TYPE_STRING == m_res->get_column(1).m_type);

  cdk::string type;
  m_res->get_column(1).get<cdk::TYPE_STRING>()
    .m_codec.from_bytes(row->at(1).data(), type);

  return Table(m_schema, Name_src::iterator_get(), type == L"VIEW");
}


/*
  Implementation of List_init<> source class taking items from
  query results.
*/


bool internal::Query_src::iterator_next()
{
  assert(m_res);
  m_row = m_res->get_row();
  return nullptr != m_row;
}


string internal::Query_src::iterator_get()
{
  assert(m_row);
  auto *row = static_cast<const common::Row_data*>(m_row);

  const auto &name_col = m_res->get_column(0);
  const auto &data = row->at(0).data();
  cdk::string name;

  // TDOD: Investigate why we get column type other than STRING.
  // This is realted to changed default collation in newer servers and logic
  // we have to interpret BYTES columns as STRING for some collations.
  // TODO: use Value to do the conversion?

  switch (name_col.m_type)
  {
  case cdk::TYPE_STRING:
    m_res->get_column(0).get<cdk::TYPE_STRING>()
      .m_codec.from_bytes(data, name);
    break;

  case cdk::TYPE_BYTES:
    /*
      Even if we see name column reported as raw bytes, we assume it is
      like an utf8 string with null byte appended at the end.
    */
    assert(0 < data.size());
    assert(0 == *data.end());
    name = std::string(data.begin(), data.end()-1);
    break;

  default:
    assert(false);
  }

  return name;
}

// ---------------------------------------------------------------------


std::string string::Impl::to_utf8(const string &other)
{
  return cdk::string(other);
}

void string::Impl::from_utf8(string &s, const std::string &other)
{
  s = cdk::string(other);
}


ostream& operator<<(ostream &out, const Error&)
{
  out <<"MYSQLX Error!";
  return out;
}
