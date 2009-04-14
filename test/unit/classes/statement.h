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

/**
 * Example of a collection of tests
 *
 */

namespace testsuite
{
namespace classes
{

class statement : public unit_fixture
{
private:
  typedef unit_fixture super;

  /*
   * Helper method to check for forward only scrolling
   */
  void checkUnbufferedScrolling();

protected:
public:

  EXAMPLE_TEST_FIXTURE(statement)
  {
    TEST_CASE(anonymousSelect);
    TEST_CASE(getWarnings);
    TEST_CASE(clearWarnings);
    TEST_CASE(callSP);
    TEST_CASE(selectZero);
    TEST_CASE(unbufferedFetch);
    TEST_CASE(unbufferedOutOfSync);
  }

  /**
   * SELECT ' ' as string
   */
  void anonymousSelect();

  /**
   * Calls getWarnings()
   */
  void getWarnings();

  /**
   * Calls clearWarnings()
   */
  void clearWarnings();

  /**
   * Calls a stored procedure
   */
  void callSP();

  /**
   * SELECT 0 as string
   */
  void selectZero();

  /**
   * Unbuffered fetch
   */
  void unbufferedFetch();

  /**
   * Unbuffered fetch - ouf of sync
   */
  void unbufferedOutOfSync();


};

REGISTER_FIXTURE(statement);
} /* namespace classes */
} /* namespace testsuite */
