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

#ifndef __TESTCASE_H_
#define __TESTCASE_H_


#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <vector>

#include "../common/ccppTypes.h"
#include "../common/stringutils.h"

_ABSTRACT class Test
{
public:

  virtual ~Test()
  {
  }
  virtual void runTest() _PURE;
  virtual const String & name() const _PURE;
};

template <class SuiteClass>
class TestCase : public Test
{
protected:
  typedef void (SuiteClass::*TestCaseMethod)();

private:
  SuiteClass * suite;
  TestCaseMethod test;
  String testName;

  TestCase();
  TestCase(const TestCase &other);

public:

  TestCase(   SuiteClass &    obj
           ,  TestCaseMethod  testMethod
           ,  const String &  _name)
      : Test      (),
        suite     ( &obj        ),
        test      ( testMethod  ),
        testName  ( _name       )
  {
  }

  virtual void runTest()
  {
    (suite->*test)();
  }

  virtual const String & name() const
  {
    return testName;
  }
};

#endif  // ifndef __TESTCASE_H_
