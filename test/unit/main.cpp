/*
Copyright (c) 2008, 2016, Oracle and/or its affiliates. All rights reserved.

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



#include "../framework/test_runner.h"
#include "../framework/start_options.h"
#include "../framework/test_tapOutputter.h"
#include "../framework/test_filter.h"
#include "tests_config.h"
#include <stdlib.h>

int main(int argc, char** argv)
{
  const String::value_type * unnamedStartParams[]={"dbUrl"
    , "dbUser"
    , "dbPasswd"
    , "dbSchema"
    , NULL};

  Properties defaultStringValues;

  defaultStringValues.insert(Properties::value_type("dbUrl"   ,TEST_DEFAULT_HOST   ));
  defaultStringValues.insert(Properties::value_type("dbUser"  ,TEST_DEFAULT_LOGIN ));
  defaultStringValues.insert(Properties::value_type("dbPasswd",TEST_DEFAULT_PASSWD));
  defaultStringValues.insert(Properties::value_type("dbSchema",TEST_DEFAULT_DB    ));

  std::map<String, bool> defaultBoolValues;

  testsuite::StartOptions options(unnamedStartParams, & defaultStringValues
                                  , & defaultBoolValues);

  options.parseParams(argc, argv);
  testsuite::FiltersSuperposition filter(options.getString("filter"));

  testsuite::TestsRunner & testsRunner=testsuite::TestsRunner::theInstance();

  testsRunner.setStartOptions(& options);
  testsRunner.setTestsFilter(filter);

  return testsRunner.runTests() ? EXIT_SUCCESS : EXIT_FAILURE;
}
