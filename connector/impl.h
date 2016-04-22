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

#ifndef MYSQLX_IMPL_H
#define MYSQLX_IMPL_H

/*
  Internal implementations for public DevAPI classes.
*/

#include <mysqlx.h>
#include <mysql/cdk.h>
#include <mysql/cdk/converters.h>
#include <expr_parser.h>
#include <map>
#include <memory>
#include <stack>


namespace mysqlx {

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


class Value_prc
    : public cdk::Expression
{
  parser::Parser_mode::value m_parser_mode;
  Value m_value;
  bool m_is_expr = false;


  //Private constructor to be used only inside Value_prc class
  Value_prc(const Value &val,
            bool is_expr,
            parser::Parser_mode::value parser_mode)
    : m_parser_mode(parser_mode)
    , m_is_expr(is_expr)
    , m_value(val)
  {}

public:
  Value_prc(const Value &val, parser::Parser_mode::value parser_mode)
    : m_parser_mode(parser_mode)
    , m_value(val)
  {}

  Value_prc(Value &&val, parser::Parser_mode::value parser_mode)
    : m_parser_mode(parser_mode)
    , m_value(std::move(val))
  {}

  Value_prc(const ExprValue &val, parser::Parser_mode::value parser_mode)
    : m_parser_mode(parser_mode)
    , m_value(val)
  {
    m_is_expr = val.isExpression();
  }

  Value_prc(ExprValue &&val, parser::Parser_mode::value parser_mode)
    : m_parser_mode(parser_mode)
    , m_value(std::move(val))
  {
    m_is_expr = val.isExpression();
  }

  void process (Processor &prc) const
  {
    switch (m_value.getType())
    {
      case Value::VNULL:
        safe_prc(prc)->scalar()->val()->null();
        break;
      case Value::UINT64:
        safe_prc(prc)->scalar()->val()->num(static_cast<uint64_t>(m_value));
        break;
      case Value::INT64:
        safe_prc(prc)->scalar()->val()->num(static_cast<int64_t>(m_value));
        break;
      case Value::FLOAT:
        safe_prc(prc)->scalar()->val()->num(static_cast<float>(m_value));
        break;
      case Value::DOUBLE:
        safe_prc(prc)->scalar()->val()->num(static_cast<double>(m_value));
        break;
      case Value::BOOL:
        safe_prc(prc)->scalar()->val()->yesno(static_cast<bool>(m_value));
        break;
      case Value::STRING:
        if (m_is_expr)
        {
          parser::Expression_parser expr(m_parser_mode,
                                          (mysqlx::string)m_value);

          expr.process(prc);
        }
        else
          safe_prc(prc)->scalar()->val()->str(static_cast<mysqlx::string>(m_value));
        break;
      case Value::DOCUMENT:
        {
          mysqlx::DbDoc doc = static_cast<mysqlx::DbDoc>(m_value);
          safe_prc(prc)->doc()->doc_begin();
          for ( Field fld : doc)
          {
            Value_prc value(doc[fld], m_is_expr, m_parser_mode);
            value.process_if(safe_prc(prc)->doc()->key_val(fld));
          }
          safe_prc(prc)->doc()->doc_end();
        }
        break;
      case Value::RAW:
        THROW("Unexpected Value Type RAW");
        break;
      case Value::ARRAY:
        {
          safe_prc(prc)->arr()->list_begin();
          for (Value val : m_value)
          {
            Value_prc value(val, m_is_expr, m_parser_mode);
            value.process_if(safe_prc(prc)->arr()->list_el());
          }
          safe_prc(prc)->arr()->list_end();
        }
        break;
    }
  }

};


struct Value::Access
{
  static Value mk_raw(const cdk::bytes data)
  {
    Value ret;
    ret.m_type = Value::RAW;
    ret.m_str.assign(data.begin(), data.end());
    return std::move(ret);
  }

  static Value mk_doc(const string &json)
  {
    Value ret;
    ret.m_type = Value::DOCUMENT;
    ret.m_doc = DbDoc(json);
    return std::move(ret);
  }

  static cdk::bytes get_bytes(const Value &val)
  {
    return cdk::bytes(val.m_str);
  }
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

/**
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

  Value& get(const Field &fld)
  {
    prepare();
    return m_map.at(fld);
  }

  // Iterating over fields of the document

  Map::iterator m_it;

  void reset() { prepare(); m_it = m_map.begin(); }

  const Field& get_current_fld() { return m_it->first; }
  void next() { ++m_it; }
  bool at_end() const { return m_it == m_map.end(); }

  struct Builder;

  friend class DocResult;
  friend class DbDoc;
  friend class RowDoc;
};


/**
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


/**
  DocResult implementation over RowResult.

  This implementation takes rows from RowResult and builds a document
  using JSON data in the row.
*/

class DocResult::Impl
  : RowResult
{
  Row  m_row;

  Impl(BaseResult &init)
    : RowResult(std::move(init))
  {
    next_doc();
  }

  DbDoc get_current_doc()
  {
    if (!m_row)
      return DbDoc();

    // @todo Avoid copying of document string.

    bytes data = m_row.getBytes(0);
    return DbDoc(std::string(data.begin(), data.end() - 1));
  }

  bool has_doc() const
  {
    return (bool)m_row;
  }

  void next_doc()
  {
    m_row = fetchOne();
  }

  friend class DocResult;
};


// --------------------------------------------------------------------


struct internal::BaseResult::Access
{
  static BaseResult mk_empty() { return BaseResult(); }

