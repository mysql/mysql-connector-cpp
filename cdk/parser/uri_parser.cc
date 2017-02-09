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
  URI parser error descriptions have one of these forms:

  "After seeing '...AAA', looking at 'BBB...': MSG"
  "After seeing '...AAA', with no more characters in the string: MSG"
  "While looking at 'BBB...': MSG"
  "While looking at empty string: MSG"

  where MSG is the message passed to error constructor.
*/

void URI_parser::Error::do_describe1(std::ostream &out) const
{
  bool seen_part = false;

  if (m_seen[0] || m_seen[1])
  {
    seen_part = true;
    out << "After seeing '";
    if (!m_seen[0])
      out << "..." << m_seen + 1;
    else
      out << m_seen;
    out << "'";
  }

  if (m_ahead[0])
  {
    if (seen_part)
      out << ", looking at '";
    else
      out << "While looking at '";

    if (1 == m_ahead[sizeof(m_ahead) - 1])
      out << m_ahead << "...";
    else
      out << m_ahead;
    out << "'";
  }
  else
  {
    if (seen_part)
      out << ", with no more characters in the string";
    else
      out << "While looking at empty string";
  }

  if (!m_msg.empty())
    out << ": " << m_msg;
}


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

URI_parser::Error::Error(const URI_parser *p, const cdk::string &descr)
  : Error_class(NULL, cdk::cdkerrc::parse_error), m_msg(descr)
{
  m_pos = p->m_pos;
  memset(m_seen, 0, sizeof(m_seen));
  memset(m_ahead, 0, sizeof(m_ahead));

  if (!p->m_uri.empty())
  {
    /*
      Calculate how much to copy into m_seen, 1 byte is left for
      null terminator.
    */

    size_t howmuch;

    if (m_pos  > sizeof(m_seen)-1)
      howmuch = sizeof(m_seen)-1;
    else
      howmuch = m_pos;

    p->m_uri.copy(m_seen, howmuch, m_pos-howmuch);

    /*
      If initial fragment is longer than size of m_seen, then
      we set first byte to 0 to indicate that '...' prefix should
      be added.
    */

    if (m_pos > sizeof(m_seen)-1)
      m_seen[0] = 0;

    /*
      Similar, if remainder of the URI string does not fit in
      m_ahead, then the last byte is set to 1 to indicate that
      '...' should be added at the end. Note: Second last byte
      is used as null terminator.
    */

    p->m_uri.copy(m_ahead, sizeof(m_ahead) - 2, m_pos);

    if (p->m_uri.length() > m_pos + sizeof(m_ahead) - 2)
      m_ahead[sizeof(m_ahead) - 1] = 1;
  }
}

DIAGNOSTIC_POP


/*
  Specialized error message with description:

  "XXX: Expected 'YYY': MSG"

  Where XXX is the base error description, YYY is the expected
  string (or character) passed to the constructor and MSG is
  optional extra message passed to the constructor.
*/

struct Unexpected_error
  : public cdk::Error_class<Unexpected_error, URI_parser::Error>
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


// ---------------------------------------------------------------

/*
  Tokens recognized by URI parser.
*/

#define URI_TOKEN_LIST(X) \
  X (COLON, ':')    \
  X (SLASH, '/')    \
  X (AT, '@')       \
  X (QUESTION, '?') \
  X (EQ, '=')       \
  X (AMP, '&')      \
  X (SQOPEN, '[')   \
  X (SQCLOSE, ']')  \
  X (COMMA, ',')    \
  X (HASH, '#')     \


#define URI_TOKEN_ENUM(T,C) T_##T,
enum token_type
{
  T_ZERO,
  T_CHAR,
  URI_TOKEN_LIST(URI_TOKEN_ENUM)
  T_LAST
};


/*
  Class representing set of token types.
*/

struct URI_parser::TokSet
{
  std::bitset<T_LAST> m_bits;

  TokSet(token_type tt)
  {
    m_bits.set(tt);
  }

  TokSet(token_type tt1, token_type tt2)
  {
    m_bits.set(tt1);
    m_bits.set(tt2);
  }

  TokSet(token_type tt1, token_type tt2, token_type tt3)
  {
    m_bits.set(tt1);
    m_bits.set(tt2);
    m_bits.set(tt3);
  }

  bool has_token(token_type tt) const
  {
    return m_bits.test(tt);
  }
};


// ---------------------------------------------------------------


