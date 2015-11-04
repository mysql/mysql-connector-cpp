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
#include <expr_parser.h>
#include <uuid_gen.h>

#include <time.h>
#include <sstream>
#include <forward_list>
#include <boost/format.hpp>

#include "impl.h"

using namespace mysqlx;
using cdk::string;
using namespace parser;


static struct UUID_initializer {

  UUID_initializer()
  {
    init_uuid((unsigned long)time(NULL));
  }

  ~UUID_initializer()
  {
    end_uuid();
  }

} uuid_initializer;


void mysqlx::GUID::generate()
{
  uuid_type uuid;
  generate_uuid(uuid);
  boost::format fmt("%02X");

  for (unsigned i = 0; i < sizeof(uuid) && 2*i < sizeof(m_data); ++i)
  {
    memcpy(m_data + 2 * i, (fmt % (unsigned)uuid[i]).str().data(), 2);
  }
}


struct BaseResult::Access
{
  static BaseResult mk_empty() { return BaseResult(); }

  template <typename A>
  static BaseResult mk(A a) { return BaseResult(a); }

  template <typename A, typename B>
  static BaseResult mk(A a, B b) { return BaseResult(a, b); }
};


struct Executable::Access
{
  static void reset_task(Executable &exec, Task::Impl *impl)
  {
    exec.m_task.reset(impl);
  }

  static Task::Access::Impl* get_impl(Executable &exec)
  {
    return Task::Access::get_impl(exec.m_task);
  }
};



class Schema_ref : public cdk::api::Schema_ref
{
  const cdk::string m_name;

  const cdk::string name() const { return m_name; }

public:

  Schema_ref(const mysqlx::string &name) : m_name(name) {}
  Schema_ref(const cdk::string &name) : m_name(name) {}
};

class Table_ref : public cdk::api::Table_ref
{
  Schema_ref m_schema;
  const cdk::string m_name;

public:

  const cdk::string name() const { return m_name; }
  const cdk::api::Schema_ref* schema() const { return &m_schema; }

  Table_ref(const Collection &coll)
    : m_schema(coll.getSchema().getName())
    , m_name(coll.getName())
  {}

  Table_ref(const Table &tbl)
    : m_schema(tbl.getSchema().getName())
    , m_name(tbl.getName())
  {}

  Table_ref(const cdk::string &schema, const cdk::string &name)
    : m_schema(schema), m_name(name)
  {}
};


/*
  Table.insert()
  ==============
*/


class Op_table_insert
  : public Task::Access::Impl
  , public cdk::Row_source
  , public cdk::Expression
  , public cdk::Format_info
{
  using string = cdk::string;
  using Row_list = std::forward_list < Row >;

  Table_ref m_table;
  Row_list  m_rows;
  Row_list::const_iterator m_cur_row;
  Row_list::iterator m_end;

  Op_table_insert(Table &tbl)
    : Impl(tbl)
    , m_table(tbl)
  {}

  void reset()
  {
    m_rows.clear();
    m_cur_row = m_rows.cbegin();
    m_end = m_rows.before_begin();
  }

  // Task::Impl

  cdk::Reply* send_command()
  {
    // Prepare iterators to make a pass through m_rows list.
    m_cur_row = m_rows.cbegin();
    m_end = m_rows.end();
    return new cdk::Reply(get_cdk_session().table_insert(m_table, *this));
  }

  // Row_source (Iterator)

  void next() { ++m_cur_row;  }
  bool is_valid() { return m_cur_row != m_end; }

  // Row_source (Expr_list)

  unsigned m_pos;

  unsigned count() const { return m_cur_row->colCount(); }
  const cdk::Expression& get(unsigned pos) const
  {
    const_cast<Op_table_insert*>(this)->m_pos = pos;
    return *this;
  }

  void process(Expression::Processor &ep) const;

  // Format_info

  bool for_type(cdk::Type_info) const { return true; }
  void get_info(cdk::Format<cdk::TYPE_BYTES>&) const {}
  using cdk::Format_info::get_info;

  friend class mysqlx::TableInsertValues;
};


inline
Op_table_insert& get_impl(TableInsertValues *p)
{
  return *static_cast<Op_table_insert*>(Executable::Access::get_impl(*p));
}


void TableInsertValues::prepare()
{
  Task::Access::reset(m_task, new Op_table_insert(m_table));
  get_impl(this).reset();
}


