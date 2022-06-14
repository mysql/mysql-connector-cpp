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



#include "test_asserts.h"
#include "test_listener.h"
#include "math.h"
#include <sstream>


namespace testsuite
{

void assertTrue(const String & msg, bool expression,
                const char * file, int line)
{
  if (!expression)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertTrue() failed in " << file << ", line #" << line;
    errmsg << ": '" << msg << "'";
    TestsListener::testHasFailed(errmsg.str());
  }
}

void assertEquals(const char * expected, const char * result,
                  const char * file, int line)
{
  String s1(expected), s2(result);

  assertEquals(s1, s2, file, line);
}

void assertEquals(bool expected, bool result,
                  const char * file, int line)
{
  if (expected != result)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertEquals(boolean) failed in " << file << ", line #" << line;
    TestsListener::testHasFailed(errmsg.str());
  }

}

void assertEquals(int expected, unsigned int result
                  , const char * file, int line)
{
  if ((unsigned) expected != result)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertEquals(int) failed in " << file << ", line #" << line;
    errmsg << " expecting '" << expected << "' got '" << result << "'";
    TestsListener::testHasFailed(errmsg.str());
  }
}

void assertEquals(int expected, int result
                  , const char * file, int line)
{
  if (expected != result)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertEquals(int) failed in " << file << ", line #" << line;
    errmsg << " expecting '" << expected << "' got '" << result << "'";
    TestsListener::testHasFailed(errmsg.str());
  }
}

void assertEquals(unsigned int expected, unsigned int result
                  , const char * file, int line)
{
  if (expected != result)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertEquals(unsigned int) failed in " << file << ", line #" << line;
    errmsg << " expecting '" << expected << "' got '" << result << "'";
    TestsListener::testHasFailed(errmsg.str());
  }
}

void assertEquals(int64_t expected, int64_t result
                  , const char * file, int line)
{
  if (expected != result)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertEquals(int64_t) failed in " << file << ", line #" << line;
    errmsg << " expecting '" << expected << "' got '" << result << "'";
    TestsListener::testHasFailed(errmsg.str());
  }
}

void assertEquals(uint64_t expected, uint64_t result
                  , const char * file, int line)
{
  if (expected != result)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertEquals(uint64_t) failed in " << file << ", line #" << line;
    errmsg << " expecting '" << expected << "' got '" << result << "'";
    TestsListener::testHasFailed(errmsg.str());
  }
}

bool isNaN(double smth)
{
  bool lessThanZero=(smth < 0.0);
  bool moreOrEqualZero=(smth >= 0.0);

  return !(lessThanZero || moreOrEqualZero);
}

double scaledEpsilon(const double & expected, const double & fuzzyEpsilon)
{
  const double aa=fabs(expected) + 1;

  return ( true) ? fuzzyEpsilon : fuzzyEpsilon * aa;
}

double scaledEpsilon(const long double & expected, const long double & fuzzyEpsilon)
{
  const long double aa=fabs(expected) + 1;

  return ( true) ? fuzzyEpsilon : fuzzyEpsilon * aa;
}

bool fuzzyEquals(double expected, double result, double fuzzyEpsilon)
{
  return ( expected == result)
          || (fabs(expected - result) <= scaledEpsilon(expected, fuzzyEpsilon));
}

bool fuzzyEquals(long double expected, long double result, long double fuzzyEpsilon)
{
  return ( expected == result)
          || (fabs(expected - result) <= scaledEpsilon(expected, fuzzyEpsilon));
}


void assertEquals(const long double & expected, const long double & result,
                  const char * file, int line)
{
  const long double fuzzyEpsilon=0.000001;

  assertEqualsEpsilon(expected, result, fuzzyEpsilon, file, line);
}


void assertEquals(const double & expected, const long double & result,
                  const char * file, int line)
{
  const long double fuzzyEpsilon=0.000001;

  assertEqualsEpsilon(expected, result, fuzzyEpsilon, file, line);
}


void assertEquals(const double & expected, const double & result,
                  const char * file, int line)
{
  const double fuzzyEpsilon=0.000001;

  assertEqualsEpsilon(expected, result, fuzzyEpsilon, file, line);
}

