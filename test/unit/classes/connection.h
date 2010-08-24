/*
  Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

  The MySQL Connector/C++ is licensed under the terms of the GPLv2
  <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
  MySQL Connectors. There are special exceptions to the terms and
  conditions of the GPLv2 as it is applied to this software, see the
  FLOSS License Exception
  <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#include "../unit_fixture.h"

/**
 * Example of a collection of tests
 *
 */

namespace testsuite
{
namespace classes
{

class connection : public unit_fixture
{
private:
  typedef unit_fixture super;

protected:
public:

  EXAMPLE_TEST_FIXTURE(connection)
  {
    TEST_CASE(getClientInfo);
    TEST_CASE(getClientOption);
    TEST_CASE(getSessionVariable);
    TEST_CASE(getNoWarningsOnNewLine);
    TEST_CASE(getNoWarningsAfterClear);
    TEST_CASE(checkClosed);
    TEST_CASE(connectUsingMap);
    TEST_CASE(connectUsingMapWrongTypes);
    TEST_CASE(connectOptReconnect);
    TEST_CASE(invalidCredentials);
    TEST_CASE(setTransactionIsolation);
    TEST_CASE(rollback);
    /*
    TODO: do we want to add this to sql::Connection?
    TEST_CASE(setSessionVariable);
     */
    // Test doesn't make sense for static binding.
#ifndef MYSQLCLIENT_STATIC_BINDING
    TEST_CASE(loadSameLibraryTwice);
#endif
  }

  /**
   * Test const std::string&  MySQL_Connection::getClientInfo(const std::string&)
   *
   * Not part of the JDBC specs? Always returns "cppconn".
   */
  void getClientInfo();

   /**
   * Test getClientOption, setClientOption
   *
   * Not part of the JDBC specs? Only supports metadataUseInfoSchema
   */
  void getClientOption();


 /**
   * Test getSessionVariable, setSessionVariable
   *
   * Not part of the JDBC specs?
   */
  void getSessionVariable();


  /**
   * Ensure that tests get a connection without any warnings
   *
   * Call the function on a clean line, it should return NULL.
   */
  void getNoWarningsOnNewLine();

  /**
   * Check if MySQL_Connection::clearWarnings() really cleans warnings reported by MySQL_Connection::getWarnings()
   *
   * TODO: THe test method should cause a warning to be able to verify that
   * clearWarnings() really clears the warnings...
   */
  void getNoWarningsAfterClear();

  /*
   * Check if MySQL_Connection::rollback() recognizes a closed connection
   *
   * NOTE: This is a little hack to reach the internal
   * MySQL_Connection::checkClosed() method.
   */
  void checkClosed();

  /*
   * Use a map to establish a connection but passing wrong types
   *
   * Pretty similar to connectUsingMap but just consistenly wrong types
   */
  void connectUsingMapWrongTypes();

   /*
   * Use a map to establish a connection
   *
   * This one does not check if the connection settings passed do anything
   * meaningful. It here to ensure that one can pass a certain setting. The
   * impact of a setting shall be tested by other dedicated test methods.
   */
  void connectUsingMap();

  /*
   * OPT_RECONNECT
   *
   * Check if it does what it is supposed to do
   */
  void connectOptReconnect();

  /*
   * Test of MySQL_Connection::setSessionVariable()
   *
   * TODO: do we want to add this to sql::Connection?
   * Focus on code coverage
   *
  void setSessionVariable();
   */

  /**
   * Establish a connection with invalid credentials
   *
   *
   */
  void invalidCredentials();

  /*
   * Test of MySQL_Connection::setTransactionIsolation()
   *
   */
  void setTransactionIsolation();

  /*
   * Test of rollback to savepoint with autocommit turned on
   */
  void rollback();


#ifndef MYSQLCLIENT_STATIC_BINDING
  /*
   * Tries to load same library twice - 1 time just by the name, 2nd time - by full path
   * (need to know which lib file is picked by name)
   * nothing should happen in the test. But crash is possible at the end of work of the program
   */
  void loadSameLibraryTwice();
#endif

};


REGISTER_FIXTURE(connection);
} /* namespace classes */
} /* namespace testsuite */
