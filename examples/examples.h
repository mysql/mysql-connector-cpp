/* Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

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

