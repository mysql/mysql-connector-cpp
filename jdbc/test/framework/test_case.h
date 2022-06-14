/*
 * Copyright (c) 2008, 2019, Oracle and/or its affiliates. All rights reserved.
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



#ifndef __TESTCASE_H_
#define __TESTCASE_H_


#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <vector>

#include "../common/ccppTypes.h"
#include "../common/stringutils.h"

#include "test_container.h"

namespace testsuite
{
  _ABSTRACT class Test: virtual public Private::TestContainer
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
    ~TestCase(){}
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
