/* Copyright 2008 Sun Microsystems, Inc.

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
  , & CreateTestCase<theFixtureClass> );

#define TEST_FIXTURE( theFixtureClass ) typedef theFixtureClass TestSuiteClass;\
  theFixtureClass( const String & name )\
  : TestSuite( #theFixtureClass )

#define TEST_CASE( methodName ) \
  RegisterTestCase( new TestCase<TestSuiteClass>( *this, &TestSuiteClass::methodName, #methodName ) );

#endif  // __FRAMEWORK_H

