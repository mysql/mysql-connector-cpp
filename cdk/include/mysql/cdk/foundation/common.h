/*
 * Copyright (c) 2016, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef CDK_FOUNDATION_COMMON_H
#define CDK_FOUNDATION_COMMON_H

/*
  Macros used to disable warnings for fragments of code.
*/

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
  Macros to disable compile warnings in system headers. Put
  PUSH_SYS_WARNINGS/POP_SYS_WARNINGS around sytem header includes.
*/

#if defined _MSC_VER

/*
  Warning 4350 is triggered by std::shared_ptr<> implementation
  - see https://msdn.microsoft.com/en-us/library/0eestyah.aspx
*/

#define PUSH_SYS_WARNINGS \
  PRAGMA(warning (push,2)) \
  DISABLE_WARNING(4350) \
  DISABLE_WARNING(4738) \
  DISABLE_WARNING(4548) \
  DISABLE_WARNING(4996)

#else

#define PUSH_SYS_WARNINGS DIAGNOSTIC_PUSH

#endif

#define POP_SYS_WARNINGS  DIAGNOSTIC_POP


// Avoid warnings from Protobuf includes

#if defined _MSC_VER

/*
  Turn /W1 level (severe warnings) - we are not really interested
  in tracing protbuf code warnings.
*/

#define PUSH_PB_WARNINGS  PRAGMA(warning(push,1))

#else

#define PUSH_PB_WARNINGS DIAGNOSTIC_PUSH \
    DISABLE_WARNING(-Wshadow) \
    DISABLE_WARNING(-Wunused-parameter) \
    DISABLE_WARNING(-Wdeprecated-declarations) \

#endif

#define POP_PB_WARNINGS   DIAGNOSTIC_POP


#if defined _MSC_VER

/*
  We want to use functions which trigger this security warning on Windows,
  for example string::copy().
*/

#define PUSH_SCL_SECURE_WARNINGS  DIAGNOSTIC_PUSH \
  DISABLE_WARNING(4996)

#else

#define PUSH_SCL_SECURE_WARNINGS

#endif

#define POP_SCL_SECURE_WARNINGS  DIAGNOSTIC_POP



/*
  Include common system headers.
*/

PUSH_SYS_WARNINGS

#if defined(_WIN32)

#if _WIN32_WINNT < 0x0600
  #undef _WIN32_WINNT
  #define _WIN32_WINNT 0x0600
#endif

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #undef WIN32_LEAN_AND_MEAN
#else
  #include <windows.h>
#endif

#else

#if defined(__APPLE__)
  #include <sys/time.h>
#endif
#include <errno.h>

#endif


#if defined(_MSC_VER) && _MSC_VER < 1900
  #define NOEXCEPT
#else
  #define NOEXCEPT noexcept
#endif


#include <mysql/cdk/config.h>

#include <cstddef>
#include <assert.h>
#include <limits>
#include <utility>

POP_SYS_WARNINGS

#undef max
#undef byte
#undef THROW

/*
  Note: we add throw statement to the definition of THROW() so that compiler won't
  complain if it is used in contexts where, e.g., a value should be returned from
  a function.
*/

#ifdef THROW_AS_ASSERT

#define THROW(MSG)  do { assert(false && (MSG)); throw (MSG); } while(false)

#else

/*
  Code which uses this macro must ensure that function throw_error(const char*)
  is available in the context in which the macro is used. Primarily this should
  be function cdk::foundation::throw_error(const char*) defined in error.h. But
  it is intentionally left unqualified so that code can use different function
  implementations if needed.
*/

#define THROW(MSG) do { throw_error(MSG); throw (MSG); } while(false)

#endif


/*
  Macro to be used to disable "implicit fallthrough" gcc warning
  <https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html>
*/

#ifndef FALLTHROUGH
# if (defined( __GNUC__ ) || defined (__clang__))
#  if defined(__GNUC__) && __GNUC__ < 7
#    define FALLTHROUGH // FALLTHROUGH
#  else
#    if __cplusplus >= 201703L
#      define FALLTHROUGH [[fallthrough]] // FALLTHROUGH C++17
#    elif __cplusplus >= 201103L
#      if defined (__clang__)
#        define FALLTHROUGH [[gnu::fallthrough]] // FALLTHROUGH C++11 and C++14
#      else
#        define FALLTHROUGH [[clang::fallthrough]] // FALLTHROUGH C++11 and C++14
#      endif
#    else
#      define FALLTHROUGH __attribute__((fallthrough))
#    endif
#  endif
# else
#   define FALLTHROUGH  // FALLTHROUGH
# endif
#endif //FALLTHROUGH

#ifdef __cplusplus

namespace cdk {
namespace foundation {


#ifdef USE_NATIVE_BYTE
  using ::byte;
#else
  typedef unsigned char byte;
#endif


/*
  Convenience class to disable copy constructor in a derived class.
*/

class nocopy
{
  nocopy(const nocopy&);
  nocopy& operator=(const nocopy&);

protected:
  nocopy() {}
};



#ifndef HAVE_IS_SAME

  template <typename T, typename U>
  struct is_same
  {
    static const bool value = false;
  };

  template <typename T>
  struct is_same<T,T>
  {
    static const bool value = true;
  };

#else

  using std::is_same;

#endif


/*
  Convenience for checking numeric limits (to be used when doing numeric
  casts).

  TODO: Maybe more templates are needed for the case where T is a float/double
  type and U is an integer type or vice versa.
*/


template <
  typename T, typename U,
  typename std::enable_if<std::is_unsigned<U>::value>::type* = nullptr
>
inline
bool check_num_limits(U val)
{
  using UT = typename std::make_unsigned<T>::type;
  return !(val > (UT)std::numeric_limits<T>::max());
}

template <
  typename T, typename U,
  typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr,
  typename std::enable_if<!std::is_unsigned<U>::value>::type* = nullptr
>
inline
bool check_num_limits(U val)
{
  return !(val < 0) && !(val > std::numeric_limits<T>::max());
}

template <
  typename T, typename U,
  typename std::enable_if<!std::is_unsigned<T>::value>::type* = nullptr,
  typename std::enable_if<!std::is_unsigned<U>::value>::type* = nullptr
>
inline
bool check_num_limits(U val)
{
  return
    !((val > std::numeric_limits<T>::max())
     || (val < std::numeric_limits<T>::lowest()));
}

#define ASSERT_NUM_LIMITS_CDK(T,V) assert(cdk::foundation::check_num_limits<T>(V))

}}  // cdk::foundation

#endif

#endif
