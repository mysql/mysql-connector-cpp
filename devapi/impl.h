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

#ifndef MYSQLX_IMPL_H
#define MYSQLX_IMPL_H

/*
  Internal implementations for public DevAPI classes.
*/

#include <mysql_devapi.h>
#include <mysql/cdk.h>
#include <mysql/cdk/converters.h>
#include <expr_parser.h>
#include <map>
#include <memory>
#include <stack>
#include <list>

#include "../global.h"


namespace mysqlx {


struct Value::Access
{

  static cdk::bytes get_bytes(const Value &val)
  {
    return cdk::bytes(val.m_raw.begin(), val.m_raw.end());
  }

  /*
    Build document value from a JSON string which is
    assumed to describe a document.
  */

  static Value mk_doc(const string &json)
  {
    Value ret;
    ret.m_type = Value::DOCUMENT;
    ret.m_doc = DbDoc(json);
    return std::move(ret);
  }

  /*
    Build value after parsing given JSON string. Depending
    on the string, the value can be a document, array or
    scalar.
  */

  static Value mk_from_json(const std::string &json);
};


struct Value_scalar_prc_converter
    : public cdk::Converter<
    Value_scalar_prc_converter,
    cdk::Expr_processor,
    cdk::Value_processor
    >
{

  virtual Value_prc*  val() override
  {
    return m_proc;
  }

  Args_prc*   op(const char*) override
  {
    THROW("Unexpected expression usage operator");
  }

  Args_prc*   call(const Object_ref&) override
  {
    THROW("Unexpected expression usage operator");
  }

  void ref(const Column_ref&, const Doc_path*) override
  {
    THROW("Unexpected expression usage operator");
  }

  void ref(const Doc_path&) override
  {
    THROW("Unexpected expression usage operator");
  }

  void param(const cdk::string&) override
  {
    THROW("Unexpected expression usage operator");
  }

  void param(uint16_t) override
  {
    THROW("Unexpected expression usage operator");
  }

  void var(const cdk::string&) override
  {
    THROW("Unexpected expression usage operator");
  }

};


typedef
cdk::Expr_conv_base<cdk::Any_prc_converter<Value_scalar_prc_converter>>
Value_converter;


/*
  Present Value object as CDK expression.
*/

class Value_expr
    : public cdk::Expression
    , cdk::Format_info
{
  parser::Parser_mode::value m_parser_mode;
  Value m_value;
  bool m_is_expr = false;


  //Private constructor to be used only inside Value_expr class
  Value_expr(const Value &val,
            bool is_expr,
            parser::Parser_mode::value parser_mode)
    : m_parser_mode(parser_mode)
    , m_value(val)
    , m_is_expr(is_expr)
  {}

public:

  Value_expr(const Value &val, parser::Parser_mode::value parser_mode)
    : m_parser_mode(parser_mode)
    , m_value(val)
  {}

  Value_expr(Value &&val, parser::Parser_mode::value parser_mode)
    : m_parser_mode(parser_mode)
    , m_value(std::move(val))
  {}

  Value_expr(const internal::ExprValue &val, parser::Parser_mode::value parser_mode)
    : m_parser_mode(parser_mode)
    , m_value(val)
  {
    m_is_expr = val.isExpression();
  }

  Value_expr(internal::ExprValue &&val, parser::Parser_mode::value parser_mode)
    : m_parser_mode(parser_mode)
    , m_value(std::move(val))
  {
    m_is_expr = val.isExpression();
  }

  void process (Processor &prc) const override
  {
    // Handle expressions

    if (m_is_expr)
    {
      assert(Value::STRING == m_value.getType());
      parser::Expression_parser expr(m_parser_mode,
        (mysqlx::string)m_value);
      expr.process(prc);
      return;
    }

    // Handle non scalar values

    switch (m_value.getType())
    {
    case Value::DOCUMENT:
      {
        mysqlx::DbDoc doc = static_cast<mysqlx::DbDoc>(m_value);
        Processor::Doc_prc *dprc = safe_prc(prc)->doc();
        if (!dprc)
          return;
        dprc->doc_begin();
        for (Field fld : doc)
        {
          Value_expr value(doc[fld], m_is_expr, m_parser_mode);
          value.process_if(dprc->key_val((string)fld));
        }
        dprc->doc_end();
      }
      return;

    case Value::ARRAY:
      {
        Processor::List_prc *lpr = safe_prc(prc)->arr();
        if (!lpr)
          return;
        lpr->list_begin();
        for (Value val : m_value)
        {
          Value_expr value(val, m_is_expr, m_parser_mode);
          value.process_if(lpr->list_el());
        }
        lpr->list_end();
      }
      return;

    default: break; // continue with scalar values
    }

    // Handle scalar values

    Processor::Scalar_prc::Value_prc *vprc;
    vprc = safe_prc(prc)->scalar()->val();

    if (!vprc)
      return;

    switch (m_value.getType())
    {
      case Value::VNULL:
        vprc->null();
        break;
      case Value::UINT64:
        vprc->num(static_cast<uint64_t>(m_value));
        break;
      case Value::INT64:
        vprc->num(static_cast<int64_t>(m_value));
        break;
      case Value::FLOAT:
        vprc->num(static_cast<float>(m_value));
        break;
      case Value::DOUBLE:
        vprc->num(static_cast<double>(m_value));
        break;
      case Value::BOOL:
        vprc->yesno(static_cast<bool>(m_value));
        break;
      case Value::STRING:
        vprc->str(static_cast<mysqlx::string>(m_value));
        break;
      case Value::RAW:
        vprc->value(cdk::TYPE_BYTES,
                    static_cast<const cdk::Format_info&>(*this),
                    Value::Access::get_bytes(m_value));
        break;
      default:
        THROW("Unexpected value type");
    }
  }

  // Trivial Format_info for raw byte values

  bool for_type(cdk::Type_info) const override { return true; }
  void get_info(cdk::Format<cdk::TYPE_BYTES>&) const override {}
  using cdk::Format_info::get_info;

};


// --------------------------------------------------------------------


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


// --------------------------------------------------------------------

/*
  DbDoc implementation which stores document data in std::map.
*/

class DbDoc::Impl
{
  class JSONDoc;

