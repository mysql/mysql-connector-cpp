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

#include "test_suite.h"

namespace testsuite
{

const String & TestSuite::name() const
{
  return suiteName;
};

TestSuite::TestSuite(const String& name)
: suiteName(name)
{
}

void TestSuite::RegisterTestCase(Test * test)
{
  testCases.push_back(test);
}

/** calls each test after setUp and tearDown TestFixture methods */
void TestSuite::runTest()
{
  TestsListener::nextSuiteStarts(suiteName, testCases.size());

  std::vector<Test*>::iterator it;

  for (it=testCases.begin(); it != testCases.end(); ++it)
  {
    //Incrementing order number of current test
    TestsListener::theInstance().incrementCounter();

    try
    {
      setUp();

      TestsListener::testHasRun();
      TestsListener::currentTestName((*it)->name());

      (*it)->runTest();

      TestsListener::testHasPassed();

      tearDown();
    }    // TODO: move interpretation of exception to TestSuite descendants
    // framework shouldn't know about sql::* exceptions
    catch (sql::MethodNotImplementedException & sqlni)
    {
      String msg("SKIP "); // or should it be TODO
      msg=msg + " relies on method " + sqlni.what()
              + ", which is not implemented at the moment.";

      TestsListener::testHasPassedWithInfo(msg);

    }    catch (std::exception & e)
    {
      TestsListener::theInstance().testHasThrown();
      TestsListener::theInstance().errorsLog()
              << "Not trapped exception occurred while running (probably while setUp"\
        "or tearDown):"
              << (*it)->name() << ". Message: " << e.what()
              << std::endl;
    }    catch (...)
    {
      TestsListener::theInstance().testHasThrown();
      TestsListener::theInstance().errorsLog()
              << "Not trapped exception occurred while running:"
              << (*it)->name() << std::endl;
    }

    // TODO: check why did i add it and is it still needed.
    TestsListener::theInstance().currentTestName("n/a");
  }
}

TestSuite::~TestSuite()
{
  std::vector<Test*>::iterator it;

  for (it=testCases.begin(); it != testCases.end(); ++it)
    delete (*it);
}
}


