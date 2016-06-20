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


#include "expr_parser.h"

PUSH_SYS_WARNINGS
#include <stdlib.h>
POP_SYS_WARNINGS

PUSH_SYS_WARNINGS
PUSH_BOOST_WARNINGS
#include <boost/lexical_cast.hpp>
POP_BOOST_WARNINGS
POP_SYS_WARNINGS

// TODO: Complete the parser
// TODO: Better parser errors


using namespace parser;
using cdk::string;
using cdk::Expression;
typedef cdk::Expression::Processor Processor;
typedef Processor::List_prc List_prc;

using cdk::Safe_prc;
using cdk::safe_prc;


#define RETHROW_BOOST_LEXICAL \
catch(boost::bad_lexical_cast &e)\
{\
  throw Error(e.what());\
}


void Token_op_base::unexpected_token(const Token &t, const char *ctx)
{
  throw Error((boost::format("Expr parser: Unexpected token %s when expecting %s at %d")
               % t.get_name() % ctx % get_token_pos()).str());
}


/*
  Variant of std::auto_ptr such that after smart_ptr.release() the
  pointed object can still be accessed via smart_ptr->xxx() (even
  though it is no longer owned by this smart_ptr instance).
*/

template <typename T>
class smart_ptr
  : public cdk::foundation::nocopy
{
  T *m_ptr;
  bool m_owns;

public:

  smart_ptr(T *ptr = NULL)
    : m_ptr(ptr), m_owns(true)
  {}

  ~smart_ptr()
  {
    reset(NULL);
  }

  void operator=(T *ptr)
  {
    reset(ptr);
  }

  T* reset(T *ptr)
  {
    if (m_owns)
      delete m_ptr;
    m_ptr = ptr;
    m_owns = true;
    return ptr;
  }

  T* release()
  {
    m_owns = false;
    return m_ptr;
  }

  T* operator->()
  {
    return m_ptr;
  }
};


/*
  Sink expression processor that ignores the expression reproted
  to it.

  It is used below in situations where we want to ignore results
  of parsing without storing them anywhere.
*/

struct Sink : public Expression::Processor
{
  Scalar_prc* scalar() { return NULL; }
  List_prc*   arr()    { return NULL; }
  Doc_prc*    doc()    { return NULL; }

};

Expression::Processor* ignore_if(Expression::Processor *prc)
{
  static Sink sink;
  if (!prc)
    return &sink;
  return prc;
}


bool Expr_parser_base::do_parse(It &first, const It &last, Processor *prc)
{
  Token_op_base::m_first = &first;
  Token_op_base::m_last = last;

  /*
    if prc is NULL, ignore the parsed expression instead of storing it
    which would be the case if we pass NULL to parse().
    For safety, delete the object returned from parse() if any.
  */
  delete parse(FULL, ignore_if(prc));

  return true;
}

// -------------------------------------------------------------------------


/**
   castOp ::= CAST LPAREN expr AS castType RPAREN
 */

void Expr_parser_base::parse_cast(Scalar_prc *prc)
{
  consume_token(Token::CAST);

  Safe_prc<List_prc> ap = safe_prc(prc)->op("cast");

  consume_token(Token::LPAREN);

  ap->list_begin();

  // 1st arg, the expression

  delete parse(FULL, ignore_if(ap->list_el()));

  consume_token(Token::AS);

  // 2nd arg, cast_data_type

  ap->list_el()->scalar()->val()->value(cdk::TYPE_BYTES,
                                        Format_info(),
                                        cdk::bytes(parse_cast_type()));

  ap->list_end();

  consume_token(Token::RPAREN);
}


/**
   castType ::=
        SIGNED INTEGER?
      |   UNSIGNED INTEGER?
      |   CHAR lengthSpec?
      |   BINARY lengthSpec?
      |   DECIMAL (lengthSpec | (LPAREN INT COMMA INT RPAREN))?
      |   TIME
      |   DATE
      |   DATETIME
      |   JSON

   lengthSpec ::= LPAREN INT RPAREN
 */

cdk::string Expr_parser_base::parse_cast_type()
{
  std::string type_str;

  const Token& token = peek_token();
  Token::TokenType type = token.get_type();

  if ((type == Token::BINARY) || (type == Token::CHAR))
  {
    type_str += consume_token(type);
    if (cur_token_type_is(Token::LPAREN))
      type_str += cast_data_type_dimension();
  }
  else if (type == Token::DECIMAL)
  {
    type_str += consume_token(type);
    if (cur_token_type_is(Token::LPAREN))
      type_str += cast_data_type_dimension(true);
  }
  else if ((type == Token::DATE) || (type == Token::DATETIME) || (type == Token::TIME))
  {
    type_str += consume_token(type);
  }
  else if (type == Token::SIGNED)
  {
    type_str += consume_token(type);
    if (cur_token_type_is(Token::INTEGER))
      type_str += " " + consume_token(Token::INTEGER);
  }
  else if (type == Token::UNSIGNED)
  {
    type_str += consume_token(type);
    if (cur_token_type_is(Token::INTEGER))
      type_str += " " + consume_token(Token::INTEGER);
  }
  else if (type == Token::INTEGER)
  {
    type_str += consume_token(type);
  }
  else if (type == Token::JSON)
  {
    type_str += consume_token(type);
  }
  else
  {
    throw Error((boost::format("Expr parser: Unknown token type = %s when expecting cast type at %d")
                 % token.get_name() % get_token_pos()).str());
  }

  return type_str;
}


