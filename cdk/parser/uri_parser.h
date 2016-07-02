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

#ifndef _URI_PARSER_H_
#define _URI_PARSER_H_

#include <mysql/cdk/common.h>
#include "parser.h"

PUSH_SYS_WARNINGS
#include <list>
POP_SYS_WARNINGS


namespace parser {

/*
  Interface used to report contents of mysqlx URI or connection
  string.
*/

class URI_processor
{
public:

  /*
    Callbacks for the main components of the URI:

    mysqlx://<user>:<password>@<host>:<port>/<path>

    If an optional component is not present, the corresponding callback
    is not called.
  */

  virtual void user(const std::string&) {}
  virtual void password(const std::string&) {}
  virtual void host(const std::string&) {}
  virtual void port(unsigned short) {}
  virtual void path(const std::string&) {}

  /*
    Callbacks for reporting the query component, which is a sequence
    of key-value pair. Keys without any value are allowed. Key value
    can be a list: "...&key=[v1,..,vN]&...".
  */

  virtual void key_val(const std::string&) {}
  virtual void key_val(const std::string&, const std::string&) {}
  virtual void key_val(const std::string&, const std::list<std::string>&) {}
};


/*
  Parse given string as mysqlx URI ("mysqlx:://" scheme required). Pass
  extracted information to the given processor.
*/

void parse_uri(const std::string &uri, URI_processor &prc);

/*
  Parse given string as mysqlx connection string ("mysqlx:://" scheme optional).
  Pass extracted information to the given processor.
*/

void parse_conn_str(const std::string &str, URI_processor &prc);



/*
  Parser for parsing mysqlx URIs. The same parser can be used to parse
  connection string, which is like URI but without the "mysqlx://" scheme
  prefix.
*/

  class URI_parser
  : public cdk::api::Expr_base<URI_processor>
{
public:

  class Error;

  /*
    Constants representing top-level parts of an URI:

    mysqlx://........../.....?......#........
    ^^^^^^   ^^^^^^^^^^ ^^^^^ ^^^^^^ ^^^^^^^^
    SCHEME   AUTHORITY  PATH  QUERY  FRAGMENT
  */

  enum part_t {
    SCHEME, AUTHORITY, PATH, QUERY, FRAGMENT, END
  };

private:

  /*
    Represents single token in URI string. Tokens are single characters.
    If m_pct is true then this character was read in the %XX form (and then
    it is never treated as a special URI charcater.
  */
  struct Token
  {
    Token() : m_char(0), m_pct(false)
    {}

    Token(char c, bool pct =false)
      : m_char(c), m_pct(pct)
    {}

    short get_type() const;

    char get_char() const
    {
      return m_char;
    }

  private:

    char  m_char;
    bool  m_pct;

    friend class Error;
  };

  /*
    Stored constructor parameters.
  */

  std::string m_uri;
  bool        m_force_uri;

  /*
    Set to true if string contains "mysqlx:://" schema prefix (which
    is optional for connection strings).
  */

  bool        m_has_scheme;

  /*
    Parser state.

    m_tok - the current token (not yet consumed),
    m_pos - position of the current token,
    m_pos_next - position of the next token following the current one;
                 if there are no more tokens then m_pos_next = m_pos,
                 otherwise m_pos_next > m_pos.

    m_part - indicates which part of the URI is parsed now.
  */

  Token       m_tok;
  size_t      m_pos;
  size_t      m_pos_next;
  part_t      m_part;


public:

  /*
    Create parser for a given string. If 'force_uri' parameter is true,
    then the string is expecte to be a full URI with the schema part
    (errors are reported if schema is missing). Otherwise 'uri' is treated
    as a connection string with optional scheme prefix.
  */

  URI_parser(const std::string &uri, bool force_uri=false)
    : m_uri(uri), m_force_uri(force_uri)
  {}

  /*
    Method 'process' parses the string passed to constructor and reports
    information extracted from it to the given processor. Throws errors
    if parsing was not possible. These errors are derived from URI_parser::Error.
  */

  void process(Processor &prc) const;

  void process_if(Processor *prc) const
  {
    if (!prc)
      return;
    process(*prc);
  }

private:

  struct TokSet;

  bool check_scheme(bool);
  void process_query(Processor &prc) const;
  void process_list(const std::string&, Processor &prc) const;

  bool get_token(bool in_part=true);
  Token consume_token();
  bool consume_token(short tt);

  void consume_until(std::string&, const TokSet&);
  void consume_all(std::string&);
  bool has_more_tokens() const;
  bool at_end() const;

  bool next_token_is(short) const;
  bool next_token_in(const TokSet&) const;

  void next_part();
  part_t check_next_part() const;
};


/*
  Base class for URI parser errors.

  Error instance stores fragments of the parsed strings around current
  parser positions. They are used when composing error description.
*/

class URI_parser::Error
  : public cdk::Error_class<URI_parser::Error>
{
protected:

  size_t m_pos;       // Current parser position.
  char   m_seen[64];  // Characters seen before current position.
  char   m_ahead[8];  // Few characters ahead of the current position.
  cdk::string m_msg;

  Error(const URI_parser *p, const cdk::string &descr = cdk::string());

  virtual void do_describe1(std::ostream&) const;

  void do_describe(std::ostream &out) const
  {
    do_describe1(out);
    out << " (" << m_code << ")";
  }

public:

  virtual ~Error() throw ()
  {}

  size_t get_pos() const
  {
    return m_pos;
  }

  friend class URI_parser;
};


}  // parser

#endif
