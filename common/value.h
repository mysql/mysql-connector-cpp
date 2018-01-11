/*
 * Copyright (c) 2015, 2017, Oracle and/or its affiliates. All rights reserved.
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

#ifndef MYSQLX_COMMON_INT_VALUE_H
#define MYSQLX_COMMON_INT_VALUE_H


#include <mysqlx/common.h>
#include <mysql/cdk.h>
#include <expr_parser.h>


namespace mysqlx {
namespace common {

using cdk::string;
using cdk::byte;
using cdk::bytes;

template <cdk::Type_info T> class Format_descr;

struct Value::Access
{
  static Value mk_expr(const string &expr)
  {
    return { Value::EXPR, expr };
  }

  static Value mk_json(const string &json)
  {
    return { Value::JSON, json };
  }

  // Create value from raw bytes, given CDK format description.

  template<cdk::Type_info T>
  static Value mk(bytes data, common::Format_descr<T> &format);

  // Describe value to a CDK expression or value processor.

  static void
  process(parser::Parser_mode::value, const Value&, cdk::Expression::Processor&);

  static void
  process_val(const Value&, cdk::Value_processor&);
};


/*
  Wrappres which present a given Value instance as a CDK scalar or expression.
*/


class Value_scalar
  : public cdk::Any
{
  const Value &m_val;

public:

  Value_scalar(const Value &val)
    : m_val(val)
  {}

  void process(Processor &prc) const
  {
    auto *sprc = prc.scalar();
    if (!sprc)
      return;
    Value::Access::process_val(m_val, *sprc);
  }
};

class Value_expr
  : public cdk::Expression
{
  const Value &m_val;
  parser::Parser_mode::value m_pm;

public:

  Value_expr(const Value &val, parser::Parser_mode::value pm)
    : m_val(val), m_pm(pm)
  {}

  void process(Processor &prc) const
  {
    Value::Access::process(m_pm, m_val, prc);
  }
};


}}  // mysqlx::internal namespace

#endif
