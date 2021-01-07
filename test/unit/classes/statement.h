/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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
    TEST_CASE(queryTimeout);
    TEST_CASE(queryAttributes);
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

  /**
   * check setQueryTimeout() and getQueryTimeout
   */
  void queryTimeout();

  /**
   * check query attributes
   */
  void queryAttributes();

};

REGISTER_FIXTURE(statement);
} /* namespace classes */
} /* namespace testsuite */
