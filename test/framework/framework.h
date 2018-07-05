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



#ifndef __FRAMEWORK_H
#define __FRAMEWORK_H

#include "test_case.h"
#include "test_suite.h"
#include "test_asserts.h"
#include "test_listener.h"
#include "test_runner.h"
#include "test_factory.h"
#include "test_timer.h"

#ifdef _WIN32

#include <windows.h>
#define SLEEP(x) Sleep((x)*1000)

#else

#include <unistd.h>
#define SLEEP(x) sleep((x))

#endif

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

#define SKIP( message ) { TestsListener::setTestExecutionComment( String("SKIP ") + message );\
  return; }

#define TODO( message ) TestsListener::setTestExecutionComment( String("TODO ") + message ); throw TestFailedException();

#endif  // __FRAMEWORK_H
