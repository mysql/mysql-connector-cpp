/*
 * Copyright (c) 2008, 2020, Oracle and/or its affiliates.
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
    /*
*   TODO: enable it when DNS+SRV works in docker
    TEST_CASE(checkDnsSrv);
    */
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
  TEST_CASE(enableClearTextAuth);
  TEST_CASE(connectAttrAdd);
  TEST_CASE(connectAttrReset);
  TEST_CASE(connectCharsetDir);
  TEST_CASE(connectSSLEnforce);
  TEST_CASE(setAuthDir);
  TEST_CASE(setDefaultAuth);
  TEST_CASE(localInfile);
  TEST_CASE(isValid);
  TEST_CASE(reconnect);
  TEST_CASE(ssl_mode);
  TEST_CASE(tls_version);
  TEST_CASE(cached_sha2_auth);
  TEST_CASE(socket);
  TEST_CASE(dns_srv);
  TEST_CASE(mfa);
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
   * Check if DNS+SRV is working
   */
  void checkDnsSrv();

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

  void enableClearTextAuth();

  /*
   * Test for Connection attributes options MYSQL_OPT_CONNECT_ATTR_ADD
   * | MYSQL_OPT_CONNECT_ATTR_DELETE
   */
  void connectAttrAdd();

  /*
   * Test for Connection attributes options MYSQL_OPT_CONNECT_ATTR_RESET
   */
  void connectAttrReset();

  /*
   * Test for Connection attributes options MYSQL_SET_CHARSET_DIR
   */
  void connectCharsetDir();

  /*
   * Test for Connection attributes options MYSQL_OPT_SSL_ENFORCE
   */
  void connectSSLEnforce();

  /*
   * Test for Connection attributes options MYSQL_PLUGIN_DIR
   */
  void setAuthDir();

  /*
   * Test for Connection attributes options MYSQL_DEFAULT_AUTH
   */
  void setDefaultAuth();

  /*
   * Test for Connection attributes options MYSQL_OPT_LOCAL_INFILE
   */
  void localInfile();

  /*
   * Test of MySQL_Connection::isValid()
   *
   */
  void isValid();

  /*
   * Test of MySQL_Connection::reconnect()
   *
   */
  void reconnect();

  /*
   * Test of MySQL_Connection::ssl_mode()
   *
   */
  void ssl_mode();

  /*
   * Test of MySQL_Connection::tls_version()
   *
   */
  void tls_version();

  /*
   * Test of MySQL_Connection::cached_sha2_auth()
   *
   */
  void cached_sha2_auth();

  /*
   * Test of MySQL_Connection::socket()
   *
   */
  void socket();

  /*
   * Test of MySQL_Connection::dns_srv()
   *
   */
  void dns_srv();

  /*
   * Test of MySQL_Connection::mfa()
   *
   */
  void mfa();


};


REGISTER_FIXTURE(connection);
} /* namespace classes */
} /* namespace testsuite */
