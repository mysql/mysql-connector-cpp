#include "EscapeProcessorRegressionTest.h"

namespace testsuite
{
  namespace regression
  {
    EscapeProcessorRegressionTest::EscapeProcessorRegressionTest(String name)
      : super(name)
    {
      // TODO Auto-generated constructor stub
    }

    /**
    * Tests fix for BUG#11797 - Escape tokenizer doesn't respect stacked single
    * quotes for escapes.
    * 
    * @throws Exception
    *             if the test fails.
    */

    /* throws Exception */
    void EscapeProcessorRegressionTest::testBug11797() 
    {
      ASSERT_EQUALS("select 'ESCAPED BY ''\\'' ON {tbl_name | * | *.* | db_name.*}'",
        * this->conn->nativeSQL("select 'ESCAPED BY ''\\'' ON {tbl_name | * | *.* | db_name.*}'"));
    }

    /**
    * Tests fix for BUG#11498 - Escape processor didn't honor strings
    * demarcated with double quotes.
    * 
    * @throws Exception
    *             if the test fails.
    */

    /* throws Exception */
    void EscapeProcessorRegressionTest::testBug11498() 
    {
      ASSERT_EQUALS(
        "replace into t1 (id, f1, f4) VALUES(1,\"\",\"tko { zna gdje se sakrio\"),(2,\"a\",\"sedmi { kontinentio\"),(3,\"a\",\"a } cigov si ti?\")",
        * this->conn->nativeSQL("replace into t1 (id, f1, f4) VALUES(1,\"\",\"tko { zna gdje se sakrio\"),(2,\"a\",\"sedmi { kontinentio\"),(3,\"a\",\"a } cigov si ti?\")"));
    }

    /**
    * Tests fix for BUG#14909 - escape processor replaces quote character in
    * quoted string with string delimiter.
    * 
    * @throws Exception
    */

    /* throws Exception */
    void EscapeProcessorRegressionTest::testBug14909() 
    {
      ASSERT_EQUALS("select '{\"','}'", * this->conn->nativeSQL("select '{\"','}'"));
    }

    /**
    * Tests fix for BUG#25399 - EscapeProcessor gets confused by multiple backslashes
    * 
    * @throws Exception if the test fails.
    */

    /* throws Exception */
    void EscapeProcessorRegressionTest::testBug25399() 
    {
      /*
      ASSERT_EQUALS("\\' {d}",
      getSingleValueWithQuery("SELECT '\\\\\\' {d}'"));*/

    }
  }
}
