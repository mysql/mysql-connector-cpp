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

  CRUD operation objects do not have copy semantics. If CRUD object a is
  constructed from b, like in "a = b", then the operation moves from b to
  a and any attempt to execute or modify b will trigger error.

*/


#include "common.h"
#include "result.h"
#include "statement.h"
#include "crud.h"


namespace mysqlx {

class Table;


namespace internal {

  /*
    Virtual base class for TableXXXBase classes defined below.

    It defines members that can be shared between the different
    TableXXXBase classes which all are used as a base for
    the Table class. Currently the only common member is
    `m_table` which points to the table on which the operation
    is performed.
    */

  class PUBLIC_API TableOpBase
  {
  protected:

    Table *m_table;

    TableOpBase(Table &table) : m_table(&table)
    {}

    /*
      This constructor is here only to allow defining
      TableXXXBase classes without a need to explicitly
      invoke TableOpBase constructor. But in the end,
      only the constructor called from the Table class
      should be used to initialize m_table member, not this one.
      Thus we add assertion to verify that it is not used.
    */

    TableOpBase() : m_table(NULL)
    {
      assert(false);
    }
  };

}  // internal

// ---------------------------------------------------------------------------

namespace internal {

  class TableInsertBase;

  /*
    Interface to be implemented by internal implementations of
    table insert operation.
  */

  struct TableInsert_impl : public Executable_impl
  {
    /*
      Pass to the implementation names of columns specified by
      the user. Columns are passed one-by-one in the order in
      which they were specified.
    */

    virtual void add_column(const string&) = 0;

    /*
      Pass to the implementation a row that should be inserted
      into the table. Several rows can be passed.
    */

    virtual void add_row(const Row&) = 0;

    /*
      Request another row to be inserted by the operation. Method
      should return empty Row instance to be filled with field data.
    */
    virtual Row& new_row() = 0;
  };

}  // internal


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
  : public Executable<Result,TableInsert>
{
  typedef internal::TableInsert_impl Impl;

  Impl* get_impl()
  {
    check_if_valid();
    return static_cast<Impl*>(m_impl.get());
  }

protected:

  Row   *m_row = NULL;

  template <class... Cols>
  TableInsert(Table &table, const Cols&... cols)
  {
    prepare(table);
    add_columns(cols...);
  }


  /*
    Helper methods which pass column/row information to the
    internal implementation object.
  */

  void prepare(Table&);

  void add_columns()
  {}

  void add_columns(const string& col)
  {
    get_impl()->add_column(col);
  }

  void add_columns(const char* col)
  {
    add_columns(string(col));
  }

  template <typename Cols>
  void add_columns(const Cols& cols)
  {
    for (auto col : cols)
    {
      add_columns(col);
    }
  }

  template <class T,class...Type>
  void add_columns(const T &t, const Type&... rest)
  {
    add_columns(t);
    add_columns(rest...);
  }

  void add_values(col_count_t pos, const Value &val)
  {
    //assert(m_row);
    m_row->set(pos, val);
  }

  template<typename... Types>
  void add_values(col_count_t pos, const Value &val, Types... rest)
  {
    add_values(pos, val);
    add_values(pos + 1, rest...);
  }

  void add_rows() {}

  template <typename... Types>
  void add_rows(const Row &first, Types... rest)
  {
    get_impl()->add_row(first);
    add_rows(rest...);
  }

  template<typename It>
  void add_range(const It &begin, const It &end)
  {
    for (It it = begin; it != end; ++it)
      get_impl()->add_row(*it);
  }

public:

  /// Create operation which inserts rows into given table.

  TableInsert(Table &table)
  {
    prepare(table);
  }

  TableInsert(TableInsert &other)
    : Executable(other)
    , m_row(other.m_row)
  {
    other.m_row = NULL;
  }

  TableInsert(TableInsert &&other) : TableInsert(other) {}

  /*
    TODO: Implement copy semantics for this and other crud operations.
    Note that the ctor TableInsert(TableInsert &other) implements move
    semantics, because m_row pointer is moved from one instance to the
    other.

    TODO: Add ctor from the base Executable<> class.
  */

  /// Add given row to the list of rows to be inserted.

  virtual TableInsert& values(const Row &row)
  {
    get_impl()->add_row(row);
    return *this;
  }

  /**
    Add single row consisting of specified values to the list of
    rows to be inserted.
  */

  template<typename... Types>
  TableInsert& values(const Value &val, Types... rest)
  {
    try {
      m_row = &(get_impl()->new_row());
      assert(m_row);
      add_values(0, val, rest...);
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
      add_rows(first, rest...);
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
      add_range(std::begin(cont), std::end(cont));
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
      add_range(begin, end);
      return *this;
    }
    CATCH_AND_WRAP
  }

  ///@cond IGNORED
  struct INTERNAL Access;
  friend Access;
  friend internal::TableInsertBase;
  ///@endcond
};


namespace internal {

