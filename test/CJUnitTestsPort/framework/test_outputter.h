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

#ifndef __TEST_OUTPUTTER_H_
#define __TEST_OUTPUTTER_H_

#include "../common/ccppTypes.h"

namespace testsuite
{
// TODO: Add listener state passing - verbose/trace/streams
// TODO: Add failed tests list passing

_ABSTRACT class TestOutputter
{
public:

  virtual ~TestOutputter()
  {
  }

  virtual void Header(const String::value_type * text=NULL) _PURE;

  virtual void SuiteHeader(const String & name
                           , unsigned first
                           , unsigned testsInSuite) _PURE;

  virtual void TestPassed(unsigned ordNum
                          , const String & name
                          , const String::value_type * comment=NULL) _PURE;

  virtual void TestFailed(unsigned ordNum
                          , const String & name
                          , const String::value_type * comment=NULL) _PURE;

  virtual void Comment(const String & comment) _PURE;

  virtual void Summary(unsigned testsRun
                       , unsigned testsFailed
                       // a bit TAP-bound... However listener can complete
                       // needed info
                       , std::vector<int> failedTestsNum) _PURE;

  virtual void SuiteSummary(const String & suiteName
                            , unsigned testsRun
                            , unsigned testsFailed) _PURE;


  // Make string from anything shouldn't be a problem
  virtual void Assert(const String & expected
                      , const String & result
                      , const String & file
                      , int line) _PURE;
};
}

#endif
