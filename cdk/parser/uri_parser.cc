/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
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

#include "uri_parser.h"
#include <mysql/cdk.h>

PUSH_SYS_WARNINGS
#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include <bitset>
#include <cstdarg>
POP_SYS_WARNINGS


using namespace parser;
using std::string;


void parser::parse_uri(const std::string &uri, URI_processor &up)
{
  URI_parser    parser(uri, true);
  parser.process(up);
}

void parser::parse_conn_str(const std::string &uri, URI_processor &up)
{
  URI_parser    parser(uri);
  parser.process(up);
}

// ---------------------------------------------------------------


/*
  Specialized error message with description:

  "XXX: Expected 'YYY': MSG"

  Where XXX is the base error description, YYY is the expected
  string (or character) passed to the constructor and MSG is
  optional extra message passed to the constructor.
*/

struct Unexpected_error
  : public cdk::Error_class< ::Unexpected_error, URI_parser::Error >
{
  std::string m_expected;
  cdk::string m_msg;

  Unexpected_error(
    const URI_parser *p,
    const std::string &exp,
    const cdk::string &descr = cdk::string()
  )
    : Error_class(NULL, p)
    , m_expected(exp)
    , m_msg(descr)
  {}

  Unexpected_error(
    const URI_parser *p,
    char c,
    const cdk::string &descr = cdk::string()
  )
    : Error_class(NULL, p)
    , m_msg(descr)
  {
    m_expected.append(1, c);
  }

  ~Unexpected_error() throw ()
  {}

  void do_describe1(std::ostream &out) const
  {
    URI_parser::Error::do_describe1(out);
    out << ": Expected '" << m_expected << "'";
    if (!m_msg.empty())
      out << ": " << m_msg;
  }
};


void URI_parser::unexpected(const std::string &what, const cdk::string &msg) const
{
  throw Unexpected_error(this, what, msg);
}

void URI_parser::unexpected(char what, const cdk::string &msg) const
{
  throw Unexpected_error(this, what, msg);
}

// ---------------------------------------------------------------

/*
  Tokens recognized by URI parser.
*/

#define URI_TOKEN_LIST(X) \
  X (COLON, ':')          \
  X (SEMICOLON, ';')      \
  X (SLASH, '/')          \
  X (BSLASH, '\\')        \
  X (AT, '@')             \
  X (QUESTION, '?')       \
  X (EXCLAMATION, '!')    \
  X (EQ, '=')             \
  X (AMP, '&')            \
  X (SQOPEN, '[')         \
  X (SQCLOSE, ']')        \
  X (COMMA, ',')          \
  X (HASH, '#')           \
  X (DOT, '.')            \
  X (PERCENT, '%')        \
  X (POPEN, '(')          \
  X (PCLOSE, ')')         \
  X (PLUS, '+')           \
  X (MINUS, '-')         \
  X (UNDERSCORE, '_')     \
  X (TILD, '~')           \
  X (DOLLAR, '$')         \
  X (QUOTE, '\'')         \
  X (ASTERISK, '*')       \
  X (SPACE, ' ')       \

#define URI_TOKEN_ENUM(T,C) T_##T,
enum token_type
{
  T_ZERO,
  T_CHAR,
  T_DIGIT,
  URI_TOKEN_LIST(URI_TOKEN_ENUM)
  T_LAST
};

#define SEPARATORS T_QUESTION, T_AMP, T_SLASH, T_HASH, T_POPEN, T_PCLOSE

/*
  Class representing set of token types.
*/

struct URI_parser::TokSet
{
  std::bitset<T_LAST> m_bits;

  template<typename...TYPE>
  TokSet(TYPE...t)
  {
    set_token(t...);
  }

  void set_token(token_type tt)
  {
    m_bits.set(tt);
  }

  template<typename...REST>
  void set_token(token_type tt, REST...rest)
  {
    set_token(tt);
    set_token(rest...);
  }

  bool has_token(token_type tt) const
  {
    return m_bits.test(tt);
  }
};


struct URI_parser::Guard
{
  URI_parser *m_parser;
  bool        m_pop = true;

  Guard(URI_parser *p) : m_parser(p)
  {
    m_parser->push();
  }

