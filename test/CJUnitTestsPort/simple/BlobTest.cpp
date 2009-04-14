/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/


#include <time.h>
#include "BlobTest.h"

#define BYTE_MAX_VALUE 255
#define BYTE_MIN_VALUE 0

namespace testsuite
{
namespace simple
{

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

    testBlobFile.reset();

    super::tearDown();
  }


/* throws Exception */
  void BlobTest::testByteStreamInsert()
  {
    //SKIP("too slow");
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
    bool passed = true;

    std::fstream & bIn = testBlobFile->getStream();

    bIn.seekg( 0, std::ios_base::beg );

    ASSERT_MESSAGE( !bIn.fail(), "seekg 0 position from the beginning - failed" );

    unsigned int fileLength = testBlobFile->getSize();

    if (retrBytes.size() == fileLength)
    {
      int substrIdx= 0;

      while ( ! bIn.eof() )
      {
        char fromFile[8192];
        bIn.read( fromFile, sizeof(fromFile) );

        ASSERT_MESSAGE( !bIn.fail() || bIn.eof(), "read from file failed" );

        if ( retrBytes.compare( substrIdx, bIn.gcount(), fromFile, bIn.gcount() ) != 0 )
        {
          passed = false;
          int j = 0;

					TestsListener::errorsLog() << "compare returned !=0 at " << substrIdx
						<< ", read from file " << bIn.gcount() << std::endl;

          while ( j < bIn.gcount() && fromFile[ j ] == retrBytes[substrIdx + j] )
            ++j;


          if ( j < bIn.gcount() )
          {
            TestsListener::errorsLog() << "Byte pattern differed at position "
              << j << " , Retrieved: " << retrBytes[ substrIdx + j ]
              << "(" <<  StringUtils::toHexString( retrBytes[ substrIdx + j ] )
              << ") != From File:" << fromFile[ j ] << "("
              << StringUtils::toHexString( fromFile[ j ] ) << ")"
              << std::endl;

            if ( j < bIn.gcount() - 1 )
            {
              TestsListener::errorsLog() << "Following bytes (table:file):";

              while ( j < bIn.gcount()  )
              {
                // current byte was already printed;
                ++j;
                TestsListener::errorsLog()
                  << " (" << StringUtils::toHexString( retrBytes[ substrIdx + j ] )
                  << ":" << StringUtils::toHexString( fromFile[ j ] ) << ")";
              }

              TestsListener::errorsLog() << std::endl;
            }
          }

          break;
        }

				substrIdx+= static_cast<int>(bIn.gcount());
      }
    }
    else
    {
      passed = false;

      TestsListener::errorsLog() << "retrBytes.length("
        << retrBytes.size()
        << ") != testBlob.length(" << fileLength << ")" << std::endl;
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
