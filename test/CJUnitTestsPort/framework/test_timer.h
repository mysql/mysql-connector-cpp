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
