/*
  Copyright (c) 2009, 2010, Oracle and/or its affiliates. All rights reserved.

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
 * Tests SubQueries on MySQL > 4.1
 *
 * @author Mark Matthews
 * @version $Id: SubqueriesRegressionTest.java,v 1.1.2.1 2005/05/13 18:58:38
 *          mmatthews Exp $
 */

namespace testsuite
{
namespace regression
{
  class SubqueriesRegressionTest : public BaseTestFixture
  {
  private:
    typedef BaseTestFixture super;
  static const int REPETITIONS;


/* throws std::exception * */
    void createTables() ;


/* throws std::exception * */
    void dropTables() ;

  protected:

  public:

    TEST_FIXTURE( SubqueriesRegressionTest )
    {
      TEST_CASE( testSubQuery1 );
      TEST_CASE( testSubQuery2 );
      TEST_CASE( testSubQuery3 );
      TEST_CASE( testSubQuery4 );
      TEST_CASE( testSubQuery5 );
    }


    /* throws std::exception * */
    void setUp() ;


    /* throws std::exception * */
    void tearDown() ;


    /**
	   * DOCUMENT ME!
	   *
	   * @throws std::exception *
	   *             DOCUMENT ME!
	   */
    /* throws std::exception * */
    void testSubQuery1() ;


    /**
	   * DOCUMENT ME!
	   *
	   * @throws std::exception *
	   *             DOCUMENT ME!
	   */
    /* throws std::exception * */
    void testSubQuery2() ;


    /**
	   * DOCUMENT ME!
	   *
	   * @throws std::exception *
	   *             DOCUMENT ME!
	   */
    /* throws std::exception * */
    void testSubQuery3() ;


    /**
	   * DOCUMENT ME!
	   *
	   * @throws std::exception *
	   *             DOCUMENT ME!
	   */
    /* throws std::exception * */
    void testSubQuery4() ;


    /**
	   * DOCUMENT ME!
	   *
	   * @throws std::exception *
	   *             DOCUMENT ME!
	   */
    /* throws std::exception * */
    void testSubQuery5() ;

  };
REGISTER_FIXTURE(SubqueriesRegressionTest);

}
}