/**
   dimension ::= LPAREN LINTEGER RPAREN

   if double_dimention = true:
   LPAREN INT COMMA INT RPAREN

   returns textual representation of the parse, like "(N)" or "(N:M)".
 */

std::string Expr_parser_base::cast_data_type_dimension(bool double_dimension)
{
  if (!cur_token_type_is(Token::LPAREN))
    return "";
  consume_token(Token::LPAREN);
  std::string result = "(" + consume_token(Token::LINTEGER);
  if (double_dimension && cur_token_type_is(Token::COMMA))
  {
    consume_token(Token::COMMA);
    result += ", " + consume_token(Token::LINTEGER);
  }
  result += ")";
  consume_token(Token::RPAREN);
  return result;
}


// -------------------------------------------------------------------------

/*
   ident ::=
     ID
   | QUOTED_ID
 */

const std::string &Expr_parser_base::get_ident()
{
  if (cur_token_type_is(Token::ID))
  {
    return consume_token(Token::ID);
  }
  else if (cur_token_type_is(Token::QUOTED_ID))
  {
    return consume_token(Token::QUOTED_ID);
  }
  else if (peek_token().is_reserved_word())
  {
    return get_token().get_text();
  }
  else
  {
    throw Error((boost::format("Expr parser: Expected token type ID or QUOTED_ID"
                               " in JSON path at token pos %d")
                               % get_token_pos()).str());
  }
}

/**
  // [schema.]ident (currently only used for optionally qualified function calls)
  schemaQualifiedIdent ::= (ident '.')? ident


   functionCall ::= schemaQualifiedIdent LPAREN argsList? RPAREN
 */

void Expr_parser_base::parse_function_call(Scalar_prc *prc)
{
  parse_schema_ident();
  parse_function_call(*m_col_ref.table(), prc);
}


/*
  Assuming that a schema-qualified identifier was just parsed, attempt to
  parse a function call if next token starts argument list.
  Returns false if this is not the case.
*/

bool
Expr_parser_base::parse_function_call(const cdk::api::Table_ref &func, Scalar_prc *prc)
{
  if (!cur_token_type_is(Token::LPAREN))
    return false;

  List_prc *aprc = safe_prc(prc)->call(func);

  consume_token(Token::LPAREN);

  if (aprc)
    aprc->list_begin();

  if (!cur_token_type_is(Token::RPAREN))
    parse_argslist(aprc);

  if (aprc)
    aprc->list_end();

  consume_token(Token::RPAREN);

  return true;
}


/*
  Original grammar:

  // [[schema.]table.]ident
  columnIdent ::= (ident '.' (ident '.')?)? ident
        ('->' (('$' documentPath) | ("'$" documentPath "'")) )?

  is rewritten as:

  columnIdent ::= schemaQualifiedIdent columnIdent1
  columnIdent1 ::= ('.' ident)? ('->' ( columnIdentDocPath | "'" columnIdentDocPath "'" ))?

  columnIdentDocPath ::= documentField // but require DOLLAR prefix
*/

void Expr_parser_base::parse_schema_ident()
{
  const cdk::string &name = get_ident();

  if (cur_token_type_is(Token::DOT))
  {
    consume_token(Token::DOT);
    m_col_ref.m_table_ref.set(get_ident(), name);
  }
  else
    m_col_ref.m_table_ref.set(name);
}


void Expr_parser_base::parse_column_ident()
{
  parse_schema_ident();
  parse_column_ident1();
}


void Expr_parser_base::parse_column_ident1()
{
  if (cur_token_type_is(Token::DOT))
  {
    consume_token(Token::DOT);
    // Note: the table part was initialized in parse_schema_ident()
    m_col_ref.set_name(get_ident());
  }
  else
  {
    // Re-interpret table name parsed by parse_schema_ident() as a
    // column name of the form [<table>.]<column>

    if (m_col_ref.table()->schema())
      m_col_ref.set(m_col_ref.table()->name(), m_col_ref.table()->schema()->name());
    else
      m_col_ref.set(m_col_ref.table()->name());
  }

  // Clear Document path
  m_path.clear();

  if (cur_token_type_is(Token::ARROW))
  {
    consume_token(Token::ARROW);

    if (cur_token_type_is(Token::LSTRING))
    {
      Tokenizer toks(consume_token(Token::LSTRING));
      toks.get_tokens();
      It first = toks.begin();
      It last  = toks.end();
      Expr_parser_base path_parser(first, last, m_parser_mode);
      // TODO: Translate parse errors
      path_parser.parse_document_field(true);
      if (first != last)
        throw Error("Invalid quotted path component");
      m_path = path_parser.m_path;
    }
    else
    {
      parse_document_field();
    }
  }

}


