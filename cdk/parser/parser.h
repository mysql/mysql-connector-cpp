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

#ifndef CDK_PARSER_PARSER_H
#define CDK_PARSER_PARSER_H

#include <mysql/cdk/api/expression.h>
#include "tokenizer.h"

PUSH_BOOST_WARNINGS
#include <boost/format.hpp>
POP_BOOST_WARNINGS


#ifdef _WIN32

 /*
 4061 = enum constant not explicitly handled by switch() case.

 We have a lot of token type constants and we commonly use default:
 clause to catch all otherwise unhandled token types. Thus this
 warning must be disabled.

 */

DISABLE_WARNING(4061)

#endif

/*
  Infrastructure for building list and document parsers from base
  expression parsers.
*/

namespace parser {

typedef Tokenizer::iterator  It;
using cdk::throw_error;


/*
  Class that implements token navigation and usage methods
*/

class Token_op_base
{

protected:

  typedef std::set<Token::TokenType> TokSet;

  It  *m_first;
  It  m_last;

  const std::string& consume_token(Token::TokenType type)
  {
    if (!cur_token_type_is(type))
      unexpected_token(peek_token(), (boost::format("while looking for token %s")
                                      % Token::get_name(type)).str().c_str());
    return get_token().get_text();
  }

  const Token& peek_token()
  {
    if (!tokens_available())
      throw Error("unexpected end of string");
    return **m_first;
  }

  bool  cur_token_type_is(Token::TokenType type)
  {
    return tokens_available() && peek_token().get_type() == type;
  }

  bool  is_token_type_within_set(TokSet types)
  {
    return tokens_available()
           && types.find(peek_token().get_type()) != types.end();
  }

  unsigned  get_token_pos() const
  {
    // TODO
    return 0;
  }

  It& cur_pos()
  {
    assert(m_first);
    return *m_first;
  }

  const It& cur_pos() const
  {
    return const_cast<Token_op_base*>(this)->cur_pos();
  }

  const It& end_pos() const
  {
    return m_last;
  }

  bool  tokens_available() const
  {
    return m_first && cur_pos() != end_pos();
  }

  const Token& get_token()
  {
    if (!tokens_available())
      throw Error("unexpected end of string");
    const Token &t = peek_token();
    ++(*m_first);
    return t;
  }

  std::string operator_name(const std::string &name)
  {
    return Tokenizer::map.operator_names.at(name);
  }

  void unexpected_token(const Token&, const char *ctx);

public:

  Token_op_base()
    : m_first(NULL)
  {}

  void set_tokens(It &first, const It &last)
  {
    m_first = &first;
    m_last = last;
  }
};


/*
  Base class for parsers which parse tokens and present result as
  an expression over processor PRC.

  Parser objects which derive from Expr_parser<PRC> parse range of tokens
  specified when the object is created, assuming that these tokens represent
  an expression over PRC. The parsed expression is reported to a processor
  in process() method.

  Assuming that P is a class that derives from Expr_parser<PRC>, p is a
  parser object of class P and prc is a processor of type PRC, a call:

    p.process(prc);

  will report to prc the expression represented by tokens specified when p
  was created. If tokens could not be correctly parsed an error is thrown
  either when this method is called or when p is created. It is up to
  the implementation to decide when the parsing happens: it can be during
  parser creation (and then the parser must store results of parsing) or it
  can be done on-the-fly, in the process() method. Method p.parse(prc) has
  the same effect as p.process(prc).

  It is also possible to call:

    p.consume();

  which consumes tokens of the expression without reporting them to any
  parser.

  Since parsing consumes tokens, it is assumed that parse()/process() can
  be called only once for a given parser instance. Another call will throw
  an error.

  Derived classes should implement the functionality by overriding do_parse()
  and do_consume() methods. By default do_consume() is implemented by calling
  do_parse() with NULL processor pointer.
*/

template <class PRC>
class Expr_parser
  : public cdk::api::Expr_base<PRC>
  , protected Token_op_base
{
public:

  Expr_parser(It &first, const It &last)
    : m_consumed(false)
  {
    set_tokens(first, last);
  }

  void process(PRC &prc) const
  {
    if (!const_cast<Expr_parser*>(this)->parse(prc))
      cdk::throw_error("Expr_parser: failed to parse");
  }

  /*
    Parse tokens specified when creating this parser instance and
    report parsed expression to the given processor.

    This method can be called only once and it is assumed that it
    consumes the tokens that were parsed. That is, after a successful
    call to parse() the first iterator passed to the constructor
    is moved and points at the first token after the parsed expression.

    Returns false if tokens could not be parsed as an expression and
    and no tokens have been consumed (first iterator is not moved).
    Returns true if complete expression has been parsed. Otherwise
    (could not parse but some tokens were consumed) throws parse
    error.

    Note: this method is implemented in terms of do_parse() which
    should be overriden by derived classes.
  */

  bool parse(PRC &prc)
  {
    if (m_consumed)
      THROW("Expr_praser: second pass");

    if (!do_parse(cur_pos(), end_pos(), &prc))
      return false;
    m_consumed = true;
    return true;
  }

  /*
    Consume tokens that form an expression without reporting them
    to any processor.

    Like parse(), this method can be called only once and should
    move the first iterator.

    Note: this method is implemented in terms of do_consume() which
    can be overriden by derived classes to provide more efficient
    implementation.
  */

  void consume()
  {
    if (m_consumed)
      return;
    do_consume(cur_pos(), end_pos());
    m_consumed = true;
  }


  /*
    Helper method which calls consume() if prc is NULL, otherwise
    calls parse() reporting results to the processor pointed by prc.
  */

  bool process_if(PRC *prc)
  {
    if (prc)
      return parse(*prc);
    consume();
    return true;
  }


protected:

  bool m_consumed;

  /*
    Internal method that implements parse() method - to be overriden
    by derived calss.

    See documentation of parse() for return value specification and
    information how first iterator should be updated.
  */

  virtual bool do_parse(It &first, const It &last, PRC *prc) =0;

  /*
    Internal method that implements consume() method. By default it
    calls parse() with NULL processor but dervied classes can override
    to provide more efficient implementation.
  */

  virtual void do_consume(It &first, const It &last)
  {
    if (!do_parse(first, last, NULL))
      THROW("Expr_parser: parsing did not consume tokens");
  }
};


}  // parser


