/*
Copyright (c) 2009, 2011, Oracle and/or its affiliates. All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPLv2
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPLv2 as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/



#include "test_timer.h"


namespace testsuite
{

Timer::Timer()
{

}

clock_t Timer::startTest(const String & test)
{
  std::map<String, test_timer>::const_iterator cit=theInstance().timeRecorder.find(test);

  if (cit == theInstance().timeRecorder.end())
  {
    theInstance().currentTest=test;
    test_timer t=test_timer();
    theInstance().timeRecorder[test]=t;
    return theInstance().timeRecorder[test].cpu;
  }

  return static_cast<clock_t> (-1);
}

clock_t Timer::stopTest(const String & test)
{
  std::map<String, test_timer>::const_iterator cit=theInstance().timeRecorder.find(test);

  if (cit != theInstance().timeRecorder.end())
  {
    clock_t now=clock();
    theInstance().timeRecorder[test].cpu=now - theInstance().timeRecorder[test].cpu;
    return theInstance().timeRecorder[test].cpu;
  }

  return static_cast<clock_t> (-1);
}

clock_t Timer::startTimer(const String & name, const String & file, const unsigned int line)
{
  // HACK
  return startTimer(theInstance().currentTest, name, file, line);
}

clock_t Timer::startTimer(const String & test, const String & name, const String & file, const unsigned int line)
{
  std::map<String, test_timer>::const_iterator cit=theInstance().timeRecorder.find(test);
  if (cit == theInstance().timeRecorder.end())
    // unknown test - must not happen
    return static_cast<clock_t> (-1);

  std::map<String, timer>::const_iterator it=theInstance().timeRecorder[test].timers.find(name);

  clock_t now=clock();
  if (it == theInstance().timeRecorder[test].timers.end())
  {
    timer t=timer(now, file, line);
    theInstance().timeRecorder[test].timers[name]=t;
  }
  else
  {
    /* TODO: API semantics are somewhat unclear - not sure what is best */
    theInstance().timeRecorder[test].timers[name].start=now;
    theInstance().timeRecorder[name].timers[name].stopped=false;
  }

  return theInstance().timeRecorder[name].timers[name].start;
}

clock_t Timer::stopTimer(const String & name)
{
  // hack
  return stopTimer(theInstance().currentTest, name);
}

clock_t Timer::stopTimer(const String & test, const String & name)
{
  std::map<String, test_timer>::const_iterator cit=theInstance().timeRecorder.find(test);
  if (cit == theInstance().timeRecorder.end())
    // unknown test - must not happen
    return static_cast<clock_t> (-1);

  std::map<String, timer>::const_iterator it=theInstance().timeRecorder[test].timers.find(name);

  if (it == theInstance().timeRecorder[test].timers.end())
    // unknown timer
    return static_cast<clock_t> (-1);

  if (theInstance().timeRecorder[test].timers[name].stopped)
    // has been stopped before
    return static_cast<clock_t> (-1);

  clock_t runtime=clock() - theInstance().timeRecorder[test].timers[name].start;
  theInstance().timeRecorder[test].timers[name].stopped=true;
  theInstance().timeRecorder[test].timers[name].total_cpu+=runtime;
  theInstance().timeRecorder[test].timers[name].start=static_cast<clock_t> (0);

  return runtime;
}

clock_t Timer::getTime(const String &name)
{
  return getTime(theInstance().currentTest, name);
}

clock_t Timer::getTime(const String & test, const String & name)
{
  std::map<String, test_timer>::const_iterator cit=theInstance().timeRecorder.find(test);
  if (cit == theInstance().timeRecorder.end())
    // unknown test - must not happen
    return static_cast<clock_t> (-1);

  std::map<String, timer>::const_iterator it=theInstance().timeRecorder[test].timers.find(name);

  if (it == theInstance().timeRecorder[test].timers.end())
    // unknown timer
    return static_cast<clock_t> (-1);


  return theInstance().timeRecorder[test].timers[name].total_cpu;
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

unsigned int Timer::getLine(const String &name)
{
  return getLine(theInstance().currentTest, name);
}

unsigned int Timer::getLine(const String & test, const String & name)
{
  std::map<String, test_timer>::const_iterator cit=theInstance().timeRecorder.find(test);
  if (cit == theInstance().timeRecorder.end())
    // unknown test - must not happen
    return 0;

  std::map<String, timer>::const_iterator it=theInstance().timeRecorder[test].timers.find(name);

  if (it == theInstance().timeRecorder[test].timers.end())
    // unknown timer
    return 0;

  return theInstance().timeRecorder[test].timers[name].line;
}

const String Timer::getFile(const String &name)
{
  return getFile(theInstance().currentTest, name);
}

const String Timer::getFile(const String & test, const String & name)
{
  std::map<String, test_timer>::const_iterator cit=theInstance().timeRecorder.find(test);
  if (cit == theInstance().timeRecorder.end())
    // unknown test - must not happen
    return 0;

  std::map<String, timer>::const_iterator it=theInstance().timeRecorder[test].timers.find(name);

  if (it == theInstance().timeRecorder[test].timers.end())
    // unknown timer
    return 0;

  return theInstance().timeRecorder[test].timers[name].file;
}

const List & Timer::getNames()
{
  static List names;

  std::map<String, test_timer>::const_iterator cit=theInstance().timeRecorder.find("perf_statement::anonymousSelect");
  std::map<String, timer>::const_iterator it;

  for (; cit != theInstance().timeRecorder.end(); ++cit)
  {
    for (it=theInstance().timeRecorder[cit->first].timers.begin(); it != theInstance().timeRecorder[cit->first].timers.end(); ++it)
    {
      names.push_back(it->first);
    }
  }

  return names;
}

const List & Timer::getNames(const String & test)
{
  static List names;

  std::map<String, test_timer>::const_iterator cit=theInstance().timeRecorder.find(test);
  if (cit == theInstance().timeRecorder.end())
  {
    return names;
  }
  std::map<String, timer>::const_iterator it;

  for (it=theInstance().timeRecorder[cit->first].timers.begin(); it != theInstance().timeRecorder[cit->first].timers.end(); ++it)
  {
    names.push_back(it->first);
  }

  return names;
}

}

