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

#ifndef _JSON_PARSER_H_
#define _JSON_PARSER_H_

#include <mysql/cdk/common.h>
#include "parser.h"

namespace parser {

using cdk::JSON;
class JSON_parser;

/*
  Specialization of Token_base, which can clasify base tokens as JSON tokens.
*/

class JSON_token_base
  : public Token_base
{
protected:

  using Token_base::Error;

public:

  enum Token_type {
    OTHER,
    STRING, NUMBER, INTEGER,
    PLUS, MINUS, T_NULL, T_TRUE, T_FALSE,
  };

  static Token_type get_jtype(const Token&);

  friend JSON_parser;
};



/*
  JSON_parser is build using generic Doc_parser<> template and base
  JSON_sclar_parser which parses scalar values (numbers, strings, Booleans).
  Parsing of arrays and sub-documents is handled by Doc_parser<> logic.
*/


class JSON_scalar_parser
  : public Expr_parser<cdk::JSON_processor, JSON_token_base>
{
public:

  JSON_scalar_parser(It &first, const It &last)
    : Expr_parser<cdk::JSON_processor, JSON_token_base>(first, last)
  {}

  static Processor *get_base_prc(JSON::Processor::Any_prc *prc)
  { return prc->scalar(); }

private:

  bool do_parse(Processor*);
};



class JSON_parser
  : public JSON
{

  class Error;

  std::string m_json;
public:

  JSON_parser(const std::string &json)
    : m_json(json.begin(), json.end())
  {
    m_json.push_back('\0');
  }

  JSON_parser(std::string &&json)
    : m_json(std::move(json))
  {
    m_json.push_back('\0');
  }

  void process(Processor &prc) const;
};



/*
  Error class for JSON_parse

  It is a specialization of the generic parser::Error_base which defines
  convenience constructors.
*/

class JSON_parser::Error
    : public parser::Error_base<std::string>
{
public:
  Error(const std::string& parsed_text,
        size_t pos,
        const string& desc = string())
    : parser::Error_base<std::string>(parsed_text, pos, desc)
  {}
};

}  // parser

#endif
