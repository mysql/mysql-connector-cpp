/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#ifndef __TESTLISTENER_H_
#define __TESTLISTENER_H_

#include <sstream>
#include <list>
#include <vector>

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

  std::auto_ptr<TestOutputter> outputter;

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
  static void doTiming  ( bool timing= true );

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
