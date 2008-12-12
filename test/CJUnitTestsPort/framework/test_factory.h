/* Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.

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


#ifndef __TESTFACTORY_H_
#define __TESTFACTORY_H_

#include "../common/singleton.h"

#include "test_suite.h"

namespace testsuite
{

typedef Test* (*TestSuiteCreator)( const String::value_type * name );

template <class SuiteClass>
Test * CreateTestCase( const String::value_type * name )
{
  return new SuiteClass( name );
}

class TestSuiteFactory : public policies::Singleton<TestSuiteFactory>
{
private:
  // should be private/protected
  CCPP_SINGLETON( TestSuiteFactory );

  typedef std::map<const String::value_type *, TestSuiteCreator> TestSuiteCreators;

	TestSuiteCreators testSuites;

public:

	bool        runTests();

  static int RegisterTestSuite( const String::value_type * name, TestSuiteCreator creator);
};

}   // namespace testsuite

#endif  // __TESTFACTORY_H_

