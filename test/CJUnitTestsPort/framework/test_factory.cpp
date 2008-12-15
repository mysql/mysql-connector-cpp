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

#include "test_factory.h"

namespace testsuite
{

TestSuiteFactory::TestSuiteFactory()
{
}


bool TestSuiteFactory::runTests()
{
  std::map<const char *, TestSuiteCreator>::iterator it;

  for(it=testSuites.begin(); it!=testSuites.end(); it++)
  {
    Test* suite = it->second( it->first );
    suite->runTest();
    delete suite;
  }

  //String errors =  TestsListener::theInstance().log();

  TestsListener::theInstance().summary();

  if ( false )//&& errors!="")
    std::cout << "\n\nError Details:\n";// << errors;

  return TestsListener::theInstance().allTestsPassed();
}


int TestSuiteFactory::RegisterTestSuite( const String::value_type * name, TestSuiteCreator creator)
{
  TestSuiteFactory::theInstance().testSuites.insert( std::make_pair( name, creator ) );

  return TestSuiteFactory::theInstance().testSuites.size();
}

}   // namespace testsuite
