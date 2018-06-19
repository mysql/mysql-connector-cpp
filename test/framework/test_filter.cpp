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




#include "test_filter.h"

namespace testsuite
{

  static const char * wildCardCharacter= "*";
  // We suppose we receive trimmed string here
  SingleFilter::SingleFilter( const String & filterStr
                            , const String & NOTsymbol /*= "!"*/ )
  {
    String filterString;

    if ( filterStr.find( NOTsymbol ) == 0 )
    {
      negative=     true;
      filterString= filterStr.substr( NOTsymbol.length() ).c_str();
    }
    else
    {
      negative=     false;
      filterString= filterStr;
    }

    StringUtils::trim( filterString );
    StringUtils::split( staticPart, filterString, wildCardCharacter, true, true );
  }


  bool SingleFilter::Admits( const String & testName ) const
  {
    const ciString test2filter( testName.c_str() );

    ciString::size_type   searchStartPos= 0;
    ciString::size_type   partPosition=   0;
    const List::size_type parts=          staticPart.size();
    bool            meetPrevEndExactly=   true;

    bool                  result=         true;

    for ( List::size_type i= 0; i < parts; ++i )
    {
      const String & part= staticPart[ i ];

      // empty staticPart means wildcard at the begin or end of filter string
      // empty parts in the middle (can be caused by two successive wild cards) ignored
      if ( part.empty() )
      {
        meetPrevEndExactly= false;
        continue;
      }
      else
      {
        partPosition= test2filter.find( part.c_str(), searchStartPos );

        if ( partPosition == ciString::npos
          || ( meetPrevEndExactly && partPosition != searchStartPos ) )
        {
          result= false;
          break;
        }

        searchStartPos= partPosition + part.size();
        meetPrevEndExactly= true;
      }
    }

    // Last part wasn't wildcard and and didn't match the end of the tested string
    if ( result && meetPrevEndExactly && searchStartPos != test2filter.size() )
      result= false;

    return (negative ? ! result : result); // negative != result
  }


  /************************************************************************/
  /* SerialFilter methods                                                 */
  /************************************************************************/

  SerialFilter::SerialFilter( const String & filterString
                            , const String & ANDsymbol /*= "&&"*/
                            , const String & NOTsymbol /*= "!"*/)
  {
    List series;

    StringUtils::split( series, filterString, ANDsymbol );

    for ( List::const_iterator cit= series.begin(); cit != series.end(); ++cit )
    {
      // Skipping empty filters - they are always true
      if ( cit->length() > 0 )
        filter.push_back( new SingleFilter( *cit, NOTsymbol ) );
    }
  }


  SerialFilter::~SerialFilter()
  {
    for ( FiltersList::iterator it= filter.begin(); it != filter.end(); ++it )
    {
      delete *it;
    }
  }


  bool SerialFilter::Admits( const String & testName ) const
  {
    for ( FiltersList::const_iterator cit= filter.begin(); cit != filter.end();
      ++cit )
    {
      if ( ! (*cit)->Admits( testName ) )
        return false;
    }

    return true;
  }


  /************************************************************************/
  /* FiltersSuperposition  methods                                        */
  /************************************************************************/

  FiltersSuperposition::FiltersSuperposition( const String & filterString
                                            , const String & ORsymbol /*= "||" */
                                            , const String & ANDsymbol/*= "&&" */
                                            , const String & NOTsymbol/* = "!" */ )
  {
    List series;

    StringUtils::split( series, filterString, ORsymbol );

    for ( List::const_iterator cit= series.begin(); cit != series.end(); ++cit )
      filter.push_back( new SerialFilter( *cit, ANDsymbol, NOTsymbol ) );

  }

  FiltersSuperposition::~FiltersSuperposition()
  {
    for ( FiltersList::iterator it= filter.begin(); it != filter.end(); ++it )
    {
      delete *it;
    }
  }

  bool FiltersSuperposition::Admits( const String & testName ) const
  {
    for ( FiltersList::const_iterator cit= filter.begin(); cit != filter.end();
      ++cit )
    {
      if ( (*cit)->Admits( testName ) )
        return true;
    }

    //return true if filter is empty
    return filter.size() == 0;
  }

} // namespace testsuite
