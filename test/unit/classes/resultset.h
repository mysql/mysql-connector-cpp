/*
 Copyright 2008 - 2009 Sun Microsystems, Inc.

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
