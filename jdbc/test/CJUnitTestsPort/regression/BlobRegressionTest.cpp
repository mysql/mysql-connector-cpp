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



#include "BlobRegressionTest.h"
#include "../../common/file.h"
#include <sstream>

namespace testsuite
{
namespace regression
{

  /**
   * Dunno how this test should look like in C++
   *
   * @throws Exception
   *             ...
   */
  /*
  void BlobRegressionTest::testBug2670()
    {
      if (!isRunningOnJdk131()) {
        try {
          char blobData[32];
          for (int i = 0; i < blobData.length; i++) {
            blobData[i] = 1;
          }
          stmt->executeUpdate("DROP TABLE IF EXISTS testBug2670");
          stmt->executeUpdate("CREATE TABLE testBug2670(blobField LONGBLOB)");
          PreparedStatement pstmt-> conn->prepareStatement("INSERT INTO testBug2670 (blobField) VALUES (?)");
          pstmt->setBytes(1, blobData);
          pstmt->executeUpdate();
          rs = stmt->executeQuery("SELECT blobField FROM testBug2670");
          rs->next();
          Blob blob = rs->getBlob(1);
          blob.setBytes(4, new char { 2, 2, 2, 2 });
          char newBlobData = blob.getBytes(1L, (int) blob.length());
          assertTrue("Blob changed length",
              blob.length() == blobData.length);
          assertTrue(
              "New data inserted wrongly",
              ((newBlobData[3] == 2) && (newBlobData[4] == 2)
                  && (newBlobData[5] == 2) && (newBlobData[6] == 2)));
          blob.setBytes(32, new char { 2, 2, 2, 2 });
          assertTrue("Blob length should be 3 larger",
              blob.length() == (blobData.length + 3));
        }  {
          stmt->executeUpdate("DROP TABLE IF EXISTS testUpdateLongBlob");
        }
      }
    }*/


  /**
   * @throws Exception
   *             ...
   */
  void BlobRegressionTest::testUpdateLongBlobGT16M()
  {
    if ( this->versionMeetsMinimum(4, 0) )
    {
      int size18M= 18 * 1024 * 1024;
      String blobData; // 18M blob
      blobData.append( size18M, 'a' );

      stmt->executeUpdate("DROP TABLE IF EXISTS testUpdateLongBlob");
      stmt->executeUpdate("CREATE TABLE testUpdateLongBlob(blobField LONGBLOB)");
      stmt->executeUpdate("INSERT INTO testUpdateLongBlob (blobField) VALUES (NULL)");

      pstmt.reset( conn->prepareStatement("UPDATE testUpdateLongBlob SET blobField=?") );

      pstmt->setString(1, blobData );
      pstmt->execute();

      stmt->executeUpdate("DROP TABLE IF EXISTS testUpdateLongBlob");
    }
  }

