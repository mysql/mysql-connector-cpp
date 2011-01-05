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



#include "../BaseTestFixture.h"

/**
 * @author
 */

namespace testsuite
{
namespace regression
{

class PreparedStatementRegressionTest : public BaseTestFixture
{
private:

  typedef BaseTestFixture super;

  DatabaseMetaData dbmd;

protected:

  /**
   * setUp() function for tests
   */
  /* throws std::exception * */
  void setUp();

public:

  TEST_FIXTURE(PreparedStatementRegressionTest)
  {
    TEST_CASE( testStmtClose      );
  }


  void testStmtClose();


};

REGISTER_FIXTURE(PreparedStatementRegressionTest);

} //namespace regression
} //namespace testsuite
