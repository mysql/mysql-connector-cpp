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
