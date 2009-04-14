/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
