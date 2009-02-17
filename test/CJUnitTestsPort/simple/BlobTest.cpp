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


#include <time.h>
#include "BlobTest.h"

#define BYTE_MAX_VALUE 255
#define BYTE_MIN_VALUE 0

namespace testsuite
{
namespace simple
{

  /*
  static int dummySimpleBlob= atexit
        public void run() {
          for (int i = 0; i < 5; i++) {
            try {
              if (testBlobFile.delete()) {
                break;
              }
            } catch ( std::exception & t) {
            }
          }
        }*/

  const String BlobTest::TEST_BLOB_FILE_PREFIX( "cmj-testblob" );


  /**
	 * Setup the test case
	 * 
	 * @throws Exception
	 *             if an error occurs
	 */
  void BlobTest::setUp() 
  {
    super::setUp();

    testBlobFile.reset( new FileUtils::ccppFile( TEST_BLOB_FILE_PREFIX + ".dat" ) );

    int requiredSize = 32 * 1024 * 1024;

    if (! versionMeetsMinimum(4, 0))
    {
      requiredSize = 8 * 1024 * 1024;
    }

    createBlobFile(requiredSize);

    createTestTable();
  }

  /**
	 * Destroy resources created by test case
	 * 
	 * @throws Exception
	 *             if an error occurs
	 */

/* throws Exception */
  void BlobTest::tearDown() 
  {
    stmt->executeUpdate( "DROP TABLE IF EXISTS BLOBTEST" );

    super::tearDown();
  }


/* throws Exception */
  void BlobTest::testByteStreamInsert() 
  {
    testByteStreamInsert( conn );
  }

  /**
	 * Tests inserting blob data as a stream
	 * 
	 * @throws Exception
	 *             if an error occurs
	 */
  void BlobTest::testByteStreamInsert(Connection & c) 
  {
    std::fstream & bIn= testBlobFile->getStream();
    
    ASSERT( ! bIn.fail() );

    pstmt.reset( conn->prepareStatement("INSERT INTO BLOBTEST(blobdata) VALUES (?)") );
    pstmt->setBlob( 1, & bIn );
    pstmt->execute();
    pstmt->clearParameters();

    doRetrieval();
  }


/* throws Exception */
  bool BlobTest::checkBlob( const String & retrBytes) 
  {
    bool passed = false;

    std::fstream & bIn = testBlobFile->getStream();

    TestsListener::theInstance().errorsLog() << "fail state before seek" << bIn.fail() << std::endl;

    bIn.seekg( 0, std::ios_base::beg );

    ASSERT_MESSAGE( !bIn.fail(), "seekg 0 position from the beginning - failed" );

    int fileLength = testBlobFile->getSize();

    std::stringstream inFile, inTable;

    if (retrBytes.size() == fileLength)
    {
      for ( int i = 0; i < fileLength; ++i )
      {
        char fromFile;
        bIn.read( & fromFile, 1 );

        inFile  << StringUtils::toHexString( fromFile )   << " ";
        inTable << StringUtils::toHexString(retrBytes[i]) << " ";

        ASSERT_MESSAGE( !bIn.fail(), "read from file failed" );

        if ( 0xff & ( retrBytes[i] ^ fromFile ) )
        {
          passed = false;
          TestsListener::theInstance().errorsLog()
            << "Byte pattern differed at position "
            << i << " , Retrieved: " << retrBytes[i]
            << "(" <<  StringUtils::toHexString(retrBytes[i])
            << ") != From File:" << fromFile << "("
            << StringUtils::toHexString( fromFile ) << ")"
            << std::endl << "Retrieved: " << inTable.str() << "< ";

          for ( int j = i + 1; (j < (i + 10)) /* && (j < i) */; ++j )
          {
            TestsListener::theInstance().errorsLog() 
              << StringUtils::toHexString(retrBytes[j]) << " ";
          }


          TestsListener::theInstance().errorsLog() << std::endl << "From File: "
            << inFile.str() << "< ";

          for ( int j = 1; j < 10 && ! bIn.fail(); ++j )
          {
            bIn >> fromFile;
            TestsListener::theInstance().errorsLog()
              << StringUtils::toHexString( fromFile )
              << " ";
          }

          TestsListener::theInstance().errorsLog() << std::endl;

          break;
        }

        passed = true;
      }
    }
    else
    {
      passed = false;

      TestsListener::theInstance().errorsLog() << "retrBytes.length("
        << retrBytes.size()
        << ") != testBlob.length(" << fileLength << ")";
    }

    return passed;
  }


/* throws Exception */
  void BlobTest::createTestTable() 
  {
    try
    {
      stmt->executeUpdate("DROP TABLE BLOBTEST");
    }
    catch (sql::SQLException & )
    {
    }

    stmt->executeUpdate("CREATE TABLE BLOBTEST (pos int PRIMARY KEY auto_increment, "\
                        "blobdata LONGBLOB)");
  }

