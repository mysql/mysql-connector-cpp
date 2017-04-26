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
#include <stack>
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
  virtual void host(unsigned short /*priority*/,
                    const std::string &/*host*/)
  {}
  virtual void host(unsigned short /*priority*/,
                    const std::string &/*host*/,
                    unsigned short /*port*/)
  {}
  // Report Unix socket path.
  virtual void socket(unsigned short /*priority*/,
                      const std::string &/*socket_path*/)
  {}
  // Report Win pipe path, including "\\.\" prefix.
  virtual void pipe(unsigned short /*priority*/, const std::string &/*pipe*/)
  {}
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
    Token() : m_char(0)
    {}

    Token(char c)
      : m_char(c)
    {}

    short get_type() const;

    char get_char() const
    {
      return m_char;
    }

  private:

    char  m_char;

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

  */

  std::stack<Token>  m_tok;
  std::stack<size_t> m_pos;
  std::stack<size_t> m_pos_next;



public:

  /*
    Create parser for a given string. If 'force_uri' parameter is true,
    then the string is expecte to be a full URI with the schema part
    (errors are reported if schema is missing). Otherwise 'uri' is treated
    as a connection string with optional scheme prefix.
  */

  URI_parser(const std::string &uri, bool force_uri=false)
    : m_uri(uri), m_force_uri(force_uri)
  {
    m_tok.push(Token('\0'));
    m_pos.push(0);
    m_pos_next.push(0);
  }

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

  enum class Address_type { NONE, IP, SOCKET, PIPE};

  struct TokSet;

  void process_userinfo(Processor &prc) const;
  Address_type process_adress(std::string &address, std::string &port);
  unsigned short convert_val(const std::string &port) const;
  bool process_ip_address(std::string &host, std::string &port);
  bool process_socket_prefix(std::string &socket);
  bool process_pct_socket(std::string &socket);
  bool process_unencoded_socket(std::string &socket);
  bool process_socket(std::string &socket);
  bool process_pipe_prefix(std::string &pipe);
  bool process_pct_pipe(std::string &pipe);
  bool process_unencoded_pipe(std::string &pipe);
  bool process_pipe(std::string &pipe);
  bool process_unencoded_file(std::string &file);
  bool process_file(std::string &file);

  bool process_value(TokSet, std::string &value);
  bool process_tokens(TokSet, std::string &value);
  bool process_pct_encoded_value(std::string &encoded);
  TokSet unreserved() const;
  TokSet sub_delims() const;
  TokSet sub_delims_qry() const;
  TokSet gen_delims() const;
  bool report_address(Processor &prc,
                      Address_type type,
                      unsigned short priority,
                      const std::string &host,
                      const std::string &port) const;
  bool process_adress_list(Processor &prc) const;
  bool process_adress_priority(Processor &prc) const;
  void process_path(Processor &prc) const;



  bool check_scheme(bool);
  void process_query(Processor &prc) const;
  void process_list(const std::string&, Processor &prc) const;

  bool get_token();
  Token consume_token();
  bool consume_token(short tt);
  template <typename C>
  bool consume_word_base(const std::string &word, C compare);
  bool consume_word(const std::string &word);
  bool consume_word_ci(const std::string& word);

  void consume_until(std::string&, const TokSet&);
  void consume_while(std::string&, const TokSet&);
  void consume_all(std::string&);
  void trim_spaces();
  bool has_more_tokens() const;
  bool at_end() const;

  bool next_token_is(short) const;
  bool next_token_in(const TokSet&) const;

  void push();
  void pop();

  void parse_error(const cdk::string&) const;
  void unexpected(const std::string&, const cdk::string &msg = cdk::string()) const;
  void unexpected(char, const cdk::string &msg = cdk::string()) const;

  struct Guard;

  };


/*
  Base class for URI parser errors.
*/

class URI_parser::Error
  : public parser::Error_base<std::string>
{
protected:

  Error(const URI_parser *p, const cdk::string &descr = cdk::string())
  : Error_base<std::string>(p->m_uri, p->m_pos.top(), descr)
  {
  }

  friend class URI_parser;
};


inline
void URI_parser::parse_error(const cdk::string &msg) const
{
  throw Error(this, msg);
}

}  // parser

#endif
