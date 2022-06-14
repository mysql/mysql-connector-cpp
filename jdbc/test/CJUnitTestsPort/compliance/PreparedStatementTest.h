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

#define __SET_NULL_ADDED_
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

class PreparedStatementTest : public BaseTestFixture
{
private:

  typedef BaseTestFixture super;

  DatabaseMetaData dbmd;

protected:

  /**
   * setUp() function for tests
   */
  /* throws std::exception * */
  void setUp();

public:

  TEST_FIXTURE(PreparedStatementTest)
  {
    TEST_CASE( testGetMetaData    );
    TEST_CASE( testClearParameters);
    TEST_CASE(testExecute01);
    TEST_CASE(testExecute02);
    TEST_CASE(testExecute03);
    TEST_CASE(testExecuteQuery01);
    TEST_CASE(testExecuteQuery02);
    TEST_CASE(testExecuteQuery03);
    TEST_CASE(testExecuteUpdate01);
    TEST_CASE(testExecuteUpdate02);
    TEST_CASE(testExecuteUpdate03);

    TEST_CASE(testSetBoolean01);
    TEST_CASE(testSetBoolean02);

    TEST_CASE(testSetInt01);
    TEST_CASE(testSetInt02);

    TEST_CASE(testSetDouble01);
    TEST_CASE(testSetDouble02);

    TEST_CASE(testSetLong01);
    TEST_CASE(testSetLong02);

    TEST_CASE(testSetString01);
    TEST_CASE(testSetString02);

#ifdef __SET_TIME_ADDED_
    TEST_CASE(testSetTime01);
    TEST_CASE(testSetTime02);
#endif

#ifdef __SET_TIMESTAMP_ADDED_
    TEST_CASE(testSetTimestamp01);
    TEST_CASE(testSetTimestamp02);
#endif

#ifdef __SET_BIG_DECIMAL_ADDED_
    TEST_CASE(testSetBigDecimal01);
    TEST_CASE(testSetBigDecimal02);
#endif

#ifdef __SET_FLOAT_ADDED_
    TEST_CASE(testSetFloat01);
    TEST_CASE(testSetFloat02);
#endif

#ifdef __SET_BYTE_ADDED_
    TEST_CASE(testSetByte01);
    TEST_CASE(testSetByte02);
#endif

#ifdef __SET_SHORT_ADDED_
    TEST_CASE(testSetShort01);
    TEST_CASE(testSetShort02);
#endif

#ifdef __SET_FLOAT_ADDED_
    TEST_CASE(testSetFloat010101);
    TEST_CASE(testSetFloat020202);
#endif

#ifdef __SET_DATE_ADDED_
    TEST_CASE(testSetDate01);
    TEST_CASE(testSetDate02);
#endif

#ifdef __SET_NULL_ADDED_
    TEST_CASE(testSetNull01);
    TEST_CASE(testSetNull02);
    TEST_CASE(testSetNull03);
    TEST_CASE(testSetNull04);

// Assuming that set/getTiem and Timestamp will be added along w/ setDate
#ifdef __SET_DATE_ADDED_
    TEST_CASE(testSetNull05);
    TEST_CASE(testSetNull06);
    TEST_CASE(testSetNull07);
#endif

#ifdef __SET_BIG_DECIMAL_ADDED_
    TEST_CASE(testSetNull08);
#endif

    TEST_CASE(testSetNull10);
    TEST_CASE(testSetNull11);
    TEST_CASE(testSetNull12);
    TEST_CASE(testSetNull13);

#ifdef _WE_HAVE_SOME_REAL_TYPE
    TEST_CASE(testSetNull14);
#endif

#ifdef __SET_BIG_DECIMAL_ADDED_
    TEST_CASE(testSetNull15);
#endif

#ifdef __SET_BYTE_ADDED_
    TEST_CASE(testSetNull09);

    TEST_CASE(testSetNull16);
    TEST_CASE(testSetNull17);
    TEST_CASE(testSetNull18);
#endif
#endif

  }
  /*
   * @testName:         testGetMetaData
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods.  (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The getMetadata() method returns a valid ResultSetMetaData Object
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Execute the getMetaData() method and get the number of columns using
   *		      getColumnCount() method of ResultSetMetaData.Execute a query using
   *		      executeQuery() method and get the number of columns. Both the values
   *		      should be equal or it should throw an SQL exception.
   *
   */


