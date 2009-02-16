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

#ifndef __CCPP_STRINGUTILS_H
#define __CCPP_STRINGUTILS_H

#include <sstream>

#include "ccppTypes.h"

namespace StringUtils
{
  unsigned  split     ( List &          list
                      , const String &  str
                      , const String &  delim
                      , bool            trimItems/*includeEmpty*/ = true );

  /* Unlike Java's trim, this trims only blank spaces */
  String    trim      ( const String & victim );

  int       toInt     ( const String & str, bool isNull = false );
  bool      toBoolean ( const String & str, bool isNull = false );
  long long toLong    ( const String & str, bool isNull = false );
  float     toFloat   ( const String & str, bool isNull = false );
  double    toDouble  ( const String & str, bool isNull = false );

  const String &  defaultIfEmpty  ( const String & str, const String & defStr );

  String toHexString( char c, bool leading0x= false );

  String toHexString( char c[], int count, bool leading0x= false );

  String & concatSeparated( String & to, const String & add, const String & separator= " " );
}

#endif