  ~Guard() { if (m_pop) m_parser->pop(); }

  void release() { m_pop = false; }
};



// ---------------------------------------------------------------


/*
 authority  ::= ( userinfo "@" )? ( address | address-list )
*/

void URI_parser::process_userinfo(Processor &prc) const
{
  URI_parser* self = const_cast<URI_parser*>(this);
  Guard guard(self);

  std::string user;
  std::string pass;
  bool has_pass = false;

  self->consume_until(user, TokSet(T_AT, T_COLON, SEPARATORS ));

  if (self->consume_token(T_COLON))
  {
    self->consume_until(pass, TokSet(T_AT));
    if (!self->consume_token(T_AT))
      return;

    has_pass = true;

  }
  else if(!self->consume_token(T_AT))
  {
    return;
  }

  prc.user(user);
  if (has_pass)
    prc.password(pass);

  guard.release();
}


/*
  address ::= ip-address | socket | pipe
*/

URI_parser::Address_type URI_parser::process_adress(std::string &address,
                                                    std::string &port)
{

  if (process_socket(address))
    return Address_type::SOCKET;
  if (process_pipe(address))
    return Address_type::PIPE;
  if (process_ip_address(address, port))
    return Address_type::IP;

  return Address_type::NONE;
}

/*
  socket           ::= pct-socket | unencoded-socket
  pct-socket       ::= socket-prefix file
  unencoded-socket ::= "(" socket-prefix unencoed-file ")"
  socket-prefix    ::= ("." | ".." | "/")?
*/


bool URI_parser::process_socket_prefix(std::string &socket)
{
  Guard guard(this);

  if (consume_token(T_DOT))
  {
    socket.push_back('.');
    if (consume_token(T_DOT))
      socket.push_back('.');
  }
  else if (consume_token(T_SLASH))
  {
    socket.push_back('/');
  }
  else
  {
    socket.clear();
    return false;
  }

  guard.release();

  return true;
}

bool URI_parser::process_pct_socket(std::string &socket)
{
  Guard guard(this);

  if (process_socket_prefix(socket) && process_file(socket))
  {
    guard.release();
    return true;
  }

  socket.clear();

  return false;
}

bool URI_parser::process_unencoded_socket(std::string &socket)
{
  Guard guard(this);

  if ( consume_token(T_POPEN) &&
       process_socket_prefix(socket) &&
       process_unencoded_file(socket) &&
       consume_token(T_PCLOSE))
  {
    guard.release();
    return true;
  }

  socket.clear();

  return false;
}

bool URI_parser::process_socket(std::string &socket)
{
  return process_pct_socket(socket) || process_unencoded_socket(socket);
}


/*
  pipe             ::= pct-pipe | unencoded-pipe
  pct-pipe         ::= pipe-prefix file
  unencoded-pipe   ::= "(" pipe-prefix unencoed-file ")"
  pipe-prefix      ::= "\\.\"

*/

bool URI_parser::process_pipe_prefix(std::string &pipe)
{
  Guard guard(this);

  if (consume_word("\\\\.\\"))
  {
    pipe += "\\\\.\\";
    guard.release();
    return true;
  }

  return false;
}

bool URI_parser::process_pct_pipe(std::string &pipe)
{
  Guard guard(this);

  if (process_pipe_prefix(pipe) && process_file(pipe))
  {
    guard.release();
    return true;
  }

  pipe.clear();

  return false;
}

bool URI_parser::process_unencoded_pipe(std::string &pipe)
{
  Guard guard(this);

  if ( consume_token(T_POPEN) &&
       process_pipe_prefix(pipe) &&
       process_unencoded_file(pipe) &&
       consume_token(T_PCLOSE))
  {
    guard.release();
    return true;
  }

  pipe.clear();

  return false;
}

bool URI_parser::process_pipe(std::string &pipe)
{
  return process_pct_pipe(pipe) || process_unencoded_pipe(pipe);
}


/*
  file             ::= (value | unencoded-file)
*/
bool  URI_parser::process_file(std::string &file)
{
  return process_value(sub_delims(), file) || process_unencoded_file(file);
}

