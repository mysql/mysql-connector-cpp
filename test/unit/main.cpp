/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "../framework/test_runner.h"
#include "../framework/start_options.h"
#include "../framework/test_tapOutputter.h"
#include "../framework/test_filter.h"
#include <stdlib.h>

int main(int argc, char** argv)
{
  const String::value_type * unnamedStartParams[]={"dbUrl"
    , "dbUser"
    , "dbPasswd"
    , "dbSchema"
    , NULL};

  Properties defaultStringValues;

  defaultStringValues.insert(Properties::value_type("dbUrl", "tcp://127.0.0.1:3306"));
  defaultStringValues.insert(Properties::value_type("dbUser", "root"));
  defaultStringValues.insert(Properties::value_type("dbPasswd", "root"));
  defaultStringValues.insert(Properties::value_type("dbSchema", "test"));

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
