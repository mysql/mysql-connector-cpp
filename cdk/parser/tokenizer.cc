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

#include "tokenizer.h"
#include <mysql/cdk/common.h>

PUSH_SYS_WARNINGS
#include <stdexcept>
#include <memory>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cstdlib>

#ifndef WIN32
#include <strings.h>
#  define _stricmp strcasecmp
#endif
POP_SYS_WARNINGS


PUSH_SYS_WARNINGS
PUSH_BOOST_WARNINGS
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>
POP_BOOST_WARNINGS
POP_SYS_WARNINGS


using namespace parser;
using cdk::foundation::Error;

struct Tokenizer::Maps Tokenizer::map;

Tokenizer::Maps::Maps()
{
  add_reserved_word("and", Token::AND);
  add_reserved_word("or", Token::OR);
  add_reserved_word("xor", Token::XOR);
  add_reserved_word("is", Token::IS);
  add_reserved_word("not", Token::NOT);
  add_reserved_word("like", Token::LIKE);
  add_reserved_word("in", Token::IN_);
  add_reserved_word("regexp", Token::REGEXP);
  add_reserved_word("between", Token::BETWEEN);
  add_reserved_word("interval", Token::INTERVAL);
  add_reserved_word("escape", Token::ESCAPE);
  add_reserved_word("div", Token::DIV);
  add_reserved_word("hex", Token::HEX);
  add_reserved_word("bin", Token::BIN);
  add_reserved_word("true", Token::TRUE_);
  add_reserved_word("false", Token::FALSE_);
  add_reserved_word("null", Token::T_NULL);
  add_reserved_word("second", Token::SECOND);
  add_reserved_word("minute", Token::MINUTE);
  add_reserved_word("hour", Token::HOUR);
  add_reserved_word("day", Token::DAY);
  add_reserved_word("week", Token::WEEK);
  add_reserved_word("month", Token::MONTH);
  add_reserved_word("quarter", Token::QUARTER);
  add_reserved_word("year", Token::YEAR);
  add_reserved_word("microsecond", Token::MICROSECOND);
  add_reserved_word("as", Token::AS);
  add_reserved_word("asc", Token::ASC);
  add_reserved_word("desc", Token::DESC);
  add_reserved_word("cast", Token::CAST);
  add_reserved_word("character", Token::CHARACTER);
  add_reserved_word("set", Token::SET);
  add_reserved_word("charset", Token::CHARSET);
  add_reserved_word("ascii", Token::ASCII);
  add_reserved_word("unicode", Token::UNICODE);
  add_reserved_word("byte", Token::BYTE);
  add_reserved_word("binary", Token::BINARY);
  add_reserved_word("char", Token::CHAR);
  add_reserved_word("nchar", Token::NCHAR);
  add_reserved_word("date", Token::DATE);
  add_reserved_word("datetime", Token::DATETIME);
  add_reserved_word("time", Token::TIME);
  add_reserved_word("decimal", Token::DECIMAL);
  add_reserved_word("signed", Token::SIGNED);
  add_reserved_word("unsigned", Token::UNSIGNED);
  add_reserved_word("integer", Token::INTEGER);
  add_reserved_word("int", Token::INTEGER);
  add_reserved_word("json", Token::JSON);

  interval_units.insert(Token::MICROSECOND);
  interval_units.insert(Token::SECOND);
  interval_units.insert(Token::MINUTE);
  interval_units.insert(Token::HOUR);
  interval_units.insert(Token::DAY);
  interval_units.insert(Token::WEEK);
  interval_units.insert(Token::MONTH);
  interval_units.insert(Token::QUARTER);
  interval_units.insert(Token::YEAR);

  operator_names["="] = "==";
  operator_names["and"] = "&&";
  operator_names["or"] = "||";
  operator_names["not"] = "not";
  operator_names["xor"] = "xor";
  operator_names["is"] = "is";
  operator_names["regexp"] = "regexp";
  operator_names["between"] = "between";
  operator_names["in"] = "in";
  operator_names["like"] = "like";
  operator_names["!="] = "!=";
  operator_names["<>"] = "!=";
  operator_names[">"] = ">";
  operator_names[">="] = ">=";
  operator_names["<"] = "<";
  operator_names["<="] = "<=";
  operator_names["&"] = "&";
  operator_names["^"] = "^";
  operator_names["|"] = "|";
  operator_names["<<"] = "<<";
  operator_names[">>"] = ">>";
  operator_names["+"] = "+";
  operator_names["-"] = "-";
  operator_names["*"] = "*";
  operator_names["/"] = "/";
  operator_names["~"] = "~";
  operator_names["%"] = "%";

  unary_operator_names["+"] = "sign_plus";
  unary_operator_names["-"] = "sign_minus";
  unary_operator_names["~"] = "~";
  unary_operator_names["not"] = "not";
}


