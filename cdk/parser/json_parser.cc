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

#include "json_parser.h"
#include <mysql/cdk.h>

PUSH_SYS_WARNINGS
#include <stdlib.h>
POP_SYS_WARNINGS

PUSH_SYS_WARNINGS
PUSH_BOOST_WARNINGS
#include <boost/lexical_cast.hpp>
POP_BOOST_WARNINGS
POP_SYS_WARNINGS

/*
  The maximum absolute value that 64-bit signed integer can have.
  For most platforms it is the minimum negative LLONG_MIN.
  The absolute value for it cannot be obtained because
  ABS(LLONG_MAX) < ABS(LLONG_MIN). Therefore, we define a constant.
*/
#define INTEGER_ABS_MAX 9223372036854775808UL

using namespace parser;
using cdk::string;
using cdk::JSON;
typedef  cdk::JSON::Processor Processor;


void json_parse(const string &json, Processor &dp)
{
  JSON_parser    parser(json);
  parser.process(dp);
}


bool JSON_scalar_parser::do_parse(It &first, const It &last, Processor *vp)
{
  if (first == last)
    return false;

  bool neg = false;

  switch (first->get_type())
  {
  case Token::T_NULL:
    if (vp)
      vp->null();
    ++first;
    return true;

  case Token::LSTRING:
    if(vp)
      vp->str(first->get_text());
    ++first;
    return true;

  case Token::TRUE_:
  case Token::FALSE_:
    if(vp)
      vp->yesno(Token::TRUE_ == first->get_type());
    ++first;
    return true;

  case Token::MINUS:
  case Token::PLUS:
    neg = (Token::MINUS == first->get_type());
    ++first;
    break;

  default:
    // if none of the above, then it should be a number
    break;
  }

  // Numeric value

  switch (first->get_type())
  {
  case Token::LNUM:
    {
      if(vp)
      {
        double val = boost::lexical_cast<double>(first->get_text());
        vp->num(neg ? -val : val);
      }
      ++first;
      return true;
    }

  case Token::LINTEGER:
    {
      if(vp)
      {
        uint64_t val = boost::lexical_cast<uint64_t>(first->get_text());
        if (val > INTEGER_ABS_MAX)
        {
          if (neg)
            throw Error("The value is too large for a signed type");
          // Unsigned type is only returned for large values
          vp->num(val);
        }
        else
        {
          // Absolute values of 9223372036854775808UL can only be negative
          if (!neg && val == INTEGER_ABS_MAX)
            throw Error("The value is too large for a signed type");
          // All values ABS(val) < 9223372036854775808UL are treated as signed
          vp->num(neg ? -(int64_t)val : (int64_t)val);
        }
      }
      ++first;
      return true;
    }

  default: throw Error("Can not parse key value");
  }
}


