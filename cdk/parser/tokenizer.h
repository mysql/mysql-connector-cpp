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
POP_SYS_WARNINGS


namespace parser {

#define TOKEN_LIST(X) \
    X(NOT) \
    X(AND) \
    X(OR) \
    X(XOR) \
    X(IS) \
    X(LPAREN) \
    X(RPAREN) \
    X(LSQBRACKET) \
    X(RSQBRACKET) \
    X(BETWEEN) \
    X(TRUE_) \
    X(T_NULL) \
    X(FALSE_) \
    X(IN_ ) \
    X(LIKE) \
    X(RLIKE) \
    X(INTERVAL) \
    X(REGEXP) \
    X(ESCAPE) \
    X(ID) \
    X(QUOTED_ID) \
    X(LSTRING) \
    X(LNUM) \
    X(DOT) \
    /*X(AT)*/ \
    X(COMMA) \
    X(EQ) \
    X(NE) \
    X(GT) \
    X(GE) \
    X(LT) \
    X(LE) \
    X(BITAND) \
    X(BITOR) \
    X(BITXOR) \
    X(LSHIFT) \
    X(RSHIFT) \
    X(PLUS) \
    X(MINUS) \
    X(MUL) \
    X(DIV) \
    X(HEX) \
    X(BIN) \
    X(NEG) \
    X(BANG) \
    X(MICROSECOND) \
    X(SECOND) \
    X(MINUTE) \
    X(HOUR) \
    X(DAY) \
    X(WEEK) \
    X(MONTH) \
    X(QUARTER) \
    X(YEAR) \
    X(PLACEHOLDER) \
    X(DOUBLESTAR) \
    X(MOD) \
    X(AS) \
    X(USING) \
    X(ASC) \
    X(DESC) \
    X(CAST) \
    X(CHARACTER) \
    X(SET) \
    X(CHARSET) \
    X(ASCII) \
    X(UNICODE) \
    X(BYTE) \
    X(BINARY) \
    X(CHAR) \
    X(NCHAR) \
    X(DATE) \
    X(DATETIME) \
    X(TIME) \
    X(DECIMAL) \
    X(SIGNED) \
    X(UNSIGNED) \
    X(INTEGER) /* 'integer' keyword */ \
    X(LINTEGER) /* integer number */ \
    X(LHEX) /* hexadecimal number*/\
    X(DOLLAR) \
    X(JSON) \
    X(COLON) \
    X(LCURLY) \
    X(RCURLY) \
    X(ARROW)\

  class Token
  {
  public:

#define token_enum(T) T,

    enum TokenType
    {
      TOKEN_LIST(token_enum)
    };

    Token(TokenType type, const std::string& text);
    // TODO: it is better if this one returns a pointer (std::string*)
    const std::string& get_text() const;
    TokenType get_type() const;
    bool is_reserved_word() const;

#define token_name(X) case X: return #X;

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
    TokenType _type;
    std::string _text;
  };


  // TODO: We should handle wide strings (cdk::string)

  class Tokenizer
  {
  public:
    Tokenizer(const std::string& input);

    typedef std::vector<Token> tokens_t;
    typedef tokens_t::const_iterator  iterator;

    bool next_char_is(tokens_t::size_type i, int tok);
    void assert_cur_token(Token::TokenType type);
    bool cur_token_type_is(Token::TokenType type);
    bool next_token_type(Token::TokenType type);
    bool pos_token_type_is(tokens_t::size_type pos, Token::TokenType type);
    const std::string& consume_token(Token::TokenType type);
    const Token& peek_token();
    void unget_token();
    void inc_pos_token();
    int get_token_pos();
    const Token& consume_any_token();
    void assert_tok_position();
    bool tokens_available();
    bool is_interval_units_type();
    bool is_type_within_set(const std::set<Token::TokenType>& types);

    std::vector<Token>::const_iterator begin() const { return _tokens.begin(); }
    std::vector<Token>::const_iterator end() const { return _tokens.end(); }

    void get_tokens();
    std::string get_input() { return _input; }

  protected:
    std::vector<Token> _tokens;
    std::string _input;
    tokens_t::size_type _pos;

    Token::TokenType parse_number(size_t& i);
    bool parse_float_expo(size_t& i);
    bool parse_hex(size_t& i);

  public:

    struct Cmp_icase
    {
      bool operator()(const std::string& lhs, const std::string& rhs) const;
    };

    struct Maps
    {
    public:
      typedef std::map<std::string, Token::TokenType, Cmp_icase> reserved_words_t;
      reserved_words_t reserved_words;
      std::set<Token::TokenType> reserved_word_types;
      std::set<Token::TokenType> interval_units;
      std::map<std::string, std::string, Cmp_icase> operator_names;
      std::map<std::string, std::string, Cmp_icase> unary_operator_names;

      Maps();

    private:

      void add_reserved_word(const char *word, Token::TokenType type)
      {
        reserved_words[word] = type;
        reserved_word_types.insert(type);
      }
    };

  public:
    static Maps map;
  };


  class Error : public cdk::Error
  {
  public:
    Error(const std::string& msg) : cdk::Error(cdk::cdkerrc::generic_error, msg)
    {
    }
  };

}  // parser

#endif
