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



#ifndef __TESTLISTENER_H_
#define __TESTLISTENER_H_

#include <sstream>
#include <iostream>
#include <iomanip>
#include <list>
#include <vector>

#include <boost/scoped_ptr.hpp>

#include "start_options.h"
#include "test_tapOutputter.h"
#include <cppconn/exception.h>
#include "../common/stringutils.h"
#include "../common/singleton.h"

namespace testsuite
{

enum TestRunResult
{
  trrPassed = 0,
  trrFailed,
  trrThrown
};

class TestsListener : public policies::Singleton<TestsListener>
{
  CCPP_SINGLETON(TestsListener);

  boost::scoped_ptr<TestOutputter> outputter;

  String            curSuiteName;
  String            curTestName;
  unsigned          curTestOrdNum;

  std::ostringstream devNull;

  unsigned          executed;
  std::vector<int>  failedTests;
  // don't really need to count exceptions
  unsigned          exceptions;
  bool              verbose;
  bool              timing;

  String            executionComment;

public:

  static std::ostream & errorsLog();
  static void errorsLog (const String::value_type * msg);
  static void errorsLog ( const String::value_type * msg
                        , const String::value_type * file, int line);
  static void errorsLog (const String & msg);

  static std::ostream & messagesLog();
  static void messagesLog      (const String::value_type * msg);
  static void messagesLog      (const String & msg);

  static void  incrementCounter( int incrVal= 1);
  int   recordFailed    ();

  static void setVerbose( bool verbosity    );
  static bool doTiming  ( bool timing= true );

  inline int failed()
  {
    return static_cast<int>( failedTests.size() );
  }

  static        void      currentTestName   (const String & name);


  static const  String &  currentSuiteName  ();
  static        String    testFullName      ();

  static        void      nextSuiteStarts   (const String & name
                                            , int testsNumber);

  static        void      testHasStarted    ();
  static        void      testHasFinished   ( TestRunResult result
                                            , const String & msg = "" );

  static        void      testHasFailed     (const String & msg);

  /** This sets comment to the test which output along w/ test result
  (it will be output in silent(non-verbose) mode too)
  */
  static        void      setTestExecutionComment ( const String & msg );

  static        void      bailSuite         (const String & reason);

                void      summary           ();

  static        bool      allTestsPassed    ();
};

class TestFailedException
{
};

}

#endif
