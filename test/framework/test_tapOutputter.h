/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
