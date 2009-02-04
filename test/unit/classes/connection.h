/*
 Copyright 2008 - 2009 Sun Microsystems, Inc.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; version 2 of the License.

 There are special exceptions to the terms and conditions of the GPL
 as it is applied to this software. View the full text of the
 exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
 software distribution.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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
  std::string exceptionIsOK(sql::SQLException &e, const std::string& sql_state, int errno);
  std::string exceptionIsOK(sql::SQLException &e);

public:

  EXAMPLE_TEST_FIXTURE(connection)
  {
    TEST_CASE(getClientInfo);
    TEST_CASE(getNoWarningsOnNewLine);
    TEST_CASE(getNoWarningsAfterClear);
    TEST_CASE(checkClosed);
    TEST_CASE(connectUsingMap);
  }

  /**
   * Test const std::string&  MySQL_Connection::getClientInfo(const std::string&)
   *
   * Not part of the JDBC specs? Always returns "cppconn".
   */
  void getClientInfo();

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

};

REGISTER_FIXTURE(connection);
} /* namespace classes */
} /* namespace testsuite */
