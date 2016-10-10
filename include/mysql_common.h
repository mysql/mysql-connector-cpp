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

#ifndef MYSQL_COMMON_H
#define MYSQL_COMMON_H

#define DEFAULT_MYSQL_PORT  3306
#define DEFAULT_MYSQLX_PORT 33060


/*
  On Windows, dependency on the sockets library can be handled using
  #pragma comment directive.
*/

#ifdef _WIN32
#pragma comment(lib,"ws2_32")
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
  Macros for declaring public API
  ===============================

  API function declarations should be decorated with PUBLIC_API prefix. API
  classes should have PUBLIC_API marker between the "class" keyword and
  the class name.

  See: https://gcc.gnu.org/wiki/Visibility
*/

#if defined _MSC_VER

 #define DLL_EXPORT __declspec(dllexport)
 #define DLL_IMPORT __declspec(dllimport)
 #define DLL_LOCAL

#elif __GNUC__ >= 4

 #define DLL_EXPORT __attribute__ ((visibility ("default")))
 #define DLL_IMPORT
 #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))

#else

 #define DLL_EXPORT
 #define DLL_IMPORT
 #define DLL_LOCAL

#endif


#if defined CONCPP_BUILD_SHARED
  #define PUBLIC_API  DLL_EXPORT
  #define INTERNAL    DLL_LOCAL
#elif defined CONCPP_BUILD_STATIC
  #define PUBLIC_API
  #define INTERNAL
#elif !defined STATIC_CONCPP
  #define PUBLIC_API  DLL_IMPORT
  #define INTERNAL
#else
  #define PUBLIC_API
  #define INTERNAL
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
  the code that uses our connector will be built with the same C++ runtime
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


#endif
