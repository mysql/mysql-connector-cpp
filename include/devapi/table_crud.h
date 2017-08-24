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

#ifndef MYSQLX_TABLE_CRUD_H
#define MYSQLX_TABLE_CRUD_H

/**
  @file
  Crud operations on tables.

  Classes declared here represent CRUD operations on a given table. They are
  analogous to collection CRUD operation classes defined in collection_crud.h.

  The following classes for table CRUD operations are defined:
  - TableInsert
  - TableDelete
  - TableSelect
  - TableUpdate

  CRUD operation objects can be created directly, or assigned from
  result of DevAPI methods that create such operations:
  ~~~~~~
  TableInsert  insert_op(table);
  TableSelect  select_op = table.select(...).orderBy(...);
  ~~~~~~

  TODO: Fluent API grammar diagram.
*/


#include "common.h"
#include "result.h"
#include "executable.h"
#include "crud.h"
#include "detail/table_crud.h"

namespace mysqlx {

class Table;

// ---------------------------------------------------------------------------

class TableInsert;

namespace internal {

  struct Table_insert_base
    : public Executable<Result, TableInsert>
  {};

}

/**
  Operation which inserts rows into a table.

  This class defines the .values() and .rows() clauses which
  specify rows to be inserted.

  @todo Check that every row passed to .values() call has
  the same number of values. The column count should match
  the one in insert(c1,...) call. For insert() without column
  list, it should be ?

  @ingroup devapi_op
*/

class PUBLIC_API TableInsert
  : public internal::Table_insert_base
  , internal::Table_insert_detail
{

protected:

  template <class... Cols>
  TableInsert(Table &table, const Cols&... cols)
    : TableInsert(table)
  {
    add_columns(get_impl(), cols...);
  }

  // Create operation which inserts rows into given table.

  TableInsert(Table &table);

public:

  TableInsert(const internal::Table_insert_base &other)
  {
    internal::Table_insert_base::operator=(other);
  }

  TableInsert(internal::Table_insert_base &&other)
  {
    internal::Table_insert_base::operator=(std::move(other));
  }


  /// Add given row to the list of rows to be inserted.

  virtual TableInsert& values(const Row &row)
  {
    try {
      get_impl()->add_row(row);
      return *this;
    }
    CATCH_AND_WRAP
  }

  /**
    Add single row consisting of specified values to the list of
    rows to be inserted.
  */

  template<typename... Types>
  TableInsert& values(Types... rest)
  {
    try {
      add_values(get_impl(), rest...);
      return *this;
    }
    CATCH_AND_WRAP
  }

  /**
    Add rows from a container such as vector or list.
  */

  template<typename Container>
  TableInsert& rows(const Container &cont)
  {
    try {
      add_rows(get_impl(), cont);
      return *this;
    }
    CATCH_AND_WRAP
  }

  /**
    Add rows from a range given by two iterators.
  */

  template<typename It>
  TableInsert& rows(const It &begin, const It &end)
  {
    try {
      add_rows(get_impl(), begin, end);
      return *this;
    }
    CATCH_AND_WRAP
  }

  /**
    Add specified rows.
  */

  template<typename... Types>
  TableInsert& rows(const Row &first, Types... rest)
  {
    try {
      add_rows(get_impl(), first, rest...);
      return *this;
    }
    CATCH_AND_WRAP
  }

protected:

  using Impl = internal::Table_insert_impl;

  Impl* get_impl()
  {
    return static_cast<Impl*>(internal::Table_insert_base::get_impl());
  }

  ///@cond IGNORED
  friend Table;
  ///@endcond
};


// ---------------------------------------------------------------------------

class TableSelect;

namespace internal {

  class Op_view_create_alter;

  struct Table_select_cmd
    : public Executable<RowResult, TableSelect>
  {};