Row& TableInsertValues::add_row()
{
  auto &impl = get_impl(this);
  impl.m_end = impl.m_rows.emplace_after(impl.m_end);
  return *impl.m_end;
}

void TableInsertValues::add_row(const Row &row)
{
  auto &impl = get_impl(this);
  impl.m_end = impl.m_rows.emplace_after(impl.m_end, row);
}


void Op_table_insert::process(Expression::Processor &ep) const
{
  const Value &val = m_cur_row->get(m_pos);

  switch (val.getType())
  {
  case Value::VNULL:  ep.null(); return;
  case Value::STRING: ep.str((string)val); return;
  case Value::INT64:  ep.num((int64_t)(int)val); return;
  case Value::UINT64: ep.num((uint64_t)(unsigned)val); return;
  case Value::FLOAT:  ep.num((float)val); return;
  case Value::DOUBLE: ep.num((double)val); return;
    // TODO: handle other value types
  default:
    ep.value(cdk::TYPE_BYTES, *this, Value::Access::get_bytes(val));
    return;
  }
}


/*
  Collection.add()
  ================
*/

class Op_collection_add
  : public Task::Access::Impl
  , public cdk::Expr_list
  , public cdk::Expression
  , public cdk::JSON::Processor
{
  typedef cdk::string string;

  Table_ref    m_coll;
  std::vector<string> m_json;
  mysqlx::GUID  m_id;
  bool  m_generated_id;

  Op_collection_add(Collection &coll)
    : Impl(coll)
    , m_coll(coll)
    , m_generated_id(true)
  {}

  void add_json(const string &json)
  {
    m_json.push_back(json);
  }

  void add_doc(const DbDoc &doc)
  {
    // TODO: Instead of sending JSON string, send structured description
    // of the document (requires support for document expressions MYC-113)

    std::ostringstream buf;
    buf << doc;
    m_json.push_back(buf.str());
  }

  cdk::Reply* send_command()
  {
    // Issue coll_add statement where documents are described by list
    // of expressions defined by this instance.

    return new cdk::Reply(get_cdk_session().coll_add(m_coll, *this));
  }

  BaseResult get_result()
  {
    return Result::Access::mk(m_reply, m_id);
  }

  // Expr_list

  unsigned m_pos;

  unsigned count() const { return m_json.size(); }
  const cdk::Expression& get(unsigned pos) const
  {
    const_cast<Op_collection_add*>(this)->m_pos = pos;
    return *this;
  }

  void process(Expression::Processor &ep) const;

  // JSON::Processor

  void doc_begin() {}
  void doc_end() {}

  void key_doc(const string&, const Document&) {}

  void key_val(const string &key, const Value &val)
  {
    // look only at key '_id'
    if (key != string("_id"))
      return;
    // process '_id' value
    val.process(*this);
  }

  // JSON::Value::Processor

  void str(const string &val)
  {
    m_generated_id= false;
    m_id= val;
  }

  friend class mysqlx::CollectionAddExec;
};


void CollectionAddExec::initialize()
{
  Task::Access::reset(m_task, new Op_collection_add(m_coll));
}

CollectionAddExec& CollectionAddExec::do_add(const mysqlx::string &json)
{
  auto *impl
    = static_cast<Op_collection_add*>(Task::Access::get_impl(m_task));
  impl->add_json(json);
  return *this;
}

CollectionAddExec& CollectionAddExec::do_add(const DbDoc &doc)
{
  auto *impl
    = static_cast<Op_collection_add*>(Task::Access::get_impl(m_task));
  impl->add_doc(doc);
  return *this;
}


/*
  Class describing elements of expression:

    JSON_INSERT(<json>, '$._id', <id>)

  where <json> and <id> are given as constructor parameters.
*/

