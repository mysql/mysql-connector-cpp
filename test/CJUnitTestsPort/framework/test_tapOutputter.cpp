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

#include "test_tapOutputter.h"
#include <stdio.h>
#include <sstream>

namespace testsuite
{

void TAP::Header(const String::value_type * text)
{
}

void TAP::SuiteHeader(const String & name
                      , unsigned first
                      , unsigned testsInSuite)
{
  std::cout << std::endl << "# " << name << std::endl;
  std::cout << first << ".."
          << first + testsInSuite - 1 << std::endl;
}

void TAP::TestPassed(unsigned ordNum
                     , const String & name
                     , const String::value_type * comment)
{
  std::cout << "ok " << ordNum << " - " << name;

  if (comment != NULL)
  {
    std::cout << " # " << comment;
  }

  std::cout << std::endl;
}

void TAP::TestFailed(unsigned ordNum
                     , const String & name
                     , const String::value_type * comment)
{
  std::cout << "not ok " << ordNum
          << " - " << name;

  if (comment != NULL)
  {
    std::cout << " # " << comment;
  }

  std::cout << std::endl;
}

void TAP::Comment(const String & comment)
{
  std::cout << " # " << comment << std::endl;
}

void TAP::Summary(unsigned testsRun
                  , unsigned testsFailed
                  , std::vector<int> failedTestsNum)
{
  char percentage[7];

  if (testsRun)
    sprintf(percentage, "%3.2f"
            , static_cast<float> (testsRun - testsFailed)*100 / testsRun);
  else
    strcpy(percentage, "0.00");

  if (testsFailed > 0)
  {
    //TODO: move is string utils as "join" or smth
    std::vector<int>::const_iterator cit=failedTestsNum.begin();

    std::cout << std::endl << "FAILED tests " << *cit;

    while (++cit != failedTestsNum.end())
    {
      std::cout << ", " << *cit;
    }
  }

  std::cout << std::endl << "Failed " << testsFailed << "/" << testsRun
          << ", " << percentage << "% okay" << std::endl;

  std::cout;
}

void TAP::SuiteSummary(const String & suiteName
                       , unsigned testsRun
                       , unsigned testsFailed)
{

}

void TAP::Assert(const String & expected, const String & result
                 , const String & file, int line)
{
  std::ostringstream tmp("Assertion Failed in file ");

  tmp << file << " Line " << line;

  Comment(tmp.str());

  tmp.flush();
  tmp << "Expected: " << expected;

  Comment(tmp.str());

  tmp.flush();
  tmp << "Received: " << result;

  Comment(tmp.str());
}

}
