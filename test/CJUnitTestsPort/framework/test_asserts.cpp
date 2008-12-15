/* Copyright 2008 Sun Microsystems, Inc.

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

#include "test_asserts.h"
#include "test_listener.h"
#include "math.h"


namespace testsuite
{

void assertTrue( const char * msg, bool expression,
                const char* file, int line)
{
  if ( ! expression )
  {
    TestsListener::testHasFailed();
  }
}


void assertEquals( const char * expected, const char * result,
                  const char* file, int line )
{
  String s1( expected ), s2( result );

  assertEquals( s1, s2, file, line );
}


void assertEquals( bool expected, bool result,
                  const char* file, int line )
{
  assertTrue( NULL, expected == result, file, line );
}


void assertEquals ( int expected, int result
                   , const char* file, int line )
{
  assertTrue( NULL, expected == result, file, line );
}


bool isNaN( double smth )
{
  bool lessThanZero=    ( smth <  0.0 );
  bool moreOrEqualZero= ( smth >= 0.0 );

  return !( lessThanZero || moreOrEqualZero );
}


double scaledEpsilon( const double & expected, const double & fuzzyEpsilon )
{
  const double aa = fabs( expected ) + 1;

  return ( true ) ? fuzzyEpsilon : fuzzyEpsilon * aa;
}


bool fuzzyEquals( double expected, double result, double fuzzyEpsilon )
{
  return ( expected == result )
      || ( fabs( expected - result) <= scaledEpsilon( expected, fuzzyEpsilon ) );
}


void assertEquals( const double & expected, const double & result,
                   const char* file, int line )
{
  const double fuzzyEpsilon = 0.000001;

  assertEqualsEpsilon( expected, result, fuzzyEpsilon, file, line );
}


void assertEquals( const float & expected, const float & result,
                   const char * file, int line )
{
  assertEquals( static_cast<double>( expected ), static_cast<double>( result )
                , file, line);
}


void assertEquals( const long double & expected, const long double & result,
                   const char * file, int line )
{
  assertEquals( static_cast<double>( expected ), static_cast<double>( result )
              , file, line);
}


void assertEqualsEpsilon( const double & expected, const double & result
                        , const double & epsilon, const char * file, int line )
{
  if (  isNaN( expected ) && isNaN( result )
      ||
        ! isNaN(expected) && ! isNaN(result)
        && fuzzyEquals( expected, result, epsilon ) )
  {
    return;
  }


  TestsListener::testHasFailed();
}


void assertEquals( const String & expected, const String & result
                 , const char* file, int line )
{
  if( expected != result )
  {
    TestsListener::testHasFailed();
  }
}


void fail( const char* reason, const char* file, int line )
{
  // if( verbose...
  // TestsListener

  TestsListener::testHasFailed();
}

}
