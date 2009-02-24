/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.

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


namespace testsuite
{

TestsRunner::TestsRunner()
: startOptions(NULL)
{

}

bool TestsRunner::runTests()
{
  TestSuiteNames.empty();

  TestSuiteFactory::theInstance().getTestsList( TestSuiteNames );

  TestsListener::setVerbose(startOptions->verbose);
  TestsListener::doTiming( startOptions->timer );

  for ( constStrListCit cit= TestSuiteNames.begin(); cit != TestSuiteNames.end(); ++cit )
  {
    Test * ts= TestSuiteFactory::theInstance().createTest( *cit );

    ts->runTest();
  }

  TestsListener::theInstance().summary();

  //bool result= TestSuiteFactory::theInstance().runTests();
/*

  if ( startOptions->verbose_summary )
  {
    TestsListener::dumpLog();
  }*/


  return TestsListener::allTestsPassed();
}

void TestsRunner::setStartOptions(StartOptions * options)
{
  startOptions=options;
}

StartOptions * TestsRunner::getStartOptions() const
{
  return startOptions;
}
}
