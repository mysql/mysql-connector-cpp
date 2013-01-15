/*
Copyright (c) 2009, 2013, Oracle and/or its affiliates. All rights reserved.

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



#ifndef __BUGS_H_
#define __BUGS_H_

#include "../unit_fixture.h"

/**
 * Test cases for unsorted bugs (expecting that later bugs in different units
 * will be sorted to separate test suites)
 */

namespace testsuite
{
namespace regression
{

class bugs : public unit_fixture
{
private:
  typedef unit_fixture super;
protected:
public:

  EXAMPLE_TEST_FIXTURE(bugs)
  {
    TEST_CASE(net_write_timeout39878);
    TEST_CASE(store_result_error_51562);
    TEST_CASE(getResultSet_54840);
	TEST_CASE(supportIssue_52319);
	TEST_CASE(expired_pwd);
  }

  /**
   * http://bugs.mysql.com/39878
   *
   * bug report is against c/odbc, but problem is common for (almost?) all connectors
   * if we have pause between fetching rows lonher than net_write_timeout,
   * we won't receive all rows, and no error occures.
   */
  void net_write_timeout39878();

  /**
   * http://bugs.mysql.com/bug.php?id=51562
   */
  void store_result_error_51562();

  void getResultSet_54840();

  /**
  * MySQL customer suppoer issue #52319
  */
  void supportIssue_52319();

  void expired_pwd();

};

REGISTER_FIXTURE(bugs);
} /* namespace regression */
} /* namespace testsuite */

#endif
