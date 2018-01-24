/*
 * Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include <mysql/cdk.h>

PUSH_SYS_WARNINGS
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <stdexcept>
#include <sstream>
POP_SYS_WARNINGS

#undef WORD

/*
  Definitions of tokens recognized by tokenizer.

  Each macro TOKEN_LIST(), SYMBOL_LIST1() and SYMBOL_LSIT2() defines list
  of tokens with the following entry for each token:

    X(NNN,SSS)

  where Token::NNN is this token's enum constant and SSS, if not NULL, defines
  characters of the token. For tokens which are not simple 1 or 2 character
  sequences, but are recognized by tokenizer logic, SSS is NULL.
*/

#define TOKEN_LIST(X) \
    X(WORD, NULL) \
    X(QWORD, NULL)    /* word in backtick quotes */ \
    X(QSTRING, NULL)  /* string in single quotes */ \
    X(QQSTRING, NULL) /* string in double quotes */ \
    X(NUMBER, NULL)   /* floating number */ \
    X(INTEGER, NULL)  /* integer number */ \
    X(HEX, NULL)      /* hexadecimal number*/\
    SYMBOL_LIST1(X) \
    SYMBOL_LIST2(X) \

// 2 char symbols

#define SYMBOL_LIST2(X) \
    X(NE, "!=") \
    X(DF, "<>") \
    X(GE, ">=") \
    X(LE, "<=") \
    X(LSHIFT, "<<") \
    X(RSHIFT, ">>") \
    X(DOUBLESTAR, "**") \
    X(ARROW2, "->>") \
    X(ARROW, "->") \
    X(AMPERSTAND2, "&&") \
    X(BAR2, "||") \
    X(EQ2, "==")

// 1 char symbols

#define SYMBOL_LIST1(X) \
    X(LPAREN,"(") \
    X(RPAREN,")") \
    X(LCURLY, "{") \
    X(RCURLY, "}") \
    X(LSQBRACKET,"[") \
    X(RSQBRACKET,"]") \
    X(DOT, ".") \
    X(COMMA, ",") \
    X(EQ, "=") \
    X(GT, ">") \
    X(LT, "<") \
    X(AMPERSTAND, "&") \
    X(BAR, "|") \
    X(HAT, "^") \
    X(PLUS, "+") \
    X(MINUS, "-") \
    X(STAR, "*") \
    X(SLASH, "/") \
    X(PERCENT, "%") \
    X(BANG, "!") \
    X(TILDE, "~") \
    X(QUESTION, "?") \
    X(COLON, ":") \
    X(DOLLAR, "$") \
    /*X(AT,"@")*/



namespace parser {

  using cdk::string;
  using cdk::char_t;
  using cdk::throw_error;

  class Token;


  /*
    Base class for all parser and tokenizer errors.

    This class can be used in catch handlers to catch all errors
    related to parsing.
  */

  struct Error : public cdk::Error
  {
    template <typename T>
      Error(T arg)
        : cdk::Error(arg)
      {}
  };


  /*
    Tokenizer and parser error base which shows parsing context in error
    description.

    Instances of Error keep parsing context information which consists of
    the current parser position within the string, and fragments of the string
    before and after parsing position. This information is stored directly in
    the error object and uses statically allocated memory to avoid dynamic memory
    allocation at the time when error is thrown.

    Parser errors use error code cdkerrc::parser_error in the generic cdk
    category. Unless overridden, parser errors produce error descriptions of
    the form:

      "CTX: MSG"

    where MSG is the message passed to the error constructor and CTX describes
    position of the parser in the parsed string. It can be something like
    "After seeing '...', looking at '...'" (see print_ctx() for exact forms of
    the context string).

    Note: This class template is parametrized by the string type, which can
    be either a wide or a standard string, depending on which strings the
    parser is working on (we have both cases). Remainig template parameters
    specify sizes of buffers used to store input string fragments.
  */

