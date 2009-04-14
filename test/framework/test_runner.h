/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */


#ifndef __TESTSRUNNER_H_
#define __TESTSRUNNER_H_

#include "../common/singleton.h"
#include "start_options.h"

namespace testsuite
{

class Filter;

class TestsRunner : public policies::Singleton<TestsRunner>
{
private:

  typedef std::vector<const String::value_type *>   constStrList;
  typedef constStrList::const_iterator              constStrListCit;

  // should be private/protected
  CCPP_SINGLETON(TestsRunner);

  constStrList    TestSuiteNames;

  StartOptions  * startOptions;

  Filter        * filter;

public:

  bool            runTests        ();

  void            setStartOptions ( StartOptions  * options );
  void            setTestsFilter  ( Filter        & _filter );

  static StartOptions * getStartOptions ();
  static bool           Admits          ( const String & testName );         
};

}


#endif  // __TESTSRUNNER_H_
