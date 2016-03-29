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

#ifndef MYSQLX_TABLE_CRUD_H
#define MYSQLX_TABLE_CRUD_H

/**
  @file
  Crud operations on tables.
*/


#include "mysqlx/common.h"
#include "mysqlx/result.h"
#include "mysqlx/task.h"

#include <forward_list>


namespace mysqlx {

class Table;


/**
  Base of `TableInsert` class implementing `values()` methods.

  @todo Check that every row passed to .values() call has
  the same number of values. The column count should match
  the one in insert(c1,...) call. For insert() without column
  list, it should be ?
*/

class TableInsert
  : public Executable
{
protected:

  Table &m_table;
  Row   *m_row = NULL;

  TableInsert(Table &table)
    : m_table(table)
  {
    prepare();
  }

  template <class Cols,class...Type>
  TableInsert(Table &table, const Cols& cols, const Type... rest)
    : m_table(table)
  {
    prepare();
    add_column(cols, rest...);
  }


  /*
    Methods below manipulate internal `Op_table_insert` object
    which holds the list of rows to be inserted (see crud.cc).
  */

  void prepare();

  void add_column(const string& col);
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
  void add_column(const T &t, const Type... rest)
  {
    add_column(t);
    add_column(rest...);
  }

  Row& add_row();
  void add_row(const Row &row);

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

public:

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

  struct Access;
  friend struct Access;
  friend class TableInsertBase;
};


/**
  Operation which inserts rows into a table.

  The operation holds a list of rows to be inserted. New rows
  can be added to the list using .values() method.
*/

class TableInsertBase
{
  Table &m_table;

  TableInsertBase(Table &table)
    : m_table(table)
  {}

  template <typename I>
  void insert_range(TableInsert& obj, const I& begin, const I& end)
  {
    for(auto it = begin; it != end; ++it)
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
    return TableInsert(m_table);
  }


  /**
    Insert into a full table restricting the colums.

    Each row passed to the following .values() call must have
    the same number of values as the list provided
  */

  template <class T,class... Types >
   TableInsert insert(const T &t, Types... rest)
   {
     return TableInsert(m_table, t, rest...);
   }

  friend class Table;
};


/**
  TableSelect class which implements the select() operation

  Data is filtered using the where() method.
  */

class TableSelect
    : public BindExec
{
  Table &m_table;

  TableSelect(Table &table)
    : m_table(table)
  {
    prepare();
  }


  void prepare();

public:

  BindExec& where(const string& expr);

  friend class TableSelectBase;
};

/**
  Operation which retrieves rows from a table.


  */

class TableSelectBase
{
  Table &m_table;

  TableSelectBase(Table &table)
    : m_table(table)
  {}

public:

  TableSelect select()
  {
    return TableSelect(m_table);
  }

  friend class Table;
};


/**
  Class used to update values on tables

  Class stores the field value pair to be updated.
  Filter of updated rows is passed using the where() method.
  */


class TableUpdate
    : public BindExec
{
  Table& m_table;

  TableUpdate(Table& table)
    : m_table(table)
  {
    prepare();
  }

  void prepare();

public:

  TableUpdate& set(const string& field, ExprValue);

  BindExec& where(const string& expr);

  friend class TableUpdateBase;
};

class TableUpdateBase
{
  Table &m_table;

  TableUpdateBase(Table &table)
    : m_table (table)
  {}

public:

  TableUpdate update()
  {
    return TableUpdate(m_table);
  }

  friend class Table;
};


/**
  Class used to remove rows

  Rows removed are the ones that apply to the expression passed using the
  where() method.
  If where() is not used, all the rows of the table are removed.
  */

class TableRemoveBase;

class TableRemove
    : public BindExec
{
  Table& m_table;

  TableRemove(Table& table)
    : m_table(table)
  {
    prepare();
  }

  void prepare();

public:

  BindExec& where(const string&);

  friend class TableRemoveBase;

};

class TableRemoveBase
{
  Table& m_table;

  TableRemoveBase(Table& table)
    : m_table(table)
  {}

public:

  TableRemove remove()
  {
    return TableRemove(m_table);
  }

  friend class Table;
};

}  // mysqlx

#endif
