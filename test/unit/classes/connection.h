/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
    TEST_CASE(setTransactionIsolation);
    /*
    TODO: do we want to add this to sql::Connection?
    TEST_CASE(setSessionVariable);
     */
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
   * Use a map to establish a connection
   *
   * This one does not check if the connection settings passed do anything
   * meaningful. It here to ensure that one can pass a certain setting. The
   * impact of a setting shall be tested by other dedicated test methods.
   */
  void connectUsingMap();

  /*
   * Test of MySQL_Connection::setSessionVariable()
   *
   * TODO: do we want to add this to sql::Connection?
   * Focus on code coverage
   *
  void setSessionVariable();
   */

  /*
   * Test of MySQL_Connection::setTransactionIsolation()
   *
   */
  void setTransactionIsolation();

};

REGISTER_FIXTURE(connection);
} /* namespace classes */
} /* namespace testsuite */
