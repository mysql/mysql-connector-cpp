/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
