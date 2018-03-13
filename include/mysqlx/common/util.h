/*
 * Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef MYSQLX_COMMON_UTIL_H
#define MYSQLX_COMMON_UTIL_H


#include <string>
#include <stdexcept>
#include <ostream>
#include <memory>
#include <forward_list>
#include <string.h>  // for memcpy
#include <utility>   // std::move etc
#include <algorithm>
#include <functional>
#include <type_traits>


/*
  Macro to be used to disable "implicit fallthrough" gcc warning
  <https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html>
*/

#ifdef __GNUC__
# if __GNUC__ < 7
#   define FALLTHROUGH // fallthrough
# else
  #define FALLTHROUGH __attribute__((fallthrough))
# endif
#else
#define FALLTHROUGH  // fallthrough
#endif


/*
  Note: we add throw statement to the definition of THROW() so that compiler won't
  complain if it is used in contexts where, e.g., a value should be returned from
  a function.
*/

#undef THROW

#ifdef THROW_AS_ASSERT

#define THROW(MSG)  do { assert(false && (MSG)); throw (MSG); } while(false)

#else

#define THROW(MSG) do { throw_error(MSG); throw (MSG); } while(false)

#endif


/*
  Macros used to disable warnings for fragments of code.
*/

#undef PRAGMA
#undef DISABLE_WARNING
#undef DIAGNOSTIC_PUSH
#undef DIAGNOSTIC_POP


#if defined __GNUC__ || defined __clang__

#define PRAGMA(X) _Pragma(#X)
#define DISABLE_WARNING(W) PRAGMA(GCC diagnostic ignored #W)

#if defined __clang__ || __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#define DIAGNOSTIC_PUSH PRAGMA(GCC diagnostic push)
#define DIAGNOSTIC_POP  PRAGMA(GCC diagnostic pop)
#else
#define DIAGNOSTIC_PUSH
#define DIAGNOSTIC_POP
#endif

#elif defined _MSC_VER

#define PRAGMA(X) __pragma(X)
#define DISABLE_WARNING(W) PRAGMA(warning (disable:W))

#define DIAGNOSTIC_PUSH  PRAGMA(warning (push))
#define DIAGNOSTIC_POP   PRAGMA(warning (pop))

#else

#define PRAGMA(X)
#define DISABLE_WARNING(W)

#define DIAGNOSTIC_PUSH
#define DIAGNOSTIC_POP

#endif


/*
  On Windows, MSVC issues warnings if public API class definition uses
  another class which is not exported as public API (either as a base class
  or type of member). This is indeed dangerous because client code might get
  the class definition wrong if the non-exported component is not available or
  (worse) is defined in a different way than the same component during connector
  build time.

  We can not completely avoid these warnings because for some API classes we
  use standard C++ library classes as components. For example, we use
  std::shared_ptr<> a lot. We can not modify standard library headers to export
  these classes. As is the common practice, we ignore this issue assuming that
  the code that uses our connector is built with the same C++ runtime
  implementation as the one used to build the connector. To silence the warnings,
  uses of standard library classes in our public API classes should be surrounded
  with DLL_WARNINGS_PUSH/POP macros.
*/

#if defined _MSC_VER

#define DLL_WARNINGS_PUSH  DIAGNOSTIC_PUSH \
  DISABLE_WARNING(4251) \
  DISABLE_WARNING(4275)
#define DLL_WARNINGS_POP   DIAGNOSTIC_POP

#else

#define DLL_WARNINGS_PUSH
#define DLL_WARNINGS_POP

#endif


/*
  A dirty trick to help Doxygen to process 'enum class' declarations, which
  are not fully supported. Thus we replace them by plain 'enum' when processing
  sources by Doxygen.
*/

#ifdef DOXYGEN
#define enum_class enum
#else
#define enum_class enum class
#endif