  struct Table_select_base
    : public Group_by < Having < Order_by < Limit < Offset< Bind_parameters<
              Set_lock< Table_select_cmd >
             > > > > > >
  {};

}


/**
  Operation which selects rows from a table.

  Apart from clauses defined by Table_sort, it defines the
  where() clause which specifies selection criteria.

  For each row the operation can return all fields from the
  row or a set of values defined by projection expressions
  specified when operation was created.

  @ingroup devapi_op
*/

DLL_WARNINGS_PUSH

class PUBLIC_API TableSelect
  : public internal::Table_select_base
  , internal::Table_select_detail
{
DLL_WARNINGS_POP

  using Operation = Table_select_base;

public:

  TableSelect(Table &table);

  template <typename...PROJ>
  TableSelect(Table &table, const PROJ&... proj)
    : TableSelect(table)
  {
    try {
      add_proj(get_impl(), proj...);
    }
    CATCH_AND_WRAP
  }

  TableSelect(const internal::Table_select_cmd &other)
  {
    internal::Table_select_cmd::operator=(other);
  }

  TableSelect(internal::Table_select_cmd &&other)
  {
    internal::Table_select_cmd::operator=(std::move(other));
  }


  /**
    Specify row selection criteria.

    The criteria is specified as a string containing Boolean expression.
  */

  Operation& where(const string& expr)
  {
    try {
      get_impl()->add_where(expr);
      return *this;
    }
    CATCH_AND_WRAP
  }

protected:

  using Impl = internal::Table_select_impl;

  Impl* get_impl()
  {
    return static_cast<Impl*>(internal::Table_select_base::get_impl());
  }

  ///@cond IGNORED
  friend Table;
  friend internal::Op_view_create_alter;
  ///@endcond
};


// ---------------------------------------------------------------------------

class TableUpdate;

namespace internal {

  struct Table_update_cmd
    : public Executable<Result, TableUpdate>
  {};

  struct Table_update_base
    : public Order_by< Limit< Bind_parameters< Table_update_cmd > > >
  {};

}


/**
  Operation which updates values stored in rows.

  Apart from methods for specifying sorting order and binding paramater values,
  this class defines .set() method for modifying field values and .where()
  method for narrowing set of rows to be modified.

  @ingroup devapi_op
*/

class PUBLIC_API TableUpdate
: public internal::Table_update_base
{
  using Operation = internal::Table_update_base;

  TableUpdate(Table& table);

public:

  // TODO: ctor with where condition?

  TableUpdate(const internal::Table_update_cmd &other)
  {
    internal::Table_update_cmd::operator=(other);
  }

  TableUpdate(internal::Table_update_cmd &&other)
  {
    internal::Table_update_cmd::operator=(std::move(other));
  }


  /**
    Set given field in a row to the given value.

    The value can be either a direct literal or an expression given
    by `expr(<string>)`, evaluated in the server.
  */

  TableUpdate& set(const string& field, internal::ExprValue val)
  {
    try {
      get_impl()->add_set(field, std::move(val));
      return *this;
    }
    CATCH_AND_WRAP
  }

  /**
    Specify selection criteria for rows that should be updated.
  */

  Operation& where(const string& expr)
  {
    try {
      get_impl()->add_where(expr);
      return *this;
    }
    CATCH_AND_WRAP
  }

protected:

  using Impl = internal::Table_update_impl;

  Impl* get_impl()
  {
    return static_cast<Impl*>(internal::Table_update_base::get_impl());
  }

  ///@cond IGNORED
  friend Table;
  ///@endcond
};


// ---------------------------------------------------------------------------

class TableRemove;

namespace internal {

  struct Table_remove_cmd
    : public Executable<Result, TableRemove>
  {};

  struct Table_remove_base
    : Order_by< Limit< Bind_parameters< Table_remove_cmd > > >
  {};

}


/**
  Operation which removes rows from a table.

  Apart from methods for specifying sort order and binding parameter values,
  this class defines .where() method which selects rows to be removed.

  @ingroup devapi_op
*/

class PUBLIC_API TableRemove
  : public internal::Table_remove_base
{
  using Operation = internal::Table_remove_base;

  TableRemove(Table& table);

public:

  // TODO: ctor with where condition?

  TableRemove(const internal::Table_remove_cmd &other)
  {
    internal::Table_remove_cmd::operator=(other);
  }

  TableRemove(internal::Table_remove_cmd &&other)
  {
    internal::Table_remove_cmd::operator=(std::move(other));
  }


  /**
    Specify selection criteria for rows to be removed.
  */

  Operation& where(const string &expr)
  {
    try {
      get_impl()->add_where(expr);
      return *this;
    }
    CATCH_AND_WRAP
  }

protected:

  using Impl = internal::Table_remove_impl;

  Impl* get_impl()
  {
    return static_cast<Impl*>(internal::Table_remove_base::get_impl());
  }

  ///@cond IGNORED
  friend Table;
  ///@endcond
};


}  // mysqlx

#endif
