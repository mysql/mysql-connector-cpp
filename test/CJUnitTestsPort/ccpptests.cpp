#include "framework/test_runner.h"
#include "framework/start_options.h"
#include "framework/test_tapOutputter.h"

int main( int argc, char** argv )
{
  testsuite::StartOptions options= testsuite::StartOptions( argc, argv );

  testsuite::TestsRunner &  testsRunner= testsuite::TestsRunner::theInstance();

  testsRunner.setStartOptions( & options );

  return testsRunner.runTests() ? 0 : -1;
}	
