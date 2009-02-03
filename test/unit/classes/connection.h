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
public:

  EXAMPLE_TEST_FIXTURE(connection)
  {
    TEST_CASE(getClientInfo);

  }

  /**
   * Test const std::string&  MySQL_Connection::getClientInfo(const std::string&)
   *
   * Not part of the JDBC specs? Always returns "cppconn".
   */
  void getClientInfo();

};

REGISTER_FIXTURE(connection);
} /* namespace classes */
} /* namespace testsuite */
