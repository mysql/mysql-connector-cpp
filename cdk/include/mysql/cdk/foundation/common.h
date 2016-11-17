/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
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

#ifndef SDK_FOUNDATION_COMMON_H
#define SDK_FOUNDATION_COMMON_H


/*
  Disable deprecated elements of Boost.System.
*/

#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED
#endif


/*
  Enable header-only Boost implementation.
*/

#ifndef BOOST_ERROR_CODE_HEADER_ONLY
#define BOOST_ERROR_CODE_HEADER_ONLY
#endif


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
  DISABLE_WARNING(4738)

#else

#define PUSH_SYS_WARNINGS DIAGNOSTIC_PUSH

#endif

#define POP_SYS_WARNINGS  DIAGNOSTIC_POP


/*
  Macros to disable compile warnings triggered by Boost headers. One
  should put PUSH_BOOST_WARNINGS/POP_BOOST_WARNINGS around Boost header
  includes.

  Note: Clang recognizes GCC directives.
*/

#if defined _MSC_VER

#define PUSH_BOOST_WARNINGS PRAGMA(warning(push,1)) DISABLE_WARNING(4350)

#else

/*
  Boost uses std::auto_ptr<> which generates deprecated warnings when
  compiled with C++11 compiler.
*/

#define DISABLE_BOOST_WARNINGS \
  DISABLE_WARNING(-Wunknown-pragmas) \
  DISABLE_WARNING(-Wpragmas) \
  DISABLE_WARNING(-Wdeprecated-declarations) \
  DISABLE_WARNING(-Wunused-variable) \
  DISABLE_WARNING(-Wunused-local-typedef) \
  DISABLE_WARNING(-Wunused-local-typedefs)

#define PUSH_BOOST_WARNINGS DIAGNOSTIC_PUSH DISABLE_BOOST_WARNINGS

#endif

#define POP_BOOST_WARNINGS  DIAGNOSTIC_POP


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

#include <mysql/cdk/config.h>

#include <cstddef>
#include <assert.h>
#include <limits>

POP_SYS_WARNINGS

#undef max
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






#endif
