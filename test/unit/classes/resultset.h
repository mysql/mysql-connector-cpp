/*
Copyright (c) 2008, 2011, Oracle and/or its affiliates. All rights reserved.

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

protected:
#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  void doNotImplemented();
#endif

public:

  EXAMPLE_TEST_FIXTURE(resultset)
  {
    TEST_CASE(fetchBigint);
    TEST_CASE(fetchBitAsInt);
    TEST_CASE(getInt);
    TEST_CASE(getTypes);
    TEST_CASE(getResultSetType);
    TEST_CASE(getTypesMinorIssues);

#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
    TEST_CASE(notImplemented);
#endif
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


  /**
   * Test for resultset::getResultSetType()
   *
   * Test of the assorted methods to fetch data
   */
  void getResultSetType();


  /**
   * Test for resultset::get*()
   *
   * TODO - hack to temporarily seperate test failures from TODO for beta by having more fine grained test
   */
  void getTypesMinorIssues();

#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  /**
   * Calling methods which throw "not implemented" to detect API changes
   */
  void notImplemented();
#endif

  /**
   * Calling methods which throw "not implemented" to detect API changes
   */
  void fetchBigint();

  /**
   * Fetching BIT values as integers - edge cases
   */
  void fetchBitAsInt();


};

REGISTER_FIXTURE(resultset);
} /* namespace classes */
} /* namespace testsuite */
