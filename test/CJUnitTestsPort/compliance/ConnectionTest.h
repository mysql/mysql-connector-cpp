/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "../BaseTestFixture.h"

/*
#include "cppconn/connection.h"
#include "cppconn/metadata.h"
#include "cppconn/resultset.h"
#include "cppconn/exception.h"
#include "cppconn/statement.h"*/



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

class ConnectionTest : public BaseTestFixture
{
private:

  typedef BaseTestFixture super;

  DatabaseMetaData dbmd;

protected:

  /**
   * @see junit.framework.TestCase#setUp()
   */

  /* throws std::runtime_error * */
  void setUp();

public:

  /**
   * Constructor for ConnectionTest.
   * @param name
   */

  TEST_FIXTURE(ConnectionTest)
  {    
    TEST_CASE(testClose);
    TEST_CASE(testCreateStatement01);
    TEST_CASE(testGetCatalog);
    TEST_CASE(testGetMetaData);
    TEST_CASE(testGetTransactionIsolation);
    TEST_CASE(testIsClosed01);
    TEST_CASE(testIsClosed02);

#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
    TEST_CASE(testIsReadOnly);
#endif

    TEST_CASE(testNativeSQL);   
  }

  /*
   * @testName:         testClose
   *
   * @assertion:        A Connection object represents a Connection in a database.
   *                    A Connection session includes the SQL Statements that are
   *                    executed and the results that are returned over that connection.
   *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
   *                    edition).
   *
   *                    The JDBC drivers must provide accurate and complete
   *                    metadata through the Connection.getMetaData() method. (See
   *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
   *                    specification v1.2).
   *
   *                    The Close method closes the connection object and the close()
   *                    method does not return any value. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get a Connection object and call close() method
   *                    and call isClosed() method and it should return a true value
   *
   */

  /* throws std::runtime_error * */

  void testClose();
  /*
   * @testName:         testCreateStatement01
   *
   * @assertion:        A Connection object represents a Connection in a database.
   *                    A Connection session includes the SQL Statements that are
   *                    executed and the results that are returned over that connection.
   *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
   *                    edition).
   *
   *                    The JDBC drivers must provide accurate and complete
   *                    metadata through the Connection.getMetaData() method. (See
   *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
   *                    specification v1.2).
   *
   *                    The createStatement() method returns  a Statement object that
   *                    will produce non-scrollable and non-updatable result set.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Connection object and call createStatement()
   *                    method and call instanceof to check
   *                    It should return a Statement object
   */

  /* throws std::runtime_error * */

  void testCreateStatement01();
  /*
   * @testName:         testGetCatalog
   *
   * @assertion:        A Connection object represents a Connection in a database.
   *                    A Connection session includes the SQL Statements that are
   *                    executed and the results that are returned over that connection.
   *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
   *                    edition)
   *
   *                    The JDBC drivers must provide accurate and complete
   *                    metadata through the Connection.getMetaData() method. (See
   *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
   *                    specification v1.2)
   *
   *                    The getCatalog() method returns a String object; the string
   *                    represents the connection object's catalog name and null if there
   *                    is none.  (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Connection object and call getCatalog() method
   *                    It should return a String value The getCatalogs() method in
   *                    Databasemeta data object will return a Resultset object that contains
   *                    the catalog name in the column TABLE_CAT .The String returned by
   *                    Connection.getCatalog() method will be checked against these
   *                    column values.
   */

  /* throws std::runtime_error * */

  void testGetCatalog();
  /*
   * @testName:         testGetMetaData
   *
   * @assertion:        A Connection object represents a Connection in a database.
   *                    A Connection session includes the SQL Statements that are
   *                    executed and the results that are returned over that connection.
   *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
   *                    edition)
   *
   *                    The JDBC drivers must provide accurate and complete
   *                    metadata through the Connection.getMetaData() method. (See
   *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
   *                    specification v1.2)
   *
   *                    The getMetaData method returns a DatabaseMetaData object.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Connection object and call getMetaData()
   *                    method and call insctanceof method to check
   *                    It should return a DatabaseMetaData object
   *
   */

  /* throws std::runtime_error * */