  /* throws std::exception * */

  void testGetMetaData();
  /*
   * @testName:         testClearParameters
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0
   *                    API Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for Prepared Statement
   *                    methods.  (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The method clearParameters() clears the values set for the
   *                    PreparedStatement object's IN parameters and releases the
   *                    resources used by those values.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Set the value for the IN parameter of the Prepared Statement object.
   *                    Call the clearParameters() method.Call the executeQuery() method
   *                    to check if the call to clearParameters() clears the IN parameter
   *                    set by the Prepared Statement object.
   */


  /* throws std::exception * */

  void testClearParameters();
  /*
   * @testName:         testExecute01
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The execute()method executes the SQL statement contained in
   *                    the Prepared Statement object and indicates whether the first
   *                    result is a resultset,an update count,or there are no results.
   *                    (See JDK 1.2.2 API Documentation)
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Set the value for the IN parameter of the Prepared Statement
   *                    object. Execute the precompiled SQL Statement of deleting a row.
   *                    It should return a boolean value and the value should be equal to
   *                    false.  (See JDK 1.2.2 API documentation)
   *
   */


  /* throws std::exception * */

  void testExecute01();
  /*
   * @testName:         testExecute02
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0
   *                    API Reference & Tutorial,Second Edition)
   *                    The drivers must provide full support for Prepared Statement
   *                    methods.  (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The execute()method executes the SQL statement contained in the
   *                    Prepared Statement object and indicates whether the first result
   *                    is a resultset,an update count,or there are no results.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Set the value for the IN parameter of the Prepared Statement object.
   *                    Execute the precompiled SQL Statement by calling executeQuery()
   *                    method with a non-existent row.Call ResultSet.next() method.
   *                    It should return a boolean value and the value should be equal
   *                    to false. (See JDK 1.2.2 API documentation)
   *
   */


  /* throws std::exception * */

  void testExecute02();
  /*
   * @testName:         testExecute03
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    edition(J2EE) specification v 1.2).
   *
   *                    The execute() method executes the SQL statement contained in
   *                    the Prepared Statement object and indicates whether the first
   *                    result is a resultset,an update count,or there are no results.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Execute the precompiled SQL Statement by calling execute() method
   *                    without setting the parameters.An SQL std::exception * must be thrown.
   *                    (See JDK 1.2.2 API documentation)
   *
   */


  /* throws std::exception * */

  void testExecute03();
  /*
   * @testName:         testExecuteQuery01
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0
   *                    API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The executeQuery() method executes SQL query in this
   *                    PreparedStatement object and returns the result set generated
   *                    by the query.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Set the value for the IN parameter of the Prepared Statement object.
   *                    Execute the precompiled SQL Statement by calling executeQuery()
   *                    method. It should return a ResultSet object.
   *
   */


  /* throws std::exception * */

  void testExecuteQuery01();
  /*
   * @testName:         testExecuteQuery02
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The executeQuery() method executes SQL query in this
   *                    PreparedStatement object and returns the result set generated
   *                    by the query. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Set the value for the IN parameter of the Prepared Statement object.
   *                    Execute the precompiled SQL Statement by calling executeQuery()
   *                    method with a non existent row.  A call to ResultSet.next()
   *                    should return a false value.
   */


  /* throws std::exception * */

  void testExecuteQuery02();
  /*
   * @testName:         testExecuteQuery03
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters).(See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The executeQuery() method executes SQL query in this
   *                    PreparedStatement object and returns the result set generated
   *                    by the query. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the
   *                    database. Execute the precompiled SQL Statement by calling
   *                    executeQuery() method without setting the parameters.
   *                    It should throw a SQL std::exception *.
   */


  /* throws std::exception * */

