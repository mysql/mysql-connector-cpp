/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
