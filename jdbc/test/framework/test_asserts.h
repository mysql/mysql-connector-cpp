/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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



#ifndef __TESTASSERTS_H_
#define __TESTASSERTS_H_

#include "../../cppconn/resultset.h"

#include "../common/ccppTypes.h"

#ifndef __LINE__
#define __LINE__ "(line number n/a)"
#endif

namespace testsuite
{
void fail(const char* reason, const char * file, int line);

void assertTrue(const String & msg, bool expression
                , const char * file, int line);

void assertEquals(const char * expected, const char * result
                  , const char * file, int line);

void assertEquals(int expected, unsigned int result
                  , const char * file, int line);

void assertEquals(int expected, int result
                  , const char * file, int line);

void assertEquals(unsigned int expected, unsigned int result
                  , const char * file, int line);

void assertEquals(int64_t expected, int64_t result
                  , const char * file, int line);

void assertEquals(uint64_t expected, uint64_t result
                  , const char * file, int line);

void assertEquals(bool expected, bool result
                  , const char * file, int line);

void assertEquals(const double & expected, const long double & result
                  , const char * file, int line);

void assertEquals(const long double & expected, const long double & result
                  , const char * file, int line);

void assertEquals(const double & expected, const double & result
                  , const char * file, int line);

void assertEquals(const float & expected, const float & result
                  , const char * file, int line);

void assertEquals(const long double & expected
                  , const long double & result
                  , const char * file, int line);

void assertEqualsEpsilon(const double & expected, const double & result
                         , const double & epsilon, const char * file, int line);

void assertEquals(const String & expected, const String & result
                  , const char * file, int line);

void assertTrueMessage(bool exp, const String & msg, const char * file
                       , int line);

void assertGreaterThan(int expected, unsigned int result
                       , const char * file, int line);

void assertGreaterThan(int expected, int result
                       , const char * file, int line);

void assertGreaterThan(unsigned int expected, unsigned int result
                       , const char * file, int line);

void assertLessThan(int expected, unsigned int result
                    , const char * file, int line);

void assertLessThan(int expected, int result
                    , const char * file, int line);

void assertLessThan(unsigned int expected, unsigned int result
                    , const char * file, int line);

bool fuzzyEquals(double expected, double result, double fuzzyEpsilon);
}

// Macros should be used inside testsuite namespace

#define ASSERT_EQUALS( expected, result) \
  assertEquals( (expected), (result), __FILE__, __LINE__ )

#define ASSERT_LT( expected, result) \
  assertLessThan( (expected), (result), __FILE__, __LINE__ )

#define ASSERT_GT( expected, result) \
  assertGreaterThan( (expected), (result), __FILE__, __LINE__ )

#define ASSERT_EQUALS_EPSILON( expected, result, epsilon) \
  assertEqualsEpsilon( (expected), (result), (epsilon), __FILE__, __LINE__ )

#define ASSERT( exp ) assertTrue( (#exp), (exp), __FILE__, __LINE__)

#define ASSERT_MESSAGE( exp, message ) \
  assertTrue( (message), (exp), __FILE__, __LINE__ )

#define FAIL( why ) fail( (#why), __FILE__, __LINE__ )

#endif  // __TESTASSERTS_H_
