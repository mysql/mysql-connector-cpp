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
 * Test of mysql_art_resultset.
 * Actually so far - MyVal from there only
 */

namespace testsuite
{
namespace classes
{

class art_resultset : public unit_fixture
{
public:

  EXAMPLE_TEST_FIXTURE(art_resultset)
  {
    TEST_CASE(testMyVal);
  }

  /**
   * Test for MyVal interpretation methods
   *
   */
  void testMyVal();

};

REGISTER_FIXTURE(art_resultset);
} /* namespace classes */
} /* namespace testsuite */
