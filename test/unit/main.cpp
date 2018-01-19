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
