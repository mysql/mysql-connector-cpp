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

#ifndef MYSQLX_CRUD_H
#define MYSQLX_CRUD_H

/**
  @file
  Common classes used to define CRUD operation classes.
*/

/*
  Different CRUD operation classes derive from Executable which defines
  the execute() method that executes given operation. Derived classess
  define additional methods that can modify the operation before it gets
  executed.

  The hierarchy of classes reflects the grammar that defines the fluent
  CRUD API. The grammar can be described by the following diagram.

    Executable -> Result : execute()

    Offset = Executable
    Offset -> Executable : offset()

    Limit<F> = Executable
    Limit<false> -> Executable : limit()
    Limit<true>  -> Offset     : limit()

  In this diagram notation X -> Y : foo() means that class X defines
  public method foo() with return type Y (roughly). Notation X = Y means
  that X inherits public methods from Y. Thus, for example, if x is
  of type Limit<true> then:

    x.limit()  - returns Offset
    x.limit().offset() - returns Executable
    x.execute()  - returns Result (Limit<true> inherits from Executable)

  We have 2 variants of Limit<> class, to distinguish the case where
  .offset() can follow .limit() from the case where only .limit() clause
  is allowed.

  The remaining grammar rules concering operations specific to collections
  and tables can be found in collection_crud.h and table_crud.h,
  resepectively.

  Each class eventually derives from Executable which holds the Task object
  implementing given CRUD operation. This task is initialized when the
  CRUD operation object is first created (see CollectionFind() constructors
  for example). Then different clauses of the fluent CRUD API modify this
  task object adding things like selection criteria, sorting specifications
  etc. (to be precise, the internal implementaion of the task is modified
  directly).

  Note that Task and Executable have no copy semantics. If Executable
  instance `a` is constructed from another Executable `b`, like in
  `a = b`, then the task is moved from `b` to `a`. After this executable
  `a` becomes "empty" and an attempt to execute it throws an error.
*/


#include "common.h"
#include "task.h"

#include <map>

namespace mysqlx {
namespace internal {


class Offset
: public virtual Executable
{
protected:

  // Make default constructor protected.

  Offset() = default;

public:

  Executable& offset(unsigned rows);
};


template <bool with_offset> class Limit;

template<>
class Limit<false>
  : public virtual Executable
{
protected:

  Limit() = default;

public:

  Executable& limit(unsigned rows);
};


template<>
class Limit<true>
  : public virtual Executable
  , Offset
{
protected:

  Limit() = default;

public:

  Offset& limit(unsigned rows);
};


/*
  Base class for CollectionSort<> and TableSort<>.

  It defines the do_sort() method which modifies underlying task
  instance. But the public API is different for collections and
  tables and is defined in the derived classes.
*/

template <bool limit_with_offset>
class SortBase : public Limit<limit_with_offset>
{
protected:

  SortBase() = default;
  void do_sort(const string&);
};

}}  // mysqlx::internal

#endif