  void testExecuteQuery03();
  /*
   * @testName:         testExecuteUpdate01
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.  It can take one or more parameters as input
   *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The executeUpdate(String sql) method executes an SQL Insert,
   *                    Update or Delete Statement and returns the number of rows that
   *                    were affected.It can also be used to execute SQL statements
   *                    that have no return value such as DDL statements that create
   *                    or drop tables.  (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the
   *                    database. Set the value for the IN parameter of the PreparedStatement
   *                    object. Execute the precompiled SQL Statement by calling
   *                    executeUpdate() method. It should return an integer
   *                    value indicating the number of rows that were affected.
   *                    (The value could be zero if zero rows are affected).
   */


  /* throws std::exception * */

  void testExecuteUpdate01();
  /*
   * @testName:         testExecuteUpdate02
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters) (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The executeUpdate() method executes an SQL Insert,
   *                    Update or Delete Statement and returns the number of rows that
   *                    were affected.It can also be used to execute SQL statements
   *                    that have no return value such as DDL statements that create
   *                    or drop tables.(The value could be zero if zero rows are affected).
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Set the value for the IN parameter of the Prepared Statement
   *                    object. Execute the precompiled SQL Statement by calling
   *                    executeUpdate() method with a non existent row.
   *                    It should return an Integer value.
   */


  /* throws std::exception * */

  void testExecuteUpdate02();
  /*
   * @testName:         testExecuteUpdate03
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods.  (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The executeUpdate() method executes an SQL Insert,Update or
   *                    Delete Statement and returns the number of rows that were affected.
   *                    It can also be used to execute SQL statements that have no
   *                    return value such as DDL statements that create or drop tables.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Execute the precompiled SQL Statement without setting the IN parameter.
   *                    It should throw an SQL exception.
   */


  /* throws std::exception * */

  void testExecuteUpdate03();
  /*
   * @testName:         testSetBigDecimal01
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition).
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods.  (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setBigDecimal(int parameterIndex, BigDecimal x) method
   *                    sets the parameterIndex to x which is an instance of
   *                    java.math.BigDecimal class. The Driver converts this  to a
   *                    JDBC Numeric Value when it sends it to the database. (See
   *                    section 24.3.2 of JDBC 2.0 API Tutorial & Reference,2nd Edition).
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Execute the precompiled.SQL Statement by calling the
   *                    setBigDecimal(int parameterindex, BigDecimal x) method for updating
   *                    the value of column MIN_VAL in Numeric_Tab.Check first
   *                    the return value of executeUpdate() method used is equal to 1.
   *                    Call the ResultSet.getBigDecimal(int columnIndex)method.
   *                    Check if returns the BigDecimal Value that has been set.
   */


  /* throws std::exception * */

  //    void testSetBigDecimal01();
  /*
   * @testName:         testSetBigDecimal02
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters) (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setBigDecimal(int parameterIndex, BigDecimal x) method
   *                    sets the parameterIndex to x which is an instance of
   *                    java.math.BigDecimal class.The Driver converts this to a JDBC
   *                    Numeric Value when it sends it to the database.(See section
   *                    24.3.2 of JDBC 2.0 API Tutorial & Reference, Second Edition).
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Execute the precompiled SQL Statement by calling the
   *                    setBigDecimal(int parameterindex, BigDecimal x) method for updating
   *                    the value of column NULL_VAL in Numeric_Tab.  Call the
   *                    ResultSet.getBigDecimal(int columnIndex) method. Check if
   *                    returns the BigDecimal Value that has been set.
   */

  /* throws std::exception * */

  //    void testSetBigDecimal02();
  /*
   * @testName:         testSetBoolean01
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition).
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setBoolean(int parameterIndex, boolean x) method sets the
   *                    parameterIndex to x, a Java boolean value.The driver converts
   *                    this to JDBC BIT value when it sends it to the database. (See
   *                    section 24.3.2 of JDBC 2.0 API Tutorial & Reference, 2nd Edition).
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Execute the precompiled SQL Statement by calling the
   *                    setBoolean(int parameterIndex, boolean x) to set MAX_VAL
   *                    column of Bit_tab to the boolean value x.
   *                    Call the ResultSet.getBoolean(int columnIndex) method to check
   *                    if it returns the boolean Value that has been set.
   *
   */