  /**
   *
   * @throws Exception
   */

/* throws Exception */
  /*
  void BlobRegressionTest::testUpdatableBlobsWithCharsets()
    {
      char smallBlob[32];

      for (char i = 0; i < sizeof(smallBlob); ++i)
      {
        smallBlob[i] = i;
      }

        stmt->executeUpdate("DROP TABLE IF EXISTS testUpdatableBlobsWithCharsets");
        stmt->executeUpdate("CREATE TABLE testUpdatableBlobsWithCharsets(pk INT NOT NULL PRIMARY KEY, field1 BLOB)");

        std::istringstream str( smallBlob );

        PreparedStatement pstmt( conn->prepareStatement("INSERT INTO testUpdatableBlobsWithCharsets (pk, field1) VALUES (1, ?)") );

        pstmt->setBlob( 1, & str );
        pstmt->executeUpdate();

        Statement updstmt( conn->createStatement() );
        rs.reset( updstmt->executeQuery("SELECT pk, field1 FROM testUpdatableBlobsWithCharsets") );

        rs->next();

        MESSAGE( rs->getString(1) + "->" + rs->getString(2) );



        for (char i = 0; i < sizeof( smallBlob ); ++i)
        {
          smallBlob[i] = i + 32;
        }

        //we don't support that yet. so the whole test doesn't make sense either
  / *
        rs->setBlob(2, std::istringstream(smallBlob), sizeof(smallBlob) );
        rs->updateRow();* /


        ResultSet newRs( stmt->executeQuery("SELECT field1 FROM testUpdatableBlobsWithCharsets") );
        newRs->next();
        String updatedBlob( newRs->getString(1) );

        for (byte i = 0; i < sizeof(smallBlob); i++) {
          char origValue = smallBlob[i];
          char newValue = updatedBlob[i];
          assertTrue(String( "Original byte at position " ) + i + ", " + origValue
              + " != new value, " + newValue, origValue == newValue);
        }

        stmt->executeUpdate("DROP TABLE IF EXISTS testUpdatableBlobsWithCharsets");
    }*/



/* throws Exception */
  void BlobRegressionTest::testBug5490()
  {
    stmt->executeUpdate("DROP TABLE IF EXISTS testBug5490");
    stmt->executeUpdate("CREATE TABLE testBug5490"\
                        "(pk INT NOT NULL PRIMARY KEY, blobField BLOB)");
    String sql = "insert into testBug5490 values(?,?)";
    int blobFileSize = 871;

    FileUtils::ccppFile blobFile("Bug5490");

    if( ! blobFile.exists() || blobFile.getSize() != blobFileSize )
    {
      blobFile.deleteFile();
      std::fstream & fos= blobFile.getStream();

      fos.seekp( blobFileSize - 1, std::ios_base::beg );
      fos << 'a';
      fos.flush();
      blobFile.close();
    }

    pstmt.reset( conn->prepareStatement(sql) );
    pstmt->setInt(1, 2);
    pstmt->setBlob(2, & blobFile.getStream() );
    pstmt->execute();

    blobFile.close();

    rs.reset( stmt->executeQuery("SELECT blobField FROM testBug5490") );
    rs->next();
    String returned = rs->getString( 1 );

    ASSERT_EQUALS( blobFileSize, static_cast<int>(returned.length()) );

    stmt->executeUpdate("DROP TABLE IF EXISTS testBug5490");

    if ( blobFile.exists() )
      blobFile.deleteFile();
  }

  /**
   * Tests BUG#8096 where emulated locators corrupt binary data when using
   * server-side prepared statements.
   *
   * @throws Exception
   *             if the test fails.
   */

/* looks like doesn't make much sense here either */
  void BlobRegressionTest::testBug8096()
  {
    const int dataSize = 256;

    Properties props ;
    props["emulateLocators"]= "true";
    Connection locatorConn( getConnectionWithProps(props) );

    stmt.reset( locatorConn->createStatement() );
    selectDb( stmt );

    String createTable( "CREATE TABLE testBug8096 (ID VARCHAR(10) "\
                        "PRIMARY KEY, DATA LONGBLOB)" );

    String select( "SELECT ID, 'DATA' AS BLOB_DATA FROM testBug8096 "\
                   "WHERE ID = ?" );

    String insert( "INSERT INTO testBug8096 (ID, DATA) VALUES (?, '')" );
    String id( "1" );

    char testData[dataSize];

    for ( unsigned int i= 0; i < sizeof( testData ); ++i)
    {
      testData[i]= static_cast<char>( i & 0xff );
    }

    stmt->executeUpdate("DROP TABLE IF EXISTS testBug8096");
    stmt->executeUpdate(createTable);
    pstmt.reset( locatorConn->prepareStatement( insert ) );
    pstmt->setString(1, id);
    pstmt->execute();

    pstmt.reset( locatorConn->prepareStatement(select) );
    pstmt->setString(1, id);
    rs.reset( pstmt->executeQuery() );

    if ( rs->next() )
    {
      std::istream * b = rs->getBlob("BLOB_DATA");
      (void) b; // void is to trick the compiler into generating no warnings
      //b.setBytes(1, testData);
    }

    pstmt.reset( locatorConn->prepareStatement(select) );
    pstmt->setString(1, id);
    rs.reset(pstmt->executeQuery());

    String result;
    if ( rs->next() )
    {
      result = rs->getString(1);
    }

    ASSERT( result.length() > 0 );

    for (unsigned i = 0; i < result.length() && i < sizeof(testData); ++i)
    {
      if (result[i] != testData[i])
      {
        std::ostringstream s;

        s << "At position "<< i << " test data: " << static_cast<int>(testData[i])
          << " result " << static_cast<int>(result[i]);

        ASSERT_MESSAGE( testData[i] == result[i], s.str().c_str() );
      }
    }

    stmt->executeUpdate("DROP TABLE IF EXISTS testBug8096");
  }

