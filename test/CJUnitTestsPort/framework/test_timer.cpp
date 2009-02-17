/* Copyright 2009 Sun Microsystems, Inc.

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

#include "test_timer.h"


namespace testsuite
{
  Timer::Timer()
  {

  }


  clock_t Timer::startTimer  ( const String & name )
  {
    // If timer started 2nd time - we don't really care.

    return theInstance().timeRecorder[ name ]= clock();
  }


  clock_t Timer::stopTimer    ( const String & name )
  {
    std::map<String, clock_t>::const_iterator cit= theInstance().timeRecorder.find( name );

    clock_t result= clock();

    if ( cit != theInstance().timeRecorder.end() )
      result -= cit->second;

    return result;
  }


  clock_t Timer::getTime    ( const String & name )
  {
    std::map<String, clock_t>::const_iterator cit= theInstance().timeRecorder.find( name );

    if ( cit != theInstance().timeRecorder.end() )
      return cit->second;

    return -1;
  }


  float   Timer::getSeconds  ( const String & name )
  {
    return translate2seconds( getTime( name ) );
  }

  float   Timer::translate2seconds( clock_t inWallClocks )
  {
#ifdef CLOCKS_PER_SEC
  //it looks like CLOCKS_PER_SEC should be defined on all platforms... just to feel safe
    return static_cast<float>(inWallClocks)/CLOCKS_PER_SEC;
#else
    return static_cast<float>(inWallClocks);
#endif
  }
}

