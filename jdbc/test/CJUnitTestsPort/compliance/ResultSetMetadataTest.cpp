/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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



#include "ResultSetMetadataTest.h"

namespace testsuite
{
namespace compliance
{
/*
 * @testName:         testGetCatalogName
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The getCatalogName(int colindex) method returns a String
 *                    object representing the catalog name or an empty string ("")
 *                    if not applicable.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the
 *                    getCatalogName(int colindex) method.It should return a String object.
 */

/* throws Exception */
void ResultSetMetadataTest::testGetCatalogName()
{
  logMsg("Calling getCatalogName on ResultSetMetadata");
  String sRetValue=rsmd->getCatalogName(1);

  if (sRetValue.empty()) {
    logMsg("getCatalogName method does not return the column's table's catalog name");
  } else {
    logMsg(String("getCatalogName method returns:  ") + sRetValue);
  }
}

/*
 * @testName:         testGetColumnClassName
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The getColumnClassName(int colindex) method returns a String
 *                    object representing the fully qualified name of the Java class
 *                    to which a value in the designated column will be mapped.
 *                    (See section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the
 *                    getColumnClassName(int colindex) method.It should return a
 *                    String object.
 */

/* throws Exception */

/* Not Implemented in C/C++ (not even part of interface at the moment) */
void ResultSetMetadataTest::testGetColumnClassName()
{
#ifdef getColumnClassName_IMPLEMENTED
  logMsg("Calling getColumnClassName on ResultSetMetadata");
  String sRetValue=rsmd->getColumnClassName(1);
  if (sRetValue == NULL) {
    logMsg(
           "getColumnClassName method does not returns the fully-qualified name of the class");
  } else {
    logMsg(String("getColumnClassName method returns:  ") + sRetValue);
  }
#endif
}

/*
 * @testName:         testGetColumnCount
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The getColumnCount method returns an integer value representing the
 *                    number of columns in the ResultSet object.for which this ResultSetMetaData
 *                    object stores information.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the getColumnCount()
 *                    method on the ResultSetMetaData object.It should return an integer
 *                    value greater than or equal to zero.
 */

/* throws Exception */
void ResultSetMetadataTest::testGetColumnCount()
{
  logMsg("Calling getColumnCount on ResultSetMetaData");
  int coloumnCount=rsmd->getColumnCount();
  if (coloumnCount >= 0) {
    TestsListener::messagesLog()
            << "getColumnCount method returns: " << coloumnCount << std::endl;
  } else {
    logErr(" getColumnCount method returns a negative value");
  }
}

/*
 * @testName:         testGetColumnDisplaySize
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The getColumnDisplaySize(int colindex) method returns an int value
 *                    representing the maximum width in characters.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the
 *                    getColumnDisplaySize(int colindex) method.It should return an integer
 *                    representing the normal maximum width in characters for column colindex.
 */

/* throws Exception */
void ResultSetMetadataTest::testGetColumnDisplaySize()
{
  /*try
  {*/
  logMsg("Calling getColumnDisplaySize on ResultSetMetaData");

  int colDispSize=rsmd->getColumnDisplaySize(2);

  if (colDispSize >= 0) {
    TestsListener::messagesLog()
            << "getColumnDisplaySize method returns: " << colDispSize << std::endl;
  } else {
    logErr(" getColumnDisplaySize method returns a negative value");
  }
  /*}
  catch (sql::DbcException & sqle) {

      FAIL("Call to getColumnDisplaySize is Failed!");
  }*/
  /*catch (std::exception & e) {
      logErr(String( "Unexpected exception " ) + e.what());
      FAIL("Call to getColumnDisplaySize is Failed!");
  }*/
}

/*
 * @testName:         testGetColumnLabel
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The getColumnLabel(int colindex) method returns a String
 *                    object representing the suggested  title for use in printouts
 *                    and displays.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the
 *                    getColumnLabel(int colindex) method.It should return a String object.
 */

/* throws Exception */
void ResultSetMetadataTest::testGetColumnLabel()
{
  logMsg("Calling getColumnLabel on ResultSetMetadata");
  String sRetValue=rsmd->getColumnLabel(2);

  //"getColumnLabel method does not returns the suggested column title");

  logMsg(String("getColumnLabel method returned: ") + sRetValue);
}

/*
 * @testName:         testGetColumnName
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The getColumnName(int colindex) method returns a String
 *                    object representing the column name (See JDK 1.2.2 API
 *                    documentation)
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the
 *                    getColumnName(int colindex) method.It should return a String object.
 */

/* throws Exception */
void ResultSetMetadataTest::testGetColumnName()
{
  logMsg("Calling getColumnName on ResultSetMetadata");

  String sRetValue=rsmd->getColumnName(2);

  logMsg(String("getColumnName method returns:  ") + sRetValue);

  //catch (sql::DbcInvalidArgument * sqle)
  //{
  //  logMsg( String("Invalid argument exception caught") + sqle->what() );
}

/*
 * @testName:         testGetColumnType
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The getColumnType(int colindex) method returns the JDBC type from
 *                    the java.sql.Types class for the value in the designated column colindex.
 *                    (See section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the
 *                    getColumnType(int colindex) method.Check if an integer value is returned.
 */

/* throws Exception */
void ResultSetMetadataTest::testGetColumnType()
{
  logMsg("Calling getColumnType on ResultSetMetaData");

  int colType=rsmd->getColumnType(1);


  switch (colType) {
  case sql::DataType::BIT :
  case sql::DataType::TINYINT :
  case sql::DataType::SMALLINT :
  case sql::DataType::INTEGER :
  case sql::DataType::BIGINT :
            // case sql::DataType::FLOAT :
  case sql::DataType::REAL :
  case sql::DataType::DOUBLE :
  case sql::DataType::NUMERIC :
  case sql::DataType::DECIMAL :
  case sql::DataType::CHAR :
  case sql::DataType::VARCHAR :
  case sql::DataType::LONGVARCHAR :
  case sql::DataType::DATE :
  case sql::DataType::TIME :
  case sql::DataType::TIMESTAMP :
  case sql::DataType::BINARY :
  case sql::DataType::VARBINARY :
  case sql::DataType::LONGVARBINARY :
  case sql::DataType::SQLNULL :
            // case sql::DataType::OTHER :
            //            case sql::DataType::JAVA_OBJECT:
            // case sql::DataType::DISTINCT :
            // case sql::DataType::STRUCT :
            // case sql::DataType::ARRAY :
            // case sql::DataType::BLOB :
            // case sql::DataType::CLOB :
            // case sql::DataType::REF :
            TestsListener::messagesLog()
            << "getColumnType method returns: " << colType << std::endl;
    break;
  default:
    TestsListener::errorsLog()
            << "getColumnType method returns a illegal value " << colType << std::endl;
    FAIL("Call to getColumnTypeName failed !");
  }
}

/*
 * @testName:         testGetColumnTypeName
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The getColumnTypeName(int colindex) method returns a String
 *                    object representing the type name used by the database.
 *                    (See section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the
 *                    getColumnTypeName(int colindex) method.It should return a
 *                    String object.
 */

/* throws Exception */
void ResultSetMetadataTest::testGetColumnTypeName()
{
  logMsg("Calling getColumnTypeName on ResultSetMetadata");
  String sRetValue=rsmd->getColumnTypeName(1);

  logMsg(String("getColumnTypeName method returns:  ") + sRetValue);
}

/*
 * @testName:         testGetPrecision
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The getPrecision(int colindex) method returns an integer value
 *                    representing the number of decimal digits for number types,maximum
 *                    length in characters for character types,maximum length in bytes
 *                    for JDBC binary datatypes.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the
 *                    getPrecision(int colindex) method.It should return an integer greater
 *                    than or equal to zero.
 */

/* throws Exception */
void ResultSetMetadataTest::testGetPrecision()
{
  logMsg("Calling getPrecision on ResultSetMetaData");
  int precisionSize=rsmd->getPrecision(1);
  if (precisionSize >= 0) {
    TestsListener::messagesLog()
            << "getPrecision method returns: " << precisionSize << std::endl;
  } else {
    logErr(" getPrecision method returns a negative value");
  }
}

/*
 * @testName:         testGetScale
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The getScale(int colindex) method returns an integer value
 *                    representing the number of digits to right of the decimal point.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the getScale(int colindex)
 *                    method.It should return an integer greater than or equal to zero.
 */

/* throws Exception */
void ResultSetMetadataTest::testGetScale()
{
  logMsg("Calling getScale on ResultSetMetaData");
  int scaleSize=rsmd->getScale(2);
  if (scaleSize >= 0) {
    TestsListener::messagesLog()
            << "getScale method returns: " << scaleSize << std::endl;
  } else {
    logErr(" getScale method returns a negative value");
  }
}

/*
 * @testName:         testGetSchemaName
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The getSchemaName(int colindex) method returns a String
 *                    object representing the schema name or an empty string ("")
 *                    if the DBMS.does not support this feature.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the
 *                    getSchemaName(int colindex) method.It should return a String object.
 */

/* throws Exception */
void ResultSetMetadataTest::testGetSchemaName()
{
  logMsg("Calling getSchemaName on ResultSetMetadata");
  String sRetValue=rsmd->getSchemaName(2);

  logMsg(String("getSchemaName returns:  ") + sRetValue);
}

/*
 * @testName:         testGetTableName
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The getTableName(int colindex) method returns a String
 *                    object representing the table name or an empty string ("")
 *                    if the DBMS does not support this feature.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the getTableName(int colindex)
 *                    method.It should return a String object.
 */

/* throws Exception */
void ResultSetMetadataTest::testGetTableName()
{
  logMsg("Calling getTableName on ResultSetMetadata");
  String sRetValue=rsmd->getTableName(1);

  logMsg(String("getTableName method returns:  ") + sRetValue);
}

/*
 * @testName:         testIsAutoIncrement
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The isAutoIncrement(int colindex) method returns a boolean value;
 *                    true if the column colindex is automatically numbered which makes
 *                    the column read-only and false otherwise.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the isAutoIncrement(int column)
 *                    method.It should return a boolean value.
 */

/* throws Exception */
void ResultSetMetadataTest::testIsAutoIncrement()
{
  logMsg("Calling isAutoIncrement on ResultSetMetaData");
  bool retValue=rsmd->isAutoIncrement(1);
  if (retValue) {
    logMsg(
           "isAutoIncrement method returns column is automatically numbered");
  } else {
    logMsg(
           "isAutoIncrement method returns column cannot be automatically numbered");
  }
}

/* @testName:         testIsCaseSensitive
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The isCaseSensitive(int colindex) method returns a boolean value;
 *                    true if the column colindex is case sensitive and false otherwise.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the isCaseSensitive(int column)
 *                    method.It should return a boolean value.
 */

/* throws Exception */
void ResultSetMetadataTest::testIsCaseSensitive()
{
  logMsg("Calling isCaseSensitive on ResultSetMetaData");
  bool retValue=rsmd->isCaseSensitive(1);
  if (retValue) {
    logMsg(
           "isCaseSensitive method returns column's are case sensitive");
  } else {
    logMsg(
           "isCaseSensitive method returns column's are case insensitive");
  }
}

/* @testName:         testIsCurrency
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The isCurrency(int colindex) method returns a boolean value;
 *                    true if the column colindex is a cash value and false otherwise.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the isCurrency(int column)
 *                    method.It should return a boolean value.
 */

/* throws Exception */
void ResultSetMetadataTest::testIsCurrency()
{
  logMsg("Calling IsCurrency on ResultSetMetaData");
  bool retValue=rsmd->isCurrency(2);
  if (retValue) {
    logMsg("isCurrency method returns column is a cash value");
  } else {
    logMsg(
           "isCurrency method returns column does not contains a cash value");
  }
}

/*
 * @testName:         testIsDefinitelyWritable
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The isDefinitelyWritable(int colindex) method returns a boolean value;
 *                    true if the write on column colindex will definitely succeed
 *                    and false otherwise. (See JDK 1.2.2 API documentation)
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the
 *                    isDefinitelyWritable(int column) method.It should return a boolean value.
 */

/* throws Exception */
void ResultSetMetadataTest::testIsDefinitelyWritable()
{
  logMsg("Calling isDefinitelyWritable on ResultSetMetaData");
  bool retValue=rsmd->isDefinitelyWritable(1);
  if (retValue) {
    logMsg(
           "isDefinitelyWritable method returns write on the column is definitely succeed");
  } else {
    logMsg(
           "isDefinitelyWritable method returns write on the column is definitely failed");
  }
}

/*
 * @testName:         testIsNullable
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The isNullable(int colindex) method returns an integer value
 *                    the possible values are columnNoNulls, columnNullable and
 *                    columnNullableUnknown.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the isNullable(int column)
 *                    method.It should return an integer value which is one of the constants
 *                    columnNoNulls(0),columnNullable(1) and columnNullableUnknown(2).
 */

/* throws Exception */
void ResultSetMetadataTest::testIsNullable()
{
  logMsg("Calling isNullable on ResultSetMetaData");
  int coloumnCount=rsmd->isNullable(2);

  if ((coloumnCount == sql::ResultSetMetaData::columnNoNulls)
      || (coloumnCount == sql::ResultSetMetaData::columnNullable)
      || (coloumnCount == sql::ResultSetMetaData::columnNullableUnknown)) {
    TestsListener::messagesLog()
            << "isNullable method returns: " << coloumnCount << std::endl;
  } else {
    logErr(" isNullable method returns a negative value");
  }
}

/*
 * @testName:         testIsReadOnly
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The isReadOnly(int colindex) method returns a boolean value;
 *                    true if the column colindex is definitely not writable and false
 *                    otherwise.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the isReadOnly(int column)
 *                    method.It should return a boolean value.
 */

/* throws Exception */
void ResultSetMetadataTest::testIsReadOnly()
{
  logMsg("Calling isReadOnly on ResultSetMetaData");
  bool retValue=rsmd->isReadOnly(1);

  if (retValue) {
    logMsg("isReadOnly method returns column cannot be writable");
  } else {
    logMsg("isReadOnly method returns column can be writable");
  }
}

/*
 * @testName:         testIsSearchable
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The isSearchable(int colindex) method returns a boolean value;
 *                    true if the value stored in column colindex can be used in a
 *                    WHERE clause and false otherwise.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the isSearchable(int column)
 *                    method.It should return a boolean value.
 */

/* throws Exception */
void ResultSetMetadataTest::testIsSearchable()
{
  logMsg("Calling isSearchable on ResultSetMetaData");
  bool retValue=rsmd->isSearchable(1);
  if (retValue) {
    logMsg(
           "isSearchable method returns column can be used in a where clause");
  } else {
    logMsg(
           "isSearchable method returns column cannot be used in a where clause");
  }
}

/* @testName:         testIsSigned
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The isSigned(int colindex) method returns a boolean value;
 *                    true if the value stored in column colindex is a signed number
 *                    and false otherwise.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the isSigned(int column)
 *                    method.It should return a boolean value.
 */

/* throws Exception */
void ResultSetMetadataTest::testIsSigned()
{
  logMsg("Calling isSigned on ResultSetMetaData");
  bool retValue=rsmd->isSigned(2);
  if (retValue) {
    logMsg(
           "isSigned method returns values in the column are signed numbers");
  } else {
    logMsg(
           "isSigned method returns values in the column are isUnsigned numbers");
  }
}

/*
 * @testName:         testIsWritable
 * @assertion:        The ResultSetMetaData provides information about the types and
 *                    properties of the columns in a ResultSet object.
 *                    (See section 27.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 *                    A  driver must provide full support for DatabaseMetaData and
 *                    ResultSetMetaData.  This implies that all of the methods in the
 *                    ResultSetMetaData interface must be implemented and must behave as
 *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
 *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
 *                    exception because they are not implemented. (See section 6.2.2.3
 *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
 *
 *                    The isWritable(int colindex) method returns a boolean value;
 *                    true if the column colindex is possible for write and false
 *                    otherwise.
 *                    (See Section 27.3 of JDBC 2.0 API Reference & Tutorial 2nd edition)
 *
 * @test_Strategy:    Get the ResultSetMetaData object from the corresponding ResultSet
 *                    by using the ResultSet's getMetaData method.Call the isWritable(int column)
 *                    method.It should return a boolean value.
 */

/* throws Exception */
void ResultSetMetadataTest::testIsWritable()
{
  logMsg("Calling isWritable on ResultSetMetaData");
  bool retValue=rsmd->isWritable(1);
  if (retValue) {
    logMsg("isWritable method returns column is writable");
  } else {
    logMsg("isWritable method returns column cannot be writable");
  }
}

/**
 * @see junit.framework.TestCase#setUp()
 */

/* throws Exception */
void ResultSetMetadataTest::setUp()
{
  super::setUp();
  createStandardTable(TABLE_CTSTABLE2);

  Properties::const_iterator cit=sqlProps.find("ftable");
  String fTableName;

  if (cit != sqlProps.end()) {
    fTableName=cit->second;
  }

  query=String("SELECT COF_NAME, PRICE FROM ") + fTableName;

  stmt.reset(conn->createStatement(/*sql::ResultSet::TYPE_SCROLL_INSENSITIVE,sql::ResultSet::CONCUR_READ_ONLY*/
                                   ));
  rs.reset(stmt->executeQuery(query));
  rsmd = rs->getMetaData();
}

}
}
