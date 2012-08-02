/*
Copyright (c) 2012, Oracle and/or its affiliates. All rights reserved.

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


namespace sql
{
namespace mysql
{
	class MySQL_Uri;
}
}


namespace testsuite
{
namespace classes
{


class uri: public unit_fixture
{
private:
	typedef unit_fixture super;

	boost::scoped_ptr< ::sql::mysql::MySQL_Uri> _uri;

	::sql::SQLString conn_string;

protected:
public:

	/*Overriding setUp and tearDown */
	void setUp();

	void tearDown();

	EXAMPLE_TEST_FIXTURE(uri)
	{
		TEST_CASE(tcp);
		TEST_CASE(tcpIpV6);
		TEST_CASE(socket);
		TEST_CASE(pipe);
	}

  /**
   * Test uri::tcp()
   *
   * 
   */
  void tcp();

  /**
   * Test uri::tcpIpV6()
   *
   * 
   */
  void tcpIpV6();

  /**
   * Test uri::socket()
   *
   * 
   */
  void socket();

  /**
   * Test uri::pipe()
   *
   * 
   */
  void pipe();
};

REGISTER_FIXTURE(uri);
} /* namespace classes */
} /* namespace testsuite */
