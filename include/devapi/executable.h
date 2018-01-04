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

#ifndef MYSQLX_EXECUTABLE_H
#define MYSQLX_EXECUTABLE_H

/**
  @file
  Class representing executable statements.
*/


#include "common.h"
#include "result.h"
#include "../common/op_if.h"


namespace mysqlx {

using std::ostream;


/**
  Represents an operation that can be executed.

  Creating an operation does not involve any communication with the server.
  Only when `execute()` method is called operation is sent to the server
  for execution.

  The template parameter `Res` is the type of result that
  is returned by `execute()` method.

  A derived class must create an implementation object for the operation and
  set it using reset() method. Such implementation object should implement
  common::Executable_if interface.
*/

template <class Res, class Op>
class Executable
{
protected:

  using Impl = common::Executable_if;

  std::shared_ptr<Impl> m_impl;

  Executable() = default;

  void reset(Impl *impl)
  {
    m_impl.reset(impl);
  }

  /*
    Copy semantics implementation: the current state of the other executable
    object (of its implementation, to be precise) is copied to this new
    instance. After that the two executables are independent objects describing
    the same operation.
  */

  void reset(const Executable &other)
  {
    m_impl.reset(other.m_impl->clone());
  }

  void reset(const Executable &&other)
  {
    m_impl = std::move(other.m_impl);
  }

  void check_if_valid()
  {
    if (!m_impl)
      throw Error("Attempt to use invalid operation");
  }

  Impl* get_impl()
  {
    check_if_valid();
    return m_impl.get();
  }

public:

  Executable(const Executable &other)
  {
    operator=(other);
  }

  Executable(Executable &&other)
  {
    operator=(std::move(other));
  }

  virtual ~Executable() {}


  Executable& operator=(const Executable &other)
  {
    try {
      reset(other);
      return *this;
    }
    CATCH_AND_WRAP
  }

  Executable& operator=(Executable &&other)
  {
    try {
      reset(std::move(other));
      return *this;
    }
    CATCH_AND_WRAP
  }


  /// Execute given operation and return its result.

  virtual Res execute()
  {
    try {
      check_if_valid();
      /*
        Note: The implementation object's execute() methods returns a reference
        to a common::Result_init object which provides information about
        the session and pending server reply. The returned Res instance should
        be constructible from such Result_init reference.
      */
      return m_impl->execute();
    }
    CATCH_AND_WRAP
  }

  struct Access;
  friend Access;
};


}  // mysqlx

#endif
