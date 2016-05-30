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

#include <mysqlx.h>
#include <uuid_gen.h>

#include <time.h>
#include <sstream>
#include <forward_list>
#include <boost/format.hpp>
#include <list>

#include "impl.h"


namespace mysqlx {


internal::BaseResult Executable::execute()
{
  if (!m_map.empty())
    m_task.m_impl->set_params(m_map);
  return PlainExecutable::execute();
}


namespace internal {

template <bool with_offset>
struct Crud_impl< internal::Limit<with_offset> >
{
  typedef Op_base type;
};

}

Executable& internal::Limit<false>::limit(unsigned rows)
{
  get_impl(this).limit(rows);
  return *this;
}

internal::Offset& internal::Limit<true>::limit(unsigned rows)
{
  get_impl(this).limit(rows);
  return *this;
}

namespace internal {

template<>
struct Crud_impl<internal::Offset>
{
  typedef Op_base type;
};

}

Executable& internal::Offset::offset(unsigned rows)
{
  get_impl(this).offset(rows);
  return *this;
}


namespace internal {

template<bool limit_with_offset>
struct Crud_impl< internal::SortBase<limit_with_offset> >
{
  typedef Op_base type;
};


template<>
void SortBase<false>::do_sort(const mysqlx::string &spec)
{
  get_impl(this).m_order.emplace_back(spec);
}

template<>
void SortBase<true>::do_sort(const mysqlx::string &spec)
{
  get_impl(this).m_order.emplace_back(spec);
}

}  // internal

}  // mysqlx