  template <
    typename string_t,
    size_t  seen_buf_len = 64,
    size_t  ahead_buf_len = 8
  >
  class Error_base
    : public cdk::Error_class<
        Error_base<string_t, seen_buf_len, ahead_buf_len>,
        parser::Error
      >
  {
    typedef cdk::Error_class<
              Error_base<string_t, seen_buf_len, ahead_buf_len>,
              parser::Error
            >  Base;

  protected:

    typedef typename string_t::value_type  char_t;

    // Storage for context data.

    char_t   m_seen[seen_buf_len];   // Characters seen before current position.
    char_t   m_ahead[ahead_buf_len]; // Few characters ahead of the current position.
    size_t   m_pos;                  // Current parser position.

    string   m_msg;

    //  Print parser context description to the given ostream.

    virtual void print_ctx(std::ostream&) const;

    virtual void do_describe1(std::ostream &out) const
    {
      print_ctx(out);
      if (!m_msg.empty())
        out << ": " << m_msg;
    }

    using Base::code;

    void do_describe(std::ostream &out) const
    {
      do_describe1(out);
      out << " (" << code() << ")";
    }

  public:

    /*
      Parser error for input string `inp` with given parser position
      in the string.
    */

    Error_base(
      const string_t &inp, size_t pos,
      const string &descr = string()
    );

    /*
      Parser error with parser position at a given token. The token
      contains information about its position within the input
      string. If `tok` is NULL then it is assumed that parser has
      consumed the whole of the input string.
    */
    Error_base(
      const string_t &inp, const Token *tok,
      const string &descr = string()
    );

    virtual ~Error_base() throw ()
    {}

    size_t get_pos() const
    {
      return m_pos;
    }
  };


  // -------------------------------------------------------------------------

  /*
    Class representing a single token.

    It stores token type, characters which make the token and its position
    within the parsed string (begin and end position).

    Note: For tokens such as quotted string, the characters of the token do
    not include the quotes. For that reason characters of the token are not
    always identical with the sub-range [_pos_begin, _pos_end) of the input
    string.
  */

  class Token
  {
  public:

#define token_enum(T,X) T,

    enum Type
    {
      TOKEN_LIST(token_enum)
    };

    typedef std::set<Type>  Set;

    Token(
      Type type, const string& text,
      size_t begin, size_t end
    )
      : _type(type), _text(text)
      , _pos_begin(begin), _pos_end(end)
    {}

    const string& get_text() const
    {
      return _text;
    }

    Type get_type() const
    {
      return _type;
    }

    size_t get_begin() const
    {
      return _pos_begin;
    }

    size_t get_end() const
    {
      return _pos_end;
    }

#define token_name(X,T) case X: return #X;

    static const char* get_name(int type)
    {
      switch (type)
      {
        TOKEN_LIST(token_name)
        default: return "<UNKNOWN>";
      }
    }

    const char* get_name() const
    {
      return get_name(_type);
    }

  private:

    Type _type;
    string _text;
    size_t    _pos_begin;
    size_t    _pos_end;

  };


  template <
    typename string_t,
    size_t  seen_buf_len,
    size_t  ahead_buf_len
  >
  inline
  Error_base<string_t, seen_buf_len, ahead_buf_len>::Error_base(
      const string_t &inp, const Token *tok, const string &msg
  )
    : Error_base(inp, tok ? tok->get_begin() : inp.length(), msg)
  {}


  // -------------------------------------------------------------------------

  /*
    Class implementing parsing characters into tokens.

    After creating a Tokenizer instance from a given string, one can use
    Tokenizer::iterator returned by method begin() to iterate through the
    sequence of tokens.

    Any errors in converting a string into a token sequence are thrown upon
    construction of a tokenizer.

    TODO: Avoid storing all tokens in memory.
    Idea: have it working on istream instead.
  */

  class Tokenizer
  {
  public:

    class Error;
    class  iterator;

