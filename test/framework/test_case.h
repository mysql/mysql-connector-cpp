/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#ifndef __TESTCASE_H_
#define __TESTCASE_H_


#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <vector>

#include "../common/ccppTypes.h"
#include "../common/stringutils.h"
#include "../common/nocopy.h"

#include "test_container.h"

namespace testsuite
{
  _ABSTRACT class Test: virtual private Private::TestContainer
  {
    friend class Private::TestContainer::StorableTest;

  protected:

    virtual ~Test()
    {
    }

  public:

    virtual       void      runTest ()        _PURE;
    virtual const String &  name    () const  _PURE;
  };


  template <class SuiteClass>
  class TestCase : public Test
  {
  protected:
    typedef void (SuiteClass::*TestCaseMethod)();

  private:
    SuiteClass *    suite;
    TestCaseMethod  aTest;
    String          testName;

    TestCase();
    TestCase(const TestCase &other);

  public:

    TestCase(   SuiteClass &    obj
            ,  TestCaseMethod  testMethod
            ,  const String &  _name)
      : Test      (),
      suite     ( &obj        ),
      aTest     ( testMethod  ),
      testName  ( _name       )
    {
    }

    virtual void runTest()
    {
      (suite->*aTest)();
    }

    virtual const String & name() const
    {
      return testName;
    }
  };
}

#endif  // ifndef __TESTCASE_H_
