/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

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

class resultsetmetadata : public unit_fixture
{
private:
  typedef unit_fixture super;

protected:

  /*
   * Utility: run a query and fetch the resultset
   */
  void runStandardQuery();

  /**
   * Utility: run a query through PS interface and fetch the resultset
   */
  void runStandardPSQuery();

  void doGetCatalogName(bool is_ps, bool &got_warning);
  void doGetColumnCount(bool is_ps);
  void doGetColumnDisplaySize(bool is_ps);
  void doGetColumnNameAndLabel(bool is_ps);
  void doGetPrecision(bool is_ps);
  void doGetScale(bool is_ps);
  void doGetSchemaName(bool is_ps);
  void doGetTableName(bool is_ps);
  void doIsAutoIncrement(bool is_ps);
  void doIsCaseSensitive(bool is_ps);
  void doIsZerofill(bool is_ps);
  void doIsCurrency(bool is_ps);
  void doIsDefinitelyWritable(bool is_ps);
  void doIsNullable(bool is_ps);
  void doIsReadOnly(bool is_ps);
  void doIsSearchable(bool is_ps);
  void doIsSigned(bool is_ps);
  void doIsWritable(bool is_ps);

public:

  EXAMPLE_TEST_FIXTURE(resultsetmetadata)
  {
    TEST_CASE(getCatalogName);
    TEST_CASE(getColumnCount);
    TEST_CASE(getColumnDisplaySize);
    TEST_CASE(getColumnNameAndLabel);
    TEST_CASE(getColumnType);
    TEST_CASE(getPrecision);
    TEST_CASE(getScale);
    TEST_CASE(getSchemaName);
    TEST_CASE(getTableName);
    TEST_CASE(isAutoIncrement);
    TEST_CASE(isCaseSensitive);
    TEST_CASE(isZerofill);
    TEST_CASE(isCurrency);
    TEST_CASE(isDefinitelyWritable);
    TEST_CASE(isNullable);
    TEST_CASE(isReadOnly);
    TEST_CASE(isSearchable);
    TEST_CASE(isSigned);
    TEST_CASE(isWritable);
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

  /**
   * Test for ResultSetMetaData::getPrecision
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getPrecision();

  /**
   * Test for ResultSetMetaData::getScale
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getScale();

  /**
   * Test for ResultSetMetaData::getSchemaName
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getSchemaName();

  /**
   * Test for ResultSetMetaData::getTableName
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getTableName();

  /**
   * Test for ResultSetMetaData::isAutoIncrement
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void isAutoIncrement();

  /**
   * Test for ResultSetMetaData::isCaseSensitive
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void isCaseSensitive();

  /**
   * Test for ResultSetMetaData::isZerofill
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void isZerofill();

  /**
   * Test for ResultSetMetaData::isCurrency
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void isCurrency();

  /**
   * Test for ResultSetMetaData::isDefinitelyWritable
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void isDefinitelyWritable();

  /**
   * Test for ResultSetMetaData::isNullable
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void isNullable();

  /**
   * Test for ResultSetMetaData::isReadOnly
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void isReadOnly();

  /**
   * Test for ResultSetMetaData::isSearchable
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void isSearchable();

  /**
   * Test for ResultSetMetaData::isSigned
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void isSigned();

  /**
   * Test for ResultSetMetaData::isWritable
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void isWritable();

};

REGISTER_FIXTURE(resultsetmetadata);
} /* namespace classes */
} /* namespace testsuite */