  void testGetMetaData();
  /*
   * @testName:         testGetTransactionIsolation
   *
   * @assertion:        A Connection object represents a Connection in a database.
   *                    A Connection session includes the SQL Statements that are
   *                    executed and the results that are returned over that connection.
   *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
   *                    edition).
   *
   *                    The JDBC drivers must provide accurate and complete
   *                    metadata through the Connection.getMetaData() method. (See
   *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
   *                    specification v1.2)
   *
   *                    The getTransactionIsolation method returns an int value and must
   *                    be equal to the value of  TRANSACTION_NONE or
   *                    TRANSACTION_READ_COMMITTED or TRANSACTION_READ_UNCOMMITTED
   *                    or TRANSACTION_REPEATABLE_READ or TRANSACTION_SERIALIZABLE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Connection object and call getTransactionIsolation() method
   *                    It should return a Integer value and must be equal to the value of
   *                    TRANSACTION_NONE or TRANSACTION_READ_COMMITTED or
   *                    TRANSACTION_READ_UNCOMMITTED or TRANSACTION_REPEATABLE_READ or
   *                    TRANSACTION_SERIALIZABLE which is default set by the driver
   *
   *
   */
  /* throws std::runtime_error * */
  void testGetTransactionIsolation();


  /*
   * @testName:         testIsClosed01
   *
   * @assertion:        A Connection object represents a Connection in a database.
   *                    A Connection session includes the SQL Statements that are
   *                    executed and the results that are returned over that connection.
   *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
   *                    edition).
   *
   *                    The JDBC drivers must provide accurate and complete
   *                    metadata through the Connection.getMetaData() method. (See
   *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
   *                    specification v1.2).
   *
   *                    The isClosed method returns a boolean value; true if the
   *                    connection is closed or false if it is still open.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Connection object and call isClosed() method
   *                    It should return a boolean value and the value should be
   *                    equal to false
   *
   */
  /* throws std::runtime_error * */
  void testIsClosed01();


  /*
   * @testName:         testIsClosed02
   *
   * @assertion:        A Connection object represents a Connection in a database.
   *                    A Connection session includes the SQL Statements that are
   *                    executed and the results that are returned over that connection.
   *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
   *                    edition).
   *
   *                    The JDBC drivers must provide accurate and complete
   *                    metadata through the Connection.getMetaData() method. (See
   *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
   *                    specification v1.2)
   *
   *                    The isClosed method returns a boolean value; true if the
   *                    connection is closed or false if it is still open.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Connection object and call close() method
   *                    and call isClosed() method
   *                    It should return a boolean value and the value should be
   *                    equal to true
   *
   */
  /* throws std::runtime_error * */
  void testIsClosed02();


#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  /*
   * @testName:         testIsReadOnly
   *
   * @assertion:        A Connection object represents a Connection in a database.
   *                    A Connection session includes the SQL Statements that are
   *                    executed and the results that are returned over that connection.
   *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
   *                    edition).
   *
   *                    The JDBC drivers must provide accurate and complete
   *                    metadata through the Connection.getMetaData() method. (See
   *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
   *                    specification v1.2).
   *
   *                    The isReadOnly method returns a boolean value; true if the
   *                    connection is in read-only mode and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a Connection object and call setReadOnly(boolean b)
   *                    method and call isReadOnly() method
   *                    It should return a boolean value that is been set
   *
   */
  /* throws std::runtime_error * */
  void testIsReadOnly();
#endif

  /*
   * @testName:         testNativeSQL
   *
   * @assertion:        A Connection object represents a Connection in a database.
   *                    A Connection session includes the SQL Statements that are
   *                    executed and the results that are returned over that connection.
   *                    (See section 11.1 of JDBC 2.0 API Reference & Tutorial second
   *                    edition)
   *
   *                    The JDBC drivers must provide accurate and complete
   *                    metadata through the Connection.getMetaData() method. (See
   *                    section 6.2.2.3 Java2 Platform Enterprise Edition (J2EE)
   *                    specification v1.2).
   *
   *                    The nativeSQL(String sql) method returns a String object
   *                    representing the native form of a sql.  (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get a Connection object and call nativeSQL(String sql) method
   *                    It should return a String value which represents native SQL
   *                    grammar implementation of the SQL statement if the driver supports
   *                    else it returns the actual SQL statement as a String.This is checked
   *                    by using instanceof method
   */
  /* throws std::runtime_error * */
  void testNativeSQL();
};


REGISTER_FIXTURE(ConnectionTest);
}
}
