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

#ifndef MYSQLX_ROW_H
#define MYSQLX_ROW_H

/**
  @file
  TODO
*/


#include "common.h"
#include "document.h"
#include "detail/row.h"

#include <memory>


namespace mysqlx {


/**
  Represents a single row from a result that contains rows.

  Such a row consists of a number of fields, each storing single
  value. The number of fields and types of values stored in each
  field are described by `RowResult` instance that produced this
  row.

  Values of fields can be accessed with `get()` method or using
  `row[pos]` expression. Fields are identified by 0-based position.
  It is also possible to get raw bytes representing value of a
  given field with `getBytes()` method.

  @sa `Value` class.
  @todo Support for iterating over row fields with range-for loop.

  @ingroup devapi_res
*/

class Row
  : private internal::Row_detail
{

  Row(internal::Row_detail &&other)
  try
    : Row_detail(std::move(other))
  {}
  CATCH_AND_WRAP


public:

  Row() {}

  template<typename T, typename... Types>
  explicit Row(T val, Types... vals)
  {
    try {
      Row_detail::set_values(0, val, vals...);
    }
    CATCH_AND_WRAP
  }


  col_count_t colCount() const
  {
    try {
      return Row_detail::col_count();
    }
    CATCH_AND_WRAP
  }


  /**
    Get raw bytes representing value of row field at position `pos`.

    @returns null bytes range if given field is NULL.
    @throws out_of_range if given row was not fetched from server.
  */

  bytes getBytes(col_count_t pos) const
  {
    try {
      return Row_detail::get_bytes(pos);
    }
    CATCH_AND_WRAP
  }


  /**
    Get reference to row field at position `pos`.

    @throws out_of_range if given field does not exist in the row.
  */

  Value& get(col_count_t pos)
  {
    try {
      return Row_detail::get_val(pos);
    }
    CATCH_AND_WRAP
  }


  /**
    Set value of row field at position `pos`.

    Creates new field if it does not exist.

    @returns Reference to the field that was set.
  */

  Value& set(col_count_t pos, const Value &val)
  {
    try {
      Row_detail::set_values(pos, val);
      return Row_detail::get_val(pos);
    }
    CATCH_AND_WRAP
  }

  /**
    Get const reference to row field at position `pos`.

    This is const version of method `get()`.

    @throws out_of_range if given field does not exist in the row.
  */

  const Value& operator[](col_count_t pos) const
  {
    return const_cast<Row*>(this)->get(pos);
  }


  /**
    Get modifiable reference to row field at position `pos`.

    The field is created if it does not exist. In this case
    the initial value of the field is NULL.
  */

  Value& operator[](col_count_t pos)
  {
    ensure_impl();
    try {
      return get(pos);
    }
    catch (const out_of_range&)
    {
      return set(pos, Value());
    }
  }

  /// Check if this row contains fields or is null.
  bool isNull() const { return NULL == m_impl; }
  operator bool() const { return !isNull(); }

  void clear()
  {
    try {
      Row_detail::clear();
    }
    CATCH_AND_WRAP
  }

private:

  using internal::Row_detail::m_impl;

  /// @cond IGNORED
  friend internal::Row_result_detail<Columns>;
  friend internal::Table_insert_detail;
  /// @endcond
};

}  // mysqlx

#endif
