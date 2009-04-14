/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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

  String toHexString( char c[], int count, bool leading0x= false );

  String & concatSeparated( String & to, const String & add, const String & separator= " " );
}

#endif
