/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "../unit_fixture.h"

/**
 * Example of a collection of tests
 *
 */

namespace testsuite
{
namespace performance
{

class perf_statement : public unit_fixture
{
private:
  typedef unit_fixture super;

  /*
   * Helper method to check for forward only scrolling
   */
  void checkUnbufferedScrolling();

protected:
public:

  EXAMPLE_TEST_FIXTURE(perf_statement)
  {
    TEST_CASE(anonymousSelect);
  }

  /**
   * SELECT ' ' as string
   */
  void anonymousSelect();

};

REGISTER_FIXTURE(perf_statement);
} /* namespace classes */
} /* namespace testsuite */
