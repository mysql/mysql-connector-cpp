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

clock_t Timer::startTimer(const String & name)
{
  std::map<String, timeRecorderEntry>::const_iterator cit=theInstance().timeRecorder.find(name);

  clock_t now=clock();
  if (cit == theInstance().timeRecorder.end())
  {
    timeRecorderEntry t = timeRecorderEntry(now);
    theInstance().timeRecorder[name] = t;
  }
  else
  {
    /* TODO: API semantics are somewhat unclear - not sure what is best */
    theInstance().timeRecorder[name].start=now;
    theInstance().timeRecorder[name].stopped=false;
  }

  return theInstance().timeRecorder[name].start;
}

clock_t Timer::stopTimer(const String & name)
{
  std::map<String, timeRecorderEntry>::const_iterator cit=theInstance().timeRecorder.find(name);

  if (cit == theInstance().timeRecorder.end())
    // unknown
    return static_cast<clock_t> (-1);

  if (theInstance().timeRecorder[name].stopped)
    // has been stopped before
    return static_cast<clock_t> (-1);

  clock_t runtime=clock() - theInstance().timeRecorder[name].start;
  theInstance().timeRecorder[name].stopped=true;
  theInstance().timeRecorder[name].total_cpu+=runtime;
  theInstance().timeRecorder[name].start=static_cast<clock_t> (0);

  return runtime;
}

clock_t Timer::getTime(const String & name)
{
  std::map<String, timeRecorderEntry>::const_iterator cit=theInstance().timeRecorder.find(name);

  if (cit != theInstance().timeRecorder.end())
    return theInstance().timeRecorder[name].total_cpu;

  return static_cast<clock_t> (-1);
}

std::list<String> Timer::getNames()
{
  static std::list<String> names;

  std::map<String, timeRecorderEntry>::const_iterator cit=theInstance().timeRecorder.begin();

  for (; cit != theInstance().timeRecorder.end(); ++cit)
  {
    names.push_back(cit->first);
  }

  return names;
}

float Timer::getSeconds(const String & name)
{
  return translate2seconds(getTime(name));
}

float Timer::translate2seconds(clock_t inWallClocks)
{
#ifdef CLOCKS_PER_SEC
  /* it looks like CLOCKS_PER_SEC should be defined on all platforms... just to feel safe.
  Maybe use sysconf(_SC_CLK_TCK) */

  return static_cast<float> (inWallClocks) / CLOCKS_PER_SEC;
#else
  return static_cast<float> (inWallClocks);
#endif
}
}

