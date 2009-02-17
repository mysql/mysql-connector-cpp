/* Copyright (C) 2009 Sun Microsystems, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

There are special exceptions to the terms and conditions of the GPL
as it is applied to this software. View the full text of the
exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
software distribution.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <fstream>

#include "../common/file.h"

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
    TEST_FIXTURE( BlobTest ), testBlobFile( NULL )
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
