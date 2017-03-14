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

#include <sstream>

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
  Token_op_base::set_tokens(first, last);

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
    result += "," + consume_token(Token::LINTEGER);
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
  columnIdent1 ::= ('.' ident)? ('->' ( columnIdentDocPath
                                | "'" columnIdentDocPath "'" ))?

  columnIdentDocPath ::= documentField // but require DOLLAR prefix
*/

/*
  Parse a schema-qualified identifier and store it as table/schema
  name of m_col_ref member. Schema name is optional.

  If types is not NULL then types of the consumed tokens are stored in this
  array.
*/

void Expr_parser_base::parse_schema_ident(Token::TokenType (*types)[2])
{
  if (types)
  {
    (*types)[0] = peek_token().get_type();
    // Reset the other entry in case we are not looking at more tokens.
    (*types)[1] = Token::TokenType(0);
  }
  const cdk::string &name = get_ident();

  m_col_ref.m_table_ref.set(name);

  if (cur_token_type_is(Token::DOT))
  {
    consume_token(Token::DOT);
    if (types)
      (*types)[1] = peek_token().get_type();
    m_col_ref.m_table_ref.set(get_ident(), name);
  }
}


void Expr_parser_base::parse_column_ident(Path_prc *prc)
{
  parse_schema_ident();
  parse_column_ident1(prc);
}


void Expr_parser_base::parse_column_ident1(Path_prc *prc)
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
      path_parser.parse_document_field(prc, true);
      if (first != last)
        throw Error("Invalid quotted path component");
    }
    else
    {
      parse_document_field(prc, true);
    }
  }

}


// -------------------------------------------------------------------------


/**
  The original grammar was:

   documentField ::= ID documentPath? | DOLLAR documentPath

  Which makes "*", "**.foo" or "*.foo" not valid field specifications
  while "$[3]" is a valid specification.

  We modify the grammar so that "$[..]" is not valid while "*.." or "**.."
  are valid:

    documentField ::=
      | DOLLAR documentPathLeadingDot
      | documentPath

  The grammar of documentPath was adjusted so that the first
  path item can not be an array item ("[n]" or "[*]") and we can request
  a leading DOT before member items (see parse_document_path()).

   If prefix is true, only the first form starting with DOLLAR prefix is
   accepted.
*/

void Expr_parser_base::parse_document_field(Path_prc *prc, bool prefix)
{
  if (cur_token_type_is(Token::DOLLAR))
  {
    consume_token(Token::DOLLAR);
    if (!parse_document_path(prc, true))  // require DOT before members
      throw_error("Document path expected");
    return;
  }

  if (prefix)
    throw_error("Expected DOLLAR to start a document path");

  if (!parse_document_path(prc, false))
    throw_error("Document path expected");
}


/*
  Parse a document field path with a given initial member segment.
*/

void Expr_parser_base::parse_document_field(const string &first, Path_prc *prc)
{
  Safe_prc<Path_prc> sprc = prc;

  sprc->list_begin();
  sprc->list_el()->member(first);
  parse_document_path1(prc);
  sprc->list_end();
}

/*
  Parse a document field path with given 2 initial member segment.
*/

