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

#include <mysql/cdk/common.h>

PUSH_SYS_WARNINGS
#include <stdexcept>
#include <memory>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cstdlib>
POP_SYS_WARNINGS

#include "tokenizer.h"


using namespace parser;


namespace {

  std::locale c_loc("C");
  const std::ctype<char_t> &ctf = std::use_facet<std::ctype<char_t>>(c_loc);

}


bool Tokenizer::cur_char_is_space() const
{
  return ctf.is(ctf.space, cur_char());
}

bool Tokenizer::cur_char_is_word() const
{
  if (cur_char_is(L'_'))
    return true;
  return ctf.is(ctf.alnum, cur_char());
}


void Tokenizer::get_tokens()
{
  while (chars_available())
  {
    if (0 == cur_char())
      return;

    while (chars_available() && cur_char_is_space())
      consume_char();

    if (!chars_available())
      return;

    if (parse_string())
      continue;

    if (parse_hex())
      continue;

    if (parse_number())
      continue;

    // check symbol tokens

    set_token_start();

#define  symbol_check(T,X) \
    if (consume_chars(L##X)) \
    { \
      add_token(Token::T, L##X); \
      continue; \
    } \

    SYMBOL_LIST2(symbol_check)

#define  symbol_check1(T,X) \
    if (c == (L##X)[0]) { consume_char(); add_token(Token::T,L##X); continue; }

    char_t c = cur_char();
    SYMBOL_LIST1(symbol_check1)

    /*
      Note: it is important to parse word last as some words can qualify as
      other tokens.
    */

    if (parse_word())
      continue;

    token_error(L"Could not recognize next token");

  } // while
}


/*
  Parse number literal starting at position i.

  Returns Token::T_NULL if no number literal can start at position i (and
  leaves i unchanged). Otherwise returns Token::LINTEGER or Token::LNUM
  and sets i to the first position after the literal.

  The grammar used for numeric literals:

    number -> int | float
    int -> digit+
    float -> digit* '.' digit+ expo? | digit+ expo
    expo -> ('E'|'e') ('+'|'-')? digit+

  which is replaced by equivalent:

    number -> digit* ('.' digit+)? expo?

  with extra check that there is at least one digit if fractional part is missing.

  Original grammar for floating numbers:

    FLOAT ::= DIGIT* '.' DIGIT+ ('E' ('+'|'-')? DIGIT+)? | DIGIT+ 'E' ('+'|'-')? DIGIT+
*/

bool Tokenizer::parse_digits(string *out)
{
  bool has_digits = false;

  while (chars_available() && cur_char_in(L"0123456789"))
  {
    has_digits = true;

    if (out)
      out->push_back(consume_char());
    else
      consume_char();
  }

  return has_digits;
}

bool Tokenizer::parse_number()
{
  bool is_float = false;
  bool exponent = false;

  set_token_start();

  /*
    Note: '.' starts NUMBER token only if followed by a digit.
    Otherwise it is a single DOT token.
  */

  if (cur_char_is(L'.') && !next_char_in(L"0123456789"))
    return false;

  // Parse leading digits, if any

  if (!parse_digits() && !cur_char_is(L'.'))
  {
    return false;
  }

  // Handle decimal point, if any

  if (consume_char(L'.'))
  {
    is_float = true;
    if (!parse_digits())
      token_error(L"No digits after decimal point");
  }

  // See if we have exponent (but it is not parsed yet)

  if (consume_char(L"Ee"))
  {
    is_float = true;
    exponent = true;
  }

  /*
    If nothing indicates a floating number, we have already
    parsed the digits of an integer number and we can report
    it now.
  */

  if (!is_float)
  {
    add_token(Token::INTEGER);
    return true;
  }

  // Parse exponent if present.

  if (exponent)
  {
    consume_char(L"+-");

    if (!parse_digits())
      token_error(L"No digits in the exponent");
  }

  // Report floating number.

  add_token(Token::NUMBER);
  return true;
}


/*
  Check if we have a Hexadecimal literal:

  X'12ab'
  x'12ab'
  0x12ab
*/


bool Tokenizer::parse_hex()
{
  string val;

  if (!chars_available())
    return false;

  switch (cur_char())
  {

  case L'X': case L'x':
  {
    if (!next_char_is(L'\''))
      return false;

    consume_char();
    consume_char();

    if (!parse_hex_digits(val))
      token_error(L"Unexpected character inside hex literal");

    if (!consume_char(L'\''))
      token_error(L"Unexpected character inside hex literal");

    break;
  }

  case L'0':
  {
    if (!next_char_in(L"Xx"))
      return false;

    consume_char();
    consume_char();

    if (!parse_hex_digits(val))
      token_error(L"No hex digits found after 0x");

    break;
  }

  default:
    return false;
  }

  add_token(Token::HEX, val);
  return true;
}

bool Tokenizer::parse_hex_digits(string &digits)
{
  bool ret = cur_char_in(L"0123456789ABCDEFabcdef");
  while (cur_char_in(L"0123456789ABCDEFabcdef"))
    digits.push_back(consume_char());
  return ret;
}


/*
  See if next token is:

  WORD  - plain word
  QWORD - word quotted in back-ticks
*/

bool Tokenizer::parse_word()
{
  if (!chars_available())
    return false;

  set_token_start();

  if (cur_char_is(L'`'))
  {
    string word;
    parse_quotted_string(L'`', &word);
    add_token(Token::QWORD, word);
    return true;
  }

  bool has_word = false;

  while (chars_available() && cur_char_is_word())
  {
    consume_char();
    has_word = true;
  }

  if (!has_word)
    return false;

  add_token(Token::WORD);
  return true;
}


/*
  See if next token is:

  QSTRING  - a string in single quotes
  QQSTRING - a string in double quotes
*/

bool Tokenizer::parse_string()
{
  set_token_start();
  string val;
  char_t quote = cur_char();

  if (!(L'\"' == quote || L'\'' == quote))
    return false;

  if (!parse_quotted_string(quote, &val))
    return false;

  add_token(L'\"' == quote ? Token::QQSTRING : Token::QSTRING, val);
  return true;
}


bool Tokenizer::parse_quotted_string(char_t qchar, string *val)
{
  if (!consume_char(qchar))
    return false;

  // Store first few characters for use in error message.

  static const size_t start_len = 8;
  char_t start[start_len] = { qchar };
  size_t pos = 1;

  while (chars_available())
  {
    // if we do not have escaped char, look at the end of the string

    if (!consume_char(L'\\'))
    {
      // if qute char is repeated, then it does not terminate string
      if (consume_char(qchar) && !cur_char_is(qchar))
        return true;
    }

    char_t c = consume_char();

    if (val)
      val->push_back(c);

    if (pos < start_len)
      start[pos++] = c;
  }

  if (pos < start_len)
    start[pos] = '\0';
  start[start_len - 1] = '\0';

  token_error(
    string(L"Unterminated quoted string starting with ")
    + string(start) + string(L"...")
  );

  return false;  // quiet compile warnings
}


// Constructing token sequence


size_t Tokenizer::set_token_start()
{
  _tok_pos = _in_pos;
  return _tok_pos;
}

void Tokenizer::add_token(Token::Type tt)
{
  assert(_in_pos > _tok_pos);
  add_token(tt, _input.substr(_tok_pos, _in_pos - _tok_pos));
  _tok_pos = _in_pos;
}

void Tokenizer::add_token(Token::Type tt, const string &val)
{
  _tokens.emplace_back(tt, val, _tok_pos, _in_pos);
}


// Access underlying sequence of characters


bool Tokenizer::chars_available() const
{
  return _in_pos < _input.size();
}

char_t   Tokenizer::cur_char() const
{
  if (!chars_available())
    token_error(L"More characters expected");
  return _input.at(_in_pos);
}

size_t Tokenizer::get_char_pos() const
{
  return _in_pos;
}


bool Tokenizer::next_char_is(char_t c, size_t off) const
{
  return _in_pos + off < _input.size() && _input[_in_pos + off] == c;
}

bool Tokenizer::next_char_in(const char_t *set, size_t off) const
{
  if (_in_pos + off >= _input.size())
    return false;
  char_t c = _input[_in_pos + off];

  return (0 != c) && (NULL != std::wcschr(set, c));
}


char_t Tokenizer::consume_char()
{
  char_t c = cur_char();
  _in_pos++;
  return c;
}

bool Tokenizer::consume_char(char_t c)
{
  if (!cur_char_is(c))
    return false;
  consume_char();
  return true;
}

char_t Tokenizer::consume_char(const char_t *set)
{
  if (!cur_char_in(set))
    return '\0';
  return consume_char();
}

bool Tokenizer::consume_chars(const char_t *str)
{
  if (_in_pos != _input.find(str, _in_pos))
    return false;
  _in_pos += wcslen(str);
  return true;
}
