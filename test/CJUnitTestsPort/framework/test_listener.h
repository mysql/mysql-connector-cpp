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

class TestsListener : public policies::Singleton<TestsListener>
{
  CCPP_SINGLETON(TestsListener);

  std::auto_ptr<TestOutputter>  outputter;

  String                        curSuiteName;
  String                        curTestName;
  unsigned                      curTestOrdNum;
  std::stringstream             log;
  unsigned                      executed;
  std::vector<int>              failedTests;
  // don't really need to count exceptions
  unsigned                      exceptions;

public:

  std::iostream & errorsLog   ();
  void            errorsLog   ( const String::value_type * msg );

  std::iostream & messagesLog ();
  void            messagesLog ( const String::value_type * msg );

  void            incrementCounter();
  int             recordFailed();

  inline int failed()
  {
    return failedTests.size();
  }

  static void currentTestName       ( const String & name );
  static void nextSuiteStarts       ( const String & name, int testsNumber  );

  static void testHasRun            ();

  static void testHasFailed         ( const String::value_type * msg = NULL );
  static void testHasThrown         ( const String::value_type * msg = NULL );

  static void testHasPassed         ();
  static void testHasPassedWithInfo ( const String & str    );

  static void bailSuite             ( const String & reason );

  static void dumpLog               ();

         void summary();

  static bool allTestsPassed();
};

class TestFailedException
{
};

}

#endif
