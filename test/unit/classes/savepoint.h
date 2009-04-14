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
namespace classes
{

class savepoint : public unit_fixture
{
private:
  typedef unit_fixture super;

protected:
public:

  EXAMPLE_TEST_FIXTURE(savepoint)
  {
    TEST_CASE(getSavepointId);
    TEST_CASE(getSavepointName);
  }

  /**
   * Test savepoint::getSavepointId()
   *
   * Save points must have names at the time of writing
   */
  void getSavepointId();

  /**
   * Test savepoint::getSavepointName()
   *
   * Save points must have names at the time of writing
   */
  void getSavepointName();

};

REGISTER_FIXTURE(savepoint);
} /* namespace classes */
} /* namespace testsuite */
