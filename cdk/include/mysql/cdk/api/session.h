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

#ifndef CDK_API_SESSION_H
#define CDK_API_SESSION_H

#include "mysql/cdk/foundation.h"


namespace cdk {
namespace api {


class Session
    : public Diagnostics
    , public Async_op<void>
{
public:

  virtual ~Session() {}

  // Check if given session is valid. Function is_valid() performs a lightweight, local check while
  // check_valid() might communicate with the data store to perform this check.
  // Both is_valid() and check_valid() return UNKNOWN if session state could not be determined.

  virtual option_t is_valid() = 0;
  virtual option_t check_valid() = 0;

  // Clear diagnostic information that accumulated for the session.
  // Diagnostics interface methods such as Diagnostics::error_count()
  // and Diagnostics::get_errors() report only new diagnostics entries
  // since last call to clear_errors() (or since session creation if
  // clear_errors() was not called).
  virtual void clear_errors() = 0;

  virtual void close() = 0;

};

}} // cdk::api

#endif // CDK_API_SESSION_H
