  /*
 Copyright (C) 2002-2004 MySQL AB

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

    static const double EPSILON;

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
