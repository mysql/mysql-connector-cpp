/*
 Copyright 2008 Sun Microsystems, Inc.

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License as
 published by the Free Software Foundation.

 There are special exceptions to the terms and conditions of the GPL
 as it is applied to this software. View the full text of the
 exception in file EXCEPTIONS-CONNECTOR-J in the directory of this
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
 * Example of a collection of tests
 *
 */

namespace testsuite
{
namespace example
{

class example_test_class : public unit_fixture
{
private:
  typedef unit_fixture super;
protected:
public:

  EXAMPLE_TEST_FIXTURE(example_test_class)
  {
    TEST_CASE(test_hello_world1);
    TEST_CASE(test_hello_world2);
    TEST_CASE(test_assert_equals);
    TEST_CASE(test_assert_equals_fail);
  }

  /**
   * SELECT 'Hello world' example using framework magic
   *
   * Runs SELECT 'Hello world!' using the Connection and Statement
   * object that has been implicitly created by setUp() before calling the test.
   *
   * The unit_fixture class defines a coupld of useful members for you:
   * - Connection
   * - Statement
   * - PreparedStatement
   * - ResultSet
   *
   * The Connection object is the only one which is initialized and ready for
   * use. All other objects are in an undefined state. All members are defined
   * as auto-ptr, meaning, you don't need to worry about cleaning up them.
   */
  void test_hello_world1();

  /**
   * SELECT 'Hello world' example without framework magic
   *
   * The same as test1() but with less framework magic and, for example,
   * manual connect using the framework method getConnection()
   */
  void test_hello_world2();

  /**
   * Demo of ASSERT_EQUALS macro
   *
   * ASSERT_EQUALS macro can handle the following types:
   * - int
   * - bool
   * - double
   * - float
   * - long double
   * - char*
   *
   * ASSERT_EQUALS is a wrapper for:
   * void assertEquals(<type> expected, <type> result, const char* file, int line);
   */
  void test_assert_equals();

  /**
   * Demo of a failing ASSERT_EQUALS
   */
  void test_assert_equals_fail();

};

REGISTER_FIXTURE(example_test_class);
} /* namespace example */
} /* namespace testsuite */