  /* throws std::exception * */

  void testSetBoolean01();
  /*
   * @testName:         testSetBoolean02
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setBoolean(int parameterIndex, boolean x) method sets the
   *                    parameterIndex to x, a Java boolean value.The driver converts this
   *                    to JDBC BIT value when it sends it to the database. (See section
   *                    24.3.2 of JDBC 2.0 API Tutorial & Reference, Second Edition).
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Execute the precompiled SQL Statement by calling the method
   *                    setBoolean(int parameterIndex, boolean x) to set NULL_VAL
   *                    column of Bit_tab to the boolean value x.
   *                    Call the ResultSet.getBoolean(int columnIndex) method to
   *                    check if it returns the boolean Value that has been set.
   */

  /* throws std::exception * */

  void testSetBoolean02();
  /*
   * @testName:         testSetByte01
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setByte(int parameterIndex, byte x) method sets the parameter
   *                    number parameterIndex to x,a Java byte value.The Driver converts
   *                    this to a JDBC Tinyint value when it sends it to the database. (See
   *                    section 24.3.2 of JDBC 2.0 API Tutorial & Reference, 2nd Edition).
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Execute the precompiled SQL Statement by calling the
   *                    setByte(int parameterindex, byte x) method for updating MAX_VAL
   *                    column of Tinyint_Tab.  Call the ResultSet.getByte(int columnIndex)
   *                    method to check if it returns the byte Value that has been set.
   */

  /* throws std::exception * */

  void testSetByte01();
  /*
   * @testName:         testSetByte02
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setByte(int parameterIndex, byte x) method sets the parameter
   *                    number parameterIndex to x,a Java byte value.  The Driver converts
   *                    this to a JDBC Tinyint value when it sends it to the database.
   *                    (See section 24.3.2 of JDBC 2.0 API Tutorial & Reference,2nd Edition)
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Execute the precompiled SQL Statement by calling the
   *                    setByte(int parameterindex, byte x) method for updating NULL_VAL
   *                    column of Tinyint_Tab.  Call the ResultSet.getByte(int columnIndex)
   *                    method,to check if it returns the byte Value that has been set.
   */


  /* throws std::exception * */

  void testSetByte02();
  /*
   * @testName:         testSetFloat01
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods.  (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setFloat(int parameterIndex, float x) method sets the parameter
   *                    number parameterIndex to x ,a Java float value.  The Driver converts
   *                    this to a JDBC REAL when it sends it to the database. (See section
   *                    24.3.2 of JDBC 2.0 API Tutorial & Reference,2nd Edition).
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database
   *                    execute the precompiled SQL Statement by calling the
   *                    setFloat(int parameterindex, float x) method for updating
   *                    the MAX_VAL column of Float_Tab.  Call the
   *                    ResultSet.getFloat(int columnIndex)
   *                    method to check if it returns the float Value that has been set.
   *
   */


  /* throws std::exception * */

  void testSetFloat010101();
  /*
   * @testName:         testSetFloat02
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements.It can take one or more parameters as input
   *                    arguments(IN parameters).  (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setFloat(int parameterIndex, float x) method sets the
   *                    parameter number parameterIndex to x ,a Java float value.
   *                    The Driver converts this to a JDBC REAL when it sends it to
   *                    the database. (See section 24.3.2 of JDBC 2.0 API Tutorial &
   *                    Reference, Second Edition).
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database
   *                    execute the precompiled SQL Statement by calling the
   *                    setFloat(int parameterindex, float x)  method for updating the
   *                    NULL_VAL column of Float_Tab.  Call the
   *                    ResultSet.getFloat(int columnIndex) method to check if it returns
   *                    the float Value that has been set.
   *
   */

  /* throws std::exception * */

