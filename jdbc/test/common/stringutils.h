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



#ifndef __CCPP_STRINGUTILS_H
#define __CCPP_STRINGUTILS_H

#include "ccppTypes.h"


namespace StringUtils
{
  template <class StdStringType>
  unsigned  _split    ( std::vector<StdStringType> & list
                     , const StdStringType &  str
                     , const StdStringType &  delim
                     , bool            trimItems=    true
                     , bool            includeEmpty= false);

  // just a shortcut for split<String>()
  unsigned  split     ( List &          list
                      , const String &  str
                      , const String &  delim
                      , bool            trimItems=    true
                      , bool            includeEmpty= false);

  template <class StdStringType>
  StdStringType & _trim ( StdStringType & victim );

  /* Unlike Java's trim, this trims only blank spaces */
  String &   trim      ( String & victim );


  int       toInt     ( const String & str, bool isNull = false );
  bool      toBoolean ( const String & str, bool isNull = false );
  long long toLong    ( const String & str, bool isNull = false );
  float     toFloat   ( const String & str, bool isNull = false );
  double    toDouble  ( const String & str, bool isNull = false );

  const String &  defaultIfEmpty  ( const String & str, const String & defStr );

  String toHexString( char c, bool leading0x= false );

  String toHexString( const char * c, int count, bool leading0x= false );

  String & concatSeparated( String & to, const String & add, const String & separator= " " );
}

#endif
