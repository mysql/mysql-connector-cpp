/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
