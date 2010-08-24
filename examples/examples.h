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

#ifndef _EXAMPLES_H
#define	_EXAMPLES_H

// Portable __FUNCTION__
#ifndef __FUNCTION__
 #ifdef __func__
   #define __FUNCTION__ __func__
 #else
   #define __FUNCTION__ "(function n/a)"
 #endif
#endif

#ifndef __LINE__
  #define __LINE__ "(line number n/a)"
#endif

// Connection properties
#define EXAMPLE_DB   "test"
#define EXAMPLE_HOST "tcp://127.0.0.1:3306"
#define EXAMPLE_USER "root"
#define EXAMPLE_PASS "root"

// Sample data
#define EXAMPLE_NUM_TEST_ROWS 4
struct _test_data {
	int id;
	const char* label;
};
static _test_data test_data[EXAMPLE_NUM_TEST_ROWS] = {
	{1, ""}, {2, "a"}, {3, "b"}, {4, "c"},
};


#endif	/* _EXAMPLES_H */

