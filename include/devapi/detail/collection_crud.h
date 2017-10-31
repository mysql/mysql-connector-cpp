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

#ifndef MYSQLX_DETAIL_COLLECTION_CRUD_H
#define MYSQLX_DETAIL_COLLECTION_CRUD_H

#include "../common.h"
#include "crud.h"

#include <sstream>
#include <type_traits>

namespace mysqlx {

namespace internal {

  /*
    Interface to internal implementations of CRUD add operation.
  */

  struct Collection_add_impl : public Executable_impl
  {
    /*
      Note: Current implementation only supports sending
      documents in form of JSON strings.
    */

    virtual void add_json(const string&) = 0;
  };


  /*
    Interface to internal implementations of CRUD modify operation.

    Methods `add_operation` are used to pass to the implementation object
    the modifications requested by the user.
  */

  struct Collection_modify_impl : public Sort_impl
  {
    enum Operation
    {
      SET,
      UNSET,
      ARRAY_INSERT,
      ARRAY_APPEND,
      ARRAY_DELETE,
      MERGE_PATCH
    };

    virtual void add_operation(Operation, ExprValue&&) = 0;
    virtual void add_operation(Operation, const Field&, ExprValue&&) = 0;
    virtual void add_operation(Operation, const Field&) = 0;
  };

}  // internal namespace


namespace internal {


  struct PUBLIC_API Collection_add_detail
  {
  protected:

    using Impl = Collection_add_impl;
    using Args_prc = Args_processor<Collection_add_detail, Impl*>;

    static void process_one(Impl *impl, const string &json)
    {
      impl->add_json(json);
    }

    static void process_one(Impl *impl, const DbDoc &doc)
    {
      // TODO: Do it better when we support sending structured
      // document descriptions to the server.

      std::ostringstream buf;
      buf << doc;
      impl->add_json(buf.str());
    }

    template <typename... T>
    static void do_add(Impl *impl, T... args)
    {
      Args_prc::process_args(impl, args...);
    }

    friend Args_prc;
  };


  struct PUBLIC_API Collection_find_detail
  {
  protected:

    using Impl = internal::Proj_impl;
    using Args_prc = Args_processor<Collection_find_detail, Impl*>;

    static void process_one(Impl *impl, const string &proj)
    {
      impl->add_proj(proj);
    }


    static void do_fields(Impl *impl, const internal::ExprValue &proj)
    {
      if (!proj.isExpression())
        throw_error("Invalid projection");
      impl->set_proj(proj);
    }

    /*
      Note: If e is an expression (of type ExprValue) then only
      .fields(e) is valid - the multi-argument variant .fields(e,...)
      should be disabled.
    */

    template <
      typename T, typename... R,
      typename = enable_if_t<!std::is_same<T, internal::ExprValue>::value>
    >
      static void do_fields(Impl *impl, T first, R... rest)
    {
      Args_prc::process_args(impl, first, rest...);
    }

    friend Args_prc;
  };

}  // internal namespace

}  // mysqlx namespace

#endif
