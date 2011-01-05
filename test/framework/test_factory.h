/*
Copyright (c) 2008, 2011, Oracle and/or its affiliates. All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPLv2
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPLv2 as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/




#ifndef __TESTFACTORY_H_
#define __TESTFACTORY_H_

#include "../common/singleton.h"

#include "test_container.h"

namespace testsuite
{

typedef Test* (*TestSuiteCreator)(const String::value_type * name);

template <class SuiteClass>
Test * CreateTestCase(const String::value_type * name)
{
  return new SuiteClass(name);
}

class TestSuiteFactory : public policies::Singleton<TestSuiteFactory>, Private::TestContainer
{
private:
  // should be private/protected
  CCPP_SINGLETON(TestSuiteFactory);

  typedef std::map<const String::value_type *, std::pair<TestSuiteCreator,Private::TestContainer::StorableTest *> > TestSuites;

  TestSuites testSuites;

  ~TestSuiteFactory();

public:
          Test *  createTest        ( const String::value_type * name );

  static  int     RegisterTestSuite ( const String::value_type *  name
                                    , TestSuiteCreator            creator);

  List::size_type getTestsList      ( std::vector<const String::value_type *> & list ) const;
};

} // namespace testsuite

#endif  // __TESTFACTORY_H_