// -------------------------------------------------------------------------


/**
   documentField ::= ID documentPath? | DOLLAR documentPath
*/

void Doc_path_parser_base::parse_document_field()
{
  //Clear Doc_path obj
  m_path.clear();

  if (cur_token_type_is(Token::DOLLAR))
  {
    consume_token(Token::DOLLAR);

  } else if (!cur_token_type_is(Token::DOT) &&
             !cur_token_type_is(Token::LSQBRACKET) &&
             !cur_token_type_is(Token::DOUBLESTAR))
  {
  /*
     Special case, starting with MEMBER

     On this case, we check if documentPath starts with DOT, LSQBRACKET or
     DOUBLESTAR. If not, it is parsed as ID
   */

    parse_docpath_member();
  }

  parse_document_path(false);
}

/**
   documentField ::= ID documentPath? | DOLLAR documentPath

   If prefix is true, only second form starting with DOLLAR prefix is
   accepted.
*/

void Expr_parser_base::parse_document_field(bool prefix)
{
  if (cur_token_type_is(Token::ID) && !prefix)
  {
    return parse_document_path(consume_token(Token::ID));
  }

  if (cur_token_type_is(Token::DOLLAR))
  {
    consume_token(Token::DOLLAR);
    return parse_document_path();
  }



  throw Error(
    (boost::format("Expr parser: Expected token type IDENT or DOLLAR in JSON path"
                   " at token pos %d") % get_token_pos()).str());
}


/**
  Original Grammar:

   documentPath ::= documentPathItem* documentPathLastItem

   documentPathItem ::=
          documentPathLastItem
      |   DOUBLESTAR

   documentPathLastItem ::=
          ARRAYSTAR
      |   LSQBRACKET INT RSQBRACKET
      |   DOTSTAR
      |   DOT documentPathMember

   documentPathMember ::=
        ID
      | STRING1

  This grammar has been re-written to equivalent one

   documentPath ::= documentPathItem+

   documentPathItem ::=
     | DOUBLESTAR
     | LSQBRACKET documentPathArrayLoc RSQBRACKET
     | DOT MUL
     | DOT documentPathMember

   documentPathArrayLoc ::=
     | MUL
     | INT

   documentPathMember ::=
       ID
     | STRING1

   A check that DOUBLESTAR is not last element of a path is done separately.

 */

void Doc_path_parser_base::parse_document_path(bool clear)
{
  if (clear)
    m_path.clear();

  while (true)
  {
    if (cur_token_type_is(Token::DOT))
    {
      consume_token(Token::DOT);
      if (cur_token_type_is(Token::MUL))
      {
        consume_token(Token::MUL);
        m_path.add(Doc_path::MEMBER_ASTERISK);
      }
      else
      {
        parse_docpath_member();
      }
    }
    else if (cur_token_type_is(Token::LSQBRACKET))
    {
      consume_token(Token::LSQBRACKET);
      parse_docpath_array_loc();
      consume_token(Token::RSQBRACKET);
    }
    else if (cur_token_type_is(Token::DOUBLESTAR))
    {
      consume_token(Token::DOUBLESTAR);
      m_path.add(Doc_path::DOUBLE_ASTERISK);
    }
    else
    {
      break;
    }
  }
  unsigned int size = m_path.length();
  if (size > 0 && (m_path.get_type(size - 1) == Doc_path::DOUBLE_ASTERISK))
  {
    throw Error((boost::format("Expr parser: JSON path may not end in '**' at %d") % get_token_pos()).str());
  }
}

void Doc_path_parser_base::parse_document_path(const cdk::string &first)
{
  m_path.clear();
  m_path.add(Doc_path::MEMBER, first);
  parse_document_path(false);
}

void Doc_path_parser_base::parse_document_path(const cdk::string &first,
                                           const cdk::string &second)
{
  m_path.clear();
  m_path.add(Doc_path::MEMBER, first);
  m_path.add(Doc_path::MEMBER, second);
  parse_document_path(false);
}


/**
    documentPathMember ::=
        ID
    |   STRING1

    TODO: Does STRING1 differ from plain STRING in any way?

   Note: In the context of parsing documentPath, words such as "year", which
   are otherwise treated by tokenizer as reserved, are treated as normal identifiers.
*/

