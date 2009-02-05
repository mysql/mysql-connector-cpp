/*
 Copyright 2008 - 2009 Sun Microsystems, Inc.

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

#include "../unit_fixture.h"
#include <vector>

/**
 * Example of a collection of tests
 *
 */

namespace testsuite
{
namespace classes
{

struct columndefinition
{
  std::string name;
  std::string sqldef;
  int ctype;
  std::string value;

  columndefinition(std::string n, std::string s, int c, std::string v) :
  name(n),
  sqldef(s),
  ctype(c),
  value(v)
  {
  }
};

class resultsetmetadata : public unit_fixture
{
private:
  typedef unit_fixture super;

protected:

  /**
   * List of all column types known by MySQL
   *
   */
  std::vector< columndefinition > columns;

  /**
   * Initializes the column type list
   */
  void init();

  /*
   * Utility: run a query and fetch the resultset
   */
  void runStandardQuery();

public:

  EXAMPLE_TEST_FIXTURE(resultsetmetadata)
  {
    init();
    TEST_CASE(getCatalogName);
    TEST_CASE(getColumnCount);
    TEST_CASE(getColumnDisplaySize);
    TEST_CASE(getColumnNameAndLabel);
    TEST_CASE(getColumnType);
  }

  /**
   * Test for ResultSetMetaData::getCatalogName()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getCatalogName();

  /**
   * Test for ResultSetMetaData::getColumnCount()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getColumnCount();

  /**
   * Test for ResultSetMetaData::getColumnDisplaySize
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getColumnDisplaySize();

  /**
   * Test for ResultSetMetaData::getColumnName, ResultSetMetaData::getColumnLabel
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getColumnNameAndLabel();

  /**
   * Test for ResultSetMetaData::getColumType
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getColumnType();

};

REGISTER_FIXTURE(resultsetmetadata);
} /* namespace classes */
} /* namespace testsuite */