  /**
	 * Mark this as deprecated to avoid warnings from compiler...
	 * 
	 * @deprecated
	 * 
	 * @throws Exception
	 *             if an error occurs retrieving the value
	 */

/* throws Exception */
  void BlobTest::doRetrieval() 
  {
    bool passed = false;

    rs.reset( stmt->executeQuery("SELECT blobdata from BLOBTEST LIMIT 1") );

    rs->next();

    String s( rs->getString(1) );

    //max_size supposed to contain number of bytes in blob, length w/ give string length

    TestsListener::theInstance().messagesLog() << "Capacity:"
      << s.capacity() << " MaxSize:" << s.max_size() << " Length:" << s.length()
      << std::endl;

    passed = checkBlob(s);

    ASSERT_MESSAGE(passed,
        "Inserted BLOB data did not match retrieved BLOB data for getString()." );


    s.clear();

    std::istream * inStr = rs->getBlob(1);
    char buff[8192];

    while ( ! inStr->eof() )
    {
      inStr->read( buff, sizeof(buff) );
      s.append( buff,inStr->gcount() );
    }

    passed = checkBlob(s);

    ASSERT_MESSAGE( passed, "Inserted BLOB data did not match retrieved BLOB data for getBlob()." );

    /*
    inStr = rs->getAsciiStream(1);
        bOut = new ByteArrayOutputStream();
        while ((b = inStr.read()) != -1) {
          bOut.write((byte) b);
        }
        retrBytes = bOut.toByteArray();
        passed = checkBlob(retrBytes);
        assertTrue(
            "Inserted BLOB data did not match retrieved BLOB data for getAsciiStream().",
            passed);
        inStr = rs->getUnicodeStream(1);
        bOut = new ByteArrayOutputStream();
        while ((b = inStr.read()) != -1) {
          bOut.write((byte) b);
        }
        retrBytes = bOut.toByteArray();
        passed = checkBlob(retrBytes);
        assertTrue(
            "Inserted BLOB data did not match retrieved BLOB data for getUnicodeStream().",
            passed);*/
    
  }


/* throws Exception */
  void BlobTest::createBlobFile(int size) 
  {
    if (testBlobFile.get() != NULL && testBlobFile->getSize() != size)
    {
      testBlobFile->deleteFile();
      //testBlobFile.reset( FileUtils::ccppFile::createTempFile(TEST_BLOB_FILE_PREFIX, ".dat") );
    
      //cleanupTempFiles(testBlobFile, TEST_BLOB_FILE_PREFIX);
      std::ostream & bOut = testBlobFile->getStream();

      ASSERT( ! bOut.fail() );

      int dataRange = BYTE_MAX_VALUE - BYTE_MIN_VALUE + 1;

      srand((unsigned) time(NULL));

      for (int i = 0; i < size; ++i)
      {
        bOut << static_cast<char>( ((rand() % dataRange) + BYTE_MIN_VALUE ) & 0xff );
      }

      bOut.flush();
      testBlobFile->close();
    }
  }

}
}
