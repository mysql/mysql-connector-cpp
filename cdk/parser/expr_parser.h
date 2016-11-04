/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * This code is licensed under the terms of the GPLv2
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

#ifndef _EXPR_PARSER_H_
#define _EXPR_PARSER_H_

#include <mysql/cdk/common.h>
#include "parser.h"

PUSH_SYS_WARNINGS
#include <vector>
#include <map>
POP_SYS_WARNINGS


/*
  Parsing strings containing expressions as used by DevAPI.
*/

namespace parser {

using cdk::scoped_ptr;
using cdk::Expression;


// ------------------------------------------------------------------------------

/*
  Helper classes that are used to store column references and document
  paths within the parser.
*/

struct Table_ref : public cdk::api::Table_ref
{
  struct : public cdk::api::Schema_ref
  {
    cdk::string m_name;

    virtual const cdk::string name() const { return m_name; }

  } m_schema_ref;

  cdk::string m_name;

  virtual const cdk::string name() const { return m_name; }

  virtual const cdk::api::Schema_ref* schema() const
  { return m_schema_ref.m_name.empty() ? NULL : &m_schema_ref; }

  void set(const cdk::string &name)
  { m_name = name; }

  void set(const cdk::string &name, const cdk::string &schema)
  {
    m_name = name;
    m_schema_ref.m_name = schema;
  }

  void clear()
  {
    m_name.clear();
    m_schema_ref.m_name.clear();
  }

};


struct Column_ref : public cdk::api::Column_ref
{
  Table_ref m_table_ref;


  cdk::string m_col_name;

  virtual const cdk::string name() const
  { return m_col_name; }

  virtual const cdk::api::Table_ref *table() const
  {
    return m_table_ref.m_name.empty() ? NULL :  &m_table_ref;
  }


  void set_name(const cdk::string &name)
  { m_col_name = name; }

  void set(const cdk::string &name)
  {
    m_table_ref.clear();
    set_name(name);
  }

  void set(const cdk::string &name, const cdk::string &table)
  {
    set(name);
    m_table_ref.set(table);
  }

  void set(const cdk::string &name,
           const cdk::string &table, const cdk::string &schema)
  {
    set(name);
    m_table_ref.set(table, schema);
  }


  Column_ref& operator=(const cdk::api::Column_ref &other)
  {
    m_col_name = other.name();
    if (!other.table())
      return *this;
    if (other.table()->schema())
      m_table_ref.set(other.table()->name(), other.table()->schema()->name());
    else
      m_table_ref.set(other.table()->name());
    return *this;
  }

  void clear()
  {
    m_col_name.clear();
    m_table_ref.clear();
  }
};


/*
  Trivial Format_info class that is used to report opaque blob values.
*/

struct Format_info : public cdk::Format_info
{
  bool for_type(cdk::Type_info ti) const { return cdk::TYPE_BYTES == ti; }
  void get_info(cdk::Format<cdk::TYPE_BYTES>&) const {}