  /**
    Base class which defines method that crate table insert operations.
  */

  class PUBLIC_API TableInsertBase : public virtual TableOpBase
  {
  public:

    /**
      Return operation which inserts rows into the full table without
      restricting the columns.

      Each row added by the operation must have the same number of values as
      the number of columns of the table. However, this check is done only
      after sending the insert command to the server. If value count does not
      match table column count server reports error.
    */

    TableInsert insert()
    {
      return TableInsert(*m_table);
    }


    /**
      Return operation which inserts row into the table restricting the columns.

      Each row added by the operation must have the same number of values
      as the columns specified here. However, this check is done only
      after sending the insert command to the server. If value count does not
      match table column count server reports error.
    */

    template <class... T>
    TableInsert insert(const T&... t)
    {
      return TableInsert(*m_table, t...);
    }

    friend Table;
  };

}  // internal


// ---------------------------------------------------------------------------

namespace internal {

  /**
    Class defining table CRUD .orderBy() clause.
  */

  template <class Res, class Op, bool limit_with_offset>
  class TableSort
    : public Limit<Res, Op, limit_with_offset>
  {
  protected:

    typedef internal::Sort_impl Impl;

    using Limit<Res, Op, limit_with_offset>::check_if_valid;
    using Limit<Res, Op, limit_with_offset>::m_impl;

    Impl* get_impl()
    {
      check_if_valid();
      return static_cast<Impl*>(m_impl.get());
    }

  public:

    TableSort& orderBy(const string& ord)
    {
      get_impl()->add_sort(ord);
      return *this;
    }

    TableSort& orderBy(const char* ord)
    {
      get_impl()->add_sort(ord);
      return *this;
    }

    /**
      Specify ordering of rows in the operation.

      This form accepts a vector, list or other container with strings, each
      string defining sorting direction and the value to sort on.
    */

    template <typename Ord>
    TableSort& orderBy(Ord ord)
    {
      for (auto el : ord)
      {
        get_impl()->add_sort(ord);
      }
      return *this;
    }

    /**
      Specify ordering of rows in the operation.

      Arguments are one or more strings, each defining sorting direction and
      the value to sort on.
    */

    template <typename Ord, typename...Type>
    TableSort& orderBy(Ord ord, const Type...rest)
    {
      get_impl()->add_sort(ord);
      return orderBy(rest...);
    }

  };

}  // internal


// ---------------------------------------------------------------------------

namespace internal {

  /**
    Class defining table CRUD .having() clause.
  */

  template <class Op>
  class TableHaving
    : public TableSort<RowResult, Op, true>
  {
  protected:

    typedef internal::Having_impl Impl;

    using TableSort<RowResult, Op, true>::check_if_valid;
    using TableSort<RowResult, Op, true>::m_impl;

//    typedef TableSort<RowResult, Op, true> TableSort;

    Impl* get_impl()
    {
      check_if_valid();
      return static_cast<Impl*>(m_impl.get());
    }

  public:

    /**
      Specify having filter in the operation.

      Arguments are a string defining filter to be used.
    */

    TableSort<RowResult, Op, true>& having(const string& having_spec)
    {
      get_impl()->set_having(having_spec);
      return *this;
    }

  };

}  // internal


// ----------------------------------------------------------------------

namespace internal {

  /**
    Class defining .groupBy() clause.
  */

