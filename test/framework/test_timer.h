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

struct timer
{
  clock_t start;
  clock_t total_cpu;
  bool stopped;

  timer(clock_t _start, clock_t _total_cpu, bool _stopped) :
  start(_start),
  total_cpu(_total_cpu),
  stopped(_stopped)
  {
  }

  timer(clock_t _start) :
  start(_start),
  total_cpu(static_cast<clock_t> (0)),
  stopped(false)
  {
  }

  timer() :
  start(static_cast<clock_t> (0)),
  total_cpu(static_cast<clock_t> (0)),
  stopped(false)
  {
    this->start=clock();
  }

  timer(const timer & rhs) :
  start(rhs.start),
  total_cpu(rhs.total_cpu),
  stopped(rhs.stopped)
  {
  }

};

struct test_timer
{
  clock_t cpu;
  std::map<String, timer> timers;

  test_timer()
  {
    this->cpu=clock();
  }

  test_timer(const test_timer & rhs) :
  cpu(rhs.cpu),
  timers(rhs.timers)
  {
  }
};

class Timer : public policies::Singleton<Timer>
{
  CCPP_SINGLETON(Timer);

  std::map<String, test_timer> timeRecorder;
  String currentTest;

public:

  static clock_t startTest(const String & test);
  static clock_t stopTest(const String & test);

  static clock_t startTimer(const String & test, const String & name);
  static clock_t startTimer(const String & name);

  static clock_t stopTimer(const String & test, const String & name);
  static clock_t stopTimer(const String & name);

  static std::list<String> getNames();
  static std::list<String> getNames(const String & test);

  static clock_t getTime(const String & test, const String & name);
  static clock_t getTime(const String & test);

  static float getSeconds(const String & test, const String & name);
  static float getSeconds(const String & test);

  static float translate2seconds(clock_t);
};
}

#define TIMER_START(label) Timer::startTimer(#label);
#define TIMER_STOP(label) Timer::stopTimer(#label);

#endif  // ifndef __TEST_TIMER_H_