  void testSetFloat020202();
  /*
   * @testName:         testSetInt01
   * @assertion:        A Prepared Statement object provides a way of calling precompiled
   *                    SQL statements. It can take one or more parameters as input
   *                    arguments(IN parameters). (See section 11.1.6 of JDBC 2.0 API
   *                    Reference & Tutorial,Second Edition)
   *
   *                    The JDBC drivers must provide full support for PreparedStatement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setInt(int parameterIndex, int x) method sets the parameter
   *                    number parameterIndex to x ,a Java int value.  The Driver converts
   *                    this to a JDBC INTEGER when it sends it to the database. (See
   *                    section 24.3.2 of JDBC 2.0 API Tutorial & Reference, 2nd Edition)
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database
   *                    execute the precompiled SQL Statement by calling the
   *                    setInt(int parameterindex, int x) method for updating the MAX_VAL
   *                    column of Integer_Tab.  Call the ResultSet.getInt(int columnIndex)
   *                    method to check if it returns the integer Value that has been set.
   *
   */

  /* throws std::exception * */

  void testSetInt01();
  /*
   * @testName:         testSetInt02
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setInt(int parameterIndex,int x) method sets parameter number
   *                    parameterIndex to Java int value x. The driver converts this to an
   *                    SQL INTEGER value when it sends it to the Database
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database
   *                    Using setInt(int parameterIndex,int x),update the column with the maximum
   *                    value of Integer_Tab. Now execute a query to get the maximum value and
   *                    retrieve the result of the query using the getInt(int columnIndex) method
   *                    Compare the returned value, with the maximum value extracted from the
   *                    ctssql.stmt file. Both of them should be equal
   */


  /* throws std::exception * */

  void testSetInt02();
  /*
   * @testName:         testSetDate01
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setDate(int parameterIndex,Date x) method sets parameter
   *                    number parameterIndex to java.sql.Date value x. The driver
   *                    converts this to an SQL DATE value when it sends it to the database
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database
   *                    execute the precompiled SQL Statement by calling setDate(int parameterIndex,Date x)
   *                    method and call the ResultSet.getDate(int) method to check and
   *                    it should return a String Value that it is been set
   */


  /* throws std::exception * */

  void testSetDate01();
  /*
   * @testName:         testSetDate02
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setDate(int parameterIndex,Date x,Calendar cal) method
   *                    sets the designated parameter to the given java.sql.Date value,
   *                    using the given Calendar object. The driver uses the Calendar
   *                    object to construct an SQL DATE value, which the driver then sends
   *                    to the database. With a a Calendar object, the driver can calculate
   *                    the date taking into account a custom timezone. If no Calendar
   *                    object is specified, the driver uses the default timezone, which
   *                    is that of the virtual machine running the application.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database
   *                    execute the precompiled SQL Statement by calling
   *                    setDate(int parameterIndex,Date x,Calendar cal) method
   *                    and call the ResultSet.getDate(int) method to check and
   *                    it should return a String Value that it is been set
   */


  /* throws std::exception * */

  void testSetDate02();
  /*
   * @testName:         testSetDouble01
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setDouble(int parameterIndex, double x) method Sets the
   *                    designated parameter to a Java double value. The driver converts
   *                    this to an SQL DOUBLE value when it sends it to the database.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database
   *                    Using setDouble(int parameterIndex,double x),update the column the
   *                    minimum value of Double_Tab.
   *                    Now execute a query to get the minimum value and retrieve the result
   *                    of the query using the getDouble(int columnIndex) method.Compare the
   *                    returned value, with the minimum value extracted from the ctssql.stmt file.
   *                    Both of them should be equal.
   */


  /* throws std::exception * */

  void testSetDouble01();
  /*
   * @testName:         testSetDouble02
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setDouble(int parameterIndex, double x) method Sets the
   *                    designated parameter to a Java double value. The driver converts
   *                    this to an SQL DOUBLE value when it sends it to the database.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Using setDouble(int parameterIndex,double x),update the column
   *                    the maximum value of Double_Tab.
   *                    Now execute a query to get the maximum value and retrieve the result
   *                    of the query using the getDouble(int columnIndex) method.Compare the
   *                    returned value, with the maximum value extracted from the ctssql.stmt file.
   *                    Both of them should be equal.
   */


  /* throws std::exception * */