void Expr_parser_base::parse_document_field(const string &first,
                                            const string &second,
                                            Path_prc *prc)
{
  Safe_prc<Path_prc> sprc = prc;

  sprc->list_begin();
  sprc->list_el()->member(first);
  sprc->list_el()->member(second);
  parse_document_path1(prc);
  sprc->list_end();
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

  This grammar has few flaws:

  1. It allows a document path to start with array location, which is not
     correct - array locations should be possible only after a path to some
     array member.

  2. It always requires a DOT befre a member element, but in some contexts
     we want a document path like "foo.bar.baz" to start without a dot.

  To deal with this the grammar has been changed and require_dot parameter
  has been added. Modified grammar:

   documentPath ::= documentPathFirstItem documentPathItem*

   documentPathFirstItem ::=
    | DOT? documentPathMember
    | DOUBLESTAR

   documentPathItem ::=
    | DOT documentPathMember
    | DOUBLESTAR
    | documentPathArray

   documentPathMember ::=
    | MUL
    | ID
    | STRING1

   docuemntPathArray ::= LSQBRACKET documentPathArrayLoc RSQBRACKET

   documentPathArrayLoc ::=
    | MUL
    | INT

  Parameter require_dot tells if the initial dot is required or not.

  A check that DOUBLESTAR is not last element of a path is done separately.

  Returns true if a valid document path was parsed and reported, false if the
  current token did not start a valid document path.

  Note: If false is returned then nothing is reported to the processor (not
  even an empty list).
*/

bool Expr_parser_base::parse_document_path(Path_prc *prc, bool require_dot)
{
  /*
    Below we call methods like parse_docpath_member() which expect a document
    path element processor. Our path processor prc is a list processor. So,
    before we report the first path element we must call prc->list_begin() and
    prc->list_el(). The problem is that when calling parse_docpath_member()
    we might not know yet if there is any path to report or not -- only inside
    parse_docpath_member() it will become evident.

    The Path_el_reporter wrapper around path processor solves this problem by
    deffering the initial list_begin() call and the list_el() calls to the
    moment when a path element is reported. If no path elements are reported
    then list_begin() or list_el() will not be called. Similar, call to
    list_end() will be forwarded to the wrapped processor only if list_begin()
    was called before.
  */

  struct Path_el_reporter
    : public Path_prc
    , public Path_prc::Element_prc
  {
    Safe_prc<Path_prc> m_prc;
    bool m_started;

    void list_begin()
    {
      if (!m_started)
        m_prc->list_begin();
      m_started = true;
    }

    void list_end()
    {
      if (m_started)
        m_prc->list_end();
    }

    Element_prc* list_el()
    {
      return this;
    }

    // Element_prc

    void member(const string &name)
    {
      list_begin();
      m_prc->list_el()->member(name);
    }

    void any_member()
    {
      list_begin();
      m_prc->list_el()->any_member();
    }

    void index(index_t ind)
    {
      list_begin();
      m_prc->list_el()->index(ind);
    }

    void any_index()
    {
      list_begin();
      m_prc->list_el()->any_index();
    }

    void any_path()
    {
      list_begin();
      m_prc->list_el()->any_path();
    }

    Path_el_reporter(Path_prc *prc)
      : m_prc(prc), m_started(false)
    {}
  }
  el_reporter(prc);

  // documentPathFirstItem

  bool double_star = false;

  if (cur_token_type_is(Token::DOUBLESTAR))
  {
    consume_token(Token::DOUBLESTAR);
    double_star = true;
    el_reporter.any_path();
  }
  else
  {
    if (cur_token_type_is(Token::DOT))
    {
      consume_token(Token::DOT);
      if (!parse_docpath_member(&el_reporter))
        unexpected_token(peek_token(), "Document path");
    }
    else if (require_dot)
    {
      return false;
    }
    else
    {
      if (!parse_docpath_member(&el_reporter))
        return false;
    }
  }

  // the rest of the path

  bool ret = parse_document_path1(&el_reporter);

  if (!ret && double_star)
    throw_error("Document path ending in '**'");

  el_reporter.list_end();

  return true;
}


/*
  Parse a reminder of a document path after the first item, that is, a possibly
  empty sequence of documentPathItem strings.

  The items are reported to the given Path_prc without calling list_begin() or
  list_end() (which is assumed to be done by the caller).

  Returns true if at least one path item component was parsed.
*/

bool Expr_parser_base::parse_document_path1(Path_prc *prc)
{
  Safe_prc<Path_prc> sprc = prc;

  /*
    These Booleans are used to detect if we are at the beginning of the path
    and if there was a "**" component at the end of it.
  */

  bool double_star;
  bool last_double_star = false;
  bool has_item = false;

  for (double_star = false; true;
       last_double_star =double_star,
       double_star =false,
       has_item = true)
  {
    if (!tokens_available())
      break;

    const Token &t = peek_token();

    switch (t.get_type())
    {
    case Token::DOT:
      consume_token(Token::DOT);
      if (!parse_docpath_member(sprc->list_el()))
        unexpected_token(peek_token(),
                         "when looking for a document path element");
      continue;

    case Token::DOUBLESTAR:
      consume_token(Token::DOUBLESTAR);
      sprc->list_el()->any_path();
      double_star = true;
      continue;

    case Token::LSQBRACKET:
      consume_token(Token::LSQBRACKET);
      parse_docpath_array_loc(sprc->list_el());
      consume_token(Token::RSQBRACKET);
      continue;

    default:
      break;
    }

    break;
  }

  if (last_double_star)
    throw_error("Document path ending in '**'");

  return has_item;
}


/**
    documentPathMember ::=
      | MUL
      | ID
      | STRING1

    TODO: Does STRING1 differ from plain STRING in any way?

   Note: In the context of parsing documentPath, words such as "year", which
   are otherwise treated by tokenizer as reserved, are treated as normal identifiers.
*/

bool Expr_parser_base::parse_docpath_member(Path_prc::Element_prc *prc)
{
  const Token &t = peek_token();

  switch (t.get_type())
  {
  case Token::MUL:
    if (prc)
      prc->any_member();
    break;

  case Token::ID:
  case Token::LSTRING:
    if (prc)
      prc->member(t.get_text());
    break;

  default:

    if (t.is_reserved_word())
    {
      if (prc)
        prc->member(t.get_text());
      break;
    }

    return false;
  }

  get_token();  // consume the token
  return true;
}


/**
   documentPathArrayLoc ::=
       MUL
     | INT
 */
void Expr_parser_base::parse_docpath_array_loc(Path_prc::Element_prc *prc)
{
  if (cur_token_type_is(Token::MUL))
  {
    consume_token(Token::MUL);
    if (prc)
      prc->any_index();
  }
  else if (cur_token_type_is(Token::LINTEGER))
  {
    const std::string& value = consume_token(Token::LINTEGER);
    uint32_t v = boost::lexical_cast<uint32_t>(value.c_str(), value.size());
    if (prc)
      prc->index(v);
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


bool column_ref_from_path(cdk::Doc_path &path, parser::Column_ref &column)
{
  struct Path_prc
    : public cdk::Doc_path::Processor
    , public cdk::Doc_path::Processor::Element_prc
  {
    unsigned m_len;
    parser::Column_ref &m_col;
    bool m_ret;

    Element_prc* list_el()
    {
      return this;
    }

    void member(const string &name)
    {
      switch (m_len++)
      {
      case 0: m_col.set(name); break;
      case 1: m_col.set(name, m_col.name()); break;
      case 2:
        assert(m_col.table());
        m_col.m_table_ref.set(m_col.name(), m_col.table()->name());
        m_col.set_name(name);
        break;
      default:
        // Too many path elements
        m_ret = false;
      }
    }

    void index(uint32_t)
    {
      m_ret = false;
    }

    void any_member()
    {
      m_ret = false;
    }

    void any_index()
    {
      m_ret = false;
    }

    void any_path()
    {
      m_ret = false;
    }

    Path_prc(parser::Column_ref &col)
      : m_len(0), m_col(col), m_ret(true)
    {}
  }
  prc(column);

  path.process(prc);

  return prc.m_ret;
}


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
    Expression *res = parse(FULL, prc);
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
    get_token();
    argsp = sprc->op(operator_name("!").c_str());
    break;
  case Token::NOT:
    get_token();
    argsp = sprc->op(operator_name("not").c_str());
    break;
  case Token::NEG:
    get_token();
    argsp = sprc->op(operator_name("~").c_str());
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
  case Token::LHEX:
    try {
      std::stringstream ss;
      ss << get_token().get_text();
      if (neg)
      {
        int64_t val;
        ss >> std::hex >> val;
        sprc->val()->num(-val);
      }
      else
      {
        uint64_t val;
        ss >> std::hex >> val;
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

    It is not possible to tell which of these 3 alternatives we have by
    looking at the current token. Either functionCall or columnIdent or
    documentField can start with something which looks like a schema-qualified
    name: "A" or "A.B".

    For that reason we start with a call to parse_schema_indent() which would
    parse such a schema-qualified name and store it as table/schema name of
    m_col_ref member.

    After this we try to parse a function call and if it fails we try
    columnIndent or documentField, depending on the parsing mode.
  */

  Token::TokenType types[2];
  bool schema_ident = false;

  m_col_ref.clear();

  /*
    Try to parse schema-qualified identifier, storing the types of the tokens
    that have been consumed. If parsing fails, we ignore the error because
    in this case we will try a document path below.

    Note: it is important that parse_schema_ident() stores correct tokens
    in m_col_ref even if it fails in the end.
  */

  try {
    parse_schema_ident(&types);
    schema_ident = true;
  }
  catch (const cdk::Error&)
  {}

  /*
    If parse_schema_ident() succeeded, and we have the result in
    m_col_ref.table(), we see if it is not a beginning of a function call.
    If parse_function_call() succeeds then we are done.
  */

  if (schema_ident)
  {
    assert(m_col_ref.table());

    if (parse_function_call(*m_col_ref.table(), sprc))
      return stored.release();
  }

  /*
    Otherwise we must have either a document path (in DOCUMENT mode) or
    a column identifier, possibly followed by a path (in TABLE mode).
  */

  cdk::Doc_path_storage path;

  if (Parser_mode::TABLE == m_parser_mode)
  {
    /*
      If we are in the TABLE mode, and parse_schema_ident() failed above, then
      we do not have a valid column identifier which is an error.
    */

    if (!schema_ident)
      unexpected_token(peek_token(), "when looking for a column identifier");

    /*
      Otherwise we complete parsing the column identifier and report it to
      the processor.
    */

    parse_column_ident1(&path);
    sprc->ref(m_col_ref, path.is_empty() ? NULL : &path);
    return stored.release();
  }

  /*
    Here we know that we are in DOCUMENT mode and we are expecting a document
    path. If parse_schema_ident() called above consumed some tokens, we check
    if they were not quotted identifiers. Such identifiers are allowed when
    reffering to tables or columns but are invalid in a document path.
  */

  if (Token::QUOTED_ID == types[0] || Token::QUOTED_ID == types[1])
    throw_error("invalid document path");

  /*
    Now we treat the identifiers "A.B" parsed by parse_schema_ident() and
    stored as table/schema name in m_col_ref (if any), as an initail segment
    of a document field reference and complete parsing the whole document
    field.
  */

  if (m_col_ref.table() && m_col_ref.table()->schema())
  {
    parse_document_field(
      m_col_ref.table()->schema()->name(),
      m_col_ref.table()->name(),
      &path
    );
  }
  else if (m_col_ref.table())
  {
    parse_document_field(m_col_ref.table()->name(), &path);
  }
  else
  {
    parse_document_field(&path);
  }

  sprc->ref(path);

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
  if (cur_token_type_is(Token::NEG))
  {
    get_token();
    smart_ptr<Stored_expr> stored;

    if (!prc)
      prc = stored.reset(new Stored_any());

    Safe_prc<Processor::Scalar_prc> sprc(prc->scalar());
    List_prc *argsp = NULL;
    argsp = sprc->op(operator_name("~").c_str());
    if (argsp)
    {
      argsp->list_begin();
      parse(ATOMIC, argsp->list_el());
      argsp->list_end();
      return stored.release();
    }

    return parse_bit(prc);
  }

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
    note: passing m_toks.end() directly as constructor argument results
    in "incompatible iterators" exception when comparing iterators (at
    least on win, vs2010). problem with passing temporary object?
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
    note: passing m_toks.end() directly as constructor argument results
    in "incompatible iterators" exception when comparing iterators (at
    least on win, vs2010). problem with passing temporary object?
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
    note: passing m_toks.end() directly as constructor argument results
    in "incompatible iterators" exception when comparing iterators (at
    least on win, vs2010). problem with passing temporary object?
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
