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

#include "test_suite.h"
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

/** calls each test after setUp and tearDown TestFixture methods */
void TestSuite::runTest()
{
  TestsListener::nextSuiteStarts( suiteName, testCases.size() );

  for ( testsList_it it=testCases.begin(); it != testCases.end(); ++it)
  {
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

      TestsListener::incrementCounter( testCases.size() - ( it - testCases.begin() + 1 ) );

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


