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
  TestSuiteNames.clear();

  TestSuiteFactory::theInstance().getTestsList( TestSuiteNames );

  TestsListener::setVerbose(startOptions->getBool( "verbose") );
  TestsListener::doTiming(  startOptions->getBool( "timer"  ) );

  String dummy;

  for ( constStrListCit cit= TestSuiteNames.begin(); cit != TestSuiteNames.end(); ++cit )
  {
    dummy= *cit;
    dummy+= "::"; // to be caught by filters like "!SuiteName::*"

  if ( 1 ) //TestsWillRunCount( suiteName, testCases )( dummy ) )
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
