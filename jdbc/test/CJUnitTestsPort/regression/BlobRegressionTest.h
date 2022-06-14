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
 * Tests fixes for BLOB handling.
 *
 * @author Mark Matthews
 * @version $Id: BlobRegressionTest.java,v 1.1.2.19 2005/03/09 18:16:16
 *          mmatthews Exp $
 */

namespace testsuite
{
namespace regression
{
  class BlobRegressionTest : public BaseTestFixture
  {
  private:
    typedef BaseTestFixture super;

  public:
    TEST_FIXTURE( BlobRegressionTest )
    {

      //TEST_CASE( testBug2670 );
      //TEST_CASE( testUpdatableBlobsWithCharsets );
      TEST_CASE( testBug5490                    );
      // not relevant to c/c++ (at least) at the moment
      //TEST_CASE( testBug8096                    );

      //TEST_CASE( testBug9040                    );
      TEST_CASE( testBug10850                   );
      TEST_CASE( testBug34677                   );
      // TEST_CASE( testUpdateLongBlobGT16M        );

    }

  /**
   * Looks irrelevant at least at the moment.
   * @throws std::exception *
   */
    //void testBug2670() ;


  /**
   * @throws std::exception *
   *             ...
   */
    void testUpdateLongBlobGT16M();


  /**
   * @throws std::exception *
   */
   //void testUpdatableBlobsWithCharsets();


/* throws std::exception * */
    void testBug5490();


  /**
   * Tests BUG#8096 where emulated locators corrupt binary data when using
   * server-side prepared statements.
   *
   * @throws std::exception *
   *             if the test fails.
   */
    void testBug8096();


  /**
   * Tests fix for BUG#9040 - PreparedStatement.addBatch() doesn't work with
   * server-side prepared statements and streaming BINARY data.
   *
   * @throws std::exception *
   *             if the test fails.
   Irrelevant - May become useful later
   */
   //void testBug9040();


/* throws std::exception * */
    void testBug10850();


/* throws std::exception * */
    void testBug34677();

  };

REGISTER_FIXTURE(BlobRegressionTest);

}
}
