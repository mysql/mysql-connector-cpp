/*
 * Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
 *
 * This code is licensed under the terms of the GPLv2
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

#ifndef SDK_FOUNDATION_VARIANT_H
#define SDK_FOUNDATION_VARIANT_H

/*
  Minimal implementation of variant<> type template.

  Note: Eventually this implementation should be replaced by std::variant<>.
*/

#include "common.h"

PUSH_SYS_WARNINGS
#include <type_traits> // std::aligned_storage
POP_SYS_WARNINGS

/*
  Note: MSVC 14 does not have certain C++11 constructs that are needed
  here.
*/

#if defined(_MSC_VER) && _MSC_VER < 1900
  #define CDK_CONSTEXPR   const
  #define CDK_ALIGNOF(T)  (std::alignment_of<T>::value)
#else
  #define CDK_CONSTEXPR   constexpr
  #define CDK_ALIGNOF(T)  alignof(T)
#endif



namespace cdk {
namespace foundation {


namespace detail {

template <size_t A, size_t B>
struct max
{
  static CDK_CONSTEXPR size_t value = A > B ? A : B;
};


template <
  size_t Size, size_t Align,
  typename... Types
>
class variant_base;

template <
  size_t Size, size_t Align,
  typename First,
  typename... Rest
>
class variant_base<Size, Align, First, Rest...>
  : public variant_base<
      detail::max<Size, sizeof(First)>::value,
      detail::max<Align, CDK_ALIGNOF(First)>::value,
      Rest...
    >
{
  typedef variant_base<
    detail::max<Size, sizeof(First)>::value,
    detail::max<Align, CDK_ALIGNOF(First)>::value,
    Rest...
  >  Base;

  bool m_owns;

protected:

  using Base::m_storage;


  variant_base() : m_owns(false)
  {}

  variant_base(const First &val) : m_owns(true)
  {
    new (&m_storage) First(val);
  }

  variant_base(First &&val) : m_owns(true)
  {
    new (&m_storage) First(std::move(val));
  }

  template <typename T>
  variant_base(const T &val) : Base(val), m_owns(false)
  {}

  template <typename T>
  variant_base(T &&val) : Base(std::move(val)), m_owns(false)
  {}


  const First* get(const First*) const
  {
    return reinterpret_cast<const First*>(&m_storage);
  }

  template <typename T>
  const T* get(const T *ptr) const
  {
    return Base::get(ptr);
  }

  void destroy()
  {
    if (m_owns)
      reinterpret_cast<First*>(&m_storage)->~First();
    else
      Base::destroy();
  }
};


template <size_t Size, size_t Align>
class variant_base<Size, Align>
{
protected:

  typedef typename std::aligned_storage<Size, Align>::type storage_t;

  storage_t m_storage;

  variant_base() {}
  variant_base(const variant_base&) = delete;
  variant_base(variant_base &&) = delete;

  void destroy() {}
};

}  // detail


template <
  typename... Types
>
class variant
  : private detail::variant_base<0,0,Types...>
{
  typedef detail::variant_base<0,0,Types...> Base;

public:

  template <typename T>
  variant(T&& val) : Base(std::move(val))
  {}

  template <typename T>
  variant(const T& val) : Base(val)
  {}

  ~variant()
  {
    Base::destroy();
  }

  template <typename T>
  const T& get() const
  {
    return *Base::get((const T*)nullptr);
  }
};


}}  // cdk::foundation

#endif
