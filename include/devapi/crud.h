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

#ifndef MYSQLX_CRUD_H
#define MYSQLX_CRUD_H

/**
  @file
  Common templates used to define CRUD operation classes.
*/

/*
  Different CRUD operation classes derive from `Executable` which defines
  the `execute()` method that executes given operation. Derived classes
  define additional methods that can modify the operation before it gets
  executed.

  The hierarchy of classes reflects the grammar that defines the fluent
  CRUD API. The grammar can be described by the following diagram.

    Executable<R> -> R : execute()

    Statement<R> = Executable<R>
    Statement<R> -> Statement<R> : bind()

    Offset<R> = Statement<R>
    Offset<R> -> Statement<R> : offset()

    Limit<R,F> = Statement<R>
    Limit<R,false> -> Statement<R> : limit()
    Limit<R,true>  -> Offset<R>    : limit()

  In this diagram notation X -> Y : foo() means that class X defines
  public method foo() with return type Y (roughly). Notation X = Y means
  that X inherits public methods from Y. R is the type of the result
  produced by given CRUD operation.

  Thus, for example, if x is of type Limit<RowResult,true> then:

    x.limit()  - returns Offset<RowResult>
    x.limit().offset() - returns Statement<RowResult>
    x.execute()  - returns RowResult (Limit<RowResult,true> inherits
                   from Statement<RowResult> which inherits from
                   Executable<RowResult>)

  We have 2 variants of Limit<> class, to distinguish the case where
  .offset() can follow .limit() from the case where only .limit() clause
  is allowed.

  The remaining grammar rules concerning operations specific to collections
  and tables can be found in collection_crud.h and table_crud.h,
  respectively.

  Each class eventually derives from Executable which holds the internal
  implementation object which should implement the implementation
  interface defined for given operation. The abstract interfaces to be
  implemented by internal implementation objects are given by hierarchy
  of XXX_impl classes based on Executable_impl. The public methods that
  manipulate given CRUD operation are defined in terms of methods from
  the implementation interface.

  The internal implementation object is created and passed to the
  constructor of the CRUD operation object which takes ownership of
  the implementation (and deletes it upon destruction). Public methods
  of the CRUD operation objects call methods of the internal
  implementation object which holds the definition and parameters of the
  operation to be performed. Eventually the `execute()` method is
  forwarded to the internal implementation object which executes the
  operation. Each CRUD operation base class has method `get_impl` which
  returns pointer to the internal implementation object casted to
  appropriate type.

  An internal implementation object can not be shared between two CRUD
  operation objects nor can it be copied. For that reason CRUD operations
  do not have copy semantics. Assignment `a = b` moves internal
  implementation from operation `b` to operation `a`. Using `b` after
  such assignment throws errors.

  Since internal implementation object is managed by the Executable<R>
  class at the bottom of the inheritance hierarchy, this Executable<R>
  class is a virtual base class. This ensures that there is only one copy
  of Executable<R> in any object of derived class. It also means that
  constructors of derived classes initialize the virtual base class
  directly and not through intermediate base class constructors.
  For example, see constructor of `CollectionFind`.
*/


#include "common.h"
#include "statement.h"

#include <map>

namespace mysqlx {
namespace internal {

/*
  Interface to be implemented by internal implementations of
  CRUD operations which support .limit() or .offset() clauses.
*/

struct Limit_impl : public Statement_impl
{
  virtual void set_offset(unsigned) = 0;
  virtual void set_limit(unsigned) = 0;
};


/**
  Base class defining operation's offset() clause.
*/

template <class Res, class Op>
class Offset
  : public Statement<Res, Op>
{
protected:

  typedef Limit_impl Impl;

  // Make default constructor protected.

  Offset() = default;

  using Statement<Res,Op>::check_if_valid;
  using Statement<Res,Op>::m_impl;

  Impl* get_impl()
  {
    check_if_valid();
    return static_cast<Impl*>(m_impl.get());
  }

public:

  /**
    Skip the given number of items (rows or documents) before starting
    to perform the operation.
  */

  Statement<Res,Op>& offset(unsigned rows)
  {
    get_impl()->set_offset(rows);
    return *this;
  }
};


/*
  The base class of Limit<R,F> is Offset<R> if F is true or
  Statement<R> if F is false. But otherwise definition of Limit<R,F>
  is the same in both cases. To use common template definition the
  base class of Limit<R,F> (which is also the return type of `limit`
  method) is defined by LimitRet<R,F>::type.
*/

template <class Res, class Op, bool with_offset> struct LimitRet;

template <class Res, class Op>
struct LimitRet<Res, Op, true>
{
  typedef Offset<Res,Op> type;
};

template <class Res, class Op>
struct LimitRet<Res, Op, false>
{
  typedef Statement<Res,Op> type;
};


/**
  Base class defining operation's limit() clause.
*/

// TODO: Doxygen does not see the base class

template <class Res, class Op, bool with_offset>
class Limit
  : public LimitRet<Res, Op, with_offset>::type
{
protected:

  typedef Limit_impl Impl;

  Limit() = default;

  using LimitRet<Res, Op, with_offset>::type::check_if_valid;
  using LimitRet<Res, Op, with_offset>::type::m_impl;

  Impl* get_impl()
  {
    check_if_valid();
    return static_cast<Impl*>(m_impl.get());
  }

public:

  /**
    %Limit the operation to the given number of items (rows or documents).
  */

  typename LimitRet<Res, Op, with_offset>::type& limit(unsigned items)
  {
    get_impl()->set_limit(items);
    return *this;
  }
};


/*
  Interfaces to be implemented by internal implementations of
  CRUD operations which support sorting and projection specifications.
  These specifications are passed to the implementation as sequences
  of strings via multiple calls to `add_sort` or `add_proj` methods.
  Implementation must parse these strings to get the specification.
*/

struct Sort_impl : public Limit_impl
{
  virtual void add_sort(const string&) = 0;
};

struct Having_impl : public Sort_impl
{
  virtual void set_having(const string&) = 0;
};

struct Group_by_impl : public Having_impl
{
  virtual void add_group_by(const string&) = 0;
};

struct Proj_impl : public Group_by_impl
{
  virtual void add_proj(const string&) = 0;
  virtual void set_proj(const string&) = 0;
};


}}  // mysqlx::internal

#endif
