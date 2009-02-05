/* Copyright 2008 - 2009 Sun Microsystems, Inc.

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

#include "test_listener.h"
#include <stdio.h>

namespace testsuite
{

TestsListener::TestsListener()
: curSuiteName("n/a")
, curTestName("n/a")
, curTestOrdNum(0)
, executed(0)
, exceptions(0)
{
  //TODO: Make StartOptions  dependent
  outputter.reset(new TAP());
}


//TODO: "set" counterparts

std::iostream & TestsListener::errorsLog()
{
  return log;
}

void TestsListener::errorsLog(const String::value_type * msg)
{
  if (msg != NULL)
    log << msg;
}

void TestsListener::errorsLog(const String::value_type * msg
                              , const String::value_type * file
                              , int line)
{
  if (msg != NULL)
  {
    log << msg << " File: " << file << " Line: " << line << std::endl;
  }
}

std::iostream & TestsListener::messagesLog()
{
  return log;
}

void TestsListener::messagesLog(const String::value_type * msg)
{
  if (msg != NULL)
    log << msg;
}

void TestsListener::currentTestName(const String & name)
{
  theInstance().curTestName=name;
}

void TestsListener::incrementCounter()
{
  ++curTestOrdNum;
}

int TestsListener::recordFailed()
{
  failedTests.push_back(curTestOrdNum);
  return failedTests.size();
}

void TestsListener::nextSuiteStarts(const String & name, int testsNumber)
{
  theInstance().curSuiteName=name;

  theInstance().outputter->SuiteHeader(name, theInstance().curTestOrdNum + 1
                                       , testsNumber);
}

void TestsListener::testHasRun()
{
  //std::cout << ".";
  ++theInstance().executed;
}

void TestsListener::testHasFailed(const String & msg)
{
  theInstance().errorsLog(msg.c_str());

  theInstance().outputter->TestFailed(theInstance().curTestOrdNum, theInstance().curTestName, msg.c_str());
  theInstance().recordFailed();
  throw TestFailedException();
}

void TestsListener::testHasThrown(const String::value_type * msg)
{
  theInstance().errorsLog(msg);

  theInstance().outputter->TestFailed(theInstance().curTestOrdNum, theInstance().curTestName);
  theInstance().recordFailed();
  ++theInstance().exceptions;
}

void TestsListener::testHasPassed()
{
  theInstance().outputter->TestPassed(theInstance().curTestOrdNum
                                      , theInstance().curTestName);
}

void TestsListener::testHasPassedWithInfo(const String & str)
{
  theInstance().outputter->TestPassed(theInstance().curTestOrdNum
                                      , theInstance().curTestName, str.c_str());
}

void TestsListener::summary()
{
  outputter->Summary(executed
                     , failed() + exceptions
                     , failedTests);
}

bool TestsListener::allTestsPassed()
{
  return theInstance().exceptions && !theInstance().failed() == 0;
}

void TestsListener::bailSuite(const String & reason)
{
  static const String bail("#BAIL ");
  theInstance().outputter->Comment(bail + reason);
}

void TestsListener::dumpLog()
{
  std::cerr << theInstance().log.str();
}

}