class Insert_id
  : public cdk::api::Table_ref
  , public cdk::Expr_list
  , public cdk::Expression
{
  typedef cdk::string string;

  const std::string m_json;
  const std::string &m_id;

  Insert_id(const string &json, const std::string &id)
    : m_json(json)  // note: conversion to utf-8
    , m_id(id)
  {}

  // Table_ref (function name)

  const cdk::api::Schema_ref* schema() const { return NULL; }
  const string name() const { return L"JSON_INSERT"; }

  // Expr_list (arguments)

  unsigned count() const { return 3; }

  unsigned m_pos;

  const Expression& get(unsigned pos) const
  {
    const_cast<Insert_id*>(this)->m_pos= pos;
    return *this;
  }

  using cdk::Expression::Processor;

  void process(Processor &ep) const
  {
    switch (m_pos)
    {
    case 0: ep.str(m_json); return;
    case 1: ep.str(L"$._id"); return;
    case 2: ep.str(m_id); return;
    default: throw "index out of range";
    }
  }

  friend class Op_collection_add;
};

/*
  Expression describing single document to be inserted.

  If document id was generated, then expression is a call of
  JSON_INSERT() function that adds generated id. Otherwise it
  is plain JSON string of the document.

  TODO:
  - Append '_id' field at the end of document, instead of using
    JSON_INSERT() (should be more efficient). If duplicate key is
    found in a document string, only the first occurence is taken
    into account.
*/

void Op_collection_add::process(Expression::Processor &ep) const
{
  const string &json = m_json.at(m_pos);
  auto self = const_cast<Op_collection_add*>(this);

  // Parse JSON string to find _id if defined.
  // TODO: Avoid parsing (if inserted document id is returned by server).

  cdk::Codec<cdk::TYPE_DOCUMENT> codec;
  self->m_generated_id = true;
  codec.from_bytes(cdk::bytes(json), *self);

  if (m_generated_id)
  {
    self->m_id.generate();
    std::string id(m_id);
    Insert_id expr(json, id);
    ep.call(expr, expr);
  }
  else
  {
    // TODO: ep.val(TYPE_DOCUMENT, json_format, cdk::bytes())
    ep.str(json);
  }
}


/*
  Collection.remove()
  ===================
*/


class Op_collection_remove : public Task::Access::Impl
{
  Table_ref m_coll;
  parser::Expr_parser m_expr;

  Op_collection_remove(Collection &coll)
    : Impl(coll)
    , m_coll(coll)
    , m_expr(parser::Parser_mode::DOCUMENT)
  {
    m_reply= new cdk::Reply(get_cdk_session().coll_remove(m_coll, NULL, NULL));
  }

  Op_collection_remove(Collection &coll, const mysqlx::string &expr)
    : Impl(coll)
    , m_coll(coll)
    , m_expr(parser::Parser_mode::DOCUMENT, expr)
  {
    m_reply = new cdk::Reply(get_cdk_session().coll_remove(m_coll, &m_expr, NULL));
  }

  cdk::Reply* send_command()
  {
    return m_reply;
  }

  friend class mysqlx::CollectionRemove;
};


Executable& CollectionRemove::remove()
try {
  Executable::Access::reset_task(m_exec, new Op_collection_remove(m_coll));
  return m_exec;
}
CATCH_AND_WRAP

Executable& CollectionRemove::remove(const mysqlx::string &expr)
try {
  Executable::Access::reset_task(m_exec, new Op_collection_remove(m_coll, expr));
  return m_exec;
}
CATCH_AND_WRAP


/*
  Collection.find()
  =================
*/


class Op_collection_find
  : public Task::Access::Impl
{
  Table_ref m_coll;
  parser::Expr_parser m_expr;

  Op_collection_find(Collection &coll)
    : Impl(coll)
    , m_coll(coll)
    , m_expr(parser::Parser_mode::DOCUMENT)
  {
    m_reply= new cdk::Reply(get_cdk_session().coll_find(m_coll, NULL, NULL));
  }

  Op_collection_find(Collection &coll, const mysqlx::string &expr)
    : Impl(coll)
    , m_coll(coll)
    , m_expr(parser::Parser_mode::DOCUMENT, expr)
  {
    m_reply= new cdk::Reply(get_cdk_session().coll_find(m_coll, &m_expr, NULL));
  }

  cdk::Reply* send_command()
  {
    return m_reply;
  }

  friend class mysqlx::CollectionFind;
};

Executable& CollectionFind::find()
try {
  Executable::Access::reset_task(m_exec, new Op_collection_find(m_coll));
  return m_exec;
}
CATCH_AND_WRAP

Executable& CollectionFind::find(const mysqlx::string &expr)
try {
  Executable::Access::reset_task(m_exec, new Op_collection_find(m_coll, expr));
  return m_exec;
}
CATCH_AND_WRAP