    Tokenizer(const string& input)
      : _input(input)
      , _in_pos(0)
      , _tok_pos(0)
      //, _pos(0)
    {
      get_tokens();
    }


    bool empty() const
    {
      return _tokens.empty();
    }

    iterator begin() const;
    iterator end() const;

  protected:

    // Build token sequence from the input string.

    void get_tokens();

    // Methods that parse characters into various kinds of tokens.

    bool parse_number();
    bool parse_digits(string *digits = NULL);
    bool parse_hex();
    bool parse_hex_digits(string &digits);
    bool parse_string();
    bool parse_word();
    bool parse_quotted_string(char_t, string *val = NULL);

    // access underlying sequence of characters

    const string& get_input() const { return _input; }

    char_t cur_char() const;
    size_t get_char_pos() const;
    bool chars_available() const;

    bool next_char_is(char_t, size_t off=1) const;
    bool next_char_in(const char_t*, size_t off=1) const;

    bool cur_char_is(char_t c) const
    {
      return next_char_is(c, 0);
    }

    bool cur_char_is_space() const;

    // Return true if current character can be part of a WORD token.

    bool cur_char_is_word() const;

    bool cur_char_in(const char_t *set) const
    {
      return next_char_in(set, 0);
    }


    char_t consume_char();

    // Consume next character if it equals given one.

    bool   consume_char(char_t);

    /*
      Consume next character if it is one of the character in the given
      string. Returns consumed character, '\0' otherwise.
    */

    char_t consume_char(const char_t*);

    /*
      Consume given sequence of characters. Returns true if it was possible.
      If not, the position within input string is not changed.
    */

    bool   consume_chars(const char_t*);

    // Error reporting

    void token_error(const string&) const;

    // Building token sequence

    /*
      Set current position in the input string as a starting position for
      next token.
    */

    size_t set_token_start();

    /*
      Add to the sequence new token of a given type. The token ends at the
      current position within the input string and starts at the position
      marked with set_token_start(). The characters of the token are all the
      caracters of the input string between token's start and end position.
    */

    void add_token(Token::Type);

    /*
      This method works like `add_token(Token::Type)` but the characters of
      the new token are as given by the second argument (instead of being taken
      from the input string).
    */

    void add_token(Token::Type, const string&);


    // Storage for tokens and the input string

    string _input;
    size_t _in_pos;   // current position in the input string
    size_t _tok_pos;  // start position of a token in the input string

    std::vector<Token> _tokens;
    //tokens_t::size_type _pos;

    friend Error;
  };


  /*
    Iterator for accessing a sequence of tokens of a tokenizer.
  */

  class Tokenizer::iterator
  {
    const Tokenizer *_toks;
    size_t     _pos;

    iterator(const Tokenizer *toks, size_t pos = 0)
      : _toks(toks), _pos(pos)
    {}

  public:

    iterator()
      : _toks(NULL), _pos(0)
    {}

    iterator(const Tokenizer &toks, bool at_end = false)
      : _toks(&toks), _pos(0)
    {
      if (at_end)
        _pos = toks._tokens.size();
    }

    iterator(const iterator &other)
      : _toks(other._toks), _pos(other._pos)
    {}

    const Token& operator*() const
    {
      if (!_toks)
        THROW("token iterator: accessing null iterator");
      return _toks->_tokens[_pos];
    }

    const Token* operator->() const
    {
      if (!_toks)
        THROW("token iterator: accessing null iterator");
      return &(_toks->_tokens[_pos]);
    }

    iterator& operator++()
    {
      if (_toks && _pos < _toks->_tokens.size())
        ++_pos;
      return *this;
    }

    bool operator==(const iterator &other) const
    {
      return _toks == other._toks && _pos == other._pos;
    }

    bool operator!=(const iterator &other) const
    {
      return !(*this == other);
    }

    iterator operator+(size_t diff) const
    {
      return iterator(_toks, _pos + diff > 0 ? _pos + diff : 0);
    }

