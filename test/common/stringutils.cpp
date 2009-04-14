/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

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
  template <class StdStringType>
  StdStringType & _trim( StdStringType & victim )
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

    return victim.assign( victim, begin, end - begin );
  }


  template <class StdStringType>
  unsigned _split( std::vector<StdStringType> & list, const StdStringType & str, const StdStringType & delim
    , bool trimItems, bool includeEmpty )
  {
    unsigned int      count=      0;
    String::size_type prevPos=    0;
    String::size_type newPos=     0;
    String::size_type delimSize=  delim.size();
    String::size_type strSize=    str.size();

    StdStringType item;

    while ( prevPos < strSize && (newPos = str.find(delim, prevPos)) != String::npos )
    {
      if ( newPos == prevPos && delimSize == 0 )
        ++newPos;

      if ( newPos > prevPos || includeEmpty )
      {
        item = str.substr(prevPos, newPos - prevPos );

        if ( trimItems )
          _trim<StdStringType>( item );

        list.push_back( item );
        ++count;
      }

      prevPos = newPos + delimSize;
    }

    /* if string has delimiter at the end, and includeEmpty flag is set
    - we are doing that if delimiter is present (delimSize > 0) */
    if ( prevPos == strSize && includeEmpty && delimSize > 0 )
    {
      list.push_back( _T("") );
      ++count;
    }
    /* else - adding part after last token*/
    else if (prevPos < strSize )
    {
      item = str.substr(prevPos);

      if (trimItems)
        _trim<StdStringType>(item);

      list.push_back( item );
      ++count;
    }

    return count;
  }


  // This function skips empties
  unsigned  split( List & list, const String &  str, const String &  delim
    , bool trimItems, bool includeEmpty)
  {
    return _split<String>( list, str, delim, trimItems, includeEmpty );
  }


  // hack for split template
  //ciString trim ( const String & victim )

  unsigned  split( std::vector<ciString> & list, const ciString &  str, const ciString &  delim
    , bool trimItems, bool includeEmpty)
  {
    return _split<ciString>( list, str, delim, trimItems, includeEmpty );
  }


  String & trim( String & victim )
  {
    return _trim<String>( victim );
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

    return strtoll(str.c_str(), NULL, 10);
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

  String::size_type toHexString( String & buff, char c[], int count, bool leading0x )
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
