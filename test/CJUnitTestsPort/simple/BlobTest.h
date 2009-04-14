/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include <fstream>

#include "../../common/file.h"

#include "../BaseTestFixture.h"

/**
 * Tests BLOB functionality in the driver.
 *
 * @author Mark Matthews
 * @version $Id: BlobTest.java 6707 2008-01-04 22:06:31Z mmatthews $
 */

namespace testsuite
{
namespace simple
{
  class BlobTest : public BaseTestFixture
  {
  private:
    typedef BaseTestFixture super;

    std::auto_ptr<FileUtils::ccppFile> testBlobFile;

  /**
	 * Tests inserting blob data as a stream
	 *
	 * @throws std::exception *
	 *             if an error occurs
	 */

    /* throws std::exception * */
    void testByteStreamInsert( Connection & c );


    /* throws std::exception * */
    bool checkBlob( const String & retrBytes );


    /* throws std::exception * */
    void createTestTable();

  /**
	 * Mark this as deprecated to avoid warnings from compiler...
	 *
	 * @deprecated
	 *
	 * @throws std::exception *
	 *             if an error occurs retrieving the value
	 */
    /* throws std::exception * */
    void doRetrieval();

    static const String TEST_BLOB_FILE_PREFIX;


    void createBlobFile(int size) ;

  protected:

  public:
    TEST_FIXTURE( BlobTest )
    {
			TEST_CASE( testByteStreamInsert );
    }


  /**
	 * Setup the test case
	 *
	 * @throws std::exception *
	 *             if an error occurs
	 */
    void setUp() ;

  /**
	 * Destroy resources created by test case
	 *
	 * @throws std::exception *
	 *             if an error occurs
	 */
    void tearDown() ;


    /* throws std::exception * */
    void testByteStreamInsert();

  };

  REGISTER_FIXTURE( BlobTest );

}
}
