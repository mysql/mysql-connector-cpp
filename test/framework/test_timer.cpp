/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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