  // bring in the rest of overloads that would be hidden otherwise
  // (avoid compiler warning)
  using cdk::Format_info::get_info;
};


// ------------------------------------------------------------------------------


/*
  Main parser class containing parsing logic. An instance acts
  as Expression, that is, parsed expression can be visited
  by expression processor with process() method.

  There are 2 parsing modes which determine what kind of value references
  are allowed within expression. In TABLE mode expression can refer
  to columns of a table, in DOCUMENT mode it can refer to document elements
  specified by a document path.
*/

struct Parser_mode
{
  enum value { DOCUMENT, TABLE};
};


class Expr_parser_base
    : public Expr_parser<Expression::Processor>
{

public:

  typedef Expression::Processor Processor;
  typedef Expression::Scalar::Processor Scalar_prc;
  typedef cdk::api::Doc_path::Processor Path_prc;

  static Expression::Processor *get_base_prc(Processor *prc)
  { return prc; }

protected:

  /*
    TODO: Temporary hack to meet current specs that inside expression
    "x IN ('a', 'b', 'c')" the strings in the list should be reported as
    octets, not as strings. This will probably change but in the meantime,
    to not fail the tests, the m_strings_as_blobs flag tells to report
    strings as octets. This happens only in case of IN expression
    (see parse_iliri() method).
  */

  Parser_mode::value m_parser_mode;
  bool m_strings_as_blobs;

  Expr_parser_base(It &first, const It &last,
                   Parser_mode::value parser_mode,
                   bool strings_as_blobs = false)
    : Expr_parser<Expression::Processor>(first, last)
    , m_parser_mode(parser_mode)
    , m_strings_as_blobs(strings_as_blobs)
  {
    return;
  }


  bool do_parse(It &first, const It &last, Processor *prc);


  enum Start { FULL, ATOMIC, MUL, ADD, SHIFT, BIT, COMP, ILRI, AND, OR,
               CAST_TYPE, COLID_DOCPATH, DOC, ARR};

  /*
    Parse tokens using given starting point of the expression grammar.

    If processor is not NULL, the expression is reported directly to the
    processor and this method returns NULL.

    Otherwise, if processor is NULL, the result of parsing is stored
    and returned from the method. Caller of this method takes ownership
    of the returned Expression object.
  */

  Expression* parse(Start, Processor*);

  // methods for parsing different kinds of expressions.

  Expression* parse_atomic(Processor*);
  Expression* parse_mul(Processor*);
  Expression* parse_add(Processor*);
  Expression* parse_shift(Processor*);
  Expression* parse_bit(Processor*);
  Expression* parse_comp(Processor*);
  Expression* parse_ilri(Processor*);
  Expression* parse_and(Processor*);
  Expression* parse_or(Processor*);

  // Additional helper parsing methods.

  Expression* left_assoc_binary_op(TokSet, Start, Start, Processor*);

  void parse_function_call(Scalar_prc*);
  bool parse_function_call(const cdk::api::Table_ref&, Scalar_prc*);
  void parse_argslist(Expression::List::Processor*,
                      bool strings_as_blobs = false);

  void parse_schema_ident(Token::TokenType (*types)[2] = NULL);
  void parse_column_ident(Path_prc*);
  void parse_column_ident1(Path_prc*);
  const std::string &get_ident();

  void parse_document_field(Path_prc*, bool prefix = false);
  void parse_document_field(const cdk::string&, Path_prc*);
  void parse_document_field(const cdk::string&, const cdk::string&, Path_prc*);

  bool parse_document_path(Path_prc*, bool require_dot=false);
  bool parse_document_path1(Path_prc*);
  bool parse_docpath_member(Path_prc::Element_prc*);
  void parse_docpath_array_loc(Path_prc::Element_prc*);

  void parse_cast(Scalar_prc*);
  cdk::string parse_cast_type();
  std::string cast_data_type_dimension(bool double_dimension = false);
  std::string opt_binary();

  void parse_char(Scalar_prc*);

  void parse_doc(Processor::Doc_prc*);
  void parse_arr(Processor::List_prc*);

private:

  Column_ref m_col_ref;

  friend class Expression_parser;
  friend class Order_parser;
  friend class Projection_parser;
  friend class Table_field_parser;
  friend class Doc_field_parser;
};


class Expression_parser
  : public Expression
{
  Tokenizer m_tokenizer;
  Parser_mode::value m_mode;

public:

  Expression_parser(Parser_mode::value parser_mode)
    : m_tokenizer(std::string()), m_mode(parser_mode)
  {}

  Expression_parser(Parser_mode::value parser_mode, const cdk::string &expr)
    : m_tokenizer(expr), m_mode(parser_mode)
  {
    m_tokenizer.get_tokens();
  }


  void process(Processor &prc) const
  {
    if (!const_cast<Expression_parser*>(this)->m_tokenizer.tokens_available())
      cdk::throw_error("Expression_parser: empty string");

    It first = m_tokenizer.begin();
    It last  = m_tokenizer.end();

    /*
      note: passing m_toks.end() directly as constructor argument results
      in "incompatible iterators" exception when comparing iterators (at
      least on win, vs2010). problem with passing temporary object?
    */

    Expr_parser_base parser(first, last, m_mode);
    parser.process(prc);

    if (first != last)
      cdk::throw_error("Expression_parser: could not parse string as expression"
                       " (not all tokens consumed)");
  }

};


/**
   @brief The Order_parser class parses "<expr> [ASC|DESC]" using
          Order_expr_processor

          This parser can process api::Order_expr<Expression>.

          Usage:

          When processing api::Order_by::Processor user needs to call first
          Processor::list_begin() and then pass the processor returned by
          Processor::list_el() to  Order_parser for each projection.
          In the end, Processor::list_end() has to be called.
 */

class Order_parser
    : public cdk::api::Order_expr<Expression>
{
  Tokenizer m_tokenizer;
  Parser_mode::value m_mode;

public:

  Order_parser(Parser_mode::value parser_mode, const cdk::string &expr)
  : m_tokenizer(expr), m_mode(parser_mode)
  {
    m_tokenizer.get_tokens();
  }

  void process(Processor &prc) const;

};


/**
   @brief The Projection_parser class parses "<expr> AS <alias>"
          specifications. When used in table mode the "AS <alias>" part
          is optional, otherwise error is thrown if it is not present.

          This parser has can process 2 processor types:
          api::Projection_expr<Expression>::Processor
          Expression::Document::Processor

          Usage:

          When processing api::Projection::Processor user needs to call first
          Processor::list_begin() and then pass the processor returned by
          Processor::list_el() to  Projection_parser for each projection.
          In the end, Processor::list_end() has to be called.

          When processing Expression::Document::Processor user will call
          Processor::doc_begin() and then pass the processor to
          Projection_parser for each projection.
          In the end user will call Processor::doc_end()
 */

class Projection_parser
    : public cdk::api::Projection_expr<Expression>
    , public cdk::Expression::Document
{
  typedef cdk::api::Projection_expr<Expression>::Processor Projection_processor;
  typedef cdk::Expression::Document::Processor Document_processor;
  Tokenizer m_tokenizer;
  Parser_mode::value m_mode;

public:

  Projection_parser(Parser_mode::value parser_mode)
  : m_tokenizer(std::string()), m_mode(parser_mode)
  {}

  Projection_parser(Parser_mode::value parser_mode, const cdk::string &expr)
  : m_tokenizer(expr), m_mode(parser_mode)
  {
    m_tokenizer.get_tokens();
  }


  void process(Projection_processor &prc) const;

  void process(Document_processor &prc) const;

};

/*
  Class Used to parse Table fields.
  Format: table.column->@.field.arr[]
*/

class Table_field_parser
    : public cdk::api::Column_ref
    , public cdk::Doc_path
{
  parser::Column_ref    m_col;
  cdk::Doc_path_storage m_path;

public:

  Table_field_parser(const cdk::string &table_field)
  {
    Tokenizer toks(table_field);
    toks.get_tokens();

    It begin = toks.begin();
    const It end = toks.end();

    Expr_parser_base parser(begin, end, Parser_mode::TABLE);
    parser.parse_column_ident(&m_path);
    m_col = parser.m_col_ref;
  }

  const cdk::string name() const
  {
    return m_col.name();
  }

  const cdk::api::Table_ref *table() const
  {
    return m_col.table();
  }

  bool has_path() const
  {
    return !m_path.is_empty();
  }

  void process(Processor &prc) const
  {
    m_path.process(prc);
  }

};


/*
  This class acts as cdk::Doc_path object taking path data from a string
  containing document field specification (documentField grammar)
*/

class Doc_field_parser
    : public cdk::Doc_path
{
  Tokenizer m_tokenizer;
  cdk::scoped_ptr<Expr_parser_base> m_parser;
  It m_it;

public:

  Doc_field_parser(const cdk::string &doc_path)
    : m_tokenizer(doc_path)
  {
    m_tokenizer.get_tokens();

    m_it = m_tokenizer.begin();
    const It end = m_tokenizer.end();
    m_parser.reset(new Expr_parser_base(m_it, end, Parser_mode::DOCUMENT));
  }

  void process(Processor &prc) const
  {
    const_cast<Expr_parser_base*>(m_parser.get())->parse_document_field(&prc);
  }
};

// ------------------------------------------------------------------------------

/*
  Infrastructure for storing partial parsing results.

  An infix operator expression of the form:

    LHS OP RHS

  is reported to an expression processor so that first the name of the
  operator is reported, then the list of arguments (LHS,RHS).

  For that reason expression parser can not report LHS expression to the
  processor on-the-fly, while parsing it. Only after seeing and reporting
  OP, it can report LHS expression to the processor. Therefore, parser
  needs to store the result of parsing LHS for later reporting.

  Classes defined here provide infrastructure for storing parsed expression.
  An instance of Stored_expr class acts as expression processor and as an
  expression. When used as expression processor, it stores the expression
  reported to it via processor callbacks. Later, when used as an expression,
  it "re-plays" the stored expression to a given processor.

*/

struct Stored_expr
  : public Expression
  , public Expression::Processor
{
  virtual ~Stored_expr() {}
};


struct Stored_scalar;
struct Stored_list;
struct Stored_doc;


/*
  Storage for any kind of expression: either scalar, array or document.
*/

struct Stored_any
  : public Stored_expr
{
  scoped_ptr<Stored_scalar> m_scalar;
  scoped_ptr<Stored_list>   m_arr;
  scoped_ptr<Stored_doc>    m_doc;

  void process(Processor &prc) const;

  Scalar_prc* scalar();
  List_prc*   arr();
  Doc_prc*    doc();
};


// --------------------------------------------------------------------------


struct Stored_list
  : public Expression::List
  , public Expression::List::Processor
{
  typedef std::vector<Stored_expr*> Ptr_list;
  Ptr_list m_elements;

  ~Stored_list()
  {
    std::for_each(m_elements.begin(), m_elements.end(), delete_el);
  }

  static void delete_el(Stored_expr *ptr)
  {
    delete ptr;
  }

  // List expression (report stored list)

  void process(Processor &prc) const
  {
    for (Ptr_list::const_iterator it = m_elements.begin();
         it != m_elements.end();
         ++it)
      (*it)->process_if(prc.list_el());
  }

  // List processor (store list)

  void list_begin()
  {
    m_elements.clear();
  }

  void list_end() {}

  Element_prc* list_el()
  {
    Stored_expr *e = new Stored_any();
    m_elements.push_back(e);
    return e;
  }
};


struct Stored_doc
  : public Expression::Document
  , public Expression::Document::Processor
{
  typedef std::map<cdk::string, Stored_expr*> Ptr_map;
  Ptr_map m_keyval_map;

  // Doc expression (report stored doc)

  void process(Processor &prc) const
  {
    prc.doc_begin();
    for (Ptr_map::const_iterator it = m_keyval_map.begin();
         it != m_keyval_map.end();
         ++it)
    {
      Any_prc *aprc = prc.key_val(it->first);
      it->second->process_if(aprc);
    }
    prc.doc_end();
  }

  // Doc processor (store doc)

  void doc_begin()
  {
    m_keyval_map.clear();
  }

  void doc_end()
  {}

  Any_prc* key_val(const cdk::string &key)
  {
    Stored_expr *s = new Stored_any();
    m_keyval_map[key] = s;
    return s;
  }
};


/*
  Storage for base (scalar) expressions.
*/

struct Stored_scalar
  : public Expression::Scalar
  , public Expression::Scalar::Processor
  , public Expression::List::Processor
  , public cdk::Value_processor
{
  // Storage for the values

  parser::Column_ref  m_col_ref;
  cdk::Doc_path_storage m_doc_path;
  std::string m_op_name;
  cdk::string m_str;

  union {
    int64_t     m_int;
    uint64_t    m_uint;
    float       m_float;
    double      m_double;
    bool        m_bool;
  }
  m_num;

  /*
    Storage for arguments of function or operator call.

    As an optimization, the first argument in the list can be taken
    from external storage. This is used in infix operator parsing logic.
    When parsing:

      LHS OP RHS

    the parser first parses and stores LHS. Now, if the whole operator
    expression needs to be stored as well, then the stored LHS can be
    re-used for the first argument of the operator call.

    For that reason here we have m_first member which can point to
    externally created storage for the first argument (Stored_scalar
    takes ownership of all the stored arguments, including the first one).
    If m_first is empty, then only arguments stored in m_args are used.
  */

  scoped_ptr<Expression>  m_first;
  Stored_list  m_args;

  Stored_scalar() {}
  Stored_scalar(Expression *first)
  {
    m_first.reset(first);
  }

  enum { OP, FUNC, COL_REF, PATH, PARAM, VAR,
         V_NULL, V_OCTETS, V_STR, V_INT, V_UINT,
         V_FLOAT, V_DOUBLE, V_BOOL }
  m_type;

  // Scalar expression (report stored value to a processor)

  void process(Processor &prc) const
  {
    switch (m_type)
    {
    case OP:
    case FUNC:
      {
        Args_prc *argsp
          = (m_type == OP ? prc.op(m_op_name.c_str())
                          : prc.call(*m_col_ref.table()) );
        if (!argsp)
          return;

        argsp->list_begin();
        // if we have externally stored first argument, use it here
        if (m_first)
          m_first->process_if(argsp->list_el());
        // the rest of arguments
        m_args.process(*argsp);
        argsp->list_end();
      };
      break;

    case COL_REF:
      prc.ref(m_col_ref, m_doc_path.is_empty() ? NULL : &m_doc_path);
      break;

    case PATH:  prc.ref(m_doc_path); break;
    case PARAM: prc.param(m_str); break;
    case VAR:   prc.var(m_str); break;

    // literal values

    case V_NULL:   safe_prc(prc)->val()->null(); break;
    case V_STR:    safe_prc(prc)->val()->str(m_str); break;
    case V_INT:    safe_prc(prc)->val()->num(m_num.m_int); break;
    case V_UINT:   safe_prc(prc)->val()->num(m_num.m_uint); break;
    case V_FLOAT:  safe_prc(prc)->val()->num(m_num.m_float); break;
    case V_DOUBLE: safe_prc(prc)->val()->num(m_num.m_double); break;
    case V_BOOL:   safe_prc(prc)->val()->yesno(m_num.m_bool); break;

    case V_OCTETS:
      // note: this object acts as Format_info
      safe_prc(prc)->val()->value(cdk::TYPE_BYTES, Format_info(), cdk::bytes(m_op_name));
      break;
    }
  }

  // Processors (store value reported to us)

  // List processor (this is used to process function/op call argument list)

  bool m_first_el;

  void list_begin()
  {
    m_args.list_begin();
    m_first_el = true;
  }

  void list_end()
  {
    m_args.list_end();
  }

  Element_prc* list_el()
  {

    /*
      If we use externally stored first argument (m_first is not empty)
      then there is no need to look at the first argument reported to us,
      because we already have it. In this case we return NULL from here.

      If this is second or later argument, or if m_first is empty,
      we forward to m_args.
    */

    if (m_first_el)
    {
      m_first_el = false;
      if (m_first)
        return NULL;
    }
    return m_args.list_el();
  }


  // Scalar processor

  Value_prc* val()
  {
    return this;
  }

  Args_prc* op(const char *name)
  {
    m_type = OP;
    m_op_name = name;
    return this;
  }

  Args_prc* call(const Object_ref &func)
  {
    m_type = FUNC;

    // Set the table() part of m_col_ref to the name of the function

    if (func.schema())
      m_col_ref.set(string(), func.name(), func.schema()->name());
    else
      m_col_ref.set(string(), func.name());

    return this;
  }

  void ref(const Column_ref &col, const Doc_path *path)
  {
    m_type = COL_REF;
    m_col_ref   = col;
    if (path)
      path->process(m_doc_path);
  }

  void ref(const Doc_path &path)
  {
    m_type = PATH;
    path.process(m_doc_path);
  }

  void param(const string &name)
  {
    m_type = PARAM;
    m_str  = name;
  }

  virtual void param(uint16_t)
  {
    THROW("Positional parameter in expression");
  }

  virtual void var(const string &name)
  {
    m_type = VAR;
    m_str  = name;
  }

  // Value processor

  void null() { m_type = V_NULL; }

  void str(const string &val)
  {
    m_type = V_STR;
    m_str  = val;
  }

  void num(int64_t val)
  {
    m_type = V_INT;
    m_num.m_int = val;
  }

  void num(uint64_t val)
  {
    m_type = V_UINT;
    m_num.m_uint = val;
  }

  void num(float val)
  {
    m_type = V_FLOAT;
    m_num.m_float = val;
  }

  void num(double val)
  {
    m_type = V_DOUBLE;
    m_num.m_double = val;
  }

  void yesno(bool val)
  {
    m_type = V_BOOL;
    m_num.m_bool = val;
  }

  void value(cdk::Type_info, const cdk::Format_info&, cdk::bytes data)
  {
    // TODO: currently we ignore type information and treat everything
    // as opaque byte blobs.

    m_type = V_OCTETS;
    m_op_name.assign(data.begin(), data.end());
  }

};

// --------------------------------------------------------------------------


inline
void Stored_any::process(Processor &prc) const
{
  if (m_scalar)
    return m_scalar->process_if(prc.scalar());

  if (m_arr)
  {
    List_prc *lprc = prc.arr();
    if (!lprc)
      return;
    lprc->list_begin();
    m_arr->process(*lprc);
    lprc->list_end();
    return;
  }

  if (m_doc)
    return m_doc->process_if(prc.doc());
}

inline
Stored_any::Scalar_prc* Stored_any::scalar()
{
  m_scalar.reset(new Stored_scalar());
  return m_scalar.get();
}

inline
Stored_any::List_prc* Stored_any::arr()
{
  m_arr.reset(new Stored_list());
  return m_arr.get();
}

inline
Stored_any::Doc_prc* Stored_any::doc()
{
  m_doc.reset(new Stored_doc());
  return m_doc.get();
}


/*
  Storage of operator call expression that can re-use already
  stored LHS expression.
*/

struct Stored_op
  : public Stored_expr
  , public Stored_scalar
{
  using Stored_expr::process;
  using Stored_scalar::process;

  Stored_op(Expression *lhs)
    : Stored_scalar(lhs)
  {}

  void process(Expression::Processor &prc) const
  {
    Stored_scalar::process_if(prc.scalar());
  }

  // Store reported operator call.

  Scalar_prc* scalar() { return this; }
  List_prc* arr()      { assert(false); return NULL; }
  Doc_prc*  doc()      { assert(false); return NULL; }
};


/*
  Storage for ILRI expressions that can re-use already stored
  first part of the expression.

  When reporting stored expression, it can wrap it in unary
  "not" operator if requested.
*/

struct Stored_ilri
  : public Stored_expr
  , Stored_scalar
{
  using Stored_expr::process;
  using Stored_scalar::process;

  bool m_neg;

  Stored_ilri(Expression *first, bool neg)
    : Stored_scalar(first), m_neg(neg)
  {}

  void process(Expression::Processor &prc) const
  {
    Scalar_prc *sprc = prc.scalar();

    if (!sprc)
      return;

    if (!m_neg)
    {
      Stored_scalar::process(*sprc);
      return;
    }

    // wrap in negation

    List_prc *argsp = sprc->op("not");
    if (!argsp)
      return;
    argsp->list_begin();
    Stored_scalar::process_if(safe_prc(argsp)->list_el()->scalar());
    argsp->list_end();
  }

  // Store reported ILRI expression.

  Scalar_prc* scalar() { return this; }
  List_prc* arr() { assert(false); return NULL; }
  Doc_prc*  doc() { assert(false); return NULL; }

};


// --------------------------------------------------------------------------


inline
Expression* Expr_parser_base::parse(Start start, Processor *prc)
{
  switch (start)
  {
  case FULL:   return parse_or(prc);
  case ATOMIC: return parse_atomic(prc);
  case MUL:    return parse_mul(prc);
  case ADD:    return parse_add(prc);
  case SHIFT:  return parse_shift(prc);
  case BIT:    return parse_bit(prc);
  case COMP:   return parse_comp(prc);
  case ILRI:   return parse_ilri(prc);
  case AND:    return parse_and(prc);
  case OR:     return parse_or(prc);

  case DOC:
  case ARR:
    {
      scoped_ptr<Stored_expr> stored;

      if (!prc)
      {
        stored.reset(new Stored_any());
        prc = stored.get();
      }

      if (DOC == start)
        parse_doc(prc->doc());
      else
        parse_arr(prc->arr());

      return stored.release();
    }

  default: throw Error((boost::format("Expr parser: Invalid start state %d") % int(start)).str());
  }
}


}  // parser

#endif
