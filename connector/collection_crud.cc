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
#include <uuid_gen.h>

#include <time.h>
#include <sstream>
#include <forward_list>
#include <boost/format.hpp>
#include <list>

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


// --------------------------------------------------------------------

/*
  Collection.add()
  ================
*/

class Op_collection_add
  : public Task::Access::Impl
  , public cdk::Doc_source
  , public cdk::JSON::Processor
  , public cdk::JSON::Processor::Any_prc
  , public cdk::JSON::Processor::Any_prc::Scalar_prc
{
  typedef cdk::string string;

  Table_ref    m_coll;
  std::vector<string> m_json;
  mysqlx::GUID  m_id;
  bool  m_generated_id;
  unsigned m_pos;


  Op_collection_add(Collection &coll)
    : Impl(coll)
    , m_coll(coll)
    , m_generated_id(true)
    , m_pos(0)
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

    return new cdk::Reply(get_cdk_session().coll_add(m_coll, *this, NULL));
  }

  internal::BaseResult get_result()
  {
    return Result::Access::mk(m_reply, m_id);
  }

  // Doc_source

  bool next()
  {
    if (m_pos >= m_json.size())
      return false;
    ++m_pos;
    return true;
  }

  void process(Expression::Processor &ep) const;

  // JSON::Processor

  void doc_begin() {}
  void doc_end() {}


  cdk::JSON::Processor::Any_prc*
  key_val(const string &key)
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
  arr() { assert(false); return NULL; }
  cdk::JSON::Processor::Any_prc::Doc_prc*
  doc() { assert(false); return NULL; }

  cdk::JSON::Processor::Any_prc::Scalar_prc*
  scalar()
  {
    return this;
  }

  // JSON::Processor::Any_prc::Scalar_prc

  void str(const string &val)
  {
    m_id= val;
  }
  void num(int64_t) { assert(false); }
  void num(uint64_t) { assert(false); }
  void num(float) { assert(false); }
  void num(double) { assert(false); }
  void yesno(bool) { assert(false); }

  friend class mysqlx::CollectionAdd;
};


namespace mysqlx {

template <>
struct Crud_impl<CollectionAdd>
{
  typedef Op_collection_add type;
};

}


CollectionAdd::CollectionAdd(Collection &coll)
{
  Task::Access::reset(m_task, new Op_collection_add(coll));
}


void CollectionAdd::do_add(const mysqlx::string &json)
{
  check_if_valid();
  auto *impl
    = static_cast<Op_collection_add*>(Task::Access::get_impl(m_task));
  impl->add_json(json);
}

void CollectionAdd::do_add(const DbDoc &doc)
{
  check_if_valid();
  auto *impl
    = static_cast<Op_collection_add*>(Task::Access::get_impl(m_task));
  impl->add_doc(doc);
}


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
}


// --------------------------------------------------------------------

/*
  Collection.remove()
  ===================
*/

class Op_collection_remove
  : public Op_sort<parser::Parser_mode::DOCUMENT>
{
  Table_ref m_coll;
  parser::Expression_parser m_expr;
  bool has_expr = false;

  Op_collection_remove(Collection &coll)
    : Op_sort(coll)
    , m_coll(coll)
    , m_expr(parser::Parser_mode::DOCUMENT)
  { }

  Op_collection_remove(Collection &coll, const mysqlx::string &expr)
    : Op_sort(coll)
    , m_coll(coll)
    , m_expr(parser::Parser_mode::DOCUMENT, expr)
  {
    has_expr = true;
  }

  cdk::Reply* send_command()
  {
    m_reply =
        new cdk::Reply(get_cdk_session().coll_remove(
                              m_coll,
                              has_expr ? &m_expr : nullptr,
                              m_order.empty() ? nullptr : this, // order_by spec
                              m_has_limit ? this : nullptr,  // limit spec
                              get_params())
                      );
    return m_reply;
  }

  friend class mysqlx::CollectionRemove;
};


namespace mysqlx {

template <>
struct Crud_impl<CollectionRemove>
{
  typedef Op_collection_remove type;
};

}


CollectionRemove::CollectionRemove(Collection &coll)
try {
  Executable::Access::reset_task(*this, new Op_collection_remove(coll));
}
CATCH_AND_WRAP

