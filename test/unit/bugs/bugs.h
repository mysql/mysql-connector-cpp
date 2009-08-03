/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#ifndef __BUGS_H_
#define __BUGS_H_

#include "../unit_fixture.h"

/**
 * Test cases for unsorted bugs (expecting that later bugs in different units
 * will be sorted to separate test suites)
 */

namespace testsuite
{
namespace regression
{

class bugs : public unit_fixture
{
private:
  typedef unit_fixture super;
protected:
public:

  EXAMPLE_TEST_FIXTURE(bugs)
  {
    TEST_CASE(net_write_timeout39878);
  }

  /**
   * http://bugs.mysql.com/39878
   *
   * bug report is against c/odbc, but problem is common for (almost?) all connectors
   * if we have pause between fetching rows lonher than net_write_timeout,
   * we won't receive all rows, and no error occures.
   */
  void net_write_timeout39878();
};

REGISTER_FIXTURE(bugs);
} /* namespace regression */
} /* namespace testsuite */

#endif
