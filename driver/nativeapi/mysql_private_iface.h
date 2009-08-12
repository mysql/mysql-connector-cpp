/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