void URI_parser::process(Processor &prc) const
{
  URI_parser *self = const_cast<URI_parser*>(this);

  // Note: check_scheme() resets parser state.

  self->check_scheme(m_force_uri);
  assert(AUTHORITY == m_part);

  /*
    Look for host and port, saving them in the corresponding variables.
    If user creadentials are detected, they are reported to the
    processor and rescan flag is set to true to look for host/port data
    again.
  */

  std::string host;
  std::string port;
  bool        rescan = false;
  bool        has_port = false;

  if (self->next_token_is(T_SQOPEN))
  {
    /*
      IPv6 adress found! Will be parsed on rescan
    */
    rescan = true;
  }
  else
  {
    self->consume_until(host, TokSet(T_AT, T_COLON ));

    if (self->consume_token(T_COLON))
    {
      /*
      We have seen  "<???>:" and it still can be user followed
      by a password or host followed by a port.

      We consume further tokens until @, or end of the authority
      part, whichever comes first.
    */

      self->consume_until(port, T_AT);

      /*
      If we see @ now, then it means we were looking at user
      credentials so far (and they are stored in host and port,
      respectively). We report them and request re-scanning host/port
      data.

      Note: If we don't see @ now, then it means that initial colon
      was separating host from port and both are stored in
      the corresponding variables.
    */

      if (self->consume_token(T_AT))
      {
        // <user>:<pwd>@...
        prc.user(host);
        prc.password(port);
        rescan = true;
      }
      else
        has_port = true;
    }
    else if (self->consume_token(T_AT))
    {
      /*
      No ':' seen but we see '@'. It means user without password and
      user is stored in host variable. We report it an request
      re-scanning of host/port info.
    */
      prc.user(host);
      rescan = true;
    }
  }

  /*
    At this point, if rescan is set then we have reported
    user/password data and we need to continue looking for host/port
    info.
  */

  if (rescan)
  {
    host.clear();
    port.clear();

    if (self->consume_token(T_SQOPEN))
    {
      /*
        IPv6 address
      */
      host.clear();
      self->consume_until(host, T_SQCLOSE);
      if (!self->consume_token(T_SQCLOSE))
        throw Error(this, L"Missing ']' while parsing IPv6 address");
    }
    else
    {
      self->consume_until(host, T_COLON );
    }

    if (self->consume_token(T_COLON))
    {
      self->consume_all(port);
      has_port = true;
    }
  }


  if (has_more_tokens())
    throw Error(this, L"Unexpected characters after authority part");

  // report host and port

  prc.host(host);

  if (has_port)
  {
    if (port.empty())
      throw Error(this, L"Expected port number");

    const char *beg = port.c_str();
    char *end = NULL;
    long int val = strtol(beg, &end, 10);

    /*
      Note: strtol() returns 0 either if the number is 0
      or conversion was not possible. We distinguish two cases
      by cheking if end pointer was updated.
    */

    if (val == 0 && end == beg)
      throw Error(this, L"Expected port number");

    if (val > 65535 || val < 0)
      throw Error(this, L"Invalid port value");

    prc.port(static_cast<unsigned short>(val));
  }

  // Proceed to path or query part.

  self->next_part();

  if (PATH == m_part)
  {
    std::string path;

    /*
      We allow only one path component (which can not contain '/').
      Here we consume tokens till end of the query part or '/', whichever
      comes first. If we see '/' then error is reported.
    */

    self->consume_until(path, T_SLASH);

    if (next_token_is(T_SLASH))
      throw Error(this,( m_has_scheme ?
        L"Mysqlx URI can contain only single path component"
        : L"Mysqlx connection string can contain only single path component"
      ));

    prc.path(path);

    // Proceed to query part.
    self->next_part();
  }

  if (QUERY == m_part)
  {
    process_query(prc);

    /*
      Query should consume the rest of the URI string. If we see
      anything else then it is wrong syntax.
    */
    if (has_more_tokens())
      throw Unexpected_error(this, '&');

    self->next_part();
  }

  if (FRAGMENT == m_part)
  {
    throw Error(this,( m_has_scheme ?
      L"Mysqlx URI can not contain fragment specification"
      : L"Unexpected characters at the end"
    ));
  }

  if (END != m_part)
    throw Error(this, L"Unexpected characters at the end");
}


/*
  Process query part which consists of key-value pairs of the
  form "<key>=<value>" separated by '&'.

  The value part is optional. If it starts with '[' then we
  have a comma separated list of values.
*/

