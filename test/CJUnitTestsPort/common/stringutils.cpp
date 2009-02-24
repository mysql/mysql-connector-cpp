/*
   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include "stringutils.h"
#include <stdlib.h>
#include <stdio.h>

namespace StringUtils
{
  // In java(tests?) split's last boolean param. This function skips empties
  unsigned split( List & list, const String & str, const String & delim
    , bool trimItems/*includeEmpty*/)
  {
    /* not sure that we have to do it here*/
    /* list.clear(); */

    int count     = 0;
    int prevPos   = 0;
    int newPos    = 0;
    int delimSize = static_cast<int>( delim.size());
    int strSize   = static_cast<int>( str.size()  );

    String item;

    while ( prevPos < strSize && (newPos = str.find(delim, prevPos)) != (int)String::npos )
    {
      if ( newPos == prevPos && delimSize == 0 )
        ++newPos;

      if ( newPos > prevPos /*|| includeEmpty*/ )
      {
        item = str.substr(prevPos, newPos - prevPos );

        if (trimItems)
          trim(item);

        list.push_back( item );
        ++count;
      }

      prevPos = newPos + delimSize;
    }

    /* if string has delimiter at the end, and includeEmpty flag is set
    - we doing that, unless delimiter is present (delimSize > 0) */
/*  if ( prevPos == strSize && includeEmpty && delimSize > 0 )
    {
      list.push_back( _T("") );
      ++count;
    }
*/

    /* else - adding part after last token*/
    /*else*/ if (prevPos < strSize )
    {
      item = str.substr(prevPos);

      if (trimItems)
        trim(item);

      list.push_back( item );
      ++count;
    }

    return count;
  }

  String trim( const String & victim )
  {
    static const String::value_type * space = _T(" ");

    String::size_type begin = victim.find_first_not_of( space );
    String::size_type end   = victim.find_last_not_of(  space );

    if ( begin == String::npos )
      begin = 0;

    if ( end == String::npos )
      end = victim.size();
    else
      ++end;

    return victim.substr( begin, end - begin );
  }


  int toInt( const String & str, bool isNull )
  {
    if ( isNull )
      return 0;

    return atoi( str.c_str() );
  }


  bool toBoolean ( const String & str, bool isNull )
  {
    return toInt( str, isNull ) != 0;
  }

  long long toLong( const String & str, bool isNull )
  {
    if (isNull)
      return 0L;

    return atoll(str.c_str());
  }


  float toFloat( const String & str, bool isNull )
  {
    return static_cast<float>( toDouble( str.c_str() ) );
  }


  double toDouble( const String & str, bool isNull )
  {
    if (isNull)
      return 0.0;

    return atof(str.c_str());
  }


  const String &  defaultIfEmpty  ( const String & str, const String & defStr )
  {
    if ( str.empty() )
      return defStr;
    else
      return str;
  }

  String toHexString( char c, bool leading0x )
  {
    String result;

    if ( leading0x )
      result= "0x";

    char buf[3];

    sprintf( buf, "%02x", c & 0xff );

    result.append( buf );

    return result;
  }

  int toHexString( String & buff, char c[], int count, bool leading0x )
  {
    while( --count >= 0 )
    {
      buff+= toHexString( *c, leading0x );

      if ( count )
      {
        buff+= " ";
        ++c;
      }
    }

    return buff.size();
  }

  String & concatSeparated( String & to, const String & add, const String & separator )
  {
    if ( to.length() > 0 && add.length() > 0 )
      to+= separator;

    to+= add;

    return to;
  }

}