  void testSetDouble02();
  /*
   * @testName:         testSetLong01
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setLong(int parameterIndex, long x) method Sets the
   *                    designated parameter to a Java long value. The driver converts
   *                    this to an SQL BIGINT value when it sends it to the database.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Using setLong(int parameterIndex,long x),update the column the
   *                    minimum value of Bigint_Tab.
   *                    Now execute a query to get the minimum value and retrieve the result
   *                    of the query using the getLong(int columnIndex) method.Compare the
   *                    returned value, with the minimum value extracted from the ctssql.stmt file.
   *                    Both of them should be equal.
   */


  /* throws std::exception * */

  void testSetLong01();
  /*
   * @testName:         testSetLong02
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setLong(int parameterIndex, long x) method Sets the
   *                    designated parameter to a Java long value. The driver converts
   *                    this to an SQL BIGINT value when it sends it to the database.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Using setLong(int parameterIndex,long x),update the column the
   *                    maximum value of Bigint_Tab.
   *                    Now execute a query to get the maximum value and retrieve the result
   *                    of the query using the getLong(int columnIndex) method.Compare the
   *                    returned value, with the maximum value extracted from the ctssql.stmt file.
   *                    Both of them should be equal.
   */


  /* throws std::exception * */

  void testSetLong02();
  /*
   * @testName:         testSetShort01
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setShort(int parameterIndex, short x) method Sets the
   *                    designated parameter to a Java short value. The driver converts
   *                    this to an SQL SMALLINT value when it sends it to the database.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Using setShort(int parameterIndex,short x),update the column the
   *                    minimum value of Smallint_Tab.
   *                    Now execute a query to get the minimum value and retrieve the result
   *                    of the query using the getShort(int columnIndex) method.Compare the
   *                    returned value, with the minimum value extracted from the ctssql.stmt file.
   *                    Both of them should be equal.
   */


  /* throws std::exception * */

  void testSetShort01();
  /*
   * @testName:         testSetShort02
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setShort(int parameterIndex, short x) method Sets the
   *                    designated parameter to a Java short value. The driver converts
   *                    this to an SQL SMALLINT value when it sends it to the database.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Using setShort(int parameterIndex,short x),update the column the
   *                    maximum value of Smallint_Tab.
   *                    Now execute a query to get the maximum value and retrieve the result
   *                    of the query using the getShort(int columnIndex) method.Compare the
   *                    returned value, with the maximum value extracted from the ctssql.stmt file.
   *                    Both of them should be equal.
   */
  /* throws std::exception * */
  void testSetShort02();


  /*
   * @testName:         testSetString01
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setString(int parameterIndex, String x) method sets the designated
   *                    parameter to a Java String value. The driver converts this to an
   *                    SQL VARCHAR or LONGVARCHAR value (depending on the argument's size
   *                    relative to the driver's limits on VARCHAR values) when it sends
   *                    it to the database.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Using the setString(int parameterIndex, String x) method,update the
   *                    column value with the maximum value of Char_Tab. Call the
   *                    getString(String columnName) method to retrieve this value. Extract
   *                    the maximum value from the ctssql.stmt file. Compare this value
   *                    with the value returned by the getString(String columnName) method.
   *                    Both the values should be equal.
   */
  /* throws std::exception * */
  void testSetString01();


#ifdef __SET_TIME_ADDED_
  /*
   * @testName:         testSetTime01
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setTime(int parameterIndex, Time x) method sets the
   *                    designated parameter to a java.sql.Time value. The driver
   *                    converts this to an SQL TIME value when it sends it to the database.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Using the setTime(int parameterIndex, Time x) method,update the
   *                    column value with the Non-Null Time value. Call the getTime(int columnno)
   *                    method to retrieve this value. Extract the Time value
   *                    from the ctssql.stmt file. Compare this value with the value returned
   *                    by the getTime(int columnno)  method. Both the values should be equal
   */
  /* throws std::exception * */
  void testSetTime01();


