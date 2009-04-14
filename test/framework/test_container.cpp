/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "test_case.h"
#include "test_container.h"

#include "../common/ccppTypes.h"


namespace testsuite
{
  namespace Private
  {
    TestContainer::StorableTest::~StorableTest()
    {
      delete test;
    }


    TestContainer::StorableTest::StorableTest( Test & test2decorate )
    {
       test= &test2decorate;
    }

    void TestContainer::StorableTest::runTest()
    {
      test->runTest();
    }


    const String &  TestContainer::StorableTest::name() const
    {
      return test->name();
    }


    /*Test & TestContainer::StorableTest::operator * ()
    {
      return *test;
    }


    Test * TestContainer::StorableTest::operator ->()
    {
      return get();
    }*/



    Test * TestContainer::StorableTest::get()
    {
      //assert(test);
      return test;
    }
  }
}
