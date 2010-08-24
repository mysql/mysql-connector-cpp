/*
  Copyright (c) 2009, 2010, Oracle and/or its affiliates. All rights reserved.

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
#include <cppconn/parameter_metadata.h>

/**
 * Example of a collection of tests
 *
 */

namespace testsuite
{
namespace classes
{

class parametermetadata : public unit_fixture
{
private:
  typedef unit_fixture super;

protected:
public:

  EXAMPLE_TEST_FIXTURE(parametermetadata)
  {
    TEST_CASE(getMeta);
    TEST_CASE(getParameterCount);
#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
    TEST_CASE(notImplemented);
#endif
  }

  /**
   * Fetch meta data from open/closed PS
   */
  void getMeta();

  /**
   * Test of ParameterMetaData::getParameterCount()
   */
  void getParameterCount();

#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  /**
   * Tests of all "not implemented" methods to track API changes
   */
  void notImplemented();
#endif

};

REGISTER_FIXTURE(parametermetadata);
} /* namespace classes */
} /* namespace testsuite */