  /**
   * Tests fix for BUG#9040 - PreparedStatement.addBatch() doesn't work with
   * server-side prepared statements and streaming BINARY data.
   *
   * @throws Exception
   *             if the test fails.
   */

/* not relevant at the moment - no batches */
/*
  void BlobRegressionTest::testBug9040()
  {
      stmt->executeUpdate("DROP TABLE IF EXISTS testBug9040");
      stmt->executeUpdate("create table if not exists testBug9040 "
          + "(primary_key int not NULL primary key, "
          + "data mediumblob)");
      pstmt-> conn->prepareStatement("replace into testBug9040 (primary_key, data) values(?,?)");
      int primaryKey = 1;
      char data[] = "First Row";

      pstmt->setInt(1, primaryKey);
      pstmt->setBinaryStream(2, new ByteArrayInputStream(data),
          data.length);
      pstmt->addBatch();
      primaryKey = 2;
      data = "Second Row".getBytes();
      pstmt->setInt(1, primaryKey);
      pstmt->setBinaryStream(2, new ByteArrayInputStream(data),
          data.length);
      pstmt->addBatch();
      pstmt->executeBatch();

      stmt->executeUpdate("DROP TABLE IF EXISTS testBug9040");

    }
  }*/


/* throws Exception */
  void BlobRegressionTest::testBug10850()
  {
    String tableName = "testBug10850";
    createTable(tableName, "(field1 TEXT)");

    pstmt.reset( conn->prepareStatement(String( "INSERT INTO " ) +
                                        tableName + " VALUES (?)") );

    std::istringstream str("");
    pstmt->setBlob(1, & str);

    pstmt->executeUpdate();

    ASSERT( getSingleIndexedValueWithQuery(1,
        String( "SELECT LENGTH(field1) FROM " ) + tableName).toString() == "0");

    stmt->executeUpdate(String( "TRUNCATE TABLE " ) + tableName);
    pstmt->clearParameters();

    std::istringstream str2;

    pstmt->setBlob( 1, &str );
    pstmt->executeUpdate();

    ASSERT( getSingleIndexedValueWithQuery(1,
        String( "SELECT LENGTH(field1) FROM " ) + tableName).toString() == "0" );

    stmt->executeUpdate(String( "TRUNCATE TABLE " ) + tableName);
  }


/* throws Exception */
  /** Doesn't make much sense. As well as i'm not sure it's equivalenly translated*/
  void BlobRegressionTest::testBug34677()
  {
    SKIP( "The test is wrong" );
    createTable("testBug34677", "(field1 BLOB)");

    stmt->executeUpdate("INSERT INTO testBug34677 VALUES ('abc')");

    rs.reset( stmt->executeQuery("SELECT field1 FROM testBug34677") );
    rs->next();

    MESSAGE( rs->getString( 1 ) + "<- field1 in testBug34677" );
    std::istream * blob = rs->getBlob(1);
    blob->width(0L);
    ASSERT_EQUALS(0, (int)blob->width());
    char tmp;

    if ( ! blob->read(&tmp,1).fail() )
    {
      TestsListener::errorsLog() << "read: " << tmp << static_cast<int>(tmp)
        << std::endl;

      FAIL( "But probably that's ok - bad test likely" );
    }
  }

}
}