void assertEquals(const float & expected, const float & result,
                  const char * file, int line)
{
  assertEquals(static_cast<double> (expected), static_cast<double> (result)
               , file, line);
}


void assertEqualsEpsilon(const long double & expected, const long double & result
                         , const long double & epsilon, const char * file, int line)
{
  if ( ( isNaN(expected) && isNaN(result) )
      ||
      ( !isNaN(expected) && !isNaN(result)
      && fuzzyEquals(expected, result, epsilon) ) )
  {
    return;
  }

  std::stringstream errmsg;
  errmsg.str("");
  errmsg << "assertEquals(double) failed in " << file << ", line #" << line;
  errmsg << " expecting '" << expected << "' got '" << result << "'";
  TestsListener::testHasFailed(errmsg.str());
}


void assertEqualsEpsilon(const double & expected, const double & result
                         , const double & epsilon, const char * file, int line)
{
  if ( ( isNaN(expected) && isNaN(result) )
      ||
      ( !isNaN(expected) && !isNaN(result)
      && fuzzyEquals( expected, result, epsilon ) ) )
  {
    return;
  }

  std::stringstream errmsg;
  errmsg.str("");
  errmsg << "assertEquals(double) failed in " << file << ", line #" << line;
  errmsg << " expecting '" << expected << "' got '" << result << "'";
  TestsListener::testHasFailed(errmsg.str());
}

void assertEquals(const String & expected, const String & result
                  , const char * file, int line)
{
  if (expected != result)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertEquals(std::string) failed in" << file << ", line #" << line;
    errmsg << " expecting '" << expected << "' got '" << result << "'";
    TestsListener::testHasFailed(errmsg.str());
  }
}


void assertLessThan(int expected, unsigned int result
                    , const char * file, int line)
{
  if ((unsigned) expected < result)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertLessThan(int) failed in " << file << ", line #" << line;
    errmsg << " expecting '" << expected << "' got '" << result << "'";
    TestsListener::testHasFailed(errmsg.str());
  }
}

void assertLessThan(int expected, int result
                    , const char * file, int line)
{
  if (expected < result)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertLessThan(int) failed in " << file << ", line #" << line;
    errmsg << " expecting '" << expected << "' got '" << result << "'";
    TestsListener::testHasFailed(errmsg.str());
  }
}

void assertLessThan(unsigned int expected, unsigned int result
                    , const char * file, int line)
{
  if (expected < result)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertLessThan(unsigned int) failed in " << file << ", line #" << line;
    errmsg << " expecting '" << expected << "' got '" << result << "'";
    TestsListener::testHasFailed(errmsg.str());
  }
}

void assertGreaterThan(int expected, unsigned int result
                       , const char * file, int line)
{
  if ((unsigned) expected > result)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertGreaterThan(int) failed in " << file << ", line #" << line;
    errmsg << " expecting '" << expected << "' got '" << result << "'";
    TestsListener::testHasFailed(errmsg.str());
  }
}

void assertGreaterThan(int expected, int result
                       , const char * file, int line)
{
  if (expected > result)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertGreaterThan(int) failed in " << file << ", line #" << line;
    errmsg << " expecting '" << expected << "' got '" << result << "'";
    TestsListener::testHasFailed(errmsg.str());
  }
}

void assertGreaterThan(unsigned int expected, unsigned int result
                       , const char * file, int line)
{
  if (expected > result)
  {
    std::stringstream errmsg;
    errmsg.str("");
    errmsg << "assertGreaterThan(unsigned int) failed in " << file << ", line #" << line;
    errmsg << " expecting '" << expected << "' got '" << result << "'";
    TestsListener::testHasFailed(errmsg.str());
  }
}

void fail(const char* reason, const char * file, int line)
{
  // if( verbose...
  // TestsListener
  std::stringstream errmsg;
  errmsg.str("");
  errmsg << "failed in " << file << ", line #" << line;
  errmsg << ". Reason: '" << reason << "'";
  TestsListener::testHasFailed(errmsg.str());
}

}
