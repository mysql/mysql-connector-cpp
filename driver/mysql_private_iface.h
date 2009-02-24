/* Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   There are special exceptions to the terms and conditions of the GPL 
   as it is applied to this software. View the full text of the 
   exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this 
   software distribution.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef MYSQL_PRIVATE_IFACE_H
#define MYSQL_PRIVATE_IFACE_H

#if !defined(bool)
#define WE_DEFINE_BOOL_TO_SKIP_LIBMYSQL
#define bool
#endif
#if !defined(max)
#define WE_DEFINE_MAX_TO_SKIP_LIBMYSQL
#define max
#endif

#if ( defined(_WIN32) || defined(_WIN64) ) && defined(WE_DEFINE_BOOL_TO_SKIP_LIBMYSQL)
#undef bool
#endif

#if ( defined(_WIN32) || defined(_WIN64) ) && defined(WE_DEFINE_MAX_TO_SKIP_LIBMYSQL)
#undef max
#endif

#if ( defined(_WIN32) || defined(_WIN64) ) && !defined(snprintf)
#define snprintf _snprintf
#endif


#if !defined(_WIN32) && !defined(_WIN64)
extern "C"
{
#endif
#if defined(_WIN32)
#include <my_global.h>
#endif
#if A0_IF_WE_NEED_GET_CHARSET_FROM_LIBMYSQL
#include <my_sys.h>
#endif
#include <errmsg.h>
#include <mysql.h>

#if !defined(_WIN32) && !defined(_WIN64)
}
#endif


/* my_global.h introduces bool and max */
#ifdef WE_DEFINE_BOOL_TO_SKIP_LIBMYSQL
#undef bool
#undef WE_DEFINE_BOOL_TO_SKIP_LIBMYSQL
#endif
#ifdef WE_DEFINE_MAX_TO_SKIP_LIBMYSQL
#undef max
#undef WE_DEFINE_MAX_TO_SKIP_LIBMYSQL
#endif

#if defined(_WIN32) || defined(_WIN64) 
#pragma warning(disable:4251)
#endif

#endif /* MYSQL_PRIVATE_IFACE_H */