void Doc_path_parser_base::parse_docpath_member()
{
  const Token &t = get_token();

  switch (t.get_type())
  {
  case Token::ID:
  case Token::LSTRING:

    m_path.add(Doc_path::MEMBER, t.get_text());
    break;

  default:

    if (t.is_reserved_word())
      m_path.add(Doc_path::MEMBER, t.get_text());
    else
      throw Error(
        (boost::format("Expr parser: Expected token type IDENT or LSTRING in JSON path"
                       " at token pos %d") % get_token_pos()).str());
  }
}


/**
   documentPathArrayLoc ::=
       MUL
     | INT
 */
void Doc_path_parser_base::parse_docpath_array_loc()
{
  if (cur_token_type_is(Token::MUL))
  {
    consume_token(Token::MUL);
    m_path.add(Doc_path::ARRAY_INDEX_ASTERISK);
  }
  else if (cur_token_type_is(Token::LINTEGER))
  {
    const std::string& value = consume_token(Token::LINTEGER);
    uint32_t v = boost::lexical_cast<uint32_t>(value.c_str(), value.size());
    m_path.add(Doc_path::ARRAY_INDEX, v);
  }
  else
  {
    throw Error(
          (boost::format(
             "Expr parser: Exception token type MUL or LINTEGER"
             " in JSON path array index at token pos %d")
             % get_token_pos()).str());
  }
}


// -------------------------------------------------------------------------


/**
   atomicExpr ::=
        placeholder
    |   columnIdent     // TABLE mode
    |   documentField   // DOCUMENT mode
    |   functionCall
    |   groupedExpr
    |   unaryOp
    |   castOp
    |   literal
    |   jsonDoc
    |   array

  placeholder ::= COLON ID

  groupedExpr ::= LPAREN expr RPAREN

  unaryOp ::=
          BANG atomicExpr
      |   NEG atomicExpr
      |   PLUS atomicExpr
      |   MINUS atomicExpr

  literal ::=
          INT
      |   FLOAT
      |   STRING1
      |   STRING2
      |   NULL
      |   FALSE
      |   TRUE

  We extend this grammar with nullary operators:

  nullaryOp ::= MUL

  TODO: "default" operator
 */

