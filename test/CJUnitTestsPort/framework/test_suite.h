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

#ifndef __TESTSUITE_H_
#define __TESTSUITE_H_

#include <vector>

#include "../common/ccppTypes.h"
#include "../common/stringutils.h"

#include "start_options.h"

#include <cppconn/exception.h>

#include "test_case.h"
#include "test_listener.h"

namespace testsuite
{

  class TestSuite : public Test, virtual public Private::TestContainer
{
private:

  typedef std::vector<Private::TestContainer::StorableTest *> testsList;
  typedef testsList::iterator testsList_it;

  testsList testCases;
  String    suiteName;

public:
  TestSuite(const String& name="Unnamed Test Suite");
  ~TestSuite();

  virtual void setUp()
  {
  }

  virtual void tearDown()
  {
  }

  const String & name() const;

  virtual void RegisterTestCase(Test * test);

  void runTest();
};

} // namespace testsuite

#endif  // __TESTSUITE_H_
