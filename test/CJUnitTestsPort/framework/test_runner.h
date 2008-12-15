/* Copyright 2008 Sun Microsystems, Inc.

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


#ifndef __TESTSRUNNER_H_
#define __TESTSRUNNER_H_

#include "../common/singleton.h"
#include "start_options.h"

namespace testsuite
{

class TestsRunner : public policies::Singleton<TestsRunner>
{
private:
  // should be private/protected
  CCPP_SINGLETON(TestsRunner);

  std::vector<const String::value_type *> TestSuites;

  StartOptions * startOptions;

public:

  bool runTests();

  void setStartOptions(StartOptions * options);

  StartOptions * getStartOptions() const;
};

}


#endif  // __TESTSRUNNER_H_

