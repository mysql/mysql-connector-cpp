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

#ifndef MYSQLX_ERROR_H
#define MYSQLX_ERROR_H

/**
  @file
  Classes used to access query and command execution results.
*/


#include "common.h"
#include "detail/error.h"

#include <memory>


namespace mysqlx {



/**
  An error, warning or other diagnostic information reported by server
  when executing queries or statements. The information can be printed to
  output stream using `<<` operator.

  @note Normally, errors reported by server are not represented by `Warning`
  instances but instead they are thrown as instances of `mysqlx::Error`.

  @ingroup devapi
*/

class PUBLIC_API Warning
  : public internal::Printable
  , internal::Warning_detail
{
public:

  /// Type of diagnostic information.

  enum Level {
    LEVEL_ERROR,   ///< %Error
    LEVEL_WARNING, ///< %Warning
    LEVEL_INFO     ///< Other information
  };

private:

  Warning(Level level, uint16_t code, const string &msg)
    : Warning_detail(byte(level),code,msg)
  {
  }

  Warning(Warning_detail &&init)
    : Warning_detail(std::move(init))
  {}

  void print(std::ostream&) const;

public:

  /**
    Return level of the diagnostic info stored in this object.
  */

  Level getLevel() const
  {
    return Level(m_level);
  }

  /**
    Return error/warning code reported by server.
  */

  uint16_t getCode() const
  {
    return m_code;
  }

  /**
    Return diagnostic message reported by server.
  */

  const string& getMessage() const
  {
    return m_msg;
  }

  struct Access;
  friend Access;
};


inline
void Warning::print(std::ostream &out) const
{
  switch (getLevel())
  {
  case LEVEL_ERROR: out << "Error"; break;
  case LEVEL_WARNING: out << "Warning"; break;
  case LEVEL_INFO: out << "Info"; break;
  }

  if (getCode())
    out << " " << getCode();

  out << ": " << getMessage();
}



}  // mysqlx

#endif
