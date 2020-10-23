/*
 * Copyright (c) 2008, 2020, Oracle and/or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */



#include <stdio.h>

#include "test_listener.h"
#include "test_timer.h"

namespace testsuite
{

TestsListener::TestsListener()
: curSuiteName("n/a")
, curTestName("n/a")
, curTestOrdNum(0)
, executed(0)
, exceptions(0)
, verbose(false)
, timing(false)
{
  //TODO: Make StartOptions  dependent
  outputter.reset(new TAP());
}

void TestsListener::setVerbose(bool verbosity)
{
  theInstance().verbose=verbosity;
}

bool TestsListener::doTiming(bool timing)
{
  bool preserve= theInstance().timing;

  theInstance().timing=timing;

  return preserve;
}

//TODO: "set" counterparts

std::ostream & TestsListener::errorsLog()
{
  if (theInstance().verbose)
    return theInstance().outputter->errorsLog();
  else
  {
    theInstance().devNull.str("");
    return theInstance().devNull;
  }
}

void TestsListener::errorsLog(const String::value_type * msg)
{
  if (msg != NULL)
    errorsLog() << msg << std::endl;
}

void TestsListener::errorsLog(const String & msg)
{
  errorsLog() << msg << std::endl;
}

void TestsListener::errorsLog(const String::value_type * msg
                              , const String::value_type * file
                              , int line)
{
  if (msg != NULL)
  {
    errorsLog() << msg << " File: " << file << " Line: " << line << std::endl;
  }
}

std::ostream & TestsListener::messagesLog()
{
  if (theInstance().verbose)
    return theInstance().outputter->messagesLog();
  else
  {
    theInstance().devNull.str("");
    return theInstance().devNull;
  }
}

void TestsListener::messagesLog(const String::value_type * msg)
{
  if (msg != NULL)
    messagesLog() << msg;
}

void TestsListener::messagesLog(const String & msg)
{
  if (theInstance().verbose)
    messagesLog() << msg;
}

void TestsListener::currentTestName(const String & name)
{
  theInstance().curTestName=name;
}

const String & TestsListener::currentSuiteName()
{
  return theInstance().curSuiteName;
}

String TestsListener::testFullName()
{
  return theInstance().curSuiteName + "::" + theInstance().curTestName;
}

void TestsListener::incrementCounter(int incrVal)
{
  theInstance().curTestOrdNum+=incrVal;
}

int TestsListener::recordFailed()
{
  failedTests.push_back(curTestOrdNum);
  return static_cast<int> (failedTests.size());
}

void TestsListener::nextSuiteStarts(const String & name, int testsNumber)
{
  /*
  if ( name.length() > 0 )
      theInstance().messagesLog()
        << "=============== " << name << " ends. " << "==============="
        << std::endl;*/



  theInstance().curSuiteName=name;

  /*
  theInstance().messagesLog()
      << "=============== " << name << " starts. " << "==============="
      << std::endl;*/


  theInstance().outputter->SuiteHeader(name, theInstance().curTestOrdNum + 1
                                       , testsNumber);
}

void TestsListener::testHasStarted()
{
  //std::cout << ".";
  ++theInstance().executed;
  theInstance().executionComment= "";

  if (theInstance().timing)
  {
    Timer::startTest(testFullName());
  }

}

void TestsListener::testHasFinished(TestRunResult result, const String & msg)
{
  static String timingResult("");

  if (theInstance().timing)
  {

    clock_t time=Timer::stopTest(testFullName());
    float total = Timer::translate2seconds(time);

    static std::stringstream tmp;

    tmp.precision(10);
    tmp.str("");
    tmp << std::showpoint;
    tmp << std::endl << "# " << std::setw(40) << std::left << "Total" << " = ";
    tmp << std::setw(13) << std::right << total << "s";
    tmp << " (100.00%)" << std::endl;

    const List & names=Timer::getNames(testFullName());
    ConstIterator it=names.begin();
    for (; it != names.end(); ++it)
    {
      time=Timer::getTime(*it);
      tmp << "#   " << std::setw(38) << std::left << *it;
      tmp << " = " << std::setw(13) << std::right << Timer::translate2seconds(time) << "s";

      tmp << " (";
      tmp << std::setw(6) << std::right;
      if (total > 0.0) {
        tmp.precision(5);
        tmp << ((100 / total) * Timer::translate2seconds(time));
        tmp.precision(10);
      } else {
        tmp << "n/a ";
      }
      tmp << "%)";

      tmp << " (line " << Timer::getLine(*it) << ")" << std::endl;
    }

    timingResult=tmp.str();
  }
  else
    timingResult="";

  if (!msg.empty())
    StringUtils::concatSeparated(theInstance().executionComment, msg);
  if (!timingResult.empty())
    StringUtils::concatSeparated(theInstance().executionComment, timingResult);

  if (result != trrPassed)
  {
    // Output about test fail and recording info

    theInstance().recordFailed();

    if (result == trrThrown)
      ++theInstance().exceptions;

    theInstance().outputter->TestFailed(theInstance().curTestOrdNum
                                        , theInstance().curTestName
                                        , theInstance().executionComment);
  }
  else
  {
    // Output about test success
    theInstance().outputter->TestPassed(theInstance().curTestOrdNum
                                        , theInstance().curTestName
                                        , theInstance().executionComment);
  }
}

void TestsListener::setTestExecutionComment(const String & msg)
{
  theInstance().executionComment= msg;
}

void TestsListener::testHasFailed(const String & msg)
{
  setTestExecutionComment(msg);

  errorsLog(msg);

  throw TestFailedException();
}

void TestsListener::summary()
{
  outputter->Summary(executed
                     , failed() /*+ exceptions - exceptions are already counted in failed()*/
                     , failedTests);
}

bool TestsListener::allTestsPassed()
{
  return (theInstance().exceptions == 0) && (theInstance().failed() == 0);
}

void TestsListener::bailSuite(const String & reason)
{
  static const String bail("BAIL ");
  theInstance().outputter->Comment(bail + reason);
}

}
