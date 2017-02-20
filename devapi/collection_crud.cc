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
#include <uuid_gen.h>

#include <time.h>
#include <sstream>
#include <forward_list>
#include <boost/format.hpp>
#include <list>

#include "impl.h"

using namespace mysqlx;
using namespace uuid;

// --------------------------------------------------------------------

void mysqlx::GUID::generate()
{
  uuid::uuid_type uuid;
  mysqlx::generate_uuid(uuid);
  boost::format fmt("%02X");

  for (unsigned i = 0; i < sizeof(uuid) && 2 * i < sizeof(m_data); ++i)
  {
    memcpy(m_data + 2 * i, (fmt % (unsigned)uuid[i]).str().data(), 2);
  }
}


// --------------------------------------------------------------------

/*
  Collection add
  ==============
*/

/*
  Internal implementation for collection CRUD add operation.

  Implementation object stores list of JSON strings describing documents
  to be added and passed with `add_json` method. It presents this list
  of documents via cdk::Doc_source interface. See method `process` for
  details.

  Overriden method Op_base::send_command() sends the collection add
  command to the CDK session.
*/

class Op_collection_add
  : public Op_base< internal::CollectionAdd_impl >
  , public cdk::Doc_source
  , public cdk::JSON::Processor
  , public cdk::JSON::Processor::Any_prc
  , public cdk::JSON::Processor::Any_prc::Scalar_prc
{
  typedef cdk::string string;

  Table_ref    m_coll;
  std::vector<string> m_json;
  mysqlx::GUID  m_id;
  std::vector<mysqlx::GUID> m_id_list;
  bool  m_generated_id;
  unsigned m_pos;


  Op_collection_add(Collection &coll)
    : Op_base(coll)
    , m_coll(coll)
    , m_generated_id(true)
    , m_pos(0)
  {}

  Executable_impl* clone() const override
  {
    return new Op_collection_add(*this);
  }


  void add_json(const mysqlx::string &json) override
  {
    m_json.push_back(json);
  }


  cdk::Reply* send_command() override
  {
    // Do nothing if no documents were specified.

    if (m_json.empty())
      return NULL;

    // Issue coll_add statement where documents are described by list
    // of expressions defined by this instance.

    return new cdk::Reply(get_cdk_session().coll_add(m_coll, *this, NULL));
  }


  internal::BaseResult mk_result(cdk::Reply *reply) override
  {
    return Result::Access::mk(m_sess, reply, m_id_list);
  }


  // Doc_source

  bool next() override
  {
    if (m_pos >= m_json.size())
      return false;
    ++m_pos;
    return true;
  }

  void process(cdk::Expression::Processor &ep) const override;


  // JSON::Processor

  void doc_begin() override {}
  void doc_end() override {}


  cdk::JSON::Processor::Any_prc*
  key_val(const string &key) override
  {
    // look only at key '_id'
    if (key != string("_id"))
      return NULL;
    // process '_id' value
    m_generated_id= false;
    return this;
  }

  // JSON::Processor::Any_prc

  cdk::JSON::Processor::Any_prc::List_prc*
  arr() override { assert(false); return NULL; }
  cdk::JSON::Processor::Any_prc::Doc_prc*
  doc() override { assert(false); return NULL; }

  cdk::JSON::Processor::Any_prc::Scalar_prc*
  scalar() override
  {
    return this;
  }

  // JSON::Processor::Any_prc::Scalar_prc

  void str(const string &val) override
  {
    m_id= val;
  }

  void null() override
  {
    using mysqlx::throw_error;
    THROW("Document id can not be null");
  }

  void num(int64_t) override
  {
    using mysqlx::throw_error;
    THROW("Document id must be a string");
  }

  void num(uint64_t) override
  {
    using mysqlx::throw_error;
    THROW("Document id must be a string");
  }

  void num(float) override
  {
    using mysqlx::throw_error;
    THROW("Document id must be a string");
  }

  void num(double) override
  {
    using mysqlx::throw_error;
    THROW("Document id must be a string");
  }

  void yesno(bool) override
  {
    using mysqlx::throw_error;
    THROW("Document id must be a string");
  }


  friend mysqlx::CollectionAdd;
};


