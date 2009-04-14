/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "test_tapOutputter.h"
#include <stdio.h>
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
  char percentage[7];

  if (testsRun)
    sprintf(percentage, "%3.2f"
            , static_cast<float> (testsRun - testsFailed)*100 / testsRun);
  else
    strcpy(percentage, "0.00");

  if (testsFailed > 0)
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
          << ", " << percentage << "% okay" << std::endl;

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
