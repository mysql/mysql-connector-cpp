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

  TODO: Revise and complete comments.
*/


#include "common.h"
#include "result.h"
#include "task.h"
#include "crud.h"

#include <forward_list>


namespace mysqlx {

class Table;


namespace internal {

  /*
    Virtual base class for TableXXXBase classes defined below.

    It defines members that can be shared between the different
    TableXXXBase classes which all are used as a base for
    the Table class.
  */

  class TableOpBase
  {
  protected:

    Table *m_table;

    TableOpBase(Table &table) : m_table(&table)
    {}

    /*
      This constructor is here only to alow defining
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

// ----------------------------------------------------------------------------------

namespace internal {
  class TableInsertBase;
}

/**
  Operation which inserts rows into a table.

  The operation holds a list of rows to be inserted. New rows
  can be added to the list using .values() method.

  @todo Check that every row passed to .values() call has
  the same number of values. The column count should match
  the one in insert(c1,...) call. For insert() without column
  list, it should be ?
*/

class TableInsert
  : public Executable
{
protected:

  Row   *m_row = NULL;

  template <class... Cols>
  TableInsert(Table &table, const Cols&... cols)
  {
    prepare(table);
    add_column(cols...);
  }


  /*
    Methods below manipulate internal `Op_table_insert` object
    which holds the list of rows to be inserted (see crud.cc).
  */

  void prepare(Table&);

  void add_column(const string& col);
  void add_column(string&& col);
  void add_column(const char* col) { add_column(string(col)); }
  template <typename Cols>
  void add_column(const Cols& cols)
  {
    for (auto col : cols)
    {
      add_column(col);
    }
  }

  template <class T,class...Type>
  void add_column(const T &t, const Type&... rest)
  {
    add_column(t);
    add_column(rest...);
  }

  Row& add_row();

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

  void add_row(const Row &row);

  void add_rows() {}

  template <typename... Types>
  void add_rows(const Row &first, Types... rest)
  {
    add_row(first);
    add_rows(rest...);
  }

  template<typename It>
  void add_range(const It &begin, const It &end)
  {
    for (It it = begin; it != end; ++it)
      add_row(*it);
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


  /// Add given row to the list of rows to be inserted.

  virtual TableInsert& values(const Row &row)
  {
    add_row(row);
    return *this;
  }

  /**
    Add row consisting of specified values to the list of
    rows to be inserted.
  */

  template<typename... Types>
  TableInsert& values(const Value &val, Types... rest)
  {
    try {
      m_row = &add_row();
      add_values(0, val, rest...);
      return *this;
    }
    CATCH_AND_WRAP
  }

  template<typename... Types>
  TableInsert& rows(const Row &first, Types... rest)
  {
    try {
      add_rows(first, rest...);
      return *this;
    }
    CATCH_AND_WRAP
  }

  template<typename Container>
  TableInsert& rows(const Container &cont)
  {
    try {
      add_range(std::begin(cont), std::end(cont));
      return *this;
    }
    CATCH_AND_WRAP
  }

  template<typename It>
  TableInsert& rows(const It &begin, const It &end)
  {
    try {
      add_range(begin, end);
      return *this;
    }
    CATCH_AND_WRAP
  }

  struct Access;
  friend struct Access;
  friend class internal::TableInsertBase;
};


namespace internal {

  class TableInsertBase : public virtual TableOpBase
  {

    template <typename I>
    void add_columns(TableInsert& obj, const I& begin, const I& end)
    {
      for (auto it = begin; it != end; ++it)
      {
        obj.add_column(*it);
      }
    }

  public:

    /**
      Insert into a full table without restrincting the colums.

      Each row passed to the following .values() call must have
      the same number of values as the number of columns of the
      table. However, this check is done only after seding the insert
      command to the server. If value count does not match table column
      count server reports error.
    */

    TableInsert insert()
    {
      return TableInsert(*m_table);
    }


    /*
      Insert into a full table restricting the colums.

      Each row passed to the following .values() call must have
      the same number of values as the list provided
    */

    template <class... T>
    TableInsert insert(const T&... t)
    {
      return TableInsert(*m_table, t...);
    }

    friend class Table;
  };

}  // internal


// ----------------------------------------------------------------------------------

namespace internal {

  /*
    Class implementing CRUD sort operations on tables.
  */

  template <bool limit_with_offset>
  class TableSort
    : public SortBase<limit_with_offset>
  {

  public:

    TableSort& orderBy(const string& ord)
    {
      /*
        Note: this-> is required by gcc because this is inside
        a template and then one has to distinct methods from
        global functions etc.
        see: http://stackoverflow.com/questions/29390663/error-there-are-no-arguments-to-at-that-depend-on-a-template-parameter-so-a
      */

      this->do_sort(ord);
      return *this;
    }

    TableSort& orderBy(const char* ord)
    {
      this->do_sort(ord);
      return *this;
    }

    template <typename Ord>
    TableSort& orderBy(Ord ord)
    {
      for (auto el : ord)
      {
        this->do_sort(ord);
      }
      return *this;
    }

    template <typename Ord, typename...Type>
    TableSort& orderBy(Ord ord, const Type...rest)
    {
      this->do_sort(ord);
      return orderBy(rest...);
    }

  };

}  // internal


// ----------------------------------------------------------------------------------

namespace internal {
  class TableSelectBase;
}

/**
  TableSelect class which implements the select() operation

  Data is filtered using the where() method.
*/

class TableSelect
  : public internal::TableSort<true>
{

  void prepare(Table &table);

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

  void add_proj(const string& projection);
  void add_proj(const char* projection)
  {
    add_proj(string(projection));
  }

  template <typename Proj>
  void add_proj(const Proj& proj)
  {
    for(auto el : proj)
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


  TableSelect(TableSelect &other) : Executable(other) {}
  TableSelect(TableSelect &&other) : TableSelect(other) {}

DIAGNOSTIC_POP

  TableSort& where(const string& expr);

  friend class internal::TableSelectBase;
};


namespace internal {

  class TableSelectBase : public virtual TableOpBase
  {
  public:

    template<typename ...PROJ>
    TableSelect select(const PROJ&...proj)
    {
      return TableSelect(*m_table, proj...);
    }

    friend class Table;
  };

}  // internal


// ----------------------------------------------------------------------------------

namespace internal {
  class TableUpdateBase;
}

/**
  Class used to update values on tables

  Class stores the field value pair to be updated.
  Filter of updated rows is passed using the where() method.
*/

class TableUpdate
: public internal::TableSort<false>
{

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

  TableUpdate(TableUpdate &other) : Executable(other) {}
  TableUpdate(TableUpdate &&other) : TableUpdate(other) {}

DIAGNOSTIC_POP

  TableUpdate& set(const string& field, internal::ExprValue val);

  internal::TableSort<false>& where(const string& expr);

  friend class internal::TableUpdateBase;
};


namespace internal {

  class TableUpdateBase : public virtual TableOpBase
  {
  public:

    TableUpdate update()
    {
      return TableUpdate(*m_table);
    }

    friend class Table;
  };

}  // internal


// ----------------------------------------------------------------------------------

namespace internal {
  class TableRemoveBase;
}

/**
  Class used to remove rows

  Rows removed are the ones that apply to the expression passed using the
  where() method.
  If where() is not used, all the rows of the table are removed.
*/


class TableRemove
: public internal::TableSort<false>
{

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

  TableRemove(TableRemove &other) : Executable(other) {}
  TableRemove(TableRemove &&other) : TableRemove(other) {}

DIAGNOSTIC_POP

  TableSort& where(const string&);

  friend class internal::TableRemoveBase;
};


namespace internal {

  class TableRemoveBase : public virtual TableOpBase
  {
  public:

    TableRemove remove()
    {
      return TableRemove(*m_table);
    }

    friend class Table;
  };

}  // internal

}  // mysqlx

#endif
