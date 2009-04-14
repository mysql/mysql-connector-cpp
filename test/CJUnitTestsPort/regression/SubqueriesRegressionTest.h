/*
Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPL
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPL as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