CollectionAdd::CollectionAdd(Collection &coll)
try
  : Executable(new Op_collection_add(coll))
{}
CATCH_AND_WRAP


/*
  Class describing elements of expression:

    JSON_INSERT(<json>, '$._id', <id>)

  where <json> and <id> are given as constructor parameters.
*/

class Insert_id
  : public cdk::api::Table_ref
  , public cdk::Expr_list
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

  using cdk::Expr_list::Processor;

  void process(Processor &lp) const
  {
    lp.list_begin();   // FIXME
    lp.list_el()->scalar()->val()->str(m_json);
    lp.list_el()->scalar()->val()->str(L"$._id");
    lp.list_el()->scalar()->val()->str(m_id);
    lp.list_end();
  }

  friend Op_collection_add;
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

void Op_collection_add::process(cdk::Expression::Processor &ep) const
{
  assert(m_pos > 0);  // this method should be called after calling next()

  const string &json = m_json.at(m_pos-1);
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
    expr.process(*ep.scalar()->call(expr));
  }
  else
  {
    // TODO: ep.val(TYPE_DOCUMENT, json_format, cdk::bytes())
    ep.scalar()->val()->str(json);
  }

  //Save added "_id" to the list
  self->m_id_list.push_back(m_id);
}


// --------------------------------------------------------------------

/*
  Collection remove
  =================
*/

/*
  Internal implementation for collection CRUD remove operation.

  This implementation is constructed from Op_sort<> and Op_select<>
  templates. It defines `send_command` method to send the remove
  operation to the underlying CDK session.
*/

class Op_collection_remove
  : public Op_select<
      Op_sort<
        internal::Sort_impl,
        parser::Parser_mode::DOCUMENT
      >,
      parser::Parser_mode::DOCUMENT
    >
{
  Table_ref m_coll;

  Op_collection_remove(Collection &coll)
    : Op_select(coll)
    , m_coll(coll)
  { }

  Op_collection_remove(Collection &coll, const mysqlx::string &expr)
    : Op_select(coll)
    , m_coll(coll)
  {
    add_where(expr);
  }

  Executable_impl* clone() const override
  {
    return new Op_collection_remove(*this);
  }


  cdk::Reply* send_command() override
  {
    return
      new cdk::Reply(get_cdk_session().coll_remove(
                            m_coll,
                            get_where(),
                            get_order_by(),
                            get_limit(),
                            get_params()
                    ));
  }

  friend mysqlx::CollectionRemove;
};


CollectionRemove::CollectionRemove(Collection &coll)
try
  : Executable(new Op_collection_remove(coll))
{}
CATCH_AND_WRAP

CollectionRemove::CollectionRemove(Collection &coll, const mysqlx::string &expr)
try
  : Executable(new Op_collection_remove(coll, expr))
{}
CATCH_AND_WRAP


// --------------------------------------------------------------------

/*
  Collection find
  ===============
*/

/*
  Implementation of collection CRUD find operation.
*/

class Op_collection_find
    : public Op_select<
        Op_projection<
          internal::Proj_impl,
          parser::Parser_mode::DOCUMENT
        >,
        parser::Parser_mode::DOCUMENT
      >
{
  Table_ref m_coll;

  Op_collection_find(Collection &coll)
    : Op_select(coll)
    , m_coll(coll)
  {
  }

  Op_collection_find(Collection &coll, const mysqlx::string &expr)
    : Op_select(coll)
    , m_coll(coll)
  {
    add_where(expr);
  }

  Executable_impl* clone() const override
  {
    return new Op_collection_find(*this);
  }

  cdk::Reply* send_command() override
  {
    return
      new cdk::Reply(get_cdk_session().coll_find(
                          m_coll,
                          NULL,           // view spec
                          get_where(),
                          get_doc_proj(),
                          get_order_by(),
                          get_group_by(),
                          get_having(),
                          get_limit(),
                          get_params()
                    ));
  }

  friend mysqlx::CollectionFind;
};


CollectionFind::CollectionFind(Collection &coll)
try
  : Executable(new Op_collection_find(coll))
{}
CATCH_AND_WRAP

