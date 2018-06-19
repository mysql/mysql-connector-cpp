/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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



#ifndef MYSQL_PRIVATE_IFACE_H
#define MYSQL_PRIVATE_IFACE_H

#if !defined(max)
#define WE_DEFINE_MAX_TO_SKIP_LIBMYSQL
#define max
#endif

#if ( defined(_WIN32) || defined(_WIN64) ) && defined(WE_DEFINE_MAX_TO_SKIP_LIBMYSQL)
#undef max
#endif

#if ( defined(_WIN32) || defined(_WIN64) ) && !defined(snprintf) && (_MSC_VER < 1900)
#define snprintf _snprintf
#endif

#if ( defined(_WIN32) || defined(_WIN64) ) && (_MSC_VER >= 1900)
#define HAVE_STRUCT_TIMESPEC
#endif

#if !defined(_WIN32) && !defined(_WIN64)
extern "C"
{
#endif
#if defined(_WIN32)
//#include <my_global.h>
#endif
#if A0_IF_WE_NEED_GET_CHARSET_FROM_LIBMYSQL
#include <my_sys.h>
#endif
#include <errmsg.h>
#include <mysql.h>

#if !defined(_WIN32) && !defined(_WIN64)
}
#endif

#if LIBMYSQL_VERSION_ID >= 80000
#define my_bool bool
#endif

/* my_global.h introduces bool and max */
#ifdef WE_DEFINE_MAX_TO_SKIP_LIBMYSQL
#undef max
#undef WE_DEFINE_MAX_TO_SKIP_LIBMYSQL
#endif

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(disable:4251)
#endif

#endif /* MYSQL_PRIVATE_IFACE_H */