CollectionRemove::CollectionRemove(Collection &coll, const mysqlx::string &expr)
try {
  Executable::Access::reset_task(*this, new Op_collection_remove(coll, expr));
}
CATCH_AND_WRAP


// --------------------------------------------------------------------

/*
  Collection.find()
  =================
*/

class Op_collection_find
    : public Op_sort<parser::Parser_mode::DOCUMENT>
    , public Op_projection<parser::Parser_mode::DOCUMENT>
{
  Table_ref m_coll;
  parser::Expression_parser m_expr;
  bool has_expr = false;



  Op_collection_find(Collection &coll)
    : Op_sort(coll)
    , m_coll(coll)
    , m_expr(parser::Parser_mode::DOCUMENT)
  {
  }

  Op_collection_find(Collection &coll, const mysqlx::string &expr)
    : Op_sort(coll)
    , m_coll(coll)
    , m_expr(parser::Parser_mode::DOCUMENT, expr)
  {
    has_expr = true;
  }

  cdk::Reply* send_command()
  {
    m_reply =
        new cdk::Reply(get_cdk_session().coll_find(
                            m_coll,
                            has_expr ? &m_expr : nullptr,
                            has_projection()  ? this : nullptr, //projection
                            m_order.empty() ? nullptr : this,  // order_by spec
                            nullptr,  // group_by
                            nullptr,  // having
                            m_has_limit ? this : nullptr,  // limit spec
                            get_params())
                      );
    return m_reply;
  }

  friend class mysqlx::CollectionFind;
};


namespace mysqlx {

template <>
struct Crud_impl<CollectionFind>
{
  typedef Op_collection_find type;
};

}


CollectionFind::CollectionFind(Collection &coll)
try {
  Executable::Access::reset_task(*this, new Op_collection_find(coll));
}
CATCH_AND_WRAP

CollectionFind::CollectionFind(Collection &coll, const mysqlx::string &expr)
try {
  Executable::Access::reset_task(*this, new Op_collection_find(coll, expr));
}
CATCH_AND_WRAP


namespace mysqlx {

  template <>
  struct Crud_impl<internal::CollectionFields>
  {
    typedef Op_collection_find type;
  };

namespace internal{

  CollectionSort<true>& CollectionFields::do_fields(const string& field)
  {
    get_impl(this).add_projection(field);
    return *this;
  }


} // mysqlx
} // internal



// --------------------------------------------------------------------

/*
  Collection.modify()
  ===================
*/

class Field_parser
    : public cdk::Doc_path
{

  cdk::string m_field;

public:
  Field_parser(const Field &field)
    : m_field(field)
  {}

  virtual unsigned length() const override
  {
    return 1;
  }

  virtual Type     get_type(unsigned /*pos*/) const override
  {
    return MEMBER;
  }

  virtual const cdk::string* get_name(unsigned pos) const override
  {
    if (pos == 0)
      return &m_field;
    return NULL;
  }
  virtual const uint32_t* get_index(unsigned /*pos*/) const override
  {
    return NULL;
  }
};


