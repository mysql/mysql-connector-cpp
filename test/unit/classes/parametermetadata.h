/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "../unit_fixture.h"
#include <cppconn/parameter_metadata.h>

/**
 * Example of a collection of tests
 *
 */

namespace testsuite
{
namespace classes
{

class parametermetadata : public unit_fixture
{
private:
  typedef unit_fixture super;

protected:
public:

  EXAMPLE_TEST_FIXTURE(parametermetadata)
  {
    TEST_CASE(getMeta);
    TEST_CASE(getParameterCount);
#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
    TEST_CASE(notImplemented);
#endif
  }

  /**
   * Fetch meta data from open/closed PS
   */
  void getMeta();

  /**
   * Test of ParameterMetaData::getParameterCount()
   */
  void getParameterCount();

#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  /**
   * Tests of all "not implemented" methods to track API changes
   */
  void notImplemented();
#endif

};

REGISTER_FIXTURE(parametermetadata);
} /* namespace classes */
} /* namespace testsuite */