Expression* Expr_parser_base::parse_atomic(Processor *prc)
{
  Token::TokenType type = peek_token().get_type();

  switch (type)
  {
  // jsonDOC

  case Token::LCURLY:
    return parse(DOC, prc);

  // array

  case Token::LSQBRACKET:
    return parse(ARR, prc);

  // groupedExpr

  case Token::LPAREN:
    {
      consume_token(Token::LPAREN);
      Expression *res = parse(FULL,prc);
      consume_token(Token::RPAREN);
      return res;
    }

  default: break;
  }

  /*
    If prc is NULL, we are supposed to store and return the result
    of parsing. In that case initialize stored variable with appropriate
    storage object and set prc to point at it so that expression will
    be reported to the storage object.

    Note: if prc is not NULL then stored remains empty and stored.release()
    would produce NULL as required in this case.
  */

  smart_ptr<Stored_expr> stored;

  if (!prc)
    prc = stored.reset(new Stored_any());


  // CAST, parameters, nullary operators

  switch (type)
  {

  // placeholder

  case Token::COLON:
    consume_token(Token::COLON);
    safe_prc(prc)->scalar()->param(consume_token(Token::ID));
    return stored.release();

  // castOp

  case Token::CAST:
    parse_cast(prc->scalar());
    return stored.release();

  // nullary "*"

  case Token::MUL:
    {
      consume_token(Token::MUL);
      safe_prc(prc)->scalar()->op("*");
      // NOTE: arguments processor is ignored as there are no arguments
      return stored.release();
    }

  default: break;
  }

  Safe_prc<Processor::Scalar_prc> sprc(prc->scalar());

  // Unary operator.

  List_prc *argsp = NULL;
  bool     neg = false;

  switch (type)
  {
    case Token::PLUS:
    case Token::MINUS:
      {
        const Token &t = get_token();
        type = peek_token().get_type();
        if (Token::LNUM == type || Token::LINTEGER == type) {
          // treat as numeric literal with possibly negated value
          neg = (Token::MINUS == t.get_type());
          break;
        }
        // otherwise report as unary operator
        argsp = sprc->op(operator_name(t.get_text()).c_str());
        break;
      }

    case Token::BANG:
    case Token::NOT:
    case Token::NEG:
      get_token();
      argsp = sprc->op(operator_name("not").c_str());
      break;

    default:
      break;  // will continue with literal parsing
  }

  // Report the single argument of the unary operator

  if (argsp)
  {
    argsp->list_begin();
    parse(ATOMIC, argsp->list_el());
    argsp->list_end();
    return stored.release();
  }

  // Literal value

  switch (peek_token().get_type())
  {
    case Token::LSTRING:
      if (m_strings_as_blobs)
        sprc->val()->value(cdk::TYPE_BYTES, Format_info(),
                           cdk::bytes(get_token().get_text()));
      else
        sprc->val()->str(get_token().get_text());
      return stored.release();

    case Token::T_NULL:
      sprc->val()->null();
      get_token();
      return stored.release();

    case Token::LNUM:
    try {
      double val = boost::lexical_cast<double>(get_token().get_text());
      sprc->val()->num(neg ? -val : val);
      return stored.release();
    }
    RETHROW_BOOST_LEXICAL;

    case Token::LINTEGER:
    try {
      if (neg)
      {
        int64_t val = boost::lexical_cast<int64_t>(get_token().get_text());
        sprc->val()->num(-val);
      }
      else
      {
        uint64_t val = boost::lexical_cast<uint64_t>(get_token().get_text());
        sprc->val()->num(val);
      }
      return stored.release();
    }
    RETHROW_BOOST_LEXICAL;

    case Token::TRUE_:
    case Token::FALSE_:
      sprc->val()->yesno(get_token().get_type() == Token::TRUE_);
      return stored.release();

    default:
     // will continue with functionCall | columnIdent | documentField parsing
    break;

  }


  /*
    functionCall | columnIdent | documentField

    In this case the first token can be either ID, QUOTED_ID or DOLLAR:

    functionCall  ::= schemaQualifiedIdent ...
    columIdent    ::= shcemaQualifiedIdent ...
    documentField ::= ID .... | DOLLAR ...

    schemaQualifiedIdent ::= ident ...
    ident         ::= ID | QUOTED_ID

    ID can start either of the three types of atomic expression, DOLLAR
    can only start documentField, QUOTED_ID can start either functinCall
    or columnIdent.
  */


  const Token& t = peek_token();

  if (t.get_type() == Token::ID ||
      t.get_type() == Token::QUOTED_ID ||
      t.is_reserved_word())
  {
    /*
        Parse schemaQualifiedIdent - the result will be stored
        in m_col_ref.table()
      */

    parse_schema_ident();
    assert(m_col_ref.table());

    /*
        First check if this is a function call. If yes, the call will
        be reported to the processor and there is nothing more to do.
      */

    if (parse_function_call(*m_col_ref.table(), sprc))
      return stored.release();

    /*
        Otherwise, if we are in TABLE mode, identifier we parsed so far
        should be a beginning of columnIdent. We complete parsing it, report
        it to the processor and then we are done.
      */

    if (Parser_mode::TABLE == m_parser_mode)
    {
      parse_column_ident1();
      sprc->ref(m_col_ref, m_path.is_empty() ? NULL : &m_path);
      return stored.release();
    }

    /*
        Otherwise we are in DOCUMENT mode. In this case the document field
        should not start with a quotted identifier (documentField rule only
        allows ID, not QUOTED_ID).
      */

    if (Token::QUOTED_ID == type)
      unexpected_token(get_token(), "atomic expr");

    /*
        We re-interpret schemaQualifiedIdent parsed above as a beginning of a
        document field expression:

        - if it is of the form A.B, where A is schema name and B is table name,
          then A becomes main document field name and B becomes the first
          element in the document path;

        - if it is a single identifier A (table name) then it becomes the
          main field name.

        The rest of the document path is parsed within parse_document_path()
        method.
      */

    if (m_col_ref.table()->schema())
      parse_document_path(m_col_ref.table()->schema()->name(),
                          m_col_ref.table()->name());
    else
      parse_document_path(m_col_ref.table()->name());

    /*
        Note: the parsed document path will be reported to the processor below
        (after switch() statement).
      */
  }
  else if (type == Token::DOLLAR)
  {
    /*
      DOLLAR starts documentField, which is valid only in DOCUMENT mode.
    */

    if (Parser_mode::DOCUMENT != m_parser_mode)
      unexpected_token(get_token(), "atomic expr");

    parse_document_field();
  }
  else
  {

    /*
      If we see any other token, then we throw exception, since it is unexpected
    */
    unexpected_token(t,"atomic expr");

  }

  sprc->ref(m_path);

  return stored.release();
}


// -------------------------------------------------------------------------