Token::Token(Token::TokenType type, const std::string& text) : _type(type), _text(text)
{
}

const std::string& Token::get_text() const
{
  return _text;
}

Token::TokenType Token::get_type() const
{
  return _type;
}

struct Tokenizer::Maps map;

bool Token::is_reserved_word() const
{
  return map.reserved_word_types.end() != map.reserved_word_types.find(_type);
}

Tokenizer::Tokenizer(const std::string& input) : _input(input)
{
  _pos = 0;
}

bool Tokenizer::next_char_is(tokens_t::size_type i, int tok)
{
  return (i + 1) < _input.size() && _input[i + 1] == tok;
}

void Tokenizer::assert_cur_token(Token::TokenType type)
{
  assert_tok_position();
  Token::TokenType tok_type = _tokens.at(_pos).get_type();
  if (tok_type != type)
    throw Error((boost::format("Expected token type %s at pos %d but found type %s.")
                % Token::get_name(type) % _pos % Token::get_name(tok_type)).str());
}

bool Tokenizer::cur_token_type_is(Token::TokenType type)
{
  return pos_token_type_is(_pos, type);
}

bool Tokenizer::next_token_type(Token::TokenType type)
{
  return pos_token_type_is(_pos + 1, type);
}

bool Tokenizer::pos_token_type_is(tokens_t::size_type pos, Token::TokenType type)
{
  return (pos < _tokens.size()) && (_tokens[pos].get_type() == type);
}

const std::string& Tokenizer::consume_token(Token::TokenType type)
{
  assert_cur_token(type);
  const std::string& v = _tokens[_pos++].get_text();
  return v;
}

const Token& Tokenizer::peek_token()
{
  assert_tok_position();
  Token& t = _tokens[_pos];
  return t;
}

