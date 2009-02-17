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

#include <stdio.h>

#include "test_listener.h"
#include "test_timer.h"

namespace testsuite
{

TestsListener::TestsListener()
: curSuiteName  ( "n/a" )
, curTestName   ( "n/a" )
, curTestOrdNum ( 0     )
, executed      ( 0     )
, exceptions    ( 0     )
, verbose       ( false )
, timing        ( false )
{
  //TODO: Make StartOptions  dependent
  outputter.reset(new TAP());
}


void TestsListener::setVerbose(bool verbosity)
{
  theInstance().verbose = verbosity;
}


void  TestsListener::doTiming( bool timing )
{
  theInstance().timing= timing;
}

//TODO: "set" counterparts

std::iostream & TestsListener::errorsLog()
{
  return log;
}


void TestsListener::errorsLog(const String::value_type * msg)
{
  if (msg != NULL)
    log << msg  << std::endl;
}


void TestsListener::errorsLog(const String & msg)
{
  log << msg << std::endl;
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


void TestsListener::messagesLog(const String & msg)
{
  log << msg << std::endl;
  if (verbose)
    theInstance().outputter->Comment(msg);
}


void TestsListener::currentTestName(const String & name)
{
  theInstance().curTestName=name;
}


String TestsListener::TestFullName()
{
  return theInstance().curSuiteName + "::" + theInstance().curTestName;
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

  if ( theInstance().timing )
  {
    Timer::startTimer( TestFullName() );
  }

}


void TestsListener::TestHasFinished( TestRunResult result, const String & msg )
{
  static String timingResult("");

  if ( theInstance().timing )
  {
    clock_t time= Timer::stopTimer( TestFullName() );

    static std::stringstream tmp;

    tmp.str("");
    tmp << time << "(";
    tmp.precision( 7 );
    tmp << Timer::translate2seconds( time ) << "s)";

    timingResult= "Time:";
    timingResult+= tmp.str();    
  }
  else
    timingResult= "";

  StringUtils::concatSeparated( theInstance().executionComment, msg );
  StringUtils::concatSeparated( theInstance().executionComment, timingResult
    , " #" );

  if ( result != trrPassed )
  {
    // Output about test fail and recording info

    theInstance().recordFailed();

    if (result == trrThrown )
      ++theInstance().exceptions;

    theInstance().outputter->TestFailed(theInstance().curTestOrdNum
      , theInstance().curTestName
      ,  theInstance().executionComment.c_str());
  }
  else
  {
    // Output about test success
    theInstance().outputter->TestPassed(theInstance().curTestOrdNum
      , theInstance().curTestName
      , theInstance().executionComment.c_str());
  }

/*
  //log messsages from teardown goes after next test 
  if (theInstance().verbose)
    dumpLog();
  else
    // Just clearing memory
    theInstance().log.str("");
 */
}


void TestsListener::setTestExecutionComment ( const String & msg )
{
  theInstance().executionComment= msg;
}


void TestsListener::testHasFailed(const String & msg)
{
  setTestExecutionComment( msg );

  theInstance().errorsLog(msg.c_str());

  throw TestFailedException();
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
  static const String bail("BAIL ");
  theInstance().outputter->Comment(bail + reason);
}

void TestsListener::dumpLog()
{
  if ( theInstance().log.str().length() > 0 )
  {
    std::cerr << "---------------- "
    << theInstance().curTestName << " starts. " << "----------------" << std::endl;
  }

  std::cerr << theInstance().log.str();

  if (theInstance().log.str().length() > 0 )
  {
    std::cerr <<  "---------------- "
    << theInstance().curTestName << " ends. " << "----------------" << std::endl;
  }

  theInstance().log.str("");
}

}