void URI_parser::process_query(Processor &prc) const
{
  URI_parser *self = const_cast<URI_parser*>(this);
  string key;
  string val;

  if (!has_more_tokens())
    return;

  do {
    key.clear();

    /*
      After key there should be '=' or, if key has no value,
      either '&' that separates next key-value pair or end of
      the query part.
    */

    self->consume_until(key, TokSet(T_EQ, T_AMP));

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
        self->consume_until(val, T_AMP);
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

void URI_parser::process_list(const string &key, Processor &prc) const
{
  URI_parser *self = const_cast<URI_parser*>(this);

  if (!self->consume_token(T_SQOPEN))
    return;

  std::list<string> list;
  string val;

  do {
    val.clear();
    self->consume_until(val, TokSet(T_COMMA, T_SQCLOSE));
    list.push_back(val);
  }
  while (self->consume_token(T_COMMA));

  if (!self->consume_token(T_SQCLOSE))
  {
    std::ostringstream msg;
    msg << "Missing ']' while parsing list value of query key '"
        << key <<"'" << std::ends;
    throw Error(this, msg.str());
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
  Consume all remaining tokens of the current URI part and store
  them in the given buffer.
*/

void URI_parser::consume_all(std::string &buf)
{
  while (has_more_tokens())
    buf.push_back(consume_token().get_char());
}


// Check type of next token.

bool URI_parser::next_token_is(short tt) const
{
  return !at_end() && tt == m_tok.get_type();
}

//  Check if type of next token is in the given set.

bool URI_parser::next_token_in(const TokSet &toks) const
{
  if (!has_more_tokens())
    return false;
  return toks.has_token(token_type(m_tok.get_type()));
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
  m_pos_next = 0;
  m_has_scheme = false;

  m_pos = m_uri.find("://");
  if (m_pos != std::string::npos)
  {
    m_has_scheme = true;
    if (m_uri.substr(0, m_pos) != "mysqlx")
      throw Error(this, "Expected URI scheme 'mysqlx'");

    // move to the first token after '://'
    m_pos_next = m_pos + 3;
  }
  else
  {
    // set m_pos for correct error reporting
    m_pos = 0;

    if (m_uri.substr(0, 6) == "mysqlx")
    {
      m_pos = 6;
      throw Unexpected_error(this, "://");
    }

    if (force)
      throw Error(this, "URI scheme expected");
  }

  get_token(false);
  m_part = AUTHORITY;
  return m_has_scheme;
}


/*
  Look at the next token and see if we are at the end
  of the current URI part. If this is the case, return id of the
  part that follow, otherwise return the current part.

  Note that, e.g., AUTHORITY part can be followed either by
  PATH, QUERY or FRAGMENT, depending on the character that follows
  after it:

    ...://<authority>/<path>...
    ...://<authority>?<query>...
    ...://<authority>#<fragment>...
*/

URI_parser::part_t URI_parser::check_next_part() const
{
  if (at_end())
    return END;

  switch (m_part)
  {
  case SCHEME:
    if (next_token_is(T_COLON))
      return AUTHORITY;

  case AUTHORITY:
    if (next_token_is(T_SLASH))
      return PATH;

  case PATH:
    if (next_token_is(T_QUESTION))
      return QUERY;

  case QUERY:
    if (next_token_is(T_HASH))
      return FRAGMENT;

  case FRAGMENT:
  default:
    break;
  }

  return m_part;
}

/*
  Move to the next part of the URI.

  All remaining tokens in the current part are discarded.
*/

void URI_parser::next_part()
{
  assert(SCHEME != m_part);

  std::string discard;
  consume_all(discard);

  /*
    Determine the next part. Note that consume_all() should
    put us at the end of the current part, so that check_next_part()
    returns the id of the next part.
  */

  m_part = check_next_part();

  // Move to the first token of the next part.

  Token tok;

  switch(m_part)
  {
  case AUTHORITY:
    assert(false);

  case PATH:
    tok = consume_token();
    assert(T_SLASH == tok.get_type());
    break;

  case QUERY:
    tok = consume_token();
    assert(T_QUESTION == tok.get_type());
    break;

  case SCHEME:
  case FRAGMENT:
  case END:
  default:
    break;
  }
}


/*
  Get next token, store it in m_tok and update parser state.

  If in_part is true then only tokens from the current part
  are considered. Otherwise the whole string is considered.

  Returns false if there are no more tokens (in the current part).
*/

bool URI_parser::get_token(bool in_part)
{
  m_pos = m_pos_next;

  if (at_end())
    return false;

  if (in_part && !has_more_tokens())
    return false;

  if ('%' == m_uri[m_pos_next])
  {
    long c;

    // TODO: more efficient implementation.

    string hex = m_uri.substr(m_pos_next + 1, 2);
    hex.push_back('\0');
    char *end = NULL;
    c = strtol(hex.data(), &end, 16);
    if (end != hex.data() + 2 || c < 0 || c > 256)
      throw Error(this, L"Invalid pct-encoded character");

    m_tok = Token((char)c, true);
    m_pos_next += 3;
    return true;
  }

  m_tok = Token(m_uri[m_pos_next++]);
  return true;
}

/*
  Return true if all tokens from the URI string have been consumed.
*/

bool URI_parser::at_end() const
{
  return m_pos >= m_uri.length();
}

/*
  Return true if there is at least one more token which
  belongs to the current URI part (not counting the delimiter).
*/

bool URI_parser::has_more_tokens() const
{
  return !at_end() && m_part == check_next_part();
}


/*
  Return the current token and proceed to the next one (if any).
  Throws error if next token is not available.
*/

URI_parser::Token URI_parser::consume_token()
{
  if (END == m_part)
    throw Error(this, L"Expected more characters");
  Token cur_tok(m_tok);
  get_token(false);
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


// -------------------------------------------------

/*
  Check type of the token. If token was obtained from %XX pct-encoding
  then its type is always CHAR. Otherwise special URI characters are
  as defined by URI_TOKEN_LIST above.
*/

short URI_parser::Token::get_type() const
{
  if (m_pct)
    return T_CHAR;

#define URI_TOKEN_CASE(T,C)  case C: return T_##T;

  switch(m_char)
  {
  URI_TOKEN_LIST(URI_TOKEN_CASE)
  default: return T_CHAR;
  }
}
