/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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

std::ostream & TestsListener::errorsLog()
{
  if ( theInstance().verbose )
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
    errorsLog() << msg  << std::endl;
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
  if ( theInstance().verbose )
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
  if ( theInstance().verbose )
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


void TestsListener::incrementCounter( int incrVal )
{
  theInstance().curTestOrdNum+= incrVal;
}


int TestsListener::recordFailed()
{
  failedTests.push_back(curTestOrdNum);
  return static_cast<int>(failedTests.size());
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
    Timer::startTimer( testFullName() );
  }

}


void TestsListener::testHasFinished( TestRunResult result, const String & msg )
{
  static String timingResult("");

  if ( theInstance().timing )
  {
    clock_t time= Timer::stopTimer( testFullName() );

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
      ,  theInstance().executionComment );
  }
  else
  {
    // Output about test success
    theInstance().outputter->TestPassed(theInstance().curTestOrdNum
      , theInstance().curTestName
      , theInstance().executionComment );
  }
 }


void TestsListener::setTestExecutionComment ( const String & msg )
{
  theInstance().executionComment= msg;
}


void TestsListener::testHasFailed(const String & msg)
{
  setTestExecutionComment( msg );

  errorsLog( msg );

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

}
