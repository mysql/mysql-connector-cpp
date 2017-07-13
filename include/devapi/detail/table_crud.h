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

#ifndef MYSQLX_DETAIL_TABLE_CRUD_H
#define MYSQLX_DETAIL_TABLE_CRUD_H

#include "../common.h"
#include "crud.h"

#include <sstream>
#include <type_traits>

namespace mysqlx {

namespace internal {

  /*
    Interface to be implemented by internal implementations of
    table insert operation.
  */

  struct Table_insert_impl : public Executable_impl
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

      TODO: use move semantics instead
    */

    virtual void add_row(const Row&) = 0;
  };


  /*
    Interface to be implemented by internal implementations
    of table CRUD select operation.

    Method `add_where` is used to report selection criteria
    to the implementation.
  */

  struct Table_select_impl : public Proj_impl
  {
    virtual void add_where(const string&) = 0;
  };


  /*
    Interface to be implemented by internal implementations of
    table CRUD update operation. Such update operation sets values
    of fields in a row. Name of the column that should be set and
    expression defining new value are reported to the implementation
    using method `add_set`.
  */

  struct Table_update_impl : public Table_select_impl
  {
    virtual void add_set(const string&, ExprValue&&) = 0;
  };


  /*
    Interface to be implemented by internal implementations
    of table CRUD remove operation.

    Selection criteria which selects rows to be removed is
    passed to the implementation using `add_where` method.
  */

  struct Table_remove_impl : public Sort_impl
  {
    virtual void add_where(const string&) = 0;
  };

}  // internal namespace


namespace internal {

  struct PUBLIC_API Table_insert_detail
  {
  protected:

    using Impl = Table_insert_impl;

    /*
      Helper methods which pass column/row information to the
      internal implementation object.
    */

    //void prepare(Table&);

    struct Add_column
    {
      static void process_one(Impl *impl, const string &col)
      {
        impl->add_column(col);
      }
    };

    struct Add_value
    {
      using Impl = std::pair<Row, unsigned>;

      static void process_one(Impl *impl, const Value &val)
      {
        impl->first.set((impl->second)++, val);
      }
    };

    struct Add_row
    {
      static void process_one(Impl *impl, const Row &row)
      {
        impl->add_row(row);
      }
    };

    template <typename... T>
    static void add_columns(Impl *impl, T... args)
    {
      Args_processor<Add_column,Impl*>::process_args(impl, args...);
    }

    template <typename... T>
    static void add_rows(Impl *impl, T... args)
    {
      Args_processor<Add_row,Impl*>::process_args(impl, args...);
    }

    template <typename... T>
    static void add_values(Impl *impl, T... args)
    {
      Add_value::Impl row{ {}, 0 };
      Args_processor<Add_value>::process_args(&row, args...);
      impl->add_row(row.first);
    }

    friend Args_processor<Add_column, Impl*>;
    friend Args_processor<Add_row, Impl*>;
    friend Args_processor<Add_value, Impl*>;

  };

  using Table_select_detail = Proj_detail;

}  // internal namespace

}  // mysqlx namespace

#endif