  template <class Op>
  class TableGroupBy
      : public internal::TableHaving<Op>
  {

  protected:

    typedef internal::Group_by_impl Impl;

    using TableHaving<Op>::check_if_valid;
    using TableHaving<Op>::m_impl;

    typedef internal::TableHaving<Op> TableHaving;

    Impl* get_impl()
    {
      check_if_valid();
      return static_cast<Impl*>(m_impl.get());
    }


  public:

    /**
      Specify groupBy fields in the operation.

      Arguments are a one or more strings defining fields to group.
    */

    template <typename GroupBy,
              typename std::enable_if<std::is_convertible<GroupBy, string>::value>::type* = nullptr
              >
    TableHaving& groupBy(GroupBy group_by_spec)
    {
      get_impl()->add_group_by(group_by_spec);
      return *this;
    }

    template <typename GroupBy,
              typename std::enable_if<!std::is_convertible<GroupBy, string>::value>::type* = nullptr
              >
    TableHaving& groupBy(GroupBy group_by_spec)
    {
      for (auto el : group_by_spec)
      {
        get_impl()->add_group_by(el);
      }
      return *this;
    }

    template<typename GroupBy, typename...T>
    TableHaving& groupBy(GroupBy group_by_spec,
                              T...rest)
    {
      groupBy(group_by_spec);
      return groupBy(rest...);
    }


  };
}


// ---------------------------------------------------------------------------

namespace internal {

  class TableSelectBase;
  class Op_ViewCreateAlter;

  /*
    Interface to be implemented by internal implementations
    of table CRUD select operation.

    Method `add_where` is used to report selection criteria
    to the implementation.
  */

  struct TableSelect_impl : public Proj_impl
  {
    virtual void add_where(const string&) = 0;
  };

}  // internal


/**
  Operation which selects rows from a table.

  Apart from clauses defined by TableSort, it defines the
  where() clause which specifies selection criteria.

  For each row the operation can return all fields from the
  row or a set of values defined by projection expressions
  specified when operation was created.

  @ingroup devapi_op
*/

DLL_WARNINGS_PUSH

class PUBLIC_API TableSelect
  : public internal::TableGroupBy<TableSelect>
{

DLL_WARNINGS_POP

  typedef internal::TableSelect_impl Impl;

  using TableGroupBy<TableSelect>::check_if_valid;
  using TableGroupBy<TableSelect>::m_impl;

  Impl* get_impl()
  {
    check_if_valid();
    return static_cast<Impl*>(m_impl.get());
  }

  void prepare(Table &table);

  void add_proj(const string& projection)
  {
    get_impl()->add_proj(projection);
  }

  void add_proj(const char* projection)
  {
    add_proj(string(projection));
  }

  template <typename Proj>
  void add_proj(const Proj& proj)
  {
    for (auto el : proj)
    {
      add_proj(el);
    }
  }
  template <typename PROJ, typename ... REST>
  void add_proj(const PROJ& projection, const REST&...rest)
  {
    add_proj(projection);
    add_proj(rest...);
  }

public:

DIAGNOSTIC_PUSH

#if _MSC_VER && _MSC_VER < 1900
    /*
    MSVC 2013 has problems with delegating constructors for classes which
    use virtual inheritance.
    See: https://www.daniweb.com/programming/software-development/threads/494204/visual-c-compiler-warning-initvbases
    */
    DISABLE_WARNING(4100)
#endif

  TableSelect(Table &table)
  {
    prepare(table);
  }

  template <typename...PROJ>
  TableSelect(Table &table, const PROJ&... proj) : TableSelect(table)
  {
    add_proj(proj...);
  }

  TableSelect(const Executable<RowResult,TableSelect> &other)
    : Executable<RowResult,TableSelect>(other)
  {}

  TableSelect(Executable<RowResult,TableSelect> &&other)
    : Executable<RowResult,TableSelect>(std::move(other))
  {}

DIAGNOSTIC_POP

  /**
    Specify row selection criteria.

    The criteria is specified as a string containing Boolean expression.
  */

  TableSort& where(const string& expr)
  {
    get_impl()->add_where(expr);
    return *this;
  }

  ///@cond IGNORED
  friend internal::TableSelectBase;
  friend internal::Op_ViewCreateAlter;
  ///@endcond
};


namespace internal {

  /**
    Base class which defines methods that crate table queries.
  */

  class PUBLIC_API TableSelectBase : public virtual TableOpBase
  {
  public:

    /**
      Select rows from table.

      Optional list of expressions defines projection with transforms
      rows found by this operation.
    */

    template<typename ...PROJ>
    TableSelect select(const PROJ&...proj)
    {
      return TableSelect(*m_table, proj...);
    }

    friend Table;
  };

}  // internal


// ---------------------------------------------------------------------------


namespace internal {

  class TableUpdateBase;