    friend Tokenizer::Error;
  };


  inline
  Tokenizer::iterator Tokenizer::begin() const
  {
    return iterator(*this);
  }

  inline
  Tokenizer::iterator Tokenizer::end() const
  {
    return iterator(*this, true);
  }


  /*
    Tokenizer error class.

    It is a specialization of the generic Error_base which defines
    convenience constructors.
  */

  class Tokenizer::Error
    : public parser::Error_base<string>
  {
  public:

    Error(const Tokenizer *p, const string &descr = string())
      : parser::Error_base<string>(p->_input, p->_in_pos, descr)
    {}

    Error(const Tokenizer::iterator &it, const string &msg = string())
      : parser::Error_base<string>(
          it._toks->_input,
          it != it._toks->end() ? &(*it) : NULL,
          msg
        )
    {}
  };


  inline
  void Tokenizer::token_error(const string &msg) const
  {
    throw Error(this, msg);
  }


  // -------------------------------------------------------------------------
  //  Error class implementation
  // -------------------------------------------------------------------------


  /*
    Construct error instance copying fragments of the parsed string
    to the internal buffers to be used in the error description.

    Note: MSVC generates warning for std::string::copy() method
    used below because it is considered unsafe.
  */

  DIAGNOSTIC_PUSH
  #if _MSC_VER
  DISABLE_WARNING(4996)
  #endif

  template <
    typename string_t,
    size_t  seen_buf_len,
    size_t  ahead_buf_len
  >
  inline
  Error_base<string_t, seen_buf_len, ahead_buf_len>::Error_base(
    const string_t &ctx, size_t pos, const string &descr
  )
    : Base(NULL, cdk::cdkerrc::parse_error)
    , m_pos(pos), m_msg(descr)
  {
    memset(m_seen, 0, sizeof(m_seen));
    memset(m_ahead, 0, sizeof(m_ahead));

    if (!ctx.empty())
    {
      /*
        Calculate how much to copy into m_seen, 1 byte is left for
        null terminator.
      */

      size_t howmuch;

      if (m_pos  > seen_buf_len - 1)
        howmuch = seen_buf_len - 1;
      else
        howmuch = m_pos;

      ctx.copy(m_seen, howmuch, m_pos-howmuch);

      /*
        If initial fragment is longer than size of m_seen, then
        we set first byte to 0 to indicate that '...' prefix should
        be added.
      */

      if (m_pos > seen_buf_len - 1)
        m_seen[0] = 0;

      /*
        Similar, if remainder of the string does not fit in
        m_ahead, then the last byte is set to 1 to indicate that
        '...' should be added at the end. Note: Second last byte
        is used as null terminator.
      */

      ctx.copy(m_ahead, ahead_buf_len - 2, m_pos);

      if (ctx.length() > m_pos + ahead_buf_len - 2)
        m_ahead[ahead_buf_len - 1] = 1;
    }
  }

  DIAGNOSTIC_POP


  /*
    Print parser context description used in parser error descriptions.

    It has one of these forms:

    "After seeing '...AAA', looking at 'BBB...'"
    "After seeing '...AAA', with no more characters in the string"
    "While looking at 'BBB...'"
    "While looking at empty string"
  */

  template <
    typename string_t,
    size_t  seen_buf_len,
    size_t  ahead_buf_len
  >
  inline
  void parser::Error_base<string_t, seen_buf_len, ahead_buf_len>
  ::print_ctx(std::ostream &out) const
  {
    bool seen_part = false;

    // Note: cdk::string() used for utf8 conversion.

    if (m_seen[0] || m_seen[1])
    {
      seen_part = true;
      out << "After seeing '";
      if (!m_seen[0])
        out << "..." << cdk::string(m_seen + 1);
      else
        out << cdk::string(m_seen);
      out << "'";
    }

    if (m_ahead[0])
    {
      if (seen_part)
        out << ", looking at '";
      else
        out << "While looking at '";

      out << cdk::string(m_ahead);

      if (1 == m_ahead[ahead_buf_len - 1])
        out << "...";

      out << "'";
    }
    else
    {
      if (seen_part)
        out << ", with no more characters in the string";
      else
        out << "While looking at empty string";
    }
  }


