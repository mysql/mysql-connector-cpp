/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
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