//-------------------------------------------------------------------------


namespace parser {

using cdk::api::List_processor;


/*
  Template which constructs a parser for a list of expressions given a base
  parser for a single expression.

  List_parser<> is implemented using on-the-fly parsing.
*/

template <class Base>
struct List_parser
  : public Expr_parser< List_processor<typename Base::Processor> >
  , cdk::foundation::nocopy
{
  typedef typename Base::Processor  PRC;
  typedef List_processor<PRC>       LPRC;
  Token::TokenType      m_list_sep;


  List_parser(It &first, const It &last, Token::TokenType sep = Token::COMMA)
    : Expr_parser<LPRC>(first, last), m_list_sep(sep)
  {}


  bool do_parse(It& first, It const& last, LPRC *prc)
  {
    bool first_element = true;

    do {

      Base el_parser(first, last);

      if (!el_parser.process_if(prc ? prc->list_el() : NULL))
      {
        if (first_element)
          return false;
        else
          throw Error("Expected next list element");
      }

      if (m_list_sep != first->get_type())
        break;

      first_element = false;
      ++first;
    }
    while (true);

    return true;
  }

};

}  // parser


//-------------------------------------------------------------------------

namespace parser {

using cdk::api::Expr_base;
using cdk::api::Expr_list;
using cdk::api::Any;
using cdk::api::Doc_processor;
using cdk::api::Any_processor;



/*
  Extend base parser with document and array parsing.

  Given type Base of the base parser, Any_parser<Base> is a parser
  which can parse documents, arrays or expressions recognized by the
  base parser. Document and array elements can be again any kind of
  expression recognized by Any_parser. If the first token is '{' or '['
  then Any_parser<> assumes that this is document/array expression.
  Otherwise it must be base expression.

  Any_parser<Base> reports parsed expression to a processor of type
  Any_processor<SPRC>, where SPRC is a processor type for base (scalar)
  values. Normally SPRC is the processor type of the base parser, but
  a different SPRC type can be specified when instantiationg Any_parser<>
  template.

  The Base class must define static method for converting processor
  used by Any_parser<> to a processor used by the base parser. The expected
  signature of this method is:

    static Base::Processor* get_base_prc(Any_processor<SPRC>*);

  where SPRC is the scalar processor type specified for Any_parser<>
  template (so, it is Base::Processor by default).

*/

template <class Base,
          class SPRC = Any_processor<typename Base::Processor>
         >
struct Any_parser
  : public Expr_parser< Any_processor<SPRC> >
{
  typedef typename Base::Processor                PRC;
  typedef typename Any<SPRC>::Processor           APRC;
  typedef typename Any<SPRC>::Document::Processor DPRC;
  typedef typename Any<SPRC>::List::Processor     LPRC;


  Any_parser(It &first, const It &last)
    : Expr_parser<APRC>(first, last)
  {}


  bool do_parse(It &first, const It &last, APRC *prc)
  {
    if (Token::LCURLY == first->get_type())
    {
      Doc_parser doc(first, last);
      doc.process_if(prc ? prc->doc() : NULL);
    }
    else if (Token::LSQBRACKET == first->get_type())
    {
      Arr_parser arr(first, last);
      arr.process_if(prc ? prc->arr() : NULL);
    }
    else
    {
      Base val(first, last);
      return val.process_if(prc ? Base::get_base_prc(prc) : NULL);
    }

    return true;
  }

  // Array parser used by Any_parser

  struct Arr_parser : public Expr_parser<LPRC>
  {
    Arr_parser(It &first, const It &last)
      : Expr_parser<LPRC>(first, last)
    {}

    bool do_parse(It &first, const It &last, LPRC *prc)
    {
      if (Token::LSQBRACKET != first->get_type())
        return false;
      ++first;

      if (prc)
        prc->list_begin();

      if (Token::RSQBRACKET != first->get_type())
      {
        List_parser<Any_parser> list(first, last);
        bool ok = list.process_if(prc);
        if (!ok)
          throw Error("Array parser: expected array element");
      }

      if (Token::RSQBRACKET != first->get_type())
        throw Error("Array parser: expected closing ']'");
      ++first;

      if (prc)
        prc->list_end();

      return true;
    }

  };

  // Document parser used by Any_parser

  struct Doc_parser
    : public Expr_parser<DPRC>
    , cdk::foundation::nocopy
  {
    Doc_parser(It &first, const It &last)
      : Expr_parser<DPRC>(first, last)
    {}

    /*
      Document parser treats document body as a list of
      key-value pairs. KV_parser parses single key-value
      pair and reports it to a document processor (using
      key_val() callback).
    */

    struct KV_parser;

    /*
      LPrc instance converts a document processor into
      a list processor that can process results of parsing
      a list of key-value pairs. Given document processor
      is returned for each pair in the list. This way a KV_parser
      which parses the key-value pair will report it to the
      document processor.
    */

    struct LPrc : public List_processor<DPRC>
    {
      using typename List_processor<DPRC>::Element_prc;
      DPRC *m_prc;

      LPrc(DPRC *prc) : m_prc(prc)
      {}

      void list_begin() {}
      void list_end()   {}

      Element_prc* list_el()
      {
        return m_prc ? m_prc : NULL;
      }
    };


    bool do_parse(It &first, const It &last, DPRC *prc)
    {
      if (Token::LCURLY != first->get_type())
        return false;
      ++first;

      if (prc)
        prc->doc_begin();

      if (Token::RCURLY != first->get_type())
      {
        List_parser<KV_parser> kv_list(first, last);

        LPrc kv_prc(prc);
        bool ok = kv_list.parse(kv_prc);
        if (!ok)
          throw Error("Document parser: expected key-value pair");
      }

      if (Token::RCURLY != first->get_type())
        throw Error("Document parser: Expected closing '}'");
      ++first;

      if (prc)
        prc->doc_end();

      return true;
    }

    // TODO: efficient skipping of documents

    // Parser for a single key-value pair.

    struct KV_parser
      : public Expr_parser<DPRC>
    {
      cdk::string m_key;

      KV_parser(It &first, const It &last)
        : Expr_parser<DPRC>(first, last)
      {}

      bool do_parse(It &first, const It &last, DPRC *prc)
      {
        // Note: official JSON specs do not allow plain ID as key name

        if (   Token::ID != first->get_type()
            && Token::LSTRING != first->get_type())
          return false;

        m_key = first->get_text();
        ++first;
        if (Token::COLON != first->get_type())
          throw Error("Document parser: Expected ':' after key name");
        ++first;

        Any_parser val_parser(first, last);
        bool ok = val_parser.process_if(prc ? prc->key_val(m_key) : NULL);
        if (!ok)
          throw Error("Document parser: expected value for a key");

        return true;
      }
    };
  };

};  // Any_parser


/*
  Expose document and array parsers from Any_parser<> in the
  main namespace.
*/

template <class Base, class SPRC = typename Base::Processor>
struct Doc_parser : public Any_parser<Base, SPRC>::Doc_parser
{
  Doc_parser(It &first, const It &last)
    : Any_parser<Base,SPRC>::Doc_parser(first, last)
  {}
};


template <class Base, class SPRC = typename Base::Processor>
struct Arr_parser : public Any_parser<Base, SPRC>::Arr_parser
{
  Arr_parser(It &first, const It &last)
    : Any_parser<Base,SPRC>::Arr_parser(first, last)
  {}
};


}  // parser

#endif
