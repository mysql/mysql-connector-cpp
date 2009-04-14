/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "test_runner.h"
#include "test_factory.h"
#include "test_case.h"
#include "test_listener.h"
#include "test_filter.h"


namespace testsuite
{

TestsRunner::TestsRunner()
  : startOptions( NULL  )
  , filter      ( NULL  )
{
}


bool TestsRunner::runTests()
{
  TestSuiteNames.empty();

  TestSuiteFactory::theInstance().getTestsList( TestSuiteNames );

  TestsListener::setVerbose(startOptions->getBool( "verbose") );
  TestsListener::doTiming(  startOptions->getBool( "timer"  ) );

  String dummy;

  for ( constStrListCit cit= TestSuiteNames.begin(); cit != TestSuiteNames.end(); ++cit )
  {
    dummy= *cit;
    dummy+= "::"; // to be caught by filters like "!SuiteName::*"

    if ( Admits( dummy ) )
    {
      Test * ts= TestSuiteFactory::theInstance().createTest( *cit );
      ts->runTest();
    }
    //else TODO: Add skipping by filter condition message
  }

  TestsListener::theInstance().summary();

  return TestsListener::allTestsPassed();
}


void TestsRunner::setStartOptions(StartOptions * options)
{
  startOptions=options;
}


void TestsRunner::setTestsFilter  ( Filter & _filter )
{
  filter= &_filter;
}


StartOptions * TestsRunner::getStartOptions()
{
  return theInstance().startOptions;
}


bool TestsRunner::Admits( const String & testName )
{
  if ( theInstance().filter != NULL )
    return theInstance().filter->Admits( testName );

  return true;
}

}
