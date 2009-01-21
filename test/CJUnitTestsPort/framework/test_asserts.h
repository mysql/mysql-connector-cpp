/* Copyright 2008 - 2009 Sun Microsystems, Inc.

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

#ifndef __TESTASSERTS_H_
#define __TESTASSERTS_H_

#include "../common/ccppTypes.h"

#ifndef __LINE__
  #define __LINE__ "(line number n/a)"
#endif 

namespace testsuite
{
void fail(const char* reason, const char* file, int line);

void assertTrue(const char * msg, bool expression
                , const char* file, int line);

void assertEquals(const char * expected, const char * result
                  , const char* file, int line);

void assertEquals(int expected, int result
                  , const char* file, int line);

void assertEquals(bool expected, bool result
                  , const char* file, int line);

void assertEquals(const double & expected, const double & result
                  , const char* file, int line);

void assertEquals(const float & expected, const float & result
                  , const char * file, int line);

void assertEquals(const long double & expected, const long double & result
                  , const char * file, int line);

void assertEqualsEpsilon(const double & expected, const double & result
                         , const double & epsilon, const char * file, int line);

void assertEquals(const String & expected, const String & result
                  , const char* file, int line);
}

// Macroses should be used inside testsuite namespace

#define ASSERT_EQUALS( expected, result) \
	assertEquals( expected, result, __FILE__, __LINE__ )

#define ASSERT_EQUALS_EPSILON( expected, result, epsilon) \
	assertEqualsEpsilon( expected, result, epsilon, __FILE__, __LINE__ )

#define ASSERT( exp ) assertTrue( #exp, exp, __FILE__, __LINE__)

#define ASSERT_MESSAGE( exp, message ) \
	assertTrueMessage(#exp, exp, message, __FILE__, __LINE__)

#define FAIL( why ) fail( #why, __FILE__, __LINE__ )

#endif  // __TESTASSERTS_H_