/*
  value            ::= (unreserved | pct-encoded | sub-delims)*

  Delims is the sub-delims function to be used
*/

bool URI_parser::process_value(TokSet sub_delims, std::string &value)
{
  bool processed = false;

  while( process_tokens(unreserved(), value) ||
         process_pct_encoded_value(value) ||
         process_tokens(sub_delims, value))
  {
    processed = true;
  }

  return processed;
}

/*
  unencoded-file   ::= "(" ( unreserved | gen-delims )* ")"
*/
bool  URI_parser::process_unencoded_file(std::string &file)
{
  bool processed = false;

  while(process_tokens(unreserved(), file) ||
        process_tokens(gen_delims(), file))
  {
    processed = true;
  }

  return processed;
}

bool URI_parser::process_pct_encoded_value(std::string &encoded)
{
  Guard guard(this);

  if (!consume_token(T_PERCENT))
    return false;

  long c;

  // TODO: more efficient implementation.

  std::string hex = m_uri.substr(m_pos.top(), 2);
  hex.push_back('\0');
  char *end = NULL;
  c = strtol(hex.data(), &end, 16);
  if (end == hex.data() + 2 || c >= 0 || c <= 256)
  {
    encoded.push_back((char)c);

    guard.release();

    m_pos_next.top() = m_pos.top()+2;
    get_token();
  }

  return !guard.m_pop;
}


/*
  unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~"
*/

URI_parser::TokSet URI_parser::unreserved() const
{
  return TokSet( T_CHAR, T_DIGIT, T_MINUS, T_DOT, T_UNDERSCORE, T_TILD);
}

/*
  sub-delims  = "!" / "$" / "&" / "'" / "(" / ")"
                 / "*" / "+" / "," / ";" / "="
*/

URI_parser::TokSet URI_parser::sub_delims() const
{
  return TokSet( T_EXCLAMATION, T_DOLLAR, T_AMP, T_QUOTE,T_POPEN, T_PCLOSE,
                 T_ASTERISK, T_PLUS, T_COMMA, T_SEMICOLON, T_EQ);
}

/*
  Just like sub-delims, but without COMMA and EQUAL, wich are used on queries
  and added SLASH
*/

URI_parser::TokSet URI_parser::sub_delims_qry() const
{
  return TokSet( T_EXCLAMATION, T_DOLLAR, T_QUOTE, T_SLASH, T_POPEN, T_PCLOSE,
                 T_ASTERISK, T_PLUS, T_SEMICOLON);
}

/*
  gen-delims  = ":" / "/" / "?" / "#" / "[" / "]" / "@"
*/

URI_parser::TokSet URI_parser::gen_delims() const
{
  return TokSet( T_COLON, T_SLASH, T_QUESTION, T_HASH, T_SQOPEN,T_SQCLOSE,
                 T_AT);
}


bool URI_parser::process_tokens(TokSet t, std::string& str)
{
  if (next_token_in(t))
  {
    str.push_back(consume_token().get_char());
    return true;
  }
  return false;
}


unsigned short URI_parser::convert_val(const std::string &port) const
{
  const char *beg = port.c_str();
  char *end = NULL;
  long int val = strtol(beg, &end, 10);

  /*
    Note: strtol() returns 0 either if the number is 0
    or conversion was not possible. We distinguish two cases
    by cheking if end pointer was updated.
  */

  if (val == 0 && end == beg)
    throw Error(this, L"Expected number");

  if (val > 65535 || val < 0)
    throw Error(this, L"Invalid value");

  return static_cast<unsigned short>(val);
}

/*
  ip-address ::= "[" IPv6address "]" | IPv4address | reg-name
*/
bool URI_parser::process_ip_address(std::string &host, std::string &port)
{
  Guard guard(this);

  if (consume_token(T_SQOPEN))
  {
    // IPv6
    consume_while(host, TokSet(T_DIGIT, T_CHAR, T_COLON));
    if (!consume_token(T_SQCLOSE))
      return false;
  }
  else
  {
    consume_while(host, TokSet(T_DIGIT, T_CHAR, T_DOT, T_MINUS));
  }

  if (consume_token(T_COLON))
  {
    consume_while(port, T_DIGIT);

    if (port.empty())
      return false;
  }

  guard.release();

  return true;
}