void Tokenizer::unget_token()
{
  if (_pos == 0)
    throw Error("Attempt to get back a token when already at first token (position 0).");
  --_pos;
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

Token::TokenType Tokenizer::parse_number(size_t& i)
{
  if (i >= _input.size())
    return Token::T_NULL;

  // Check that first characters start a numeric literal.

  bool first_digit = (0 != std::isdigit(_input[i]));
  bool floating_point = _input[i] == '.'
                     && i+1 < _input.size()
                     && std::isdigit(_input[i+1]);

  if (!first_digit && !floating_point)
    return Token::T_NULL;

  // Parse the literal

  bool is_float = false;

  while (i < _input.size() && std::isdigit(_input[i]))
    ++i;

  if (i < _input.size() && _input[i] == '.')
  {
    is_float = true;
    ++i;

    // must have at least one digit after '.'

    if (i >= _input.size() || !std::isdigit(_input[i]))
      throw Error((boost::format("Tokenizer: Missing fractional part for floating point at char %d") % i).str());

    while (i < _input.size() && std::isdigit(_input[i]))
      ++i;
  }

  is_float = parse_float_expo(i) || is_float;
  return is_float ? Token::LNUM : Token::LINTEGER;
}

bool Tokenizer::parse_float_expo(size_t& i)
{
  if (i >= _input.size() || std::toupper(_input[i]) != 'E')
    return false;

  ++i;

  char c = _input[i];
  if (i < _input.size() && ((c == '-') || (c == '+')))
      ++i;

  if (i >= _input.size() || !std::isdigit(_input[i]))
    throw Error((boost::format("Tokenizer: Missing exponential value for floating point at char %d") % i).str());

  while (i < _input.size() && std::isdigit(_input[i]))
    i++;

  return true;
}

/*
  Check if we have a Hexadecimal literal:

  X'12ab'
  x'12ab'
  ox12ab
*/

bool Tokenizer::parse_hex(size_t& i)
{
  std::string val;
  bool has_value = false;

  if((_input[i] == 'X' || _input[i] == 'x') && next_char_is(i, '\''))
  {
    i+=2;

    size_t start = i;

    for (; i < _input.size();++i)
    {
      if (_input[i] == '\'')
      {
        // We don't want the 'either (so the -2)
        val.assign(_input, start, i-2);

        has_value = true;
        break;
      }
    }
  }
  else if (_input[i] == '0' && (next_char_is(i, 'x') || next_char_is(i, 'X')))
  {
    i+=2;

    size_t start = i;

    for (; i < _input.size() && std::isalnum(_input[i]);++i)
    {}

   --i;

    val.assign(_input, start, i-1);

    has_value = true;

  }

  if (has_value)
  {
    _tokens.push_back(Token(Token::LHEX, val));

    return true;
  }

  return false;
}

void Tokenizer::get_tokens()
{
  for (size_t i = 0; i < _input.size(); ++i)
  {
    char c = _input[i];

    if (0 == c)
      return;

    if (std::isspace(c))
    {
      // do nothing
      continue;
    }

    if ( parse_hex(i))
      continue;

    Token::TokenType tt = Token::T_NULL;
    size_t j=i;
    if (Token::T_NULL != (tt = parse_number(j)))
    {
      _tokens.push_back(Token(tt, std::string(_input, i, j-i)));
      i = j-1;
      continue;
    }

    if (!std::isalpha(c) && c != '_')
    {
      // # non-identifier, e.g. operator or quoted literal
      if (c == '?')
      {
        _tokens.push_back(Token(Token::PLACEHOLDER, std::string(1, c)));
      }
      else if (c == '+')
      {
        _tokens.push_back(Token(Token::PLUS, std::string(1, c)));
      }
      else if (c == '-')
      {
        if (next_char_is(i, '>'))
        {
          ++i;
          _tokens.push_back(Token(Token::ARROW, "->"));
        }
        else
        _tokens.push_back(Token(Token::MINUS, std::string(1, c)));
      }
      else if (c == '*')
      {
        if (next_char_is(i, '*'))
        {
          ++i;
          _tokens.push_back(Token(Token::DOUBLESTAR, std::string("**")));
        }
        else
        {
          _tokens.push_back(Token(Token::MUL, std::string(1, c)));
        }
      }
      else if (c == '/')
      {
        _tokens.push_back(Token(Token::DIV, std::string(1, c)));
      }
      else if (c == '$')
      {
        _tokens.push_back(Token(Token::DOLLAR, std::string(1, c)));
      }
      else if (c == '%')
      {
        _tokens.push_back(Token(Token::MOD, std::string(1, c)));
      }
      else if (c == '=')
      {
        _tokens.push_back(Token(Token::EQ, std::string(1, c)));
      }
      else if (c == '&')
      {
        _tokens.push_back(Token(Token::BITAND, std::string(1, c)));
      }
      else if (c == '|')
      {
        _tokens.push_back(Token(Token::BITOR, std::string(1, c)));
      }
      else if (c == '^')
      {
        _tokens.push_back(Token(Token::BITXOR, std::string(1, c)));
      }
      else if (c == '(')
      {
        _tokens.push_back(Token(Token::LPAREN, std::string(1, c)));
      }
      else if (c == ')')
      {
        _tokens.push_back(Token(Token::RPAREN, std::string(1, c)));
      }
      else if (c == '[')
      {
        _tokens.push_back(Token(Token::LSQBRACKET, std::string(1, c)));
      }
      else if (c == ']')
      {
        _tokens.push_back(Token(Token::RSQBRACKET, std::string(1, c)));
      }
      else if (c == '{')
      {
        _tokens.push_back(Token(Token::LCURLY, std::string(1, c)));
      }
      else if (c == '}')
      {
        _tokens.push_back(Token(Token::RCURLY, std::string(1, c)));
      }
      else if (c == '~')
      {
        _tokens.push_back(Token(Token::NEG, std::string(1, c)));
      }
      else if (c == ',')
      {
        _tokens.push_back(Token(Token::COMMA, std::string(1, c)));
      }
      else if (c == ':')
      {
        _tokens.push_back(Token(Token::COLON, std::string(1, c)));
      }
//      else if (c == ';')
//      {
//        _tokens.push_back(Token(Token::SEMICOLON, std::string(1, c)));
//      }
      else if (c == '!')
      {
        if (next_char_is(i, '='))
        {
          ++i;
          _tokens.push_back(Token(Token::NE, std::string("!=")));
        }
        else
        {
          _tokens.push_back(Token(Token::BANG, std::string(1, c)));
        }
      }
      else if (c == '<')
      {
        if (next_char_is(i, '<'))
        {
          ++i;
          _tokens.push_back(Token(Token::LSHIFT, std::string("<<")));
        }
        else if (next_char_is(i, '='))
        {
          ++i;
          _tokens.push_back(Token(Token::LE, std::string("<=")));
        }
        else
        {
          _tokens.push_back(Token(Token::LT, std::string("<")));
        }
      }
      else if (c == '>')
      {
        if (next_char_is(i, '>'))
        {
          ++i;
          _tokens.push_back(Token(Token::RSHIFT, std::string(">>")));
        }
        else if (next_char_is(i, '='))
        {
          ++i;
          _tokens.push_back(Token(Token::GE, std::string(">=")));
        }
        else
        {
          _tokens.push_back(Token(Token::GT, std::string(1, c)));
        }
      }
      else if (c == '.')
      {
        if ((i + 1) < _input.size() && std::isdigit(_input[i + 1]))
        {
          size_t start = i;
          ++i;
          // floating grammar is
          // float -> '.' (int | (int expo[sign] int))
          // nint->digit +
          // expo -> 'E' | 'e'
          // sign -> '-' | '+'
          parse_float_expo(i);
          _tokens.push_back(Token(Token::LNUM, std::string(_input, start, i - start)));

          if (i < _input.size())
            --i;
        }
        else
        {
          _tokens.push_back(Token(Token::DOT, std::string(1, c)));
        }
      }
      else if (c == '"' || c == '\'' || c == '`')
      {
        char quote_char = c;
        std::string val;
        size_t start = ++i;

        while (i < _input.size())
        {
          c = _input[i];
          if ((c == quote_char) && ((i + 1) < _input.size()) && (_input[i + 1] != quote_char))
          {
            // break if we have a quote char that's not double
            break;
          }
          else if ((c == quote_char) || (c == '\\'  && quote_char != '`'))
          {
            // && quote_char != '`'
            // this quote char has to be doubled
            if ((i + 1) >= _input.size())
              break;
            val.append(1, _input[++i]);
          }
          else
            val.append(1, c);
          ++i;
        }
        if ((i >= _input.size()) && (_input[i] != quote_char))
        {
          throw Error((boost::format("Unterminated quoted string starting at %d") % start).str());
        }
        if (quote_char == '`')
        {
          _tokens.push_back(Token(Token::QUOTED_ID, val));
        }
        else
        {
          _tokens.push_back(Token(Token::LSTRING, val));
        }
      }
      else
      {
        throw Error((boost::format("Unknown character at %d") % i).str());
      }
    }
    else
    {
      size_t start = i;
      while (i < _input.size() && (std::isalnum(_input[i]) || _input[i] == '_'))
        ++i;
      std::string val(_input, start, i - start);
      Maps::reserved_words_t::const_iterator it = map.reserved_words.find(val);
      if (it != map.reserved_words.end())
      {
        _tokens.push_back(Token(it->second, val));
      }
      else
      {
        _tokens.push_back(Token(Token::ID, val));
      }
      --i;
    }
  }
}

void Tokenizer::inc_pos_token()
{
  ++_pos;
}

int Tokenizer::get_token_pos()
{
  assert(_pos < (size_t)std::numeric_limits<int>::max());
  return (int)_pos;
}

const Token& Tokenizer::consume_any_token()
{
  assert_tok_position();
  Token& tok = _tokens[_pos];
  ++_pos;
  return tok;
}

void Tokenizer::assert_tok_position()
{
  if (_pos >= _tokens.size())
    throw Error((boost::format("Expected at pos %d but no tokens left.") % _pos).str());
}

bool Tokenizer::tokens_available()
{
  return _pos < _tokens.size();
}

bool Tokenizer::is_interval_units_type()
{
  assert_tok_position();
  Token::TokenType type = _tokens[_pos].get_type();
  return map.interval_units.find(type) != map.interval_units.end();
}

bool Tokenizer::is_type_within_set(const std::set<Token::TokenType>& types)
{
  assert_tok_position();
  Token::TokenType type = _tokens[_pos].get_type();
  return types.find(type) != types.end();
}

bool Tokenizer::Cmp_icase::operator()(const std::string& lhs, const std::string& rhs) const
{
  const char *c_lhs = lhs.c_str();
  const char *c_rhs = rhs.c_str();

  return _stricmp(c_lhs, c_rhs) < 0;
}

