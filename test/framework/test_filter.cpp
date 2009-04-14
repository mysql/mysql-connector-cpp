/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */


#include "test_filter.h"

namespace testsuite
{

  static const char * wildCardCharacter= "*";
  // We suppose we receive trimmed dtring here
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
