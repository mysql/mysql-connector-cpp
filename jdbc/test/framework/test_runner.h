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
