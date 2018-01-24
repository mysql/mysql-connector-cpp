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

#ifndef MYSQLX_DETAIL_ERROR_H
#define MYSQLX_DETAIL_ERROR_H

/**
  @file
  Classes used to access query and command execution results.
*/

#include "../common.h"

//#include <memory>


namespace mysqlx {
namespace internal {

class Result_detail;


class Warning_detail
  : public virtual common::Printable
{
protected:

  byte     m_level;
  uint16_t m_code;
  string   m_msg;


  // Some valid C++11 constructs do not work in MSVC 2013.

#if defined(_MSC_VER) && _MSC_VER > 1800

  Warning_detail(Warning_detail&&) = default;

#else

  Warning_detail(Warning_detail &&other)
    : m_level(other.m_level), m_code(other.m_code)
    , m_msg(std::move(other.m_msg))
  {}

#endif

  Warning_detail(const Warning_detail&) = default;

  Warning_detail(byte level, uint16_t code, const string &msg)
    : m_level(level), m_code(code), m_msg(msg)
  {}

  void print(std::ostream&) const;

  friend Result_detail;
};


}  // internal namespace
}  // mysqlx

#endif
