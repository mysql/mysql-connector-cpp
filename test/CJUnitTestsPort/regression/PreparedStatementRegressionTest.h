/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
