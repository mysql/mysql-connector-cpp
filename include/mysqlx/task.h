/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
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

namespace mysqlx {

using std::ostream;

class Session;
class Schema;
class Collection;
class Result;
class Row;
class RowResult;
class DbDoc;
class DocResult;

class Task;
class Executable;


class Task : nocopy
{
protected:

  class Impl;
  Impl  *m_impl;

  Task() : m_impl(NULL)
  {}

  Task(Task &&other) : m_impl(other.m_impl)
  {
    other.m_impl = NULL;
  }

  void reset(Impl*);

public:

  virtual ~Task();
  bool is_completed();
  BaseResult wait();
  void cont();

  friend class Impl;
  friend class Session;
  friend class NodeSession;
  friend class Result;
  friend class Collection;
  friend class Schema;

  struct Access;
  friend struct Access;
  friend class Executable;
};


}  // mysqlx

#endif
