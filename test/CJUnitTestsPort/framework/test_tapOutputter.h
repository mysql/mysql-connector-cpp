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

#ifndef __TEST_TAPOUTPUTTER_H_
#define __TEST_TAPOUTPUTTER_H_

#include "test_outputter.h"

namespace testsuite
{

class TAP : public TestOutputter
{
public:

  virtual ~TAP()
  {
  }

  virtual void Header(const String::value_type * text);

  virtual void SuiteHeader(const String & name
                           , unsigned first
                           , unsigned testsInSuite);

  virtual void TestPassed(unsigned ordNum
                          , const String & name
                          , const String::value_type * comment);

  virtual void TestFailed(unsigned ordNum
                          , const String & name
                          , const String::value_type * comment);

  virtual void Comment(const String & comment);

  virtual void Summary(unsigned testsRun
                       , unsigned testsFailed
                       , std::vector<int> failedTestsNum);

  virtual void SuiteSummary(const String & suiteName
                            , unsigned testsRun
                            , unsigned testsFailed);

  virtual void Assert(const String & expected
                      , const String & result
                      , const String & file
                      , int line);
};
}

#endif
