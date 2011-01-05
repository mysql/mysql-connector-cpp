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
 * Tests regressions w/ the Escape Processor code.
 *
 * @version $Id:$
 *
 */

namespace testsuite
{
namespace regression
{

class EscapeProcessorRegressionTest : public BaseTestFixture
{
private:
  typedef BaseTestFixture super;
protected:
public:

  TEST_FIXTURE(EscapeProcessorRegressionTest)
  {
    TEST_CASE(testBug11797);
    TEST_CASE(testBug11498);
    TEST_CASE(testBug14909);
    TEST_CASE(testBug25399);
  }


  /**
   * Tests fix for BUG#11797 - Escape tokenizer doesn't respect stacked single
   * quotes for escapes.
   *
   * @throws Exception
   *             if the test fails.
   */

  /* throws Exception */

  void testBug11797();
  /**
   * Tests fix for BUG#11498 - Escape processor didn't honor strings
   * demarcated with double quotes.
   *
   * @throws Exception
   *             if the test fails.
   */

  /* throws Exception */

  void testBug11498();
  /**
   * Tests fix for BUG#14909 - escape processor replaces quote character in
   * quoted string with string delimiter.
   *
   * @throws Exception
   */

  /* throws Exception */

  void testBug14909();
  /**
   * Tests fix for BUG#25399 - EscapeProcessor gets confused by multiple backslashes
   *
   * @throws Exception if the test fails.
   */

  /* throws Exception */

  void testBug25399();
};

REGISTER_FIXTURE(EscapeProcessorRegressionTest);
}
}