  template <typename A>
  static BaseResult mk(A a) { return BaseResult(a); }

  template <typename A, typename B>
  static BaseResult mk(A a, B b) { return BaseResult(a, b); }
};


struct Task::Access
{
  typedef Task::Impl Impl;

  static void reset(Task &task, Impl *impl)
  {
    task.reset(impl);
  }

  static Impl* get_impl(Task &task)
  {
    if (!task.m_impl)
      THROW("Empty task implementation");
    return task.m_impl;
  }
};


struct internal::PlainExecutable::Access
{
  static void reset_task(PlainExecutable &exec, Task::Impl *impl)
  {
    exec.m_task.reset(impl);
  }

  static Task::Access::Impl* get_impl(PlainExecutable &exec)
  {
    exec.check_if_valid();
    return Task::Access::get_impl(exec.m_task);
  }
};


struct Executable::Access
{
  static void reset_task(Executable &exec, Task::Impl *impl)
  {
    exec.m_task.reset(impl);
    exec.m_map.clear();
  }

  static Task::Access::Impl* get_impl(Executable &exec)
  {
    exec.check_if_valid();
    return Task::Access::get_impl(exec.m_task);
  }
};


// --------------------------------------------------------------------

/*
  Task implementation
  ===================
*/

class Task::Impl : nocopy
{
protected:

  XSession &m_sess;
  cdk::Reply *m_reply = NULL;

  typedef Executable::param_map_t param_map_t;

  Impl(XSession &sess)
    : m_sess(sess)
  {}
  Impl(Collection &coll)
    : m_sess(coll.m_schema.m_sess)
  {}
  Impl(Table &tbl)
    : m_sess(tbl.m_schema.m_sess)
  {}

  virtual ~Impl() {}

  virtual cdk::Reply* send_command() = 0;

  cdk::Session& get_cdk_session() { return m_sess.get_cdk_session(); }


  struct Params : public cdk::Param_source
  {
    param_map_t *m_map = NULL;

    void process(Processor &prc) const
    {
      prc.doc_begin();

      Value_converter conv;

      for (auto it : *m_map)
      {
        Value_prc value(it.second, parser::Parser_mode::DOCUMENT);
        conv.reset(value);
        conv.process_if(prc.key_val(it.first));
      }
      prc.doc_end();
    }
  }
  m_params;

  cdk::Param_source* get_params()
  {
    return m_params.m_map ? &m_params : NULL;
  }

  void set_params(param_map_t &param_map)
  {
    m_params.m_map = &param_map;
  }

  void init()
  {
    if (m_reply)
      return;
    m_reply = send_command();
  }

  bool is_completed()
  {
    init();
    return m_reply->is_completed();
  }

  void cont()
  {
    init();
    m_reply->cont();
  }

  internal::BaseResult wait()
  {
    init();
    m_reply->wait();
    if (0 < m_reply->entry_count())
      m_reply->get_error().rethrow();
    return get_result();
  }

  virtual internal::BaseResult get_result()
  {
    return internal::BaseResult::Access::mk(m_reply);
  }

  friend class Task;
  friend class Executable;
//  friend class BindExec;
};


// --------------------------------------------------------------------

template <class X> struct Crud_impl;

template <class X>
inline
typename Crud_impl<X>::type& get_impl(X *p)
{
  typedef typename Crud_impl<X>::type Op_type;
  return *static_cast<Op_type*>(Executable::Access::get_impl(*p));
}


// --------------------------------------------------------------------

/*
  Helper classes for CRUD operations.
*/


class Op_base
  : public Task::Access::Impl
  , public cdk::Limit
{
protected:

  row_count_t m_limit = 0;
  bool m_has_limit = false;
  row_count_t m_offset = 0;
  bool m_has_offset = false;

  std::vector<cdk::string> m_order;

  template <typename T>
  Op_base(T &x) : Impl(x)
  {}

public:

  void limit(row_count_t lm)
  {
    m_has_limit = true;
    m_limit = lm;
  }

  void offset(row_count_t _offset)
  {
    m_has_offset = true;
    m_offset = _offset;
  }

  // cdk::Limit interface
  row_count_t get_row_count() const override { return m_limit; }
  const row_count_t* get_offset() const override
  {
    return m_has_offset ? &m_offset : NULL;
  }

  friend class internal::SortBase<false>;
  friend class internal::SortBase<true>;
};


template <parser::Parser_mode::value PM>
class Op_sort
  : public Op_base
  , public cdk::Order_by
{
protected:

  template <typename T>
  Op_sort(T &x) : Op_base(x)
  {}

public:

  // cdk::Order_by interface
  void process(Processor& prc) const
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


}

#endif