  virtual void print(std::ostream &out) const
  {
    out << "<document>";  // TODO
  }

  virtual void prepare() {}

  // Data storage

  typedef std::map<Field, Value> Map;
  Map m_map;

  bool has_field(const Field &fld)
  {
    prepare();
    return m_map.end() != m_map.find(fld);
  }

  const Value& get(const Field &fld) const
  {
    const_cast<Impl*>(this)->prepare();
    return m_map.at(fld);
  }

  // Iterating over fields of the document

  Map::iterator m_it;

  void reset() { prepare(); m_it = m_map.begin(); }

  const Field& get_current_fld() { return m_it->first; }
  void next() { ++m_it; }
  bool at_end() const { return m_it == m_map.end(); }

  struct Builder;

  friend DocResult;
  friend DbDoc;
  friend RowResult;
  friend Value::Access;
};


/*
  DbDoc::Impl specialization which takes document data from
  a JSON string.
*/

class DbDoc::Impl::JSONDoc
  : public DbDoc::Impl
{
  std::string m_json;
  bool m_parsed;

public:

  JSONDoc(const std::string &json)
    : m_json(json)
    , m_parsed(false)
  {}

  void prepare();

  void print(std::ostream &out) const
  {
    out << m_json;
  }
};


/*
  DocResult implementation over RowResult.

  This implementation takes rows from RowResult and builds a document
  using JSON data in the row.
*/


class DocResult::Impl
  : RowResult
{

  Row  m_row;

  Impl(RowResult &&init)
    : RowResult(std::move(init))
  {}

  DbDoc get_next_doc()
  {
    m_row = fetchOne();

    if (!m_row)
      return DbDoc();

    // @todo Avoid copying of document string.
    bytes data = m_row.getBytes(0);
    return DbDoc(std::string(data.begin(),data.end()-1));
  }

  uint64_t count_docs()
  {
    return count();
  }

  friend DocResult;
};


// --------------------------------------------------------------------


struct internal::XSession_base::Access
{
  typedef XSession_base::Options  Options;

