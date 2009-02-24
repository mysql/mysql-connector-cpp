/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "../unit_fixture.h"

/**
 * Test of the JDBC ResultSet class counterpart
 *
 */

namespace testsuite
{
namespace classes
{

class resultset : public unit_fixture
{
private:
  typedef unit_fixture super;

public:

  EXAMPLE_TEST_FIXTURE(resultset)
  {
    TEST_CASE(getInt);
    TEST_CASE(getTypes);
  }

  /**
   * Test for resultset::getInt*()
   *
   * Test of the assorted methods to fetch integers
   */
  void getInt();

  /**
   * Test for resultset::get*()
   *
   * Test of the assorted methods to fetch data
   */
  void getTypes();

};

REGISTER_FIXTURE(resultset);
} /* namespace classes */
} /* namespace testsuite */
