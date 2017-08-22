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


namespace mysqlx {

using std::ostream;


namespace internal {

/*
  Abstract interface to be implemented by internal implementations
  of an executable object.

  The execute() method returns a Result_base object which can be
  used to initialize different result types used in DevAPI.

  Implementation of an executable object holds a description of the operation
  that should be executed. Executable objects can be copied (for example
  by copy assignment operation) and in this case a new copy of the current
  description of the operation should be created by clone() method. After
  cloning, the 2 executable implementations can be modified and executed
  independently.

  See various Op_xxx classes defined for example in devapi/collection_crud.cc
  to see examples of executable object implementations. Note that these Op_xxx
  classes do not directly inherit from Executable_impl, but use a whole
  hierarchy of implementation classes based on Executable_impl. But in the end
  they define execute() method that executes given operation using all the
  information collected using other methods of the implementation class.
*/

struct Executable_impl
{
  virtual Result_base execute() = 0;

  virtual Executable_impl *clone() const = 0;

  virtual ~Executable_impl() {}
};

}  // internal



/**
  Represents an operation that can be executed.

  Creating an operation does not involve any communication
  with the server. Only when `execute()` method is called
  operation is sent to the server for execution.

  The template parameter `Res` is the type of result that
  is returned by `execute()` method.
*/

template <class Res, class Op>
class Executable
{
protected:

  using Impl = internal::Executable_impl;

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

  internal::Executable_impl* get_impl()
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
      return m_impl->execute();
    }
    CATCH_AND_WRAP
  }

  struct Access;
  friend Access;
};


}  // mysqlx

#endif
