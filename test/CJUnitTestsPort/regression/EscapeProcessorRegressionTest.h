/*
   Copyright 2005-2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
