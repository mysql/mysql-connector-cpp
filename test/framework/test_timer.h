/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#ifndef __TEST_TIMER_H_
#define __TEST_TIMER_H_


#include <time.h>

#include "../common/ccppTypes.h"
#include "../common/singleton.h"


namespace testsuite
{
  class Timer : public policies::Singleton<Timer>
  {
    CCPP_SINGLETON( Timer );

    std::map<String, clock_t> timeRecorder;

  public:

    static clock_t startTimer  ( const String & name );
    static clock_t stopTimer   ( const String & name );

    static clock_t getTime     ( const String & name );
    static float   getSeconds  ( const String & name );

    static float   translate2seconds( clock_t );
  };
}


#endif  // ifndef __TEST_TIMER_H_
