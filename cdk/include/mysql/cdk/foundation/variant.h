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
#include <typeinfo>
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

  variant_base(const First &val)
    : m_owns(true)
  {
    new (&m_storage) First(val);
  }

  variant_base(First &&val)
    : m_owns(true)
  {
    new (&m_storage) First(std::move(val));
  }

  template<typename T>
  variant_base(T &&val)
    : Base(std::move(val))
    , m_owns(false)
  {}

  template<typename T>
  variant_base(const T &val)
    : Base(val)
    , m_owns(false)
  {}


  // Copy/move semantics

  variant_base(const variant_base &other)
    : Base(other)
  {
    if (!other.m_owns)
      return;
    *reinterpret_cast<First*>(&m_storage)
      = *other.get((First*)nullptr);
  }

  variant_base(variant_base &&other)
    : Base(std::move(other))
  {
    if (!other.m_owns)
      return;
    *reinterpret_cast<First*>(&m_storage)
      = std::move(*other.get((First*)nullptr));
  }

  void set(const First &val)
  {
    m_owns = true;
    new (&m_storage) First(val);
  }

  void set(First &&val)
  {
    m_owns = true;
    new (&m_storage) First(std::move(val));
  }

  template <
    typename T,
    typename std::enable_if<
      !std::is_same<
        typename std::remove_reference<T>::type,
        typename std::remove_reference<First>::type
      >::value
    >::type * = nullptr
  >
  void set(const T &val)
  {
    m_owns = false;
    Base::set(val);
  }

  template <
    typename T,
    typename std::enable_if<
      !std::is_same<
        typename std::remove_reference<T>::type,
        typename std::remove_reference<First>::type
      >::value
    >::type * = nullptr
  >
  void set(T &&val)
  {
    m_owns = false;
    Base::set(std::move(val));
  }


  const First* get(const First*) const
  {
    if (!m_owns)
      throw std::bad_cast();

    return reinterpret_cast<const First*>(&m_storage);
  }

  template <typename T>
  const T* get(const T *ptr) const
  {
    return Base::get(ptr);
  }

  template <class Visitor>
  void visit(Visitor& vis) const
  {
    if (m_owns)
      vis(*reinterpret_cast<const First*>(&m_storage));
    else
      Base::visit(vis);
  }

  void destroy()
  {
    if (m_owns)
    {
      reinterpret_cast<First*>(&m_storage)->~First();
      m_owns = false;
    }
    else
      Base::destroy();
  }

  operator bool()
  {
    if (m_owns)
      return true;
    return Base::operator bool();
  }

};


template <size_t Size, size_t Align>
class variant_base<Size, Align>
{
protected:

  typedef typename std::aligned_storage<Size, Align>::type storage_t;

  storage_t m_storage;

  variant_base() {}

  variant_base(const variant_base&) {}
  variant_base(variant_base &&) {}

  void destroy() {}

  operator bool()
  {
    return false;
  }

  template <class Visitor>
  void visit(Visitor&) const
  {
    assert(false);
  }

  template<typename T>
  void set(T &&)
  {
    assert(false);
  }
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

  variant() {}

  variant(const variant &other)
    : Base(static_cast<const Base&>(other))
  {}

  variant(variant &&other)
    : Base(std::move(static_cast<Base&&>(other)))
  {}

  template <typename T>
  variant(const T &val)
    : Base(val)
  {}

  template <typename T>
  variant(T &&val)
    : Base(std::move(val))
  {}

  template <typename T>
  variant& operator=(T&& val)
  {
    Base::set(std::move(val));
    return *this;
  }

  template <typename T>
  variant& operator=(const T& val)
  {
    Base::set(val);
    return *this;
  }

  ~variant()
  {
    Base::destroy();
  }

  operator bool()
  {
    return Base::operator bool();
  }

  template <class Visitor>
  void visit(Visitor& vis) const
  {
    Base::visit(vis);
  }

  template <typename T>
  const T& get() const
  {
    return *Base::get((const T*)nullptr);
  }
};


}}  // cdk::foundation

#endif
