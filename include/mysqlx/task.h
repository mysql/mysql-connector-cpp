/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
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

#ifndef MYSQLX_TASK_H
#define MYSQLX_TASK_H

/**
  @file
  Classes to control (asynchronous) execution of commands.

  @note Asynchronous execution is not implemented yet.
*/


#include "common.h"
#include "result.h"

#include <map>

namespace mysqlx {

using std::ostream;


class Task;
class Executable;

namespace internal {
  class PlainExecutable;
}

class Task : internal::nocopy
{
public:

  Task& operator=(Task &&other)
  {
    reset(other.m_impl);
    other.m_impl = NULL;
    return *this;
  }

  virtual ~Task();
  bool is_completed();
  internal::BaseResult wait();
  void cont();

protected:

  class Impl;
  Impl  *m_impl;  // TODO: use unique_ptr<> instead.

  Task() : m_impl(NULL)
  {}

  void reset(Impl*);

public:

  struct Access;
  friend struct Access;
  friend class internal::PlainExecutable;
  friend class Executable;
};


/**
  Represents an operation that can be executed.

  Creating an operation does not involve any communication
  with the server. Only when `execute()` method is called
  operation is sent to the server for execution.
*/

namespace internal {

  class PlainExecutable : nocopy
  {
  protected:

    Task m_task;

    void check_if_valid()
    {
      if (!m_task.m_impl)
        throw Error("Attempt to use invalid operation");
    }

    PlainExecutable() {}

  public:

    PlainExecutable(PlainExecutable &&other)
    {
      m_task = std::move(other.m_task);
    }

    /// Execute given operation and wait for its result.

    virtual BaseResult execute()
    {
      check_if_valid();
      return m_task.wait();
    }

    struct Access;
    friend struct Access;
  };

}

/**
  Base class for arguments binding operations.

  This class defines `bind()` methods that bind values to its parameters.

  TODO: Better documentation.
*/

class Executable : public internal::PlainExecutable
{
protected:

  typedef std::map<string, Value> param_map_t;
  param_map_t m_map;

  Executable() = default;

public:

  Executable(Executable &other)
    : PlainExecutable(std::move(other))
    , m_map(std::move(other.m_map))
  {}

  Executable(Executable &&other) : Executable(other) {}
  Executable(PlainExecutable &&other) : PlainExecutable(std::move(other))
  {
    m_map.clear();
  }

  internal::BaseResult execute() override;

  Executable& bind(const string &parameter, Value val)
  {
    check_if_valid();
    m_map[parameter] = std::move(val);
    return *this;
  }

#if 0

  /*
    Currently protocol supports binding only to scalar values,
    not arrays or documents.

    TODO: Add this overload when binding to arrays is supported
    in the protocol.
  */

  template <typename Iterator>
  BindExec& bind(const string &parameter, const Iterator &begin, const Iterator &end)
  {
    m_map[parameter] = Value(begin, end);
    return *this;
  }

#endif

  struct Access;
  friend struct Access;
  friend class Task;
  friend class Task::Impl;
};


}  // mysqlx

#endif