  // -------------------------------------------------------------------------
  //  String to number conversions.
  // -------------------------------------------------------------------------
  //
  // TODO: Consider if it should not be implemented as a numeric codec.


  // Numeric conversion error classes.

  class Numeric_conversion_error
    : public cdk::Error_class<Numeric_conversion_error>
  {
    typedef cdk::Error_class<Numeric_conversion_error> Base;

  protected:

    string m_inp;

    void do_describe(std::ostream &out) const
    {
      out << msg() << " (" << code() << ")";
    }

  public:

    Numeric_conversion_error(const string &inp)
      : Base(NULL, cdk::cdkerrc::parse_error)
      , m_inp(inp)
    {}

    virtual string msg() const
    {
      string msg(L"Failed to convert string '");
      msg.append(m_inp);
      msg.append(L"' to a number");
      return msg;
    }
  };


  class Numeric_conversion_partial
    : public cdk::Error_class<
        Numeric_conversion_partial, Numeric_conversion_error
      >
  {
    typedef cdk::Error_class<
              Numeric_conversion_partial, Numeric_conversion_error
            > Base;

  public:

    Numeric_conversion_partial(const string &inp)
      : Base(NULL, inp)
    {}

    string msg() const override
    {
      string msg(L"Not all characters consumed when converting string '");
      msg.append(m_inp);
      msg.append(L"' to a number");
      return msg;
    }
  };


  /*
    Generic string to number conversion function template.

    Retrurns numeric value after converting given string in a given base,
    which should be either 10, 16 or 8. Throws error if the whole string
    could not be converted to a number.

    Unlike strtod() and friends, this function does not depend on the current
    locale setting but always uses the "C" locale (so that, e.g., decimal point
    character is always '.').
  */

  template<
    typename Num_t
  >
  inline
  Num_t strtonum(const string &str, int radix = 10)
  {
    // TODO: Allow white-space at the beggingin or end of the string?

    typedef std::istreambuf_iterator<char_t> iter_t;
    static std::locale c_locale("C");
    static const std::num_get<char_t> &cvt
      = std::use_facet<std::num_get<char_t>>(c_locale);

    std::wistringstream inp(str);
    Num_t val;

    inp.imbue(c_locale);

    switch (radix) {
    case 10: inp.setf(std::ios_base::dec, std::ios_base::basefield); break;
    case 16: inp.setf(std::ios_base::hex, std::ios_base::basefield); break;
    case  8: inp.setf(std::ios_base::oct, std::ios_base::basefield); break;
    default:
      inp.setf(std::ios_base::fmtflags(0), std::ios_base::basefield);
      break;
    }

    /*
      Note: We could use istream::operator>>() to do conversion, but then
      there are problems with detecting conversion errors on some platforms
      (OSX). For that reason we instead use a number conversion facet directly.
      This gives direct access to the error information.
    */

    iter_t beg(inp), end;
    std::ios::iostate err = std::ios_base::goodbit;

    iter_t last = cvt.get(beg, end, inp, err, val);

    if (std::ios_base::goodbit != err && std::ios_base::eofbit != err)
      throw Numeric_conversion_error(str);

    if (last != end)
      throw Numeric_conversion_partial(str);

    return val;
  }


  inline
  double strtod(const std::string &str)
  {
    return strtonum<double>(str);
  }

  inline
  uint64_t strtoui(const std::string &str, int radix = 10)
  {
    return strtonum<uint64_t>(str, radix);
  }

  inline
  int64_t strtoi(const std::string &str, int radix = 10)
  {
    return strtonum<int64_t>(str, radix);
  }

}  // parser

#endif