/*
  Macro to put at the end of other macros that define lists of items. This is
  another dirty trick for Doxygen to hide from it a documentation of the last
  item in the list. Otherwise, in a situation like this:

    #define ITEM_LIST(X) \
      X(item1) \
      ...
      X(itemN) /##< Doc for last item #/

  Doxegen treats the documentation of the last item as documentation for
  the whole ITEM_LIST() macro. This does not happen if END_LIST is added at
  the end:

    #define ITEM_LIST(X) \
      X(item1) \
      ...
      X(itemN) /##< Doc for last item #/ \
      END_LIST
*/

#define END_LIST


namespace mysqlx {

namespace common {

   /*
    Defined here because std::enable_if_t is not defined on all platforms on
    which we build (clang is missing one). Note: it is C++14 feature.
  */

  template<bool Cond, typename T = void>
  using enable_if_t = typename std::enable_if<Cond, T>::type;

  template<typename... Ty>
  using is_constructible_t = typename std::is_constructible<Ty...>::type;

  template<typename T>
  using remove_reference_t = typename std::remove_reference<T>::type;

  template <typename A, typename B>
  using is_same = std::is_same<A, B>;

}  // common


namespace common {

/*
  Convenience for checking numeric limits (to be used when doing numeric
  casts).

  TODO: Maybe more templates are needed for the case where T is a float/double
  type and U is an integer type or vice versa.
*/

#ifdef __cplusplus

template <
  typename T, typename U,
  enable_if_t<std::is_unsigned<U>::value>* = nullptr
>
inline
bool check_num_limits(U val)
{
  using UT = typename std::make_unsigned<T>::type;
  return !(val > (UT)std::numeric_limits<T>::max());
}

template <
  typename T, typename U,
  enable_if_t<std::is_unsigned<T>::value>* = nullptr,
  enable_if_t<!std::is_unsigned<U>::value>* = nullptr
>
inline
bool check_num_limits(U val)
{
  return !(val < 0) && !(val > std::numeric_limits<T>::max());
}

template <
  typename T, typename U,
  enable_if_t<!std::is_unsigned<T>::value>* = nullptr,
  enable_if_t<!std::is_unsigned<U>::value>* = nullptr
>
inline
bool check_num_limits(U val)
{
  return
    !((val > std::numeric_limits<T>::max())
     || (val < std::numeric_limits<T>::lowest()));
}

#define ASSERT_NUM_LIMITS(T,V) assert(mysqlx::common::check_num_limits<T>(V))

#endif


inline
std::string to_upper(const std::string &val)
{
  using std::transform;

  std::string uc_val;
  uc_val.resize(val.size());
  transform(val.begin(), val.end(), uc_val.begin(), ::toupper);
  return std::move(uc_val);
}

inline
std::string to_lower(const std::string &val)
{
  using std::transform;

  std::string uc_val;
  uc_val.resize(val.size());
  transform(val.begin(), val.end(), uc_val.begin(), ::tolower);
  return std::move(uc_val);
}

}  // common


namespace common {

#ifdef USE_NATIVE_BYTE
  using ::byte;
#else
  typedef unsigned char byte;
#endif

  class nocopy
  {
  public:
    nocopy(const nocopy&) = delete;
    nocopy& operator=(const nocopy&) = delete;
  protected:
    nocopy() {}
  };


  class Printable
  {
    virtual void print(std::ostream&) const = 0;
    friend std::ostream& operator<<(std::ostream&, const Printable&);
  };

  inline
  std::ostream& operator<<(std::ostream &out, const Printable &obj)
  {
    obj.print(out);
    return out;
  }


}  // common


namespace common {

using std::find_if;

/*
  Remove from a container all elements that satisfy the given predicate.
*/

template <class CONT, class PRED>
void remove_from(CONT &cont, PRED pred)
{
  using It = typename CONT::iterator;
  It end = std::remove_if(cont.begin(), cont.end(), pred);
  cont.erase(end, cont.end());
}


}  // common

}  // mysqlx


#endif