  static cdk::Session& get_cdk_session(XSession_base &sess)
  {
    return sess.get_cdk_session();
  }

  static void register_result(XSession_base &sess, BaseResult *res)
  {
    sess.register_result(res);
  }
};


struct internal::BaseResult::Access
{
  static BaseResult mk_empty() { return BaseResult(); }

  template <typename A>
  static BaseResult mk(XSession_base *sess, A a)
  {
    return BaseResult(sess, a);
  }

  template <typename A, typename B>
  static BaseResult mk(XSession_base *sess, A a, B b)
  {
    return BaseResult(sess, a, b);
  }
};


// --------------------------------------------------------------------

/*
  CRUD operation implementations
  ==============================

  Templates and classes defined in public headers crud.h,
  collection_crud.h and table_crud.h define the public API of classes
  representing CRUD operations.

  Here we define templates that are used to build a hierarchy of
  classes that implement these CRUD operations. Each CRUD operation
  expects that its internal implementation object implements an
  appropriate interface defined by XXX_impl classes. The interface
  classes are structured into an inheritance chierarchy with
  Executable_impl at the bottom. Thus each CRUD implementation object
  must define the Executable_impl::execute() methods and then any
  additional methods required by the CRUD operation being implemented.
*/


/*
  Base for CRUD implementation classes which implements the following
  implementation aspects:

  - Storing values of named parameters in `m_map` member,
  - Storing limit/offset information (if any).

  This information is available in forms expected by CDK:

  - get_params() returns pointer to cdk::Param_source (NULL if no
    parameter values were specified),
  - get_limit() returns a pointer to cdk::Limit (NULL if no
    limit/offset was specified).

  This class also handles the final execution of an operation, which
  is performed as follows (see method `wait`).

  1. First, appropriate CRUD operation is sent to the server using
     underlying CDK session. This produces a cdk::Reply object which
     is used for further processing. Sending the CRUD operation is
     performed by method `send_command` which should be overwriten by
     derived class. Derived class has access to the underlying CDK session
     with method `get_cdk_session()`.

  2. After getting cdk::Reply object implementation waits for it to
     receive server reply and then returns BaseResult instance created
     from the cdk::Reply object.

  The Op_base template is parametrized by the implementation interface
  `Impl` that derived class wants to implement. The Op_base template
  implements some of the interface methods, other templates and derived
  class should implement the rest.
*/

template<class Impl>
class Op_base
  : public Impl
  , public cdk::Limit
  , public cdk::Param_source
{
protected:

  internal::XSession_base   *m_sess;

  /*
    Note: using cdk::scoped_ptr to be able to trnasfer ownership
    to a different object.
  */
  cdk::scoped_ptr<cdk::Reply> m_reply;

  row_count_t m_limit = 0;
  bool m_has_limit = false;
  row_count_t m_offset = 0;
  bool m_has_offset = false;

  typedef std::map<string, Value> param_map_t;
  param_map_t m_map;


  Op_base(internal::XSession_base &sess)
    : m_sess(&sess)
  {}
  Op_base(Collection &coll)
    : m_sess(&coll.getSession())
  {}
  Op_base(Table &tbl)
    : m_sess(&tbl.getSession())
  {}
  Op_base(const Op_base& other)
    : m_sess       (other.m_sess      )
    , m_limit      (other.m_limit     )
    , m_has_limit  (other.m_has_limit )
    , m_offset     (other.m_offset    )
    , m_has_offset (other.m_has_offset)
    , m_map        (other.m_map       )
  {}

  virtual ~Op_base()
  {}

  cdk::Session& get_cdk_session()
  {
    assert(m_sess);
    return internal::XSession_base::Access::get_cdk_session(*m_sess);
  }

  virtual cdk::Reply* send_command() = 0;

  /*
    Given cdk reply object for the statement, return BaseResult object
    that handles that reply. The reply pointer can be NULL in case no
    reply has been generated for the statement (TODO: explain in what
    scenario reply can be NULL).

    The returned BaseResult object should take ownership of the cdk reply
    object passed here (if any).
  */

  virtual internal::BaseResult mk_result(cdk::Reply *reply)
  {
    return reply ? internal::BaseResult::Access::mk(m_sess, reply)
      : internal::BaseResult::Access::mk_empty();
  }

  // Limit and offset

  void set_limit(unsigned lm)
  {
    m_has_limit = true;
    m_limit = lm;
  }

  void set_offset(unsigned offset)
  {
    m_has_offset = true;
    m_offset = offset;
  }

  cdk::Limit* get_limit()
  {
    return m_has_limit || m_has_offset ? this : nullptr;
  }


  // Parameters

  void add_param(const mysqlx::string &name, Value &&val)
  {
    auto el = m_map.emplace(name, std::move(val));
    //substitute if exists
    if (!el.second)
    {
      el.first->second = std::move(val);
    }
  }

  void clear_params()
  {
    m_map.clear();
  }

  cdk::Param_source* get_params()
  {
    return m_map.empty() ? nullptr : this;
  }


  // Async execution

  bool m_inited = false;
  bool m_completed = false;

  void init()
  {
    if (m_inited)
      return;
    m_inited = true;
    m_reply.reset(send_command());
  }

  bool is_completed()
  {
    if (m_completed)
      return true;

    init();
    m_completed = (!m_reply) || m_reply->is_completed();
    return m_completed;
  }

  void cont()
  {
    if (m_completed)
      return;
    init();
    if (m_reply)
      m_reply->cont();
  }

  internal::BaseResult wait()
  {
    init();
    if (m_reply)
    {
      m_reply->wait();
      if (0 < m_reply->entry_count())
        m_reply->get_error().rethrow();
    }
    return get_result();
  }

  internal::BaseResult get_result()
  {
    if (!is_completed())
      THROW("Attempt to get result of incomplete operation");

    /*
      Note: result created by mk_result() takes ownership of the cdk::Reply
      object.
    */

    return mk_result(m_reply.release());
  }


  // Synchronous execution

  internal::BaseResult execute()
  {
    // Deregister current Result, before creating a new one
    internal::XSession_base::Access::register_result(*m_sess, NULL);

    if (m_completed)
      THROW("Can not execute operation for the second time");
    return wait();
  }


  // cdk::Limit interface

  row_count_t get_row_count() const { return m_limit; }
  const row_count_t* get_offset() const
  {
    return m_has_offset ? &m_offset : NULL;
  }


  // cdk::Param_source

  void process(Processor &prc) const
  {
    prc.doc_begin();

    Value_converter conv;

    for (auto it : m_map)
    {
      Value_expr value(it.second, parser::Parser_mode::DOCUMENT);
      conv.reset(value);
      conv.process_if(prc.key_val(it.first));
    }
    prc.doc_end();
  }

};


/*
  This template adds handling of order specifications on top of Op_base.

  It implements the `add_order` method required by implementations of
  CRUD operations that support ordering. Ordering information is stored
  internally and transformed to the form expected by CDK: method
  `get_order_by` returns pointer to cdk::Order_by or NULL if no order
  specifications were given.

  Template parameters are the implementation interface class being
  implemented and parser mode for parsing expressions in order
  specifications.
*/

template <class Impl, parser::Parser_mode::value PM>
class Op_sort
  : public Op_base<Impl>
  , public cdk::Order_by
{
  std::list<cdk::string> m_order;

  void add_sort(const mysqlx::string &sort)
  {
    m_order.push_back(sort);
  }

protected:

  template <class X,
            typename std::enable_if<
              std::is_convertible<X*, DatabaseObject*>::value
              >::type* = nullptr
            >
  Op_sort(X &x) : Op_base<Impl>(x)
  {}


public:

  cdk::Order_by* get_order_by()
  {
    return m_order.empty() ? nullptr : this;
  }

private:

  // cdk::Order_by interface

  void process(Order_by::Processor& prc) const
  {
    prc.list_begin();

    for (cdk::string el : m_order)
    {

      parser::Order_parser order_parser(PM, el);
      order_parser.process_if(prc.list_el());

    }

    prc.list_end();
  }
};


/*
  This template adds handling of having specification on top of
  Op_sort.

  It implements the `set_having` method required by implementations of
  CRUD operations that support having expression. The Expression is stored
  internally and transformed to the form expected by CDK:
  method `get_having` return pointer to cdk::Expression (as expected
  for table/collection having definition) or NULL if no having specification
  is given.

  Template parameters are the implementation interface class being
  implemented and parser mode for parsing expressions in order
  specifications.
*/

template <class Impl, parser::Parser_mode::value PM>
class Op_having
  : public Op_sort<Impl, PM>
  , public cdk::Expression
{
  cdk::string m_having;

  void set_having(const mysqlx::string &having)
  {
    m_having = having;
  }

protected:

  template <class X,
            typename std::enable_if<
              std::is_convertible<X*, DatabaseObject*>::value
              >::type* = nullptr
            >
  Op_having(X &x) : Op_sort<Impl,PM>(x)
  {}

public:

  cdk::Expression* get_having()
  {
    return m_having.empty() ? nullptr : this;
  }

private:

  // cdk::Expression processor

  void process(cdk::Expression::  Processor& prc) const
  {
    parser::Expression_parser expr_parser(PM, m_having);
    expr_parser.process(prc);
  }
};


/*
  This template adds handling of groupBy specification on top of
  Op_having.

  It implements the `add_group_by` method required by implementations of
  CRUD operations that support groupBy expressions. The Expressions are stored
  internally and transformed to the form expected by CDK:
  method `get_group_by` return pointer to cdk::Expr_list (as expected
  for table/collection groupBy definition) or NULL if no groupBy specification
  is given.

  Template parameters are the implementation interface class being
  implemented and parser mode for parsing expressions in order
  specifications.
*/

template <class Impl, parser::Parser_mode::value PM>
class Op_group_by
  : public Op_having<Impl, PM>
  , public cdk::Expr_list
{
  std::vector<cdk::string> m_group_by;

  void add_group_by(const mysqlx::string &group_by)
  {
    m_group_by.push_back(group_by);
  }

protected:

  template <class X,
            typename std::enable_if<
              std::is_convertible<X*, DatabaseObject*>::value
              >::type* = nullptr
            >
  Op_group_by(X &x) : Op_having<Impl,PM>(x)
  {}

public:

  cdk::Expr_list* get_group_by()
  {
    return m_group_by.empty() ? nullptr : this;
  }

private:

  void process(cdk::Expr_list::Processor& prc) const
  {
    prc.list_begin();

    for (cdk::string el : m_group_by)
    {
      parser::Expression_parser expr_parser(PM, el);
      expr_parser.process_if(prc.list_el());
    }

    prc.list_end();
  }
};


/*
  This template adds handling of projection specifications on top
  of Op_group_by.

  It implements the `add_proj` method required by implementations of
  CRUD operations that support projection. Projection specifications
  are stored internally and transformed to the form expected by CDK:
  methods `get_xxx_proj` return pointer to cdk::Projection (as expected
  for table projections) or cdk::Expression::Document (as expected by
  document projections or NULL if no projection specifications were
  given.

  Template parameters are the implementation interface class being
  implemented and parser mode for parsing expressions in projection
  specifications.
*/

template <class Impl, parser::Parser_mode::value PM>
class Op_projection
    : public Op_group_by<Impl, PM>
    , public cdk::Projection
    , public cdk::Expression::Document
{

  std::vector<cdk::string> m_projections;
  cdk::string  m_doc_proj;

protected:


  template <class X,
            typename std::enable_if<
              std::is_convertible<X*, DatabaseObject*>::value
              >::type* = nullptr
            >
  Op_projection(X &init) : Op_group_by<Impl,PM>(init)
  {}

public:

  void set_proj(const mysqlx::string& doc)
  {
    m_doc_proj = doc;
  }

  void add_proj(const mysqlx::string& field)
  {
    m_projections.push_back(field);
  }

  cdk::Projection* get_tbl_proj()
  {
    return m_projections.empty() ? nullptr : this;
  }

  cdk::Expression::Document* get_doc_proj()
  {
    return m_projections.empty() && m_doc_proj.empty() ? nullptr : this;
  }

private:

  void process(cdk::Expression::Document::Processor& prc) const
  {
    if (!m_doc_proj.empty())
    {
      struct : public cdk::Expression::Processor
      {
        Doc_prc *m_prc;

        Scalar_prc* scalar()
        {
          throw_error("Scalar expression can not be used as projection");
          return NULL;
        }

        List_prc* arr()
        {
          throw_error("Array expression can not be used as projection");
          return NULL;
        }

        // Report that any value is a document.

        Doc_prc* doc()
        {
          return m_prc;
        }

      }
      eprc;

      eprc.m_prc = &prc;

      parser::Expression_parser parser(parser::Parser_mode::DOCUMENT, m_doc_proj);

      parser.process(eprc);

      return;
    }

    prc.doc_begin();

    for (cdk::string field : m_projections)
    {
      parser::Projection_parser expr_parser(PM, field);
      expr_parser.process(prc);
    }

    prc.doc_end();

  }

  void process(cdk::Projection::Processor& prc) const
  {
    prc.list_begin();

    for (cdk::string el : m_projections)
    {

      parser::Projection_parser order_parser(PM, el);
      auto prc_el = prc.list_el();
      if (prc_el)
        order_parser.process(*prc_el);

    }

    prc.list_end();

  }

};


/*
  This template adds handling of selection criteria on top
  of Op_projection.

  It implements the `add_where` method required by implementations of
  CRUD operations that support document/row selection. The selection
  criteria is transformed into the form expected by cdk: method
  `get_where()` returns pointer to cdk::Expression or NULL if no
  selection criteria was specified.

  Template parameters are the base class which can be one of the other
  implementation templates and parser mode for parsing selection
  expression.
*/

template <class Base, parser::Parser_mode::value PM>
class Op_select : public Base
{
protected:

  mysqlx::string m_where_expr;
  std::unique_ptr<parser::Expression_parser> m_expr;

  template <class X,
            typename std::enable_if<
              std::is_convertible<X*, DatabaseObject*>::value
              >::type* = nullptr
            >
  Op_select(X &init) : Base(init)
  {}

  Op_select(const Op_select &other)
    : Base(other)
    , m_where_expr(other.m_where_expr)
  {
    if (!m_where_expr.empty())
      m_expr.reset(new parser::Expression_parser(PM, m_where_expr));
  }

public:

  void add_where(const mysqlx::string &expr)
  {
    m_where_expr = expr;
    if (!m_where_expr.empty())
      m_expr.reset(new parser::Expression_parser(PM, m_where_expr));
  }

  cdk::Expression* get_where() const
  {
    return m_expr.get();
  }
};


}

#endif
