/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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



#ifndef __TEST_TAPOUTPUTTER_H_
#define __TEST_TAPOUTPUTTER_H_

#include "test_outputter.h"

namespace testsuite
{

class TAP : public TestOutputter
{
  String suiteName;

  std::ostream &  output;
  std::ostream &  msgLog;
  std::ostream &  errLog;

public:

  TAP();

  virtual ~TAP()
  {
  }

  virtual void Header(const String & text);

  virtual void SuiteHeader(const String & name
                           , unsigned     first
                           , int          testsInSuite);

  virtual void TestPassed(unsigned ordNum
                          , const String & name
                          , const String & comment);

  virtual void TestFailed(unsigned ordNum
                          , const String & name
                          , const String & comment);

  virtual void Comment(const String & comment);

  virtual void Summary(unsigned testsRun
                       , unsigned testsFailed
                       , std::vector<int> & failedTestsNum);

  virtual void SuiteSummary(const String & suiteName
                            , unsigned testsRun
                            , unsigned testsFailed);

  virtual void Assert(const String & expected
                      , const String & result
                      , const String & file
                      , int line);

  // Methods returning messages and error out streams.
  virtual std::ostream & messagesLog();
  virtual std::ostream & errorsLog();
};
}

#endif