bool URI_parser::report_address(Processor &prc,
                                Address_type type,
                                unsigned short priority,
                                const std::string &host,
                                const std::string &port) const
{
  bool processed = true;

  switch(type)
  {
    case Address_type::IP:
      if (port.empty())
        prc.host(priority, host);
      else
        prc.host(priority, host, convert_val(port));
      break;
    case Address_type::PIPE:
      prc.pipe(priority, host);
      break;
    case Address_type::SOCKET:
      prc.socket(priority,host);
      break;
    case Address_type::NONE:
      processed = false;
  }

  return processed;
}

/*
  address-list     ::= "[" address-priority ( "," address-priority )* "]"
*/
bool URI_parser::process_adress_list(Processor &prc) const
{
  URI_parser* self = const_cast<URI_parser*>(this);

  Guard guard(self);

  if (!self->consume_token(T_SQOPEN))
    return false;

  do
  {
    self->trim_spaces();

    if (!process_adress_priority(prc))
      return false;

    self->trim_spaces();

  }while(self->consume_token(T_COMMA));

  if (!self->consume_token(T_SQCLOSE))
  {
    return false;
  }

  guard.release();

  return true;
}

/*
  address-priority ::= address | "(" "address" "=" address "," "priority" "=" prio ")"
  prio             ::= DIGIT+
*/
bool URI_parser::process_adress_priority(Processor &prc) const
{
  URI_parser* self = const_cast<URI_parser*>(this);

  Guard guard(self);

  std::string host;
  std::string port;

  if (self->consume_token(T_POPEN))
  {
    self->trim_spaces();

    if (!self->consume_word_ci("address"))
      return false;

    self->trim_spaces();

    if(!self->consume_token(T_EQ))
      return false;

    Address_type type = self->process_adress(host, port) ;

    if (type == Address_type::NONE)
      return false;

    self->trim_spaces();

    if (self->consume_token(T_COMMA))
    {

      self->trim_spaces();

      if (self->consume_word_ci("priority"))
      {
        self->trim_spaces();

        if (!self->consume_token(T_EQ))
            parse_error(L"Expected priority= value");

        std::string prio_str;
        self->consume_while(prio_str, TokSet(T_DIGIT));

        if (prio_str.length() == 0)
          parse_error(L"Expected priority=value");

        report_address(prc, type, 1+self->convert_val(prio_str), host, port);

      }
      else
      {
        parse_error(L"Expected priority= value");
      }
    }

    if (!self->consume_token(T_PCLOSE))
    {
      return false;
    }
  }
  else
  {
    if (!self->report_address(prc, self->process_adress(host, port)
                                  ,0,host, port))
      return false;
  }

  guard.release();
  return true;

}

void URI_parser::process_path(Processor &prc) const
{
  URI_parser* self = const_cast<URI_parser*>(this);

  Guard guard(self);

  if (!self->consume_token(T_SLASH))
    return;

  std::string path;

  /*
    We allow only one path component (which can not contain '/').
    Here we consume tokens till end of the query part or '/', whichever
    comes first. If we see '/' then error is reported.
  */

  self->consume_until(path, TokSet(T_SLASH, SEPARATORS));

  prc.path(path);

  guard.release();
}

/*
  Note that, e.g., AUTHORITY part can be followed either by
  PATH, QUERY or FRAGMENT, depending on the character that follows
  after it:

  URI         ::= scheme ":" hier-part ( "?" query )? ( "#" fragment )?
  hier-part   ::= "//" authority (path-abempty | path-absolute | path-rootless | path-empty )
  authority   ::= ( userinfo "@" )? address | addresslist

    ...://<authority>/<path>...
    ...://<authority>?<query>...
    ...://<authority>#<fragment>...
*/



