/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "../unit_fixture.h"

/**
 * Test for Bug [TODO - add bug number]
 *
 */

namespace testsuite
{
namespace regression
{

class template_bug123_class : public unit_fixture
{
private:
  typedef unit_fixture super;
protected:
public:

  EXAMPLE_TEST_FIXTURE(template_bug123_class)
  {
    TEST_CASE(template_bug123_method);
  }

  /**
   * http://bugs.mysql.com/[TODO - add bug number]
   *
   * [TODO - any longer description may go here]
   */
  void template_bug123_method();

};

REGISTER_FIXTURE(template_bug123_class);
} /* namespace regression */
} /* namespace testsuite */
