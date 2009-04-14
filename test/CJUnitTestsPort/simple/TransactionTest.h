/*
Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPL
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPL as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include "cppconn/exception.h"

#include "../BaseTestFixture.h"

/**
 * 
 * @author Mark Matthews
 * @version $Id: TransactionTest.java,v 1.1.2.1 2005/05/13 18:58:37 mmatthews
 *          Exp $
 */

namespace testsuite
{
namespace simple
{
  class TransactionTest : public BaseTestFixture
  {
  private:
    typedef BaseTestFixture super;
  // ---------------------------------------------

    static const double DOUBLE_CONST;

/* throws sql::DbcException */

    void createTestTable() ;  

protected:
  
  public:
    TEST_FIXTURE( TransactionTest )
    {
      TEST_CASE( testTransaction );
    }


    /* throws std::exception * */
    void setUp();

  /**
	 * DOCUMENT ME!
	 * 
	 * @throws sql::SQLException
	 *             DOCUMENT ME!
	 */
  /* throws sql::SQLException */

    void testTransaction();
  };

REGISTER_FIXTURE(TransactionTest);

}
}
