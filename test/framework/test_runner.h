/*
Copyright (c) 2008, 2011, Oracle and/or its affiliates. All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPLv2
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPLv2 as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
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
