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



#ifndef _EXAMPLES_H
#define	_EXAMPLES_H

/*
  __FUNCTION__/__func__ is not portable. We do not promise
  that  our example definition covers each and every compiler.
  If not, it is up to you to find a different definition for
  your setup.
*/

#if __STDC_VERSION__ < 199901L
#  if __GNUC__ >= 2
#    define EXAMPLE_FUNCTION __FUNCTION__
#  else
#    define EXAMPLE_FUNCTION "(function n/a)"
#  endif
#elif defined(_MSC_VER)
#  if _MSC_VER < 1300
#    define EXAMPLE_FUNCTION "(function n/a)"
#  else
#    define EXAMPLE_FUNCTION __FUNCTION__
#  endif
#elif (defined __func__)
#  define EXAMPLE_FUNCTION __func__
#else
#  define EXAMPLE_FUNCTION "(function n/a)"
#endif

/*
  Again, either you are lucky and this definition
  works for you or you have to find your own.
*/
#ifndef __LINE__
  #define __LINE__ "(line number n/a)"
#endif

// Connection properties
#define EXAMPLE_DB   "test"
#define EXAMPLE_HOST "tcp://127.0.0.1:3006"
#define EXAMPLE_USER "root"
#define EXAMPLE_PASS ""

// Sample data
#define EXAMPLE_NUM_TEST_ROWS 4
struct _test_data {
  int id;
  const char* label;
};
_test_data test_data[EXAMPLE_NUM_TEST_ROWS] = {
  {1, ""}, {2, "a"}, {3, "b"}, {4, "c"},
};


#endif	/* _EXAMPLES_H */