class Op_collection_modify
    : public Op_sort<parser::Parser_mode::DOCUMENT>
    , public cdk::Update_spec
{

  Table_ref m_coll;
  parser::Expression_parser m_expr;
  bool has_expr = false;

  struct Field_Op
  {

    enum Operation
    {
      SET,
      UNSET,
      ARRAY_INSERT,
      ARRAY_APPEND,
      ARRAY_DELETE
    } ;

    Operation m_op;
    Field m_field;
    ExprValue m_val;

    Field_Op(Operation op, const Field &field)
      : m_op(op)
      , m_field(field)
    {}

    Field_Op(Operation op, const Field &field,ExprValue &&val)
      : Field_Op(op, field)
    {
      m_val = std::move(val);
    }

  };

  std::list<Field_Op> m_update;
  std::list<Field_Op>::const_iterator m_update_it = m_update.end();


  Op_collection_modify(Collection &coll)
    : Op_sort(coll)
    , m_coll(coll)
    , m_expr(parser::Parser_mode::DOCUMENT)
  {
  }

  Op_collection_modify(Collection &coll, const mysqlx::string &expr)
    : Op_sort(coll)
    , m_coll(coll)
    , m_expr(parser::Parser_mode::DOCUMENT, expr)
  {
    has_expr = true;
  }

  cdk::Reply* send_command() override
  {
    m_reply =
        new cdk::Reply(get_cdk_session().coll_update(m_coll,
                                                     has_expr ? &m_expr : nullptr,
                                                     *this,
                                                     m_order.empty() ? nullptr : this,
                                                     m_has_limit ? this : nullptr,
                                                     get_params()));
    return m_reply;
  }

  template <typename V>
  void add_field_operation(Field_Op::Operation op,
                           const Field &field,
                           V &&val)
  {
    m_update.push_back(Field_Op(op, field, std::move(val)));
  }

  void add_field_operation(Field_Op::Operation op,
                           const Field &field)
  {
    m_update.push_back(Field_Op(op, field));
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
    Field_parser field(m_update_it->m_field);

    switch (m_update_it->m_op)
    {
      case Field_Op::SET:
        {
          Value_prc value_prc(m_update_it->m_val, parser::Parser_mode::DOCUMENT);

          value_prc.process_if(prc.set(&field));

        }
        break;
      case Field_Op::UNSET:
        prc.remove(&field);
        break;

      case Field_Op::ARRAY_INSERT:
        {
          Value_prc value_prc(m_update_it->m_val, parser::Parser_mode::DOCUMENT);

          value_prc.process_if(prc.array_insert(&field));
        }
        break;

      case Field_Op::ARRAY_APPEND:
        {
          Value_prc value_prc(m_update_it->m_val, parser::Parser_mode::DOCUMENT);

          value_prc.process_if(prc.array_append(&field));
        }
        break;
      case Field_Op::ARRAY_DELETE:
        prc.remove(&field);
        break;
    }

  }

  friend class mysqlx::CollectionModify;
  friend class internal::CollectionModifyInterface;

};


namespace mysqlx {

template <>
struct Crud_impl<CollectionModify>
{
  typedef Op_collection_modify type;
};

}


struct CollectionModify::Access
{
  static void reset_task(CollectionModify &exec, Task::Access::Impl *impl)
  {
    Executable::Access::reset_task(exec, impl);
  }

  static Task::Access::Impl* get_impl(CollectionModify &exec)
  {
    return Task::Access::get_impl(exec.m_task);
  }
};


CollectionModify::CollectionModify(Collection &coll)
try {
  CollectionModify::Access::reset_task(*this, new Op_collection_modify(coll));
}
CATCH_AND_WRAP

CollectionModify::CollectionModify(Collection &coll, const mysqlx::string &expr)
try {
  CollectionModify::Access::reset_task(*this, new Op_collection_modify(coll, expr));
}
CATCH_AND_WRAP


CollectionModify&
internal::CollectionModifyInterface::set(const Field &field,
                                         ExprValue &&val)
{
  CollectionModify &op = get_op();
  get_impl(&op).add_field_operation(Op_collection_modify::Field_Op::SET,
                                     field,
                                     std::move(val));
  return op;
}

CollectionModify&
internal::CollectionModifyInterface::unset(const Field &field)
{
  CollectionModify &op = get_op();
  get_impl(&op).add_field_operation(Op_collection_modify::Field_Op::UNSET,
                                    field);
  return op;
}

CollectionModify&
internal::CollectionModifyInterface::arrayInsert(const Field &field,
                                                 ExprValue &&val)
{
  CollectionModify &op = get_op();
  get_impl(&op).add_field_operation(Op_collection_modify::Field_Op::ARRAY_INSERT,
                                    field,
                                    std::move(val));
  return op;
}

CollectionModify&
internal::CollectionModifyInterface::arrayAppend(const Field &field,
                                                 ExprValue &&val)
{
  CollectionModify &op = get_op();
  get_impl(&op).add_field_operation(Op_collection_modify::Field_Op::ARRAY_APPEND,
                                    field,
                                    std::move(val));
  return op;
}

CollectionModify&
internal::CollectionModifyInterface::arrayDelete(const Field &field)
{
  CollectionModify &op = get_op();
  get_impl(&op).add_field_operation(Op_collection_modify::Field_Op::ARRAY_DELETE,
                                    field);
  return op;
}
