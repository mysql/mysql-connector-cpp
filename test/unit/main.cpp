/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "../CJUnitTestsPort/framework/test_runner.h"
#include "../CJUnitTestsPort/framework/start_options.h"
#include "../CJUnitTestsPort/framework/test_tapOutputter.h"

int main(int argc, char** argv)
{
  testsuite::StartOptions options=testsuite::StartOptions(argc, argv);

  testsuite::TestsRunner & testsRunner=testsuite::TestsRunner::theInstance();

  testsRunner.setStartOptions(& options);

  return testsRunner.runTests() ? 0 : -1;
}