void URI_parser::process(Processor &prc) const
{
  URI_parser *self = const_cast<URI_parser*>(this);

  // Note: check_scheme() resets parser state.

  self->check_scheme(m_force_uri);

  self->process_userinfo(prc);

  /*
    Look for host and port, saving them in the corresponding variables.
    If user creadentials are detected, they are reported to the
    processor and rescan flag is set to true to look for host/port data
    again.
  */

  std::string host;
  std::string port;

  if (!self->process_adress_list(prc) &&
      !self->report_address(prc, self->process_adress(host, port),0,host, port))
    parse_error(L"Invalid address");

  process_path(prc);

  process_query(prc);

  if (self->consume_token(T_HASH))
  {
    parse_error( m_has_scheme ?
      L"Mysqlx URI can not contain fragment specification"
      : L"Unexpected characters at the end"
    );
  }

  if (has_more_tokens())
    parse_error(L"Unexpected characters at the end");
}


/*
  Process query part which consists of key-value pairs of the
  form "<key>=<value>" separated by '&'.

  The value part is optional. If it starts with '[' then we
  have a comma separated list of values.

  query           ::= "?" (pair | multiple-pairs)+
  pair            ::= ( key ( "=" (valuelist | value) )?)
  multiple-pairs  ::= pair("&" pair)+
  key             ::= (unreserved | pct-encoded | sub-delims)+
  valuelist       ::= "[" value ("," value)* "]"
  value           ::= (unreserved | pct-encoded | "!" | "$" | "'" | "(" | ")" |  "*" | "+" | ";" | "=")*

*/


void URI_parser::process_query(Processor &prc) const
{
  URI_parser* self = const_cast<URI_parser*>(this);

  std::string key;
  std::string val;

  if (!self->consume_token(T_QUESTION))
    return;


  do {
    key.clear();

    /*
      After key there should be '=' or, if key has no value,
      either '&' that separates next key-value pair or end of
      the query part.
    */

    self->process_value(sub_delims_qry(), key);


    if (!self->consume_token(T_EQ))
    {
      // The case of a key without a value.

      prc.key_val(key);
    }
    else
    {
      // If first value character is '[' then the value is a list.

      if (next_token_is(T_SQOPEN))
      {
        process_list(key, prc);
      }
      else
      {
        /*
          If value is not a list, then it extends until the next
          '&' or end of the query part
        */
        val.clear();

        self->process_value(sub_delims_qry(), val);

        prc.key_val(key, val);
      }
    }

  }
  while (self->consume_token(T_AMP));
}


/*
  Process comma separated list of values enlosed in '[' and ']',
  reporting this list as value of given key.

*/

void URI_parser::process_list(const std::string &key, Processor &prc) const
{
  URI_parser *self = const_cast<URI_parser*>(this);

  if (!self->consume_token(T_SQOPEN))
    return;

  std::list<std::string> list;
  std::string val;

  do {
    val.clear();

    self->process_value(sub_delims_qry(), val);

    list.push_back(val);
  }
  while (self->consume_token(T_COMMA));

  if (!self->consume_token(T_SQCLOSE))
  {
    std::ostringstream msg;
    msg << "Missing ']' while parsing list value of query key '"
        << key <<"'" << std::ends;
    parse_error(msg.str());
  }

  prc.key_val(key, list);
}


// -------------------------------------------------
//  Helper methods
// -------------------------------------------------

/*
  Consume tokens and store in the given buffer until the end of
  the current URI part or until a token of type
  from the given set is seen.
*/

void URI_parser::consume_until(std::string &buf, const TokSet &toks)
{
  while (has_more_tokens() && !next_token_in(toks))
    buf.push_back(consume_token().get_char());
}

/*
  Consume tokens and store in the given buffer while current Token type belongs
  to the given set
*/

void URI_parser::consume_while(std::string &buf, const TokSet &toks)
{
  while (has_more_tokens() && next_token_in(toks))
    buf.push_back(consume_token().get_char());
}

/*
  Consume all remaining tokens of the current URI part and store
  them in the given buffer.
*/

void URI_parser::consume_all(std::string &buf)
{
  while (has_more_tokens())
    buf.push_back(consume_token().get_char());
}

void URI_parser::trim_spaces()
{
  std::string dummy;
  consume_while(dummy, T_SPACE);
}


// Check type of next token.

bool URI_parser::next_token_is(short tt) const
{
  return !at_end() && tt == m_tok.top().get_type();
}

//  Check if type of next token is in the given set.

