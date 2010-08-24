/*
  Copyright (c) 2009, 2010, Oracle and/or its affiliates. All rights reserved.

  The MySQL Connector/C++ is licensed under the terms of the GPLv2
  <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
  MySQL Connectors. There are special exceptions to the terms and
  conditions of the GPLv2 as it is applied to this software, see the
  FLOSS License Exception
  <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
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

protected:

  static int TestsWillRunCount( const String & suiteName, const testsList & tl );

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
