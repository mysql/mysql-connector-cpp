/* Copyright 2008 - 2009 Sun Microsystems, Inc.

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
  std::stringstream log;
  unsigned          executed;
  std::vector<int>  failedTests;
  // don't really need to count exceptions
  unsigned          exceptions;
  bool              verbose;
  bool              timing;

  String            executionComment;

public:

  static std::iostream & errorsLog();
  static void errorsLog (const String::value_type * msg);
  static void errorsLog ( const String::value_type * msg
                        , const String::value_type * file, int line);
  static void errorsLog (const String & msg);

  std::iostream & messagesLog();
  void messagesLog      (const String::value_type * msg);
  void messagesLog      (const String & msg);

  static void  incrementCounter( int incrVal= 1);
  int   recordFailed    ();

  static void setVerbose( bool verbosity    );
  static void doTiming  ( bool timing= true ); 

  inline int failed()
  {
    return failedTests.size();
  }

  static void             currentTestName   (const String & name);


  static const String &   currentSuiteName  ();
  static String           testFullName      ();

  static void   nextSuiteStarts (const String & name, int testsNumber);

  static void   testHasStarted  ();
  static void   testHasFinished ( TestRunResult result, const String & msg = "" );

  static void   testHasFailed    (const String & msg);

  /** This sets comment to the test which output along w/ test result
  (it will be output in silent(non-verbose) mode too)
  */
  static void   setTestExecutionComment ( const String & msg );

  static void   bailSuite       (const String & reason);

  static void   dumpLog         ();

  void          summary         ();



  static bool   allTestsPassed  ();
};

class TestFailedException
{
};

}

#endif