  /*
   * @testName:         testSetTime02
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setTime(int parameterIndex, String x, Calendar cal) method sets the
   *                    designated parameter to the given java.sql.Time value, using the given
   *                    Calendar object. The driver uses the Calendar object to construct an
   *                    SQL TIME value, which the driver then sends to the database. With a Calendar
   *                    object, the driver can calculate the time taking into account a custom
   *                    timezone. If no Calendar object is specified, the driver uses the default
   *                    timezone, which is that of the virtual machine running the application.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Using the setTime(int parameterIndex, Time x, Calendar cal) method,update the
   *                    column value with the Non-Null Time value using the Calendar Object. Call the
   *                    getTime(int columnno) method to retrieve this value. Extract the Time value
   *                    from the ctssql.stmt file. Compare this value with the value returned
   *                    by the getTime(int columnno)  method. Both the values should be equal.
   */
  /* throws std::exception * */
  void testSetTime02();
#endif


#ifdef __SET_TIMESTAMP_ADDED_
  /*
   * @testName:         testSetTimestamp01
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setTimestamp(int parameterIndex,Timestamp x) method sets
   *                    the designated parameter to a java.sql.Timestamp value.
   *                    The driver converts this to an SQL TIMESTAMP value when it
   *                    sends it to the database.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Using the setTimestamp(int parameterIndex, Timestamp x) method,update the
   *                    column value with the Non-Null Timestamp value. Call the getTimestamp(int columnno)
   *                    method to retrieve this value. Extract the Timestamp value
   *                    from the ctssql.stmt file. Compare this value with the value returned
   *                    by the getTimestamp(int columnno)  method. Both the values should be equal.
   */
  /* throws std::exception * */
  void testSetTimestamp01();


  /*
   * @testName:         testSetTimestamp02
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setTimestamp(int parameterIndex, String x, Calendar cal) method sets the
   *                    designated parameter to the given java.sql.Timestamp value, using the given
   *                    Calendar object. The driver uses the Calendar object to construct an
   *                    SQL TIMESTAMP value, which the driver then sends to the database. With a Calendar
   *                    object, the driver can calculate the time taking into account a custom
   *                    timezone. If no Calendar object is specified, the driver uses the default
   *                    timezone, which is that of the virtual machine running the application.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Using the setTimestamp(int parameterIndex, Time x, Calendar cal) method,update the
   *                    column value with the Non-Null Timestamp value using the Calendar Object. Call the
   *                    getTimestamp(int columnno) method to retrieve this value. Extract the Timestamp value
   *                    from the ctssql.stmt file. Compare this value with the value returned
   *                    by the getTimestamp(int columnno) method. Both the values should be equal.
   */
  /* throws std::exception * */
  void testSetTimestamp02();
#endif


  /*
   * @testName:         testSetString02
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setString(int parameterIndex, String x) method sets the designated
   *                    parameter to a Java String value. The driver converts this to an
   *                    SQL VARCHAR or LONGVARCHAR value (depending on the argument's size
   *                    relative to the driver's limits on VARCHAR values) when it sends
   *                    it to the database.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Using setString(int parameterIndex, String x),update the column
   *                    with the maximum value which is a SQL VARCHAR. Call the
   *                    getString(int ColumnIndex) method to retrieve this value. Extract
   *                    the maximum value as a String from the ctssql.stmt file.
   *                    Compare this value with the value returned by the getString method.
   *                    Both the values should be equal.
   */


  /* throws std::exception * */
  void testSetString02();

#ifdef __SET_FLOAT_ADDED_
  /*
   * @testName:         testSetFloat01
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setFloat(int parameterIndex, float x) sets the designated parameter
   *                    to a Java float value. The driver converts this to an SQL FLOAT value
   *                    when it sends it to the database.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Using setFloat(int parameterIndex,float x),update the column with the
   *                    minimum value of Real_Tab.
   *                    Now execute a query to get the minimum value and retrieve the result
   *                    of the query using the getFloat(int columnIndex) method.Compare the
   *                    returned value, with the minimum value extracted from the ctssql.stmt file.
   *                    Both of them should be equal.
   */
  /* throws std::exception * */
  void testSetFloat01();


