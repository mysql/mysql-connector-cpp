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

#ifndef MYSQLX_COMMON_ERROR_H
#define MYSQLX_COMMON_ERROR_H

/*
  TODO: Error handling infrastructure for XDevAPI and XAPI still needs to
  be done. Current code is just a temporary hack.
*/

#include <string>
#include <stdexcept>
#include <ostream>
#include <memory>
#include <forward_list>
#include <string.h>  // for memcpy
#include <utility>   // std::move etc


namespace mysqlx {

namespace common {

/**
  Base class for connector errors.

  @internal
  TODO: Derive from std::system_error and introduce proper
  error codes.
  @endinternal

  @ingroup devapi
*/

class Error : public std::runtime_error
{
public:

  Error(const char *msg)
    : std::runtime_error(msg)
  {}
};


inline
std::ostream& operator<<(std::ostream &out, const Error &e)
{
  out << e.what();
  return out;
}


inline
void throw_error(const char *msg)
{
  throw Error(msg);
}

}  // internal namespace


}  // mysqlx


#endif
