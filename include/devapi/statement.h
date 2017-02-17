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

#ifndef MYSQLX_STATEMENT_H
#define MYSQLX_STATEMENT_H

/**
  @file
  Classes which represent executable statements.
*/


#include "common.h"
#include "result.h"


namespace mysqlx {

using std::ostream;


namespace internal {

class XSession_base;

/*
  Abstract interface to be implemented by internal implementation
  of an executable object.

  The execute() method returns a BaseStatement object which can be
  used to initialize different result types used in DevAPI.
*/

struct Executable_impl
{
  virtual BaseResult execute() = 0;

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

  typedef internal::Executable_impl Impl;

  std::shared_ptr<Impl> m_impl;

  Executable() = default;
  Executable(Impl *impl)
  {
    m_impl.reset(impl);
  }

  void check_if_valid()
  {
    if (!m_impl)
      throw Error("Attempt to use invalid operation");
  }

public:

  Executable(const Executable &other)
    : m_impl(other.m_impl->clone())
  {}

  Executable(Executable &&other)
    : m_impl(other.m_impl)
  {}

  virtual ~Executable() {}

  /// Execute given operation and wait for its result.

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


namespace internal {

/*
  Interface to be implemented by internal implementation
  of statement object.

  Statement object uses `add_param` method to report values
  bound to named parameters to the implementation.
*/

struct Statement_impl : public Executable_impl
{
  virtual void add_param(const string&, Value&&) = 0;
};

}  // internal


/**
  Represents a statement that can be executed.

  Instances of this class are created by various methods creating
  CRUD operations. Before executing a `Statement`, values of named
  parameters that appear in expressions which define the statement
  can be bound to values using `bind()` method.

  The template parameter `Res` is the type of result that
  is returned by `execute()` method.
*/

template <class Res, class Op>
class Statement
  : public virtual Executable<Res, Op>
{
protected:

  typedef internal::Statement_impl Impl;

  Statement() = default;
  Statement(Impl *impl)
    : Executable<Res,Op>(impl)
  {}

  using Executable<Res,Op>::check_if_valid;
  using Executable<Res,Op>::m_impl;

  Impl* get_impl()
  {
    check_if_valid();
    return static_cast<Impl*>(m_impl.get());
  }

public:

  Statement(Statement &other)
    : Executable<Res,Op>(other)
  {}

  Statement(Statement &&other) : Executable<Res,Op>(std::move(other)) {}


  /// Bind parameter with given name to the given value.

  Statement& bind(const string &parameter, Value val)
  {
    try {
      get_impl()->add_param(parameter, std::move(val));
      return *this;
    }
    CATCH_AND_WRAP
  }

  /**
    Bind parameters to values given by a map from parameter
    names to their values.
  */

  template <class Map>
  Executable<Res,Op>& bind(const Map &args)
  {
    check_if_valid();
    for (const auto &keyval : args)
    {
      bind(keyval.first, keyval.second);
    }
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
    get_impl()->add_param(parameter, Value(begin, end));
    return *this;
  }

#endif

  struct Access;
  friend Access;
};


namespace internal {

/*
  Interface to be implemented by internal implementation
  of SQL statement object.

  Like with statement implementation, the `add_param` method
  is used to report values bound to SQL query parameters.
  Unlike the CRUD operations, SQL query parameters are
  not named but positional.
*/


struct SqlStatement_impl : public Executable_impl
{
  virtual void add_param(Value) = 0;
};

}  // internal


/**
  Represents an operation which executes an SQL statement.

  Before executing the statement, values of "?" placeholders
  that appear in it can be specified using `bind()` method.

  SqlStatement's method `execute` returns result of type
  `SqlResult`.

  @ingroup devapi_op
*/

DLL_WARNINGS_PUSH

class PUBLIC_API SqlStatement
  : public virtual Executable<SqlResult,SqlStatement>
{

DLL_WARNINGS_POP

protected:

  typedef internal::SqlStatement_impl Impl;

  SqlStatement() = default;

  using Executable<SqlResult,SqlStatement>::check_if_valid;
  using Executable<SqlResult,SqlStatement>::m_impl;

  Impl* get_impl()
  {
    check_if_valid();
    return static_cast<Impl*>(m_impl.get());
  }

  INTERNAL void reset(internal::XSession_base&, const string&);

public:

  SqlStatement(SqlStatement &other)
    : Executable<SqlResult,SqlStatement>(std::move(other))
  {}

  SqlStatement& bind(Value val)
  {
    get_impl()->add_param(val);
    return *this;
  }

  template <typename... Types>
  SqlStatement& bind(Value first, Types... rest)
  {
    bind(first);
    return bind(rest...);
  }

  template <
    class Container,
    typename = internal::enable_if_t<
      !std::is_convertible<Container, Value>::value
    >
  >
  SqlStatement& bind(const Container &c)
  {
    check_if_valid();
    for (const auto &val : c)
      get_impl()->add_param(val);
    return *this;
  }

  struct Access;
  friend Access;
  friend NodeSession;
};

}  // mysqlx

#endif
