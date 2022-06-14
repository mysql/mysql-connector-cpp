/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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
    fullName+=  (*cit)->get()->name();

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
    fullName+=  (*it)->get()->name();

    if ( ! TestsRunner::Admits( fullName ) )
    {
      // TODO: Add skipping by filter condition message
      continue;
    }

    //Incrementing order number of current test
    TestsListener::incrementCounter();

    TestsListener::currentTestName( (*it)->get()->name() );

    try
    {
      setUp();
    }
    catch ( std::exception & e )
    {
      TestsListener::bailSuite(
        String( "An exception occurred while running setUp before " )
        + (*it)->get()->name() + ". Message: " + e.what() + ". Skipping all tests in the suite" );

      TestsListener::testHasFinished( trrThrown, "Test setup has failed, all tests in the suite will be skipped" );

      TestsListener::incrementCounter( static_cast<int>(testCases.size()
                                          - ( it - testCases.begin() + 1 )) );

      break;
    }

    TestRunResult result=   trrPassed;

    try
    {
      TestsListener::testHasStarted();

      (*it)->get()->runTest();
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

      msg+= (*it)->get()->name();
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
        << (*it)->get()->name() << std::endl;
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
        << (*it)->get()->name() << ". Message: " << e.what()
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