Expression*
Expr_parser_base::left_assoc_binary_op(TokSet op, Start lhs, Start rhs,
                                       Processor *prc)
{
  /*
    An optimization under assumption that array or document can never be
    an argument of binary operator.

    If we see '{' or '[' then we do not expect the binary operator later
    (the whole expression consists of the LHS only). Thus, there is no need
    to store LHS for later reporting.
  */

  if (cur_token_type_is(Token::LCURLY))
    return parse(DOC, prc);

  if (cur_token_type_is(Token::LSQBRACKET))
    return parse(ARR, prc);


  // Store LHS of the expression

  smart_ptr<Expression> stored_lhs(parse(lhs, NULL));

  if (!tokens_available() || !is_token_type_within_set(op))
  {
    /*
      There is no RHS, so LHS is the whole expression.
      If prc is NULL then we return already stored LHS. Otherwise
      we report stored LHS to the processor.
    */

    if (!prc)
      return stored_lhs.release();

    stored_lhs->process(*prc);
    return NULL;
  }

  // consume the operator token and store its name
  const std::string op_name(operator_name(get_token().get_text()));

  /*
    If storing operator call expression (prc is NULL), use specialized
    Stored_op class that can re-use already stored LHS expression.
  */

  smart_ptr<Stored_expr> stored;

  if (!prc)
    // Note: Stored_op takes ownership of the stored LHS expr.
    prc = stored.reset(new Stored_op(stored_lhs.release()));

  // pass lhs and rhs as operator arguments

  List_prc *aprc = safe_prc(prc)->scalar()->op(op_name.c_str());

  if (aprc)
  {
    aprc->list_begin();

    // Report stored LHS as the 1st argument.

    stored_lhs->process_if(aprc->list_el());

    // then parse rhs, passing it as 2nd argument

    parse(rhs, aprc->list_el());

    aprc->list_end();
  }

  return stored.release();
}


Expression* Expr_parser_base::parse_mul(Processor *prc)
{
  TokSet ops;
  ops.insert(Token::MUL);
  ops.insert(Token::DIV);
  ops.insert(Token::MOD);
  return left_assoc_binary_op(ops, ATOMIC, MUL, prc);
}


Expression* Expr_parser_base::parse_add(Processor *prc)
{
  TokSet ops;
  ops.insert(Token::PLUS);
  ops.insert(Token::MINUS);
  return left_assoc_binary_op(ops, MUL, ADD, prc);
}

Expression* Expr_parser_base::parse_shift(Processor *prc)
{
  TokSet ops;
  ops.insert(Token::LSHIFT);
  ops.insert(Token::RSHIFT);
  return left_assoc_binary_op(ops, ADD, SHIFT, prc);
}

Expression* Expr_parser_base::parse_bit(Processor *prc)
{
  TokSet ops;
  ops.insert(Token::BITAND);
  ops.insert(Token::BITOR);
  ops.insert(Token::BITXOR);
  return left_assoc_binary_op(ops, SHIFT, BIT, prc);
}

Expression* Expr_parser_base::parse_comp(Processor *prc)
{
  TokSet ops;
  ops.insert(Token::GE);
  ops.insert(Token::GT);
  ops.insert(Token::LE);
  ops.insert(Token::LT);
  ops.insert(Token::EQ);
  ops.insert(Token::NE);
  return left_assoc_binary_op(ops, BIT, COMP, prc);
}

Expression* Expr_parser_base::parse_and(Processor *prc)
{
  TokSet ops;
  ops.insert(Token::AND);
  return left_assoc_binary_op(ops, ILRI, AND, prc);
}

Expression* Expr_parser_base::parse_or(Processor *prc)
{
  TokSet ops;
  ops.insert(Token::OR);
  return left_assoc_binary_op(ops, AND, OR, prc);
}


// -------------------------------------------------------------------------

/**
   Expression Parser EBNF:
   note; No reptetition, must be connected by logical operators
    ilriExpr ::=
            compExpr IS NOT? (NULL|TRUE|FALSE)
        |   compExpr NOT? IN LPAREN argsList? RPAREN
            // TODO: param to ESCAPE should be better defined
        |   compExpr NOT? LIKE compExpr (ESCAPE compExpr)?
        |   compExpr NOT? BETWEEN compExpr AND compExpr
        |   compExpr NOT? REGEXP compExpr
        |   compExpr
 */

