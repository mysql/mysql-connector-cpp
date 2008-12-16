/*
 Copyright (C) 2005-2007 MySQL AB

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

#include "../example_base.h"

/**
 * Tests regressions w/ the Escape Processor code.
 *
 * @version $Id:$
 *
 */

namespace testsuite
{
namespace example
{

class example_test_class : public example_fixture
{
private:
  typedef example_fixture super;
protected:
public:

  TEST_FIXTURE2(example_test_class)
  {
    TEST_CASE(testBug11797);

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

};

REGISTER_FIXTURE(example_test_class);
}
}
