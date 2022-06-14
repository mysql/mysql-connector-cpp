/*
 * Copyright (c) 2009, 2020, Oracle and/or its affiliates.
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




#include <time.h>
#include "BlobTest.h"


#define BYTE_MAX_VALUE 255
#define BYTE_MIN_VALUE 0

namespace testsuite
{
namespace simple
{

const String BlobTest::TEST_BLOB_FILE_PREFIX("cmj-testblob");

/**
 * Setup the test case
 *
 * @throws Exception
 *             if an error occurs
 */
void BlobTest::setUp()
{
  static int counter= 0;
  // Just to make 2nd run(setString) to use smaller file(and be faster)
  ++counter;

  super::setUp();

  realFrameworkTiming=TestsListener::doTiming();

  testBlobFile.reset(new FileUtils::ccppFile(TEST_BLOB_FILE_PREFIX + ".dat"));

  int requiredSize=4 * 1024 * 1024/counter - (counter - 1)*128;

  if (!versionMeetsMinimum(5, 6, 5))
  {
    requiredSize=1 * 1024 * 1024;
  }

  Timer::startTimer("BlobTest::testByteStreamInsert", "Blob File Creation", __FILE__, __LINE__);
  createBlobFile(requiredSize);
  TestsListener::messagesLog() << "Blob File Creation" << Timer::translate2seconds(Timer::stopTimer("BlobTest::testByteStreamInsert", "Blob File Creation")) << std::endl;

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
  stmt->executeUpdate("DROP TABLE IF EXISTS BLOBTEST");

  testBlobFile.reset();

  TestsListener::doTiming(realFrameworkTiming);

  super::tearDown();
}

/* throws Exception */
  void BlobTest::testBlobStreamInsert()
  {
    //SKIP("too slow");
    testBlobInsert( conn );
  }


  void BlobTest::testBlobStringInsert()
  {
    //SKIP("too slow");
    testBlobInsert( conn, true );
  }

  /**
   * Tests inserting blob data as a stream
   *
   * @throws Exception
   *             if an error occurs
   */
  void BlobTest::testBlobInsert( Connection & c, bool asString )
  {
    TIMER_START( "Populating blob table" );
    pstmt.reset( conn->prepareStatement("INSERT INTO BLOBTEST(blobdata) VALUES (?)") );

    if ( asString )
    {
      sql::SQLString str;
      testBlobFile->readFile( const_cast<std::string&>(str.asStdString()) );

      pstmt->setString( 1, str );
      pstmt->execute();
    }
    else
    {
      std::fstream & bIn= testBlobFile->getStream();
      ASSERT( ! bIn.fail() );

      pstmt->setBlob( 1, & bIn );
      pstmt->execute();
    }
  TIMER_STOP("Populating blob table");
  pstmt->clearParameters();

  doRetrieval();
}

/* throws Exception */
bool BlobTest::checkBlob(const String & retrBytes)
{
  bool passed=true;

  std::fstream & bIn=testBlobFile->getStream();

  bIn.seekg(0, std::ios_base::beg);

  ASSERT_MESSAGE(!bIn.fail(), "seekg 0 position from the beginning - failed");
  ASSERT_MESSAGE(!bIn.eof(), "stream is at eof");

  unsigned int fileLength=testBlobFile->getSize();

  ASSERT_EQUALS((unsigned int) retrBytes.size(), fileLength);

  int substrIdx=0;

  while (!bIn.eof())
  {
    char fromFile[8192];
    bIn.read(fromFile, sizeof (fromFile));

    ASSERT_MESSAGE(!bIn.fail() || bIn.eof(), "read from file failed");
    if (bIn.gcount() == 0) {
      logMsg("We did not get any data from our input stream, we cannot do the compare input and output.");
      logMsg("Lets be gentle and consider the test as passed.");
      break;
    }

    if (retrBytes.compare(substrIdx, bIn.gcount(), fromFile, bIn.gcount()) != 0)
    {
      passed=false;
      int j=0;

      TestsListener::errorsLog() << "compare returned !=0 at " << substrIdx
              << ", read from file " << bIn.gcount() << std::endl;

      while (j < bIn.gcount() && fromFile[ j ] == retrBytes[substrIdx + j])
        ++j;


      if (j < bIn.gcount())
      {
        TestsListener::errorsLog() << "Byte pattern differed at position "
                << j << " , Retrieved: " << retrBytes[ substrIdx + j ]
                << "(" << StringUtils::toHexString(retrBytes[ substrIdx + j ])
                << ") != From File:" << fromFile[ j ] << "("
                << StringUtils::toHexString(fromFile[ j ]) << ")"
                << std::endl;

        if (j < bIn.gcount() - 1)
        {
          TestsListener::errorsLog() << "Following bytes (table:file):";

          while (j < bIn.gcount())
          {
            // current byte was already printed;
            ++j;
            TestsListener::errorsLog()
                    << " (" << StringUtils::toHexString(retrBytes[ substrIdx + j ])
                    << ":" << StringUtils::toHexString(fromFile[ j ]) << ")";
          }

          TestsListener::errorsLog() << std::endl;
        }
      }

      break;
    }

    substrIdx+=static_cast<int> (bIn.gcount());
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
  catch (sql::SQLException &)
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
  bool passed=false;

  TIMER_START("Blob Retrieval");
  rs.reset(stmt->executeQuery("SELECT blobdata from BLOBTEST LIMIT 1"));

  rs->next();
  TIMER_STOP("Blob Retrieval");

  TIMER_START("getString");
  String s(rs->getString(1));
  TIMER_STOP("getString");

  TIMER_START("Blob Check 1");
  passed=checkBlob(s);
  TIMER_STOP("Blob Check 1");

  ASSERT_MESSAGE(passed,
                 "Inserted BLOB data did not match retrieved BLOB data for getString().");

  s.clear();

  TIMER_START("getBlob");
  boost::scoped_ptr<std::istream> inStr(rs->getBlob(1));
  TIMER_STOP("getBlob");

  TIMER_START("Stream Reading");
  char buff[1048];

  while (!inStr->eof())
  {
    inStr->read(buff, sizeof (buff));
    s.append(buff, inStr->gcount());
  }
  TIMER_STOP("Stream Reading");

  TIMER_START("Blob Check 2");
  passed=checkBlob(s);
  TIMER_STOP("Blob Check 2");

  ASSERT_MESSAGE(passed, "Inserted BLOB data did not match retrieved BLOB data for getBlob().");

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
    TestsListener::messagesLog("creating file!!!");
    testBlobFile->deleteFile();
    //testBlobFile.reset( FileUtils::ccppFile::createTempFile(TEST_BLOB_FILE_PREFIX, ".dat") );

    //cleanupTempFiles(testBlobFile, TEST_BLOB_FILE_PREFIX);
    std::ostream & bOut=testBlobFile->getStream();

    ASSERT(!bOut.fail());

    int dataRange=BYTE_MAX_VALUE - BYTE_MIN_VALUE + 1;

    srand((unsigned) time(NULL));

    for (int i=0; i < size; ++i)
    {
      bOut << static_cast<char> (((rand() % dataRange) + BYTE_MIN_VALUE) & 0xff);
    }

    bOut.flush();
    testBlobFile->close();
  }
}

}
}
