/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include "test_suite.h"
#include "test_runner.h"
//#include "deletable_wrapper.h"

namespace testsuite
{
const String & TestSuite::name() const
{
  return suiteName;
};


TestSuite::TestSuite( const String& name )
: suiteName( name )
{
}


void TestSuite::RegisterTestCase( Test * test )
{
  if ( test != NULL )
    testCases.push_back( new TestContainer::StorableTest( *test ) );
}


int TestSuite::TestsWillRunCount( const String & suiteName, const testsList & tl )
{
  int count= static_cast<int>( tl.size() );

  String fullName;

  for ( testsList::const_iterator cit=tl.begin(); cit != tl.end(); ++cit )
  {
    fullName=   suiteName;
    fullName+=  "::";
    fullName+=  (*cit)->name();

    if ( ! TestsRunner::Admits( fullName ) )
    {
      --count;
    }
  }

  return count;
}


/** calls each test after setUp and tearDown TestFixture methods */
void TestSuite::runTest()
{
  TestsListener::nextSuiteStarts( suiteName, TestsWillRunCount( suiteName, testCases ) );

  String fullName;

  for ( testsList_it it=testCases.begin(); it != testCases.end(); ++it)
  {
    fullName=   suiteName;
    fullName+=  "::";
    fullName+=  (*it)->name();

    if ( ! TestsRunner::Admits( fullName ) )
    {
      // TODO: Add skipping by filter condition message
      continue;
    }

    //Incrementing order number of current test
    TestsListener::incrementCounter();

    TestsListener::currentTestName( (*it)->name() );

    try
    {
      setUp();
    }
    catch ( std::exception & e )
    {
      TestsListener::bailSuite(
        String( "An exception occurred while running setUp before " )
        + (*it)->name() + ". Message: " + e.what() + ". Skipping all tests in the suite" );

      //not really needed probably
      //TestsListener::testHasFinished( trrThrown, "Test setup has failed, all tests in the suite will be skipped" );

      TestsListener::incrementCounter( static_cast<int>(testCases.size()
                                          - ( it - testCases.begin() + 1 )) );

      break;
    }

    TestRunResult result=   trrPassed;

    try
    {
      TestsListener::testHasStarted();

      (*it)->runTest();
    }
    // TODO: move interpretation of exception to TestSuite descendants
    // framework shouldn't know about sql::* exceptions
    catch ( sql::MethodNotImplementedException & sqlni )
    {
      String msg( "SKIP relies on method " ); // or should it be TODO
      msg= msg + sqlni.what()
        + ", which is not implemented at the moment.";

      TestsListener::setTestExecutionComment( msg );
    }
    catch ( std::exception & e )
    {
      result= trrThrown;

      String msg( "Standard exception occurred while running test: " );

      msg+= (*it)->name();
      msg+= ". Message: ";
      msg+= e.what();

      TestsListener::setTestExecutionComment( msg );
      TestsListener::errorsLog( msg );
    }
    catch ( TestFailedException &)
    {
      // Thrown by fail. Just used to stop test execution
      result= trrFailed;
    }
    catch (...)
    {
      result= trrThrown;
      TestsListener::errorsLog()
        << "Unknown exception occurred while running:"
        << (*it)->name() << std::endl;
    }

    TestsListener::testHasFinished( result );

    try
    {
      tearDown();
    }
    catch ( std::exception & e )
    {
      TestsListener::errorsLog()
        << "Not trapped exception occurred while running while tearDown after:"
        << (*it)->name() << ". Message: " << e.what()
        << std::endl;
    }

    // TODO: check why did i add it and is it still needed.
    //TestsListener::theInstance().currentTestName( "n/a" );
  }
}

TestSuite::~TestSuite()
{
  for ( testsList_it it=testCases.begin(); it != testCases.end(); ++it)
    delete (*it);
}
}


