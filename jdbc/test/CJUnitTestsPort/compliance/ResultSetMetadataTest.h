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



#include "../BaseTestFixture.h"

/**
 * @author mmatthew
 *
 * To change this generated comment edit the template variable "typecomment":
 * Window>Preferences>Java>Templates.
 * To enable and disable the creation of type comments go to
 * Window>Preferences>Java>Code Generation.
 */

namespace testsuite
{
namespace compliance
{

class ResultSetMetadataTest : public BaseTestFixture
{
private:
  typedef BaseTestFixture super;

  ResultSetMetaData rsmd;
  String            query;

protected:

  /**
   * @see junit.framework.TestCase#setUp()
   */

  /* throws std::exception * */

  void setUp();
public:

  TEST_FIXTURE(ResultSetMetadataTest)
  {
    TEST_CASE(testGetCatalogName);
    TEST_CASE(testGetColumnClassName);
    TEST_CASE(testGetColumnCount);
    TEST_CASE(testGetColumnDisplaySize);
    TEST_CASE(testGetColumnLabel);
    TEST_CASE(testGetColumnName);
    TEST_CASE(testGetColumnType);
    TEST_CASE(testGetColumnTypeName);
    TEST_CASE(testGetPrecision);
    TEST_CASE(testGetScale);
    TEST_CASE(testGetSchemaName);
    TEST_CASE(testGetTableName);
    TEST_CASE(testIsAutoIncrement);
    TEST_CASE(testIsCaseSensitive);
    TEST_CASE(testIsCurrency);
    TEST_CASE(testIsDefinitelyWritable);
    TEST_CASE(testIsNullable);
    TEST_CASE(testIsReadOnly);
    TEST_CASE(testIsSearchable);
    TEST_CASE(testIsSigned);
    TEST_CASE(testIsWritable);
  }

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

  /* throws std::exception * */

  void testGetCatalogName();
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

  /* throws std::exception * */

  void testGetColumnClassName();
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

  /* throws std::exception * */

  void testGetColumnCount();
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

  /* throws std::exception * */

  void testGetColumnDisplaySize();
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

  /* throws std::exception * */

  void testGetColumnLabel();
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

  /* throws std::exception * */

  void testGetColumnName();
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

  /* throws std::exception * */

  void testGetColumnType();
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

  /* throws std::exception * */

  void testGetColumnTypeName();
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

  /* throws std::exception * */

  void testGetPrecision();
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

  /* throws std::exception * */

  void testGetScale();
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

  /* throws std::exception * */

  void testGetSchemaName();
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

  /* throws std::exception * */

  void testGetTableName();
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

  /* throws std::exception * */

  void testIsAutoIncrement();
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

  /* throws std::exception * */

  void testIsCaseSensitive();
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

  /* throws std::exception * */

  void testIsCurrency();
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

  /* throws std::exception * */

  void testIsDefinitelyWritable();
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

  /* throws std::exception * */

  void testIsNullable();
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

  /* throws std::exception * */

  void testIsReadOnly();
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

  /* throws std::exception * */

  void testIsSearchable();
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

  /* throws std::exception * */

  void testIsSigned();
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

  /* throws std::exception * */

  void testIsWritable();
};
REGISTER_FIXTURE(ResultSetMetadataTest);

}
}
