/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#ifndef __FRAMEWORK_H
#define __FRAMEWORK_H

#include "test_case.h"
#include "test_suite.h"
#include "test_asserts.h"
#include "test_listener.h"
#include "test_runner.h"
#include "test_factory.h"


#define REGISTER_FIXTURE( theFixtureClass ) \
  \
  static int dummy##theFixtureClass = TestSuiteFactory::RegisterTestSuite( #theFixtureClass \
  , & CreateTestCase<theFixtureClass> )

// probably pretty much useless ifndef
#ifndef TEST_FIXTURE
#define TEST_FIXTURE( theFixtureClass ) typedef theFixtureClass TestSuiteClass;\
  theFixtureClass( const String & name )\
  : TestSuite( #theFixtureClass )
#endif

#define TEST_CASE( methodName ) \
  RegisterTestCase( new TestCase<TestSuiteClass>( *this, &TestSuiteClass::methodName, #methodName ) )

#define SKIP( message ) TestsListener::setTestExecutionComment( String("SKIP ") + message );\
  return

#define TODO( message ) TestsListener::setTestExecutionComment( String("TODO ") + message ); throw TestFailedException();

#endif  // __FRAMEWORK_H