Expression* Expr_parser_base::parse_ilri(Processor *prc)
{
  // Array/doc optimization like in left_assoc_binary_op().

  if (cur_token_type_is(Token::LCURLY))
    return parse(DOC, prc);

  if (cur_token_type_is(Token::LSQBRACKET))
    return parse(ARR, prc);


  // Store the first expression.

  smart_ptr<Expression> first(parse(COMP, NULL));

  /*
    Check next token after the first expression. If it is not an expected
    one, then the ilri phrase consists of the first expression only.
  */

  TokSet next;
  next.insert(Token::NOT);
  next.insert(Token::IS);
  next.insert(Token::IN_);
  next.insert(Token::LIKE);
  next.insert(Token::BETWEEN);
  next.insert(Token::REGEXP);

  if (!tokens_available() || !is_token_type_within_set(next))
  {
    // If prc is NULL return already stored expression.

    if (!prc)
      return first.release();

    // Otherwise report stored expression to the processor.

    first->process(*prc);
    return NULL;
  }

  // Get the operator and see if we have negation

  bool neg = false;

  if ( cur_token_type_is(Token::NOT) )
  {
    neg = true;
    consume_token(Token::NOT);
  }

  const Token &t = get_token();
  std::string op_name = operator_name(t.get_text());

  if (neg && Token::IS == t.get_type())
     throw Error("Expr parser: Unexpected token NOT before token IS");

  if (Token::IS == t.get_type() && Token::NOT == peek_token().get_type())
  {
    neg = true;
    consume_token(Token::NOT);
  }


  /*
    If prc is NULL and we are supposed to store parsed epression, use
    specialized Stored_ilri class that can re-use the already stored first
    part of the expression.
  */

  List_prc *not_arg_prc = NULL;
  smart_ptr<Stored_ilri> stored;

  if (!prc)
  {
    // Note: Stored_ilri handles wrapping in "not" operator if needed.
    prc = stored.reset(new Stored_ilri(first.release(), neg));
  }
  else if (neg)
  {
    /*
      In case of negation, the main operator is wrapped in unary "not" operator.
      We report "not" operator with 1 argument and replace prc with the processor
      of this single argument. This way the code below will report the main
      expression as the argument of the "not" operator. Without negation prc
      will remain pointing at the original processor.
    */

    not_arg_prc = safe_prc(prc)->scalar()->op(operator_name("not").c_str());
    if (not_arg_prc)
      not_arg_prc->list_begin();
    prc = safe_prc(not_arg_prc)->list_el();
  }

  // report the main operator

  Safe_prc<List_prc> aprc = safe_prc(prc)->scalar()->op(op_name.c_str());
  aprc->list_begin();

  // 1st argument

  first->process_if(aprc->list_el());

  // other arguments

  switch (t.get_type())
  {
    case Token::IS:
    {
      switch(peek_token().get_type())
      {
        case Token::TRUE_:  aprc->list_el()->scalar()->val()->yesno(true); break;
        case Token::FALSE_: aprc->list_el()->scalar()->val()->yesno(true); break;
        case Token::T_NULL: aprc->list_el()->scalar()->val()->null(); break;
        default:
          throw Error("Expr parser: Unexpected token after IS [NOT]");
      }

      get_token();
      break;
    }

    case Token::IN_:
    {
      consume_token(Token::LPAREN);

      // Note: true flag means that strings will be reported as blobs.
      parse_argslist(aprc, true);

      consume_token(Token::RPAREN);

      break;
    }

    case Token::LIKE:
    {
      parse(COMP, aprc->list_el());

      if (cur_token_type_is(Token::ESCAPE))
      {
        //TODO: Check ESCAPE
        THROW("ESCAPE clause for LIKE operator not supported yet");
      }

      break;
    }

    case Token::REGEXP:
      parse(COMP, aprc->list_el());
      break;

    case Token::BETWEEN:
      parse(COMP, aprc->list_el());
      consume_token(Token::AND);
      parse(COMP, aprc->list_el());
      break;

    default: assert(false);
  }

  // close argument list

  aprc->list_end();
  if (not_arg_prc)
    not_arg_prc->list_end();

  return stored.release();
}


// -------------------------------------------------------------------------

/*
  Below we want to use Expr_parser_base with parser templates such
  as Doc_parser<> or List_parser<>. These templates assume that the
  base parser can be constructed with a constructor which accepts only
  2 parameters defining the range of tokens to be parsed.

  But Expr_parser_base constructor also needs parser mode parameter
  and the flag which tells if strings should be reported as blobs.
  To fix this, we define Base_parser<> template parametrized with parser
  mode, which will construct required flavour of the parser.
*/

template <Parser_mode::value Mode,
          bool strings_as_blobs = false>
struct Base_parser : public Expr_parser_base
{
  Base_parser(It &first, const It &last)
    : Expr_parser_base(first, last, Mode, strings_as_blobs)
  {}
};


template <Parser_mode::value Mode,
          bool strings_as_blobs>
void parse_args(Processor::List_prc *prc, It &first, const It &last)
{
  List_parser< Base_parser<Mode, strings_as_blobs> >
    args_parser(first, last);
  args_parser.process_if(prc);
}

template <bool strings_as_blobs>
void parse_args(Parser_mode::value mode, Processor::List_prc *prc,
                It &first, const It &last)
{
  if (Parser_mode::DOCUMENT == mode)
    parse_args<Parser_mode::DOCUMENT, strings_as_blobs>(prc, first, last);
  else
    parse_args<Parser_mode::TABLE, strings_as_blobs>(prc, first, last);
}


void
Expr_parser_base::parse_argslist(Processor::List_prc *prc,
                                 bool strings_as_blobs)
{
  /*
     argsList ::= expr (COMMA expr)*
  */

  if (strings_as_blobs)
    parse_args<true>(m_parser_mode, prc, cur_pos(), end_pos());
  else
    parse_args<false>(m_parser_mode, prc, cur_pos(), end_pos());
}


void Expr_parser_base::parse_arr(Processor::List_prc *prc)
{
  if (Parser_mode::DOCUMENT == m_parser_mode)
  {
    Arr_parser<Base_parser<Parser_mode::DOCUMENT>,
               Expression::Scalar::Processor>
      arr_parser(cur_pos(), end_pos());
    arr_parser.process_if(prc);
  }
  else
  {
    Arr_parser<Base_parser<Parser_mode::TABLE>, Expression::Scalar::Processor>
      arr_parser(cur_pos(), end_pos());
    arr_parser.process_if(prc);
  }
}


