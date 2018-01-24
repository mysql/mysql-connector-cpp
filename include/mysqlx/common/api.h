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

#ifndef MYSQLX_COMMON_API_H
#define MYSQLX_COMMON_API_H


/*
  Macros for declaring public API
  ===============================

  API function declarations should be decorated with PUBLIC_API prefix. API
  classes should have PUBLIC_API marker between the "class" keyword and
  the class name.

  See: https://gcc.gnu.org/wiki/Visibility

  TODO: Use better name than PUBLIC_API - not all public API classes should
  be decorated with these declarations but only these whose implementation
  is inside the library (so, not the ones which are implemented in headers).
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


#endif
