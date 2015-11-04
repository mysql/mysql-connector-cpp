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

class TableInsertValues
  : public Executable
{
protected:

  Table &m_table;
  Row   *m_row = NULL;

  TableInsertValues(Table &table)
    : m_table(table)
  {
    prepare();
  }

  /*
    Methods below manipulate internal `Op_table_insert` object
    which holds the list of rows to be inserted (see crud.cc).
  */

  void prepare();
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

  virtual TableInsertValues& values(const Row &row)
  {
    add_row(row);
    return *this;
  }

  /**
    Add row consisting of specified values to the list of
    rows to be inserted.
  */

  template<typename... Types>
  TableInsertValues& values(const Value &val, Types... rest)
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
  friend class TableInsert;
};


/**
  Operation which inserts rows into a table.

  The operation holds a list of rows to be inserted. New rows
  can be added to the list using .values() method.
*/

class TableInsert
{
  Table &m_table;

  TableInsert(Table &table)
    : m_table(table)
  {}

public:

  /**
    Insert into a full table without restrincting the colums.

    Each row passed to the following .values() call must have
    the same number of values as the number of columns of the
    table. However, this check is done only after seding the insert
    command to the server. If value count does not match table column
    count server reports error.
  */

  virtual TableInsertValues insert()
  {
    return TableInsertValues(m_table);
  }

  friend class Table;
};


}  // mysqlx

#endif
