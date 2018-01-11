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