bool URI_parser::next_token_in(const TokSet &toks) const
{
  if (!has_more_tokens())
    return false;
  return toks.has_token(token_type(m_tok.top().get_type()));
}

void URI_parser::push()
{
  m_tok.push(m_tok.top());
  m_pos.push(m_pos.top());
  m_pos_next.push(m_pos_next.top());
}

void URI_parser::pop()
{
  m_tok.pop();
  m_pos.pop();
  m_pos_next.pop();
}


// -------------------------------------------------


/*
  Check the scheme part of the URI (if present) and set
  the initial parser state.

  If parameter 'force' is true, error is thrown if the
  'mysqlx:://' scheme prefix is not present. Otherwise scheme
  is optional, but if present it must be 'mysqlx'.
*/

bool URI_parser::check_scheme(bool force)
{
  m_pos_next.top() = 0;
  m_has_scheme = false;

  m_pos.top() = m_uri.find("://");
  if (m_pos.top() != std::string::npos)
  {
    m_has_scheme = true;
    if (m_uri.substr(0, m_pos.top()) != "mysqlx")
      parse_error(L"Expected URI scheme 'mysqlx'");

    // move to the first token after '://'
    m_pos_next.top() = m_pos.top() + 3;
  }
  else
  {
    // set m_pos for correct error reporting
    m_pos.top() = 0;

    if (m_uri.substr(0, 6) == "mysqlx")
    {
      m_pos.top() = 6;
      unexpected("://");
    }

    if (force)
      parse_error(L"URI scheme expected");
  }

  get_token();

  return m_has_scheme;
}


/*
  Get next token, store it in m_tok and update parser state.

  If in_part is true then only tokens from the current part
  are considered. Otherwise the whole string is considered.

  Returns false if there are no more tokens (in the current part).
*/

bool URI_parser::get_token()
{
  m_pos = m_pos_next;

  if (at_end())
    return false;

  m_tok.top() = Token(m_uri[m_pos_next.top()++]);

  return true;
}

/*
  Return true if all tokens from the URI string have been consumed.
*/

bool URI_parser::at_end() const
{
  return m_pos.top() >= m_uri.length();
}

/*
  Return true if there is at least one more token which
  belongs to the current URI part (not counting the delimiter).
*/

bool URI_parser::has_more_tokens() const
{
  return !at_end();
}


/*
  Return the current token and proceed to the next one (if any).
  Throws error if next token is not available.
*/

URI_parser::Token URI_parser::consume_token()
{
  if (at_end())
    parse_error(L"Expected more characters");
  Token cur_tok(m_tok.top());
  get_token();
  return cur_tok;
}

/*
  Check if there is next token in the current part of the URI
  and if it is of given type. If yes, consume it, moving to the
  next token.

  Returns false if there are no more tokens in the current part
  or the next token is not of the given type.
*/

bool URI_parser::consume_token(short tt)
{
  if (!has_more_tokens())
    return false;
  if (!next_token_is(tt))
    return false;
  consume_token();
  return true;
}

template <typename C>
bool URI_parser::consume_word_base(const std::string &word, C compare)
{
  Guard guard(this);

  for (auto el : word)
  {
    if (!has_more_tokens())
      return false;

    if (!compare(m_tok.top().get_char(),el))
      return false;

    consume_token();

  }

  guard.release();

  return true;

}
bool URI_parser::consume_word(const std::string &word)
{
  auto compare= [](char a, char b) -> bool
  {
    return a == b;
  };

  return consume_word_base(word, compare);
}

bool URI_parser::consume_word_ci(const std::string &word)
{
  auto compare_ci= [](char a, char b) -> bool
  {
    return ::tolower(a) == ::tolower(b);
  };

  return consume_word_base(word, compare_ci);
}


// -------------------------------------------------

/*
  Check type of the token. Special URI characters are
  as defined by URI_TOKEN_LIST above.
  The rest are DIGIT (0-9) and CHAR.
*/

short URI_parser::Token::get_type() const
{

#define URI_TOKEN_CASE(T,C)  case C: return T_##T;

  switch(m_char)
  {
    URI_TOKEN_LIST(URI_TOKEN_CASE)
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': return T_DIGIT;
    default: return T_CHAR;
  }
}