void Expr_parser_base::parse_doc(Processor::Doc_prc *prc)
{
  if (Parser_mode::DOCUMENT == m_parser_mode)
  {
    Doc_parser<Base_parser<Parser_mode::DOCUMENT>,
               Expression::Scalar::Processor>
      doc_parser(cur_pos(), end_pos());
    doc_parser.process_if(prc);
  }
  else
  {
    Doc_parser<Base_parser<Parser_mode::TABLE>,
               Expression::Scalar::Processor>
      doc_parser(cur_pos(), end_pos());
    doc_parser.process_if(prc);
  }
}


void Order_parser::process(Processor& prc) const
{
  if (!const_cast<Order_parser*>(this)->m_tokenizer.tokens_available())
    cdk::throw_error("Expression_parser: empty string");

  It first = m_tokenizer.begin();
  It last  = m_tokenizer.end();

  /*
     *    note: passing m_toks.end() directly as constructor argument results
     *    in "incompatible iterators" exception when comparing iterators (at
     *    least on win, vs2010). problem with passing temporary object?
     */

  Stored_any store_expr;

  Expr_parser_base parser(first, last, m_mode);
  parser.process(store_expr);

  cdk::api::Sort_direction::value dir = cdk::api::Sort_direction::ASC;

  // get ASC/DESC token if available
  if (first != last)
  {
    switch(first->get_type())
    {
      case Token::ASC:
        ++first;
        dir = cdk::api::Sort_direction::ASC;
        break;
      case Token::DESC:
        ++first;
        dir = cdk::api::Sort_direction::DESC;
        break;
      default:
        throw Error(
          (boost::format("Order parser: Expected token type ASC or DESC at token  %s") % first->get_text()).str());
    }
  }

  if (first != last)
    cdk::throw_error("Expression_parser: could not parse string as expression"
    " (not all tokens consumed)");

  store_expr.process_if(prc.sort_key(dir));

}


void Projection_parser::process(Projection_processor& prc) const
{
  if (!const_cast<Projection_parser*>(this)->m_tokenizer.tokens_available())
    cdk::throw_error("Expression_parser: empty string");

  It first = m_tokenizer.begin();
  It last  = m_tokenizer.end();

  /*
     *    note: passing m_toks.end() directly as constructor argument results
     *    in "incompatible iterators" exception when comparing iterators (at
     *    least on win, vs2010). problem with passing temporary object?
     */

  Expr_parser_base parser(first, last, m_mode);
  parser.process_if(prc.expr());


  // get AS token if available
  if (first != last)
  {
    if (first->get_type() != Token::AS)
      cdk::throw_error((boost::format("Projections parser: Unexpected token %s when expecting AS")
                     % first->get_name()).str());

    ++first;

    if (first->get_type() != Token::ID &&
        first->get_type() != Token::QUOTED_ID)
      cdk::throw_error((boost::format("Projections parser: Unexpected token %s when expecting ID")
                     % first->get_name()).str());

    prc.alias(first->get_text());

    ++first;
  }


  if (first != last)
    cdk::throw_error("Expression_parser: could not parse string as expression"
    " (not all tokens consumed)");
}

void Projection_parser::process(Document_processor& prc) const
{
  if (!const_cast<Projection_parser*>(this)->m_tokenizer.tokens_available())
    cdk::throw_error("Expression_parser: empty string");

  It first = m_tokenizer.begin();
  It last  = m_tokenizer.end();

  /*
     *    note: passing m_toks.end() directly as constructor argument results
     *    in "incompatible iterators" exception when comparing iterators (at
     *    least on win, vs2010). problem with passing temporary object?
     */

  Stored_any store_expr;

  Expr_parser_base parser(first, last, m_mode);
  parser.process(store_expr);


  // AS is mandatory on Collections
  if (first == last)
    cdk::throw_error("Projections parser: Unexpected end of string when"
                     "expecting token 'AS'");


  if (first->get_type() != Token::AS)
    cdk::throw_error((boost::format("Projections parser: Unexpected token %s when expecting AS")
                      % first->get_name()).str());

  ++first;

  if (first == last)
    cdk::throw_error("Projections parser: Expected token <name>");

  if (first->get_type() != Token::ID &&
      first->get_type() != Token::QUOTED_ID &&
      !first->is_reserved_word())
    cdk::throw_error((boost::format("Projections parser: Unexpected token %s when expecting ID")
                      % first->get_name()).str());

  if (first+1 != last)
    cdk::throw_error("Expression_parser: could not parse string as expression"
                     " (not all tokens consumed)");

  store_expr.process_if(prc.key_val(first->get_text()));

}