  /*
   * @testName:         testSetFloat02
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setFloat(int parameterIndex, float x) sets the designated parameter
   *                    to a Java float value. The driver converts this to an SQL FLOAT value
   *                    when it sends it to the database.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    Using setFloat(int parameterIndex,float x),update the column with the
   *                    maximum value of Real_Tab.
   *                    Now execute a query to get the maximum value and retrieve the result
   *                    of the query using the getFloat(int columnIndex) method.Compare the
   *                    returned value, with the maximum value extracted from the ctssql.stmt file.
   *                    Both of them should be equal.
   */
  /* throws std::exception * */
  void testSetFloat02();
#endif


#ifdef __SET_NULL_ADDED_
  /*
   * @testName:         testSetNull01
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for INTEGER Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   *
   */
  /* throws std::exception * */
  void testSetNull01();


  /*
   * @testName:         testSetNull02
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for FLOAT Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   */
  /* throws std::exception * */
  void testSetNull02();


  /*
   * @testName:         testSetNull03
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for SMALLINT Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   */
  /* throws std::exception * */
  void testSetNull03();


  /*
   * @testName:         testSetNull04
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for CHAR Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   */
  /* throws std::exception * */
  void testSetNull04();


  /*
   * @testName:         testSetNull05
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2)
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for TIME Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   */
  /* throws std::exception * */
  void testSetNull05();


  /*
   * @testName:         testSetNull06
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for TIMESTAMP Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   */
  /* throws std::exception * */
  void testSetNull06();


  /*
   * @testName:         testSetNull07
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for DATE Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   */
  /* throws std::exception * */
  void testSetNull07();


  /*
   * @testName:         testSetNull08
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for NUMERIC Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   */
  /* throws std::exception * */
  void testSetNull08();


  /*
   * @testName:         testSetNull09
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for TINYINT Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   */
  /* throws std::exception * */
  void testSetNull09();

  /*
   * @testName:         testSetNull10
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for DOUBLE Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   */
  /* throws std::exception * */
  void testSetNull10();


  /*
   * @testName:         testSetNull11
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for BIGINT Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   */
  /* throws std::exception * */
  void testSetNull11();


  /*
   * @testName:         testSetNull12
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for Varchar Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   *
   */
  /* throws std::exception * */
  void testSetNull12();

  /*
   * @testName:         testSetNull13
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for LONGVARCHAR Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   *
   */
  /* throws std::exception * */
  void testSetNull13();


  /*
   * @testName:         testSetNull14
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for REAL Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   *
   */
  /* throws std::exception * */
  void testSetNull14();


  /*
   * @testName:         testSetNull15
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for DECIMAL Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   *
   */
  /* throws std::exception * */
  void testSetNull15();


  /*
   * @testName:         testSetNull16
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for BINARY Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   *
   */
  /* throws std::exception * */
  void testSetNull16();


  /*
   * @testName:         testSetNull17
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for VARBINARY Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   *
   */
  /* throws std::exception * */
  void testSetNull17();


  /*
   * @testName:         testSetNull18
   * @assertion:        A Prepared Statement object provides a way of calling
   *                    precompiled SQL statements.It can take one or more parameters
   *                    as input arguments(IN parameters). (See section 11.1.6 of
   *                    JDBC 2.0 API Reference & Tutorial,Second Edition)
   *
   *                    The drivers must provide full support for Prepared Statement
   *                    methods. (See section 6.2.2.3 of Java2 Platform Enterprise
   *                    Edition(J2EE) specification v 1.2).
   *
   *                    The setNull(int parameterIndex, int sqlType) method sets the
   *                    designated parameter to SQL NULL(the generic SQL NULL defined in
   *                    java.sql.Types). Note that the JDBC type of the parameter to be
   *                    set to JDBC NULL must be specified.
   *
   * @test_Strategy:    Get a PreparedStatement object from the connection to the database.
   *                    execute the precompiled SQL Statement to set the value as SQL Null
   *                    for LONGVARBINARY Type and retrieve the same value by executing a query. Call
   *                    the ResultSet.wasNull() method to check it. It should return a true value.
   *
   */
  /* throws std::exception * */
  void testSetNull18();
#endif
};

REGISTER_FIXTURE(PreparedStatementTest);

} //namespace compliance
} //namespace testsuite
