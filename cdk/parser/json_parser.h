/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C++ is licensed under the terms of the GPLv2
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

#ifndef _JSON_PARSER_H_
#define _JSON_PARSER_H_

#include <mysql/cdk/common.h>
#include "parser.h"

namespace parser {

using cdk::JSON;


/*
  JSON_parser is build using generic Doc_parser<> template and base
  JSON_sclar_parser which parses scalar values (numbers, strings, Booleans).
  Parsing of arrays and sub-documents is handled by Doc_parser<> logic.
*/


class JSON_scalar_parser
  : public Expr_parser<cdk::JSON_processor>
{
public:

  JSON_scalar_parser(It &first, const It &last)
    : Expr_parser<cdk::JSON_processor>(first, last)
  {}

  static Processor *get_base_prc(JSON::Processor::Any_prc *prc)
  { return prc->scalar(); }

private:

  bool do_parse(It&, const It&, Processor*);

};



class JSON_parser
  : public JSON
{
  Tokenizer m_toks;

public:

  JSON_parser(const cdk::string &json)
    : m_toks(json)
  {
    m_toks.get_tokens();
  }

  void process(Processor &prc) const
  {
    if (!const_cast<JSON_parser*>(this)->m_toks.tokens_available())
      cdk::throw_error("JSON_parser: empty string");

    It first = m_toks.begin();
    It last  = m_toks.end();

    /*
      Note: passing m_toks.end() directly as constructor argument results
      in "incompatible iterators" exception when comparing iterators (at
      least on win, VS2010). Problem with passing temporary object?
    */

    Doc_parser<JSON_scalar_parser> parser(first, last);
    if (!parser.parse(prc) || first != last)
      cdk::throw_error("JSON_parser: could not parse string as JSON document");
  }

};

}  // parser

#endif