  /*
    Interface to be implemented by internal implementations of
    table CRUD update operation. Such update operation sets values
    of fields in a row. Name of the column that should be set and
    expression defining new value are reported to the implementation
    using method `add_set`.
  */

  struct TableUpdate_impl : public TableSelect_impl
  {
    virtual void add_set(const string&, ExprValue&&) = 0;
  };

}  // internal


/**
  Operation which updates values stored in rows.

  Apart from clauses defined by `TableSort`, this class defines
  .set() clause for specifying new field values and .where()
  clause for narrowing set of rows to be modified.

  @ingroup devapi_op
*/

class PUBLIC_API TableUpdate
: public internal::TableSort<Result, TableUpdate, false>
{

  typedef internal::TableUpdate_impl Impl;
  typedef internal::TableSort<Result, TableUpdate, false>  TableSort;

  Impl* get_impl()
  {
    check_if_valid();
    return static_cast<Impl*>(m_impl.get());
  }

  void prepare(Table&);

public:

DIAGNOSTIC_PUSH

#if _MSC_VER && _MSC_VER < 1900
    DISABLE_WARNING(4100)
#endif

  TableUpdate(Table& table)
  {
    prepare(table);
  }

  // TODO: ctor with where condition?

  TableUpdate(TableUpdate &other) : Executable<Result,TableUpdate>(other) {}
  TableUpdate(TableUpdate &&other) : TableUpdate(other) {}

DIAGNOSTIC_POP

  /**
    Set given field in a row to the given value.

    The value can be either a direct literal or an expression given
    by `expr(<string>)`, evaluated in the server.
  */

  TableUpdate& set(const string& field, internal::ExprValue val)
  {
    get_impl()->add_set(field, std::move(val));
    return *this;
  }

  /**
    Specify selection criteria for rows that should be updated.
  */

  TableSort& where(const string& expr)
  {
    get_impl()->add_where(expr);
    return *this;
  }

  ///@cond IGNORED
  friend internal::TableUpdateBase;
  ///@endcond
};


namespace internal {

  /**
    Base class which defines methods that crate table update operations.
  */

  class PUBLIC_API TableUpdateBase : public virtual TableOpBase
  {
  public:

    /**
      Return operation which updates rows in the table.
    */

    TableUpdate update()
    {
      return TableUpdate(*m_table);
    }

    friend Table;
  };

}  // internal


// ---------------------------------------------------------------------------


namespace internal {

  class TableRemoveBase;

  /*
    Interface to be implemented by internal implementations
    of table CRUD remove operation.

    Selection criteria which selects rows to be removed is
    passed to the implementation using `add_where` method.
  */

  struct TableRemove_impl : public Sort_impl
  {
    virtual void add_where(const string&) = 0;
  };

}  // internal


/**
  Operation which removes rows from a table.

  Apart from clauses defined by `TableSort` this class defines
  .where() clause which selects rows to be removed.

  @ingroup devapi_op
*/


class PUBLIC_API TableRemove
  : public internal::TableSort<Result, TableRemove, false>
{

  typedef internal::TableRemove_impl Impl;
  typedef internal::TableSort<Result, TableRemove, false>  TableSort;

  Impl* get_impl()
  {
    check_if_valid();
    return static_cast<Impl*>(m_impl.get());
  }

  void prepare(Table &);

public:

DIAGNOSTIC_PUSH

#if _MSC_VER && _MSC_VER < 1900
    DISABLE_WARNING(4100)
#endif

  TableRemove(Table& table)
  {
    prepare(table);
  }

  // TODO: ctor with where condition?

  TableRemove(TableRemove &other) : Executable<Result,TableRemove>(other) {}
  TableRemove(TableRemove &&other) : TableRemove(other) {}

DIAGNOSTIC_POP

  /**
    Specify selection criteria for rows to be removed.
  */

  TableSort& where(const string &expr)
  {
    get_impl()->add_where(expr);
    return *this;
  }

  ///@cond IGNORED
  friend internal::TableRemoveBase;
  ///@endcond
};


namespace internal {

  /**
    Base class which defines methods that crate table remove operations.
  */

  class PUBLIC_API TableRemoveBase : public virtual TableOpBase
  {
  public:

    /**
      Return operation which removes rows from the table.
    */

    TableRemove remove()
    {
      return TableRemove(*m_table);
    }

    friend Table;
  };

}  // internal

}  // mysqlx

#endif