CollectionFind::CollectionFind(Collection &coll, const mysqlx::string &expr)
try
  : Executable(new Op_collection_find(coll, expr))
{}
CATCH_AND_WRAP


// --------------------------------------------------------------------

/*
  Collection modify
  =================
*/

/*
  Implementation of collection CRUD modify operation.

  This implementation builds on top of Op_select<> and Op_sort<>
  templates. It adds storage for update operations specified by
  user. These update requests are presented via cdk::Update_spec
  interface.
*/

class Op_collection_modify
    : public Op_select<
        Op_sort<
          internal::CollectionModify_impl,
          parser::Parser_mode::DOCUMENT
        >,
        parser::Parser_mode::DOCUMENT
      >
    , public cdk::Update_spec
{
  typedef internal::CollectionModify_impl Impl;

  Table_ref m_coll;

  struct Field_Op
  {
    typedef Impl::Operation Operation;

    Operation m_op;
    Field m_field;
    internal::ExprValue m_val;

    Field_Op(Operation op, const Field &field)
      : m_op(op)
      , m_field(field)
    {}

    Field_Op(Operation op, const Field &field, internal::ExprValue &&val)
      : Field_Op(op, field)
    {
      m_val = std::move(val);
    }
  };

  std::list<Field_Op> m_update;
  std::list<Field_Op>::const_iterator m_update_it = m_update.end();


  Op_collection_modify(Collection &coll)
    : Op_select(coll)
    , m_coll(coll)
  {}

  Op_collection_modify(Collection &coll, const mysqlx::string &expr)
    : Op_select(coll)
    , m_coll(coll)
  {
    add_where(expr);
  }

  Executable_impl* clone() const override
  {
    return new Op_collection_modify(*this);
  }

  cdk::Reply* send_command() override
  {
    // Do nothing if no update specifications were added

    if (m_update.empty())
      return NULL;

    return
      new cdk::Reply(get_cdk_session().coll_update(
                       m_coll,
                       get_where(),
                       *this,
                       get_order_by(),
                       get_limit(),
                       get_params()
                     ));
  }

  void add_operation(Field_Op::Operation op,
                     const Field &field,
                     internal::ExprValue &&val) override
  {
    m_update.emplace_back(op, field, std::move(val));
  }

  void add_operation(Field_Op::Operation op,
                     const Field &field) override
  {
    m_update.emplace_back(op, field);
  }


  // cdk::Update_spec implementation

  bool next() override
  {
    if (m_update_it == m_update.end())
    {
      m_update_it = m_update.begin();
      return m_update_it!= m_update.end();
    }
    ++m_update_it;
    return m_update_it!= m_update.end();
  }

  void process(Update_spec::Processor &prc) const override
  {
    parser::Doc_field_parser doc_field((mysqlx::string)m_update_it->m_field);

    switch (m_update_it->m_op)
    {
      case Impl::SET:
        {
          Value_expr value_prc(m_update_it->m_val,
                               parser::Parser_mode::DOCUMENT);

          value_prc.process_if(prc.set(&doc_field));

        }
        break;
      case Impl::UNSET:
        prc.remove(&doc_field);
        break;

      case Impl::ARRAY_INSERT:
        {
          Value_expr value_prc(m_update_it->m_val,
                               parser::Parser_mode::DOCUMENT);

          value_prc.process_if(prc.array_insert(&doc_field));
        }
        break;

      case Impl::ARRAY_APPEND:
        {
          Value_expr value_prc(m_update_it->m_val,
                               parser::Parser_mode::DOCUMENT);

          value_prc.process_if(prc.array_append(&doc_field));
        }
        break;
      case Impl::ARRAY_DELETE:
        prc.remove(&doc_field);
        break;
    }

  }

  friend mysqlx::CollectionModify;
};


CollectionModify::CollectionModify(Collection &coll)
try
  : Executable(new Op_collection_modify(coll))
{}
CATCH_AND_WRAP

CollectionModify::CollectionModify(Collection &coll, const mysqlx::string &expr)
try
  : Executable(new Op_collection_modify(coll, expr))
{}
CATCH_AND_WRAP
