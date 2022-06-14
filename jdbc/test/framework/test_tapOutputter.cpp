/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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



#include "test_tapOutputter.h"
#include <iomanip>
#include <sstream>

namespace testsuite
{

TAP::TAP()
  : output( std::cout )
  , msgLog( std::cerr )
  , errLog( std::cerr )
{
  /*
   TODO - Discuss with Lawrin
  output << std::unitbuf;
  msgLog << std::unitbuf;
  errLog << std::unitbuf;
   */
}


void TAP::Header(const String & text)
{
}


void TAP::SuiteHeader(	const String &  name
                      , unsigned        first
                      , int             testsInSuite)
{
  suiteName= name;

  Comment( name );

  if ( testsInSuite > 0 )
    output << first << ".." << first + testsInSuite - 1 << std::endl;
  else
    Comment( "doesn't contain any tests" );
}

void TAP::TestPassed(unsigned ordNum
                     , const String & name
                     , const String & comment)
{
  output << "ok " << ordNum << " - " << suiteName << "::" << name;

  if (comment.length() > 0)
  {
    output << " # " << comment;
  }

  output << std::endl;
}

void TAP::TestFailed( unsigned        ordNum
                    , const String &  name
                    , const String &  comment )
{
  output << "not ok " << ordNum
          << " - " << suiteName << "::" << name;

  if ( comment.length() > 0 )
  {
    output << " # " << comment;
  }

  output << std::endl;
}

void TAP::Comment(const String & comment)
{
  output << "# " << comment << std::endl;
}

void TAP::Summary( unsigned           testsRun
                 , unsigned           testsFailed
                 , std::vector<int> & failedTestsNum)
{
  std::stringstream percentage;

  // Little data validation - otherwise sprintf can corrupt our precious stack
  if ( testsRun < testsFailed )
      testsFailed= testsRun;

  if ( testsRun != 0 )
    percentage << std::fixed <<std::setprecision(2) <<static_cast<float> (testsRun - testsFailed)*100.0 / testsRun;
  else
    percentage << std::fixed <<std::setprecision(2) <<(float) 0.0;

  if ( testsFailed > 0 )
  {
    //TODO: move is string utils as "join" or smth
    std::vector<int>::const_iterator cit=failedTestsNum.begin();

    output << std::endl << "FAILED tests " << *cit;

    while (++cit != failedTestsNum.end())
    {
      output << ", " << *cit;
    }
  }

  output << std::endl << "Failed " << testsFailed << "/" << testsRun
          << ", " << percentage.str() << "% okay" << std::endl;

}

void TAP::SuiteSummary(const String & _suiteName
                       , unsigned testsRun
                       , unsigned testsFailed)
{

}

void TAP::Assert(const String & expected, const String & result
                 , const String & file, int line)
{
  std::ostringstream tmp("Assertion Failed in file ");

  tmp << file << " Line " << line;

  Comment(tmp.str());

  tmp.flush();
  tmp << "Expected: " << expected;

  Comment(tmp.str());

  tmp.flush();
  tmp << "Received: " << result;

  Comment(tmp.str());
}


std::ostream & TAP::messagesLog()
{
  // if they need log for smth... we have to make it TAP conmment
  // there is risk that user doesn't output endl and that will break TAP format. So, possibly
  // it's better to supply some str stream as a buffer
  msgLog << "# ";

  return msgLog;
}

std::ostream & TAP::errorsLog()
{
  errLog << "# ";

  return errLog;
}

}
