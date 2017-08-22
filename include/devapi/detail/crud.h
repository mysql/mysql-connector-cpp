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

#ifndef MYSQLX_DETAIL_CRUD_H
#define MYSQLX_DETAIL_CRUD_H

/**
  @file
  Implementation interface hierarchy and details for templates
  defining CRUD operations.
*/


#include "../common.h"
#include "../executable.h"


namespace mysqlx {
namespace internal {

/*
  The XXX_impl classes defined below form a hierarchy of interfaces, based
  on Executable_impl, for internal implementations of crud operations.
  The concrete implementations, like Op_collection_find defined in
  devapi/collection_crud.cc, implement one of the top interfaces in this
  hierarchy but the hierarchy allows casting down the implementation to
  the layer implementing particular aspect of the operation. For example
  Limit_impl interface allows setting limit and offset for returned/affected
  rows/documents, which is common for different CRUD operations.
*/


struct Bind_impl : public Executable_impl
{
  virtual void add_param(const string&, Value&&) = 0;
  virtual void add_param(Value) = 0;
};


struct Limit_impl : public Bind_impl
{
  virtual void set_offset(unsigned) = 0;
  virtual void set_limit(unsigned) = 0;
};


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


/*
  Classes encapsulating details of various CRUD methods of the CRUD operation
  APIs defined by templates from "devapi/crud.h".
*/

struct PUBLIC_API Bind_detail
{
protected:

  using Impl = Bind_impl;
  using Args_prc = Args_processor<Bind_detail, Impl*>;

  static void process_one(Impl *impl, Value val)
  {
    impl->add_param(val);
  }

  template <typename... T>
  static void add_params(Impl *impl, T... vals)
  {
    Args_prc::process_args(impl, vals...);
  }

  friend Args_prc;
};


struct PUBLIC_API Sort_detail
{
protected:

  using Impl = Sort_impl;
  using Args_prc = Args_processor<Sort_detail, Impl*>;

  static void process_one(Impl *impl, const string &ord_spec)
  {
    impl->add_sort(ord_spec);
  }

  template <typename... T>
  static void add_sort(Impl *impl, T... args)
  {
    Args_prc::process_args(impl, args...);
  }

  friend Args_prc;
};


struct PUBLIC_API Group_by_detail
{
protected:

  using Impl = Group_by_impl;
  using Args_prc = Args_processor<Group_by_detail, Impl*>;

  static void process_one(Impl *impl, const string &spec)
  {
    impl->add_group_by(spec);
  }

  template <typename... T>
  static void do_group_by(Impl *impl, T... args)
  {
    Args_prc::process_args(impl, args...);
  }

  friend Args_prc;
};


struct PUBLIC_API Proj_detail
{
protected:

  using Impl = Proj_impl;
  using Args_prc = Args_processor<Proj_detail, Impl*>;

  static void process_one(Impl *impl, const string &spec)
  {
    impl->add_proj(spec);
  }

  template <typename... T>
  static void add_proj(Impl *impl, T... proj_spec)
  {
    Args_prc::process_args(impl, proj_spec...);
  }

  friend Args_prc;
};


}}  // mysqlx::internal

#endif
