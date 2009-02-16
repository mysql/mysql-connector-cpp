/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   There are special exceptions to the terms and conditions of the GPL
   as it is applied to this software. View the full text of the
   exception in file EXCEPTIONS-conECTOR-C++ in the directory of this
   software distribution.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "unit_fixture.h"

#ifndef L64
#ifdef _WIN32
#define L64(x) x##i64
#else
#define L64(x) x##LL
#endif
#endif

namespace testsuite
{

Driver * unit_fixture::driver=NULL;

unit_fixture::unit_fixture(const String & name)
: super(name),
con(NULL),
pstmt(NULL),
stmt(NULL),
res(NULL)
{
  init();
}

void unit_fixture::init()
{
  url=TestsRunner::theInstance().getStartOptions()->dbUrl;
  user=TestsRunner::theInstance().getStartOptions()->dbUser;
  passwd=TestsRunner::theInstance().getStartOptions()->dbPasswd;
  db=TestsRunner::theInstance().getStartOptions()->dbSchema;

  columns.push_back(columndefinition("BIT", "BIT", sql::DataType::BIT, "0", false, 1, 0, true, "", 0));
  columns.push_back(columndefinition("BIT", "BIT NOT NULL", sql::DataType::BIT, "0", false, 1, 0, false, "", 0));
  columns.push_back(columndefinition("BIT", "BIT(5) NOT NULL", sql::DataType::BIT, "0", false, 5, 0, false, "", 0));
  columns.push_back(columndefinition("BIT", "BIT(8)", sql::DataType::BIT, "0", false, 8, 0, true, "", 0));
  columns.push_back(columndefinition("TINYINT", "TINYINT", sql::DataType::TINYINT, "127", true, 3, 0, true, "", 0));
  columns.push_back(columndefinition("TINYINT", "TINYINT NOT NULL", sql::DataType::TINYINT, "127", true, 3, 0, false, "", 0));
  columns.push_back(columndefinition("TINYINT", "TINYINT NOT NULL DEFAULT -1", sql::DataType::TINYINT, "127", true, 3, 0, false, "-1", 0));
  columns.push_back(columndefinition("TINYINT", "TINYINT(1)", sql::DataType::TINYINT, "0", true, 3, 0, true, "", 0));
  columns.push_back(columndefinition("TINYINT UNSIGNED", "TINYINT UNSIGNED", sql::DataType::TINYINT, "255", false, 3, 0, true, "", 0));
  columns.push_back(columndefinition("TINYINT UNSIGNED ZEROFILL", "TINYINT ZEROFILL", sql::DataType::TINYINT, "-1", false, 3, 0, true, "", 0));
  /* Alias of BOOLEAN */
  columns.push_back(columndefinition("TINYINT", "BOOLEAN", sql::DataType::TINYINT, "1", true, 3, 0, true, "", 0));
  columns.push_back(columndefinition("TINYINT", "BOOLEAN NOT NULL", sql::DataType::TINYINT, "1", true, 3, 0, false, "", 0));
  columns.push_back(columndefinition("TINYINT", "BOOLEAN DEFAULT 0", sql::DataType::TINYINT, "1", true, 3, 0, true, "0", 0));
  columns.push_back(columndefinition("SMALLINT", "SMALLINT", sql::DataType::SMALLINT, "-32768", true, 5, 0, true, "", 0));
  columns.push_back(columndefinition("SMALLINT", "SMALLINT NOT NULL", sql::DataType::SMALLINT, "-32768", true, 5, 0, false, "", 0));
  columns.push_back(columndefinition("SMALLINT", "SMALLINT NOT NULL DEFAULT -1", sql::DataType::SMALLINT, "-32768", true, 5, 0, false, "-1", 0));
  columns.push_back(columndefinition("SMALLINT", "SMALLINT(3)", sql::DataType::SMALLINT, "-32768", true, 5, 0, true, "", 0));
  columns.push_back(columndefinition("SMALLINT UNSIGNED", "SMALLINT UNSIGNED", sql::DataType::SMALLINT, "65535", false, 5, 0, true, "", 0));
  columns.push_back(columndefinition("SMALLINT UNSIGNED ZEROFILL", "SMALLINT ZEROFILL", sql::DataType::SMALLINT, "123", false, 5, 0, true, "", 0));
  columns.push_back(columndefinition("SMALLINT UNSIGNED ZEROFILL", "SMALLINT UNSIGNED ZEROFILL DEFAULT 101", sql::DataType::SMALLINT, "123", false, 5, 0, true, "00101", 0));
  columns.push_back(columndefinition("MEDIUMINT", "MEDIUMINT", sql::DataType::MEDIUMINT, "-8388608", true, 7, 0, true, "", 0));
  columns.push_back(columndefinition("MEDIUMINT", "MEDIUMINT NOT NULL", sql::DataType::MEDIUMINT, "-8388608", true, 7, 0, false, "", 0));
  columns.push_back(columndefinition("MEDIUMINT", "MEDIUMINT(1)", sql::DataType::MEDIUMINT, "2", true, 7, 0, true, "", 0));
  columns.push_back(columndefinition("MEDIUMINT", "MEDIUMINT(2) DEFAULT 12", sql::DataType::MEDIUMINT, "2", true, 7, 0, true, "12", 0));
  columns.push_back(columndefinition("MEDIUMINT UNSIGNED", "MEDIUMINT UNSIGNED", sql::DataType::MEDIUMINT, "16777215", false, 7, 0, true, "", 0));
  columns.push_back(columndefinition("MEDIUMINT UNSIGNED ZEROFILL", "MEDIUMINT UNSIGNED ZEROFILL", sql::DataType::MEDIUMINT, "1677721", false, 7, 0, true, "", 0));
  /* Alias of INTEGER */
  columns.push_back(columndefinition("INT", "INTEGER", sql::DataType::INTEGER, "2147483647", true, 10, 0, true, "", 0));
  columns.push_back(columndefinition("INT", "INTEGER NOT NULL", sql::DataType::INTEGER, "2147483647", true, 10, 0, false, "", 0));
  columns.push_back(columndefinition("INT", "INTEGER(1)", sql::DataType::INTEGER, "3", true, 10, 0, true, "", 0));
  columns.push_back(columndefinition("INT UNSIGNED", "INT UNSIGNED", sql::DataType::INTEGER, "4294967295", false, 10, 0, true, "", 0));
  /* If you specify ZEROFILL for a numeric column, MySQL automatically adds the UNSIGNED  attribute to the column.  */
  columns.push_back(columndefinition("INT UNSIGNED ZEROFILL", "INT(4) SIGNED ZEROFILL", sql::DataType::INTEGER, "1", false, 10, 0, true, "", 0));
  columns.push_back(columndefinition("INT", "INT(4) SIGNED DEFAULT -123", sql::DataType::INTEGER, "1", true, 10, 0, true, "-123", 0));
  columns.push_back(columndefinition("BIGINT", "BIGINT", sql::DataType::BIGINT, "-9223372036854775808", true, 19, 0, true, "", 0));
  columns.push_back(columndefinition("BIGINT", "BIGINT NOT NULL", sql::DataType::BIGINT, "-9223372036854775808", true, 19, 0, false, "", 0));
  columns.push_back(columndefinition("BIGINT UNSIGNED", "BIGINT UNSIGNED", sql::DataType::BIGINT, "18446744073709551615", false, 19, 0, true, "", 0));
  columns.push_back(columndefinition("BIGINT UNSIGNED", "BIGINT UNSIGNED", sql::DataType::BIGINT, "18446744073709551615", false, 19, 0, true, "", 0));
  columns.push_back(columndefinition("BIGINT UNSIGNED ZEROFILL", "BIGINT(4) ZEROFILL DEFAULT 10101", sql::DataType::BIGINT, "2", false, 19, 0, true, "10101", 0));
  columns.push_back(columndefinition("FLOAT", "FLOAT", sql::DataType::REAL, "-1.01", true, 12, 0, true, "", 0));
  columns.push_back(columndefinition("FLOAT", "FLOAT NOT NULL", sql::DataType::REAL, "-1.01", true, 12, 0, false, "", 0));
  columns.push_back(columndefinition("FLOAT UNSIGNED", "FLOAT UNSIGNED", sql::DataType::REAL, "1.01", false, 12, 0, true, "", 0));
  columns.push_back(columndefinition("FLOAT UNSIGNED ZEROFILL", "FLOAT(5,3) UNSIGNED ZEROFILL", sql::DataType::REAL, "1.01", false, 5, 3, true, "", 0));
  columns.push_back(columndefinition("FLOAT", "FLOAT(6)", sql::DataType::REAL, "1.01", true, 12, 0, true, "", 0));
  columns.push_back(columndefinition("FLOAT", "FLOAT(9) DEFAULT 1.01", sql::DataType::REAL, "1.01", true, 12, 0, true, "1.01", 0));
  columns.push_back(columndefinition("DOUBLE", "DOUBLE", sql::DataType::DOUBLE, "-1.01", true, 22, 0, true, "", 0));
  columns.push_back(columndefinition("DOUBLE", "DOUBLE NOT NULL", sql::DataType::DOUBLE, "-1.01", true, 22, 0, false, "", 0));
  columns.push_back(columndefinition("DOUBLE UNSIGNED", "DOUBLE UNSIGNED", sql::DataType::DOUBLE, "1.01", false, 22, 0, true, "", 0));
  columns.push_back(columndefinition("DOUBLE UNSIGNED", "DOUBLE UNSIGNED DEFAULT 1.123", sql::DataType::DOUBLE, "1.01", false, 22, 0, true, "1.123", 0));
  columns.push_back(columndefinition("DOUBLE UNSIGNED ZEROFILL", "DOUBLE(5,3) UNSIGNED ZEROFILL", sql::DataType::DOUBLE, "1.01", false, 5, 3, true, "", 0));
  columns.push_back(columndefinition("DECIMAL", "DECIMAL", sql::DataType::DECIMAL, "-1.01", true, 10, 0, true, "", 0));
  columns.push_back(columndefinition("DECIMAL", "DECIMAL NOT NULL", sql::DataType::DECIMAL, "-1.01", true, 10, 0, false, "", 0));
  columns.push_back(columndefinition("DECIMAL UNSIGNED", "DECIMAL UNSIGNED", sql::DataType::DECIMAL, "1.01", false, 10, 0, true, "", 0));
  columns.push_back(columndefinition("DECIMAL UNSIGNED ZEROFILL", "DECIMAL(5,3) UNSIGNED ZEROFILL", sql::DataType::DECIMAL, "1.01", false, 5, 3, true, "", 0));
  columns.push_back(columndefinition("DECIMAL UNSIGNED ZEROFILL", "DECIMAL(6,3) UNSIGNED ZEROFILL DEFAULT 34.56", sql::DataType::DECIMAL, "1.01", false, 6, 3, true, "034.560", 0));
  columns.push_back(columndefinition("DATE", "DATE", sql::DataType::DATE, "2009-02-09", false, 10, 0, true, "", 0));
  columns.push_back(columndefinition("DATE", "DATE NOT NULL", sql::DataType::DATE, "2009-02-12", false, 10, 0, false, "", 0));
  columns.push_back(columndefinition("DATE", "DATE NOT NULL DEFAULT '2009-02-16'", sql::DataType::DATE, "2009-02-12", false, 10, 0, false, "2009-02-16", 0));
  columns.push_back(columndefinition("DATETIME", "DATETIME", sql::DataType::TIMESTAMP, "2009-02-09 20:05:43", false, 19, 0, true, "", 0));
  columns.push_back(columndefinition("DATETIME", "DATETIME NOT NULL", sql::DataType::TIMESTAMP, "2009-02-12 17:49:21", false, 19, 0, false, "", 0));
  columns.push_back(columndefinition("DATETIME", "DATETIME NOT NULL DEFAULT '2009-02-12 21:36:54'", sql::DataType::TIMESTAMP, "2009-02-12 17:49:21", false, 19, 0, false, "2009-02-12 21:36:54", 0));
  /* TODO this might be server dependent! */
  columns.push_back(columndefinition("TIMESTAMP", "TIMESTAMP", sql::DataType::TIMESTAMP, "2038-01-09 03:14:07", false, 19, 0, false, "0000-00-00 00:00:00", 0));
  columns.push_back(columndefinition("TIME", "TIME", sql::DataType::TIME, "-838:59:59", true, 8, 0, true, "", 0));
  columns.push_back(columndefinition("TIME", "TIME NOT NULL", sql::DataType::TIME, "838:59:59", true, 8, 0, false, "", 0));
  columns.push_back(columndefinition("TIME", "TIME DEFAULT '12:39:41'", sql::DataType::TIME, "-838:59:59", true, 8, 0, true, "12:39:41", 0));
  columns.push_back(columndefinition("YEAR", "YEAR", sql::DataType::DATE, "1901", false, 4, 0, true, "", 0));
  columns.push_back(columndefinition("YEAR", "YEAR NOT NULL", sql::DataType::DATE, "1902", false, 4, 0, false, "", 0));
  columns.push_back(columndefinition("YEAR", "YEAR(4)", sql::DataType::DATE, "2009", false, 4, 0, true, "", 0));
  columns.push_back(columndefinition("YEAR", "YEAR(2)", sql::DataType::DATE, "1", false, 4, 0, true, "", 0));
  columns.push_back(columndefinition("YEAR", "YEAR(3) DEFAULT '2009'", sql::DataType::DATE, "1", false, 4, 0, true, "2009", 0));
  columns.push_back(columndefinition("CHAR", "CHAR", sql::DataType::CHAR, "a", false, 1, 0, true, "", 0));
  columns.push_back(columndefinition("CHAR", "CHAR NOT NULL", sql::DataType::CHAR, "a", false, 1, 0, false, "", 0));
  columns.push_back(columndefinition("CHAR", "CHAR(255)", sql::DataType::CHAR, "abc", false, 255, 0, true, "", 0));
  columns.push_back(columndefinition("CHAR", "CHAR(254) NOT NULL", sql::DataType::CHAR, "abc", false, 254, 0, false, "", 0));
  columns.push_back(columndefinition("CHAR", "CHAR(254) NOT NULL DEFAULT 'abc'", sql::DataType::CHAR, "abc", false, 254, 0, false, "abc", 0));
  // 3byte
  columns.push_back(columndefinition("CHAR", "NATIONAL CHAR(255)", sql::DataType::CHAR, "abc", false, 255, 0, true, "", 765));
  columns.push_back(columndefinition("CHAR", "NATIONAL CHAR(215) NOT NULL", sql::DataType::CHAR, "abc", false, 215, 0, false, "", 645));
  columns.push_back(columndefinition("CHAR", "NATIONAL CHAR(215) NOT NULL DEFAULT 'Ulf'", sql::DataType::CHAR, "abc", false, 215, 0, false, "Ulf", 645));
  columns.push_back(columndefinition("CHAR", "CHAR(255) CHARACTER SET 'utf8'", sql::DataType::CHAR, "abc", false, 255, 0, true, "", 765));
  columns.push_back(columndefinition("CHAR", "CHAR(55) CHARACTER SET 'utf8' NOT NULL ", sql::DataType::CHAR, "abc", false, 55, 0, false, "", 165));
  /* TODO this might be server dependent! */
  columns.push_back(columndefinition("CHAR", "CHAR(250) CHARACTER SET 'utf8' COLLATE 'utf8_bin'", sql::DataType::CHAR, "abc", false, 250, 0, true, "", 750));
  columns.push_back(columndefinition("CHAR", "CHAR(250) CHARACTER SET 'utf8' COLLATE 'utf8_bin' DEFAULT 'Wendel'", sql::DataType::CHAR, "abc", false, 250, 0, true, "Wendel", 750));
  columns.push_back(columndefinition("CHAR", "CHAR(43) CHARACTER SET 'utf8' COLLATE 'utf8_bin' NOT NULL", sql::DataType::CHAR, "abc", false, 43, 0, false, "", 129));
  columns.push_back(columndefinition("CHAR", "CHAR(123) CHARACTER SET 'ucs2'", sql::DataType::CHAR, "abc", false, 123, 0, true, "", 246));
  /* The CHAR BYTE data type is an alias for the BINARY data type. This is a compatibility feature.  */
  columns.push_back(columndefinition("BINARY", "CHAR(255) BYTE", sql::DataType::BINARY, "abc", true, 255, 0, true, "", 255));
  columns.push_back(columndefinition("BINARY", "CHAR(12) BYTE NOT NULL", sql::DataType::BINARY, "abc", true, 12, 0, false, "", 12));
  columns.push_back(columndefinition("CHAR", "CHAR(14) DEFAULT 'Andrey'", sql::DataType::CHAR, "abc", true, 14, 0, true, "Andrey", 0));
  /*  Specifying the CHARACTER SET binary  attribute for a character data type
   causes the column to be created as the corresponding binary data type:
   CHAR becomes BINARY, VARCHAR becomes VARBINARY, and TEXT becomes BLOB.
   For the ENUM and SET data types, this does not occur; they are created as declared.   */
  columns.push_back(columndefinition("BINARY", "CHAR(25) CHARACTER SET 'binary'", sql::DataType::BINARY, "abc", false, 25, 0, true, "", 25));
  columns.push_back(columndefinition("VARCHAR", "VARCHAR(10)", sql::DataType::VARCHAR, "a", false, 10, 0, true, "", 0));
  columns.push_back(columndefinition("VARCHAR", "VARCHAR(7) NOT NULL", sql::DataType::VARCHAR, "a", false, 7, 0, false, ""));
  columns.push_back(columndefinition("VARCHAR", "VARCHAR(255) DEFAULT 'Good night twitter. BTW, go MySQL!'", sql::DataType::VARCHAR, "a", false, 255, 0, true, "Good night twitter. BTW, go MySQL!", 0));
  columns.push_back(columndefinition("VARCHAR", "VARCHAR(11) CHARACTER SET 'utf8'", sql::DataType::VARCHAR, "a", false, 11, 0, true, "", 33));
  columns.push_back(columndefinition("VARCHAR", "VARCHAR(11) CHARACTER SET 'ascii' DEFAULT 'Hristov'", sql::DataType::VARCHAR, "a", false, 11, 0, true, "Hristov", 11));
  /* TODO this might be server dependent! */
  columns.push_back(columndefinition("VARCHAR", "VARCHAR(12) CHARACTER SET 'utf8' COLLATE 'utf8_bin'", sql::DataType::VARCHAR, "a", false, 12, 0, true, "", 36));
  columns.push_back(columndefinition("VARBINARY", "VARCHAR(13) BYTE", sql::DataType::VARBINARY, "a", false, 13, 0, true, "", 13));
  columns.push_back(columndefinition("VARBINARY", "VARCHAR(14) BYTE NOT NULL", sql::DataType::VARBINARY, "a", false, 14, 0, false, "", 14));
  columns.push_back(columndefinition("BINARY", "BINARY(1)", sql::DataType::BINARY, "a", false, 1, 0, true, "", 1));
  columns.push_back(columndefinition("VARBINARY", "VARBINARY(1)", sql::DataType::VARBINARY, "a", false, 1, 0, true, "", 1));
  columns.push_back(columndefinition("VARBINARY", "VARBINARY(2) NOT NULL", sql::DataType::VARBINARY, "a", false, 2, 0, false, "", 2));
  columns.push_back(columndefinition("VARBINARY", "VARBINARY(20) NOT NULL DEFAULT 'Lawrin'", sql::DataType::VARBINARY, "a", false, 20, 0, false, "Lawrin"));
  columns.push_back(columndefinition("TINYBLOB", "TINYBLOB", sql::DataType::VARBINARY, "a", false, 255, 0, true, "", 255));
  columns.push_back(columndefinition("TINYTEXT", "TINYTEXT", sql::DataType::VARCHAR, "a", false, 255, 0, true, "", 255));
  columns.push_back(columndefinition("TINYTEXT", "TINYTEXT NOT NULL", sql::DataType::VARCHAR, "a", false, 255, 0, false, "", 255));
  columns.push_back(columndefinition("TINYTEXT", "TINYTEXT", sql::DataType::VARCHAR, "a", false, 255, 0, true, "", 255));
  columns.push_back(columndefinition("TINYTEXT", "TINYTEXT CHARACTER SET 'utf8'", sql::DataType::VARCHAR, "a", false, 255, 0, true, "", 255));
  /* TODO this might be server dependent! */
  columns.push_back(columndefinition("TINYTEXT", "TINYTEXT CHARACTER SET 'utf8' COLLATE 'utf8_bin'", sql::DataType::VARCHAR, "a", false, 255, 0, true, "", 255));
  columns.push_back(columndefinition("MEDIUMBLOB", "MEDIUMBLOB", sql::DataType::LONGVARBINARY, "a", false, 16777215, 0, true, "", 16777215));
  columns.push_back(columndefinition("MEDIUMBLOB", "MEDIUMBLOB NOT NULL", sql::DataType::LONGVARBINARY, "a", false, 16777215, 0, false, "", 16777215));
  columns.push_back(columndefinition("MEDIUMTEXT", "MEDIUMTEXT", sql::DataType::LONGVARCHAR, "a", false, 16777215, 0, true, "", 16777215));
  columns.push_back(columndefinition("MEDIUMTEXT", "MEDIUMTEXT NOT NULL", sql::DataType::LONGVARCHAR, "a", false, 16777215, 0, false, "", 16777215));
  columns.push_back(columndefinition("MEDIUMTEXT", "MEDIUMTEXT CHARSET 'utf8'", sql::DataType::LONGVARCHAR, "a", false, 16777215, 0, true, "", 16777215));
  columns.push_back(columndefinition("MEDIUMTEXT", "MEDIUMTEXT CHARSET 'utf8' COLLATE 'utf8_bin'", sql::DataType::LONGVARCHAR, "a", false, 16777215, 0, true, ""));
  columns.push_back(columndefinition("LONGBLOB", "LONGBLOB", sql::DataType::LONGVARBINARY, "a", false, L64(4294967295), 0, true, "", L64(4294967295)));
  columns.push_back(columndefinition("LONGBLOB", "LONGBLOB NOT NULL", sql::DataType::LONGVARBINARY, "a", false, L64(4294967295), 0, false, "", L64(4294967295)));
  columns.push_back(columndefinition("LONGTEXT", "LONGTEXT", sql::DataType::LONGVARCHAR, "a", false, L64(4294967295), 0, true, "", L64(4294967295)));
  columns.push_back(columndefinition("LONGTEXT", "LONGTEXT NOT NULL", sql::DataType::LONGVARCHAR, "a", false, L64(4294967295), 0, false, "", L64(4294967295)));
  columns.push_back(columndefinition("LONGTEXT", "LONGTEXT CHARSET 'utf8'", sql::DataType::LONGVARCHAR, "a", false, L64(4294967295), 0, true, "", L64(4294967295)));
  /* TODO this might be server dependent! */
  columns.push_back(columndefinition("LONGTEXT", "LONGTEXT CHARSET 'utf8' COLLATE 'utf8_bin'", sql::DataType::LONGVARCHAR, "a", false, L64(4294967295), 0, true, "", L64(4294967295)));
  columns.push_back(columndefinition("ENUM", "ENUM('yes', 'no')", sql::DataType::VARCHAR, "yes", false, 3, 0, true, "", 3));
  columns.push_back(columndefinition("ENUM", "ENUM('yes', 'no') NOT NULL", sql::DataType::VARCHAR, "yes", false, 3, 0, false, "", 3));
  columns.push_back(columndefinition("ENUM", "ENUM('yes', 'no', 'not sure') NOT NULL", sql::DataType::VARCHAR, "yes", false, 8, 0, false, "", 3));
  columns.push_back(columndefinition("ENUM", "ENUM('yes', 'no', 'buy') NOT NULL DEFAULT 'buy'", sql::DataType::VARCHAR, "yes", false, 3, 0, false, "buy", 8));
  columns.push_back(columndefinition("SET", "SET('yes', 'no')", sql::DataType::VARCHAR, "yes", false, 6, 0, true, "", 6));
  columns.push_back(columndefinition("SET", "SET('yes', 'no') CHARSET 'ascii'", sql::DataType::VARCHAR, "yes", false, 6, 0, true, "", 6));
  columns.push_back(columndefinition("SET", "SET('yes', 'no') CHARSET 'ascii' DEFAULT 'yes'", sql::DataType::VARCHAR, "yes", false, 6, 0, true, "yes", 6));
  columns.push_back(columndefinition("SET", "SET('yes', 'no', 'ascii') CHARSET 'ascii' NOT NULL", sql::DataType::VARCHAR, "yes", false, 12, 0, false, "", 12));

  /*
  ResultSet getAttributes(String catalog,
                    String schemaPattern,
                    String typeNamePattern,
                    String attributeNamePattern)
                    throws SQLException

  Retrieves a description of the given attribute of the given type for a user-defined type (UDT) that is available in the given schema and catalog.

Descriptions are returned only for attributes of UDTs matching the catalog, schema, type, and attribute name criteria. They are ordered by TYPE_CAT, TYPE_SCHEM, TYPE_NAME and ORDINAL_POSITION. This description does not contain inherited attributes.

The ResultSet object that is returned has the following columns:

  1. TYPE_CAT String => type catalog (may be null)
  2. TYPE_SCHEM String => type schema (may be null)
  3. TYPE_NAME String => type name
  4. ATTR_NAME String => attribute name
  5. DATA_TYPE int => attribute type SQL type from java.sql.Types
  6. ATTR_TYPE_NAME String => Data source dependent type name. For a UDT, the type name is fully qualified. For a REF, the type name is fully qualified and represents the target type of the reference type.
  7. ATTR_SIZE int => column size. For char or date types this is the maximum number of characters; for numeric or decimal types this is precision.
  8. DECIMAL_DIGITS int => the number of fractional digits. Null is returned for data types where DECIMAL_DIGITS is not applicable.
  9. NUM_PREC_RADIX int => Radix (typically either 10 or 2)
  10. NULLABLE int => whether NULL is allowed
   * attributeNoNulls - might not allow NULL values
   * attributeNullable - definitely allows NULL values
   * attributeNullableUnknown - nullability unknown
  11. REMARKS String => comment describing column (may be null)
  12. ATTR_DEF String => default value (may be null)
  13. SQL_DATA_TYPE int => unused
  14. SQL_DATETIME_SUB int => unused
  15. CHAR_OCTET_LENGTH int => for char types the maximum number of bytes in the column
  16. ORDINAL_POSITION int => index of the attribute in the UDT (starting at 1)
  17. IS_NULLABLE String => ISO rules are used to determine the nullability for a attribute.
   * YES --- if the attribute can include NULLs
   * NO --- if the attribute cannot include NULLs
   * empty string --- if the nullability for the attribute is unknown
  18. SCOPE_CATALOG String => catalog of table that is the scope of a reference attribute (null if DATA_TYPE isn't REF)
  19. SCOPE_SCHEMA String => schema of table that is the scope of a reference attribute (null if DATA_TYPE isn't REF)
  20. SCOPE_TABLE String => table name that is the scope of a reference attribute (null if the DATA_TYPE isn't REF)
  21. SOURCE_DATA_TYPE short => source type of a distinct type or user-generated Ref type,SQL type from java.sql.Types (null if DATA_TYPE isn't DISTINCT or user-generated REF)
   */
  attributes.push_back(udtattribute("TYPE_CAT", 0));
  attributes.push_back(udtattribute("TYPE_SCHEM", 0));
  attributes.push_back(udtattribute("TYPE_NAME", 0));
  attributes.push_back(udtattribute("ATTR_NAME", 0));
  attributes.push_back(udtattribute("DATA_TYPE", 0));
  attributes.push_back(udtattribute("ATTR_TYPE_NAME", 0));
  attributes.push_back(udtattribute("ATTR_SIZE", 0));
  attributes.push_back(udtattribute("DECIMAL_DIGITS", 0));
  attributes.push_back(udtattribute("NUM_PREC_RADIX", 0));
  attributes.push_back(udtattribute("NULLABLE", 0));
  attributes.push_back(udtattribute("REMARKS", 0));
  attributes.push_back(udtattribute("ATTR_DEF", 0));
  attributes.push_back(udtattribute("SQL_DATA_TYPE", 0));
  attributes.push_back(udtattribute("SQL_DATETIME_SUB", 0));
  attributes.push_back(udtattribute("CHAR_OCTET_LENGTH", 0));
  attributes.push_back(udtattribute("ORDINAL_POSITION", 0));
  attributes.push_back(udtattribute("IS_NULLABLE", 0));
  attributes.push_back(udtattribute("SCOPE_CATALOG", 0));
  attributes.push_back(udtattribute("SCOPE_SCHEMA", 0));
  attributes.push_back(udtattribute("SCOPE_TABLE", 0));
  attributes.push_back(udtattribute("SOURCE_DATA_TYPE", 0));
}

void unit_fixture::setUp()
{
  created_objects.clear();

  try
  {
    con.reset(getConnection());
  }
  catch (sql::SQLException & sqle)
  {
    logErr(String("Couldn't get connection") + sqle.what());
    throw sqle;
  }

  /* TODO: conect message incl. version using logDebug() */

  /*
   logDebug("Driver: " + driver->getName());
           + " " + String(driver->getMajorVersion() + driver->getMajorVersion + String(".") + driver->getMinorVersion());*/

  con->setSchema(db);
}

void unit_fixture::tearDown()
{

  res.reset();
  for (int i=0; i < static_cast<int> (created_objects.size() - 1); i+=2)
  {
    try
    {
      dropSchemaObject(created_objects[ i ], created_objects[ i + 1 ]);
    }
    catch (sql::SQLException &)
    {
    }
  }

  stmt.reset();
  pstmt.reset();
  con.reset();
}

void unit_fixture::createSchemaObject(String object_type, String object_name,
                                      String columns_and_other_stuff)
{
  created_objects.push_back(object_type);
  created_objects.push_back(object_name);

  dropSchemaObject(object_type, object_name);

  String sql("CREATE  ");
  sql.resize(object_name.length()
             + object_type.length() + columns_and_other_stuff.length() + 10);
  sql.append(object_type);
  sql.append(" ");
  sql.append(object_name);
  sql.append(" ");
  sql.append(columns_and_other_stuff);

  stmt->executeUpdate(sql);
}

void unit_fixture::dropSchemaObject(String object_type, String object_name)
{
  stmt->executeUpdate(String("DROP ") + object_type + " IF EXISTS "
                      + object_name);
}

void unit_fixture::createTable(String table_name, String columns_and_other_stuff)
{
  createSchemaObject("TABLE", table_name, columns_and_other_stuff);
}

void unit_fixture::dropTable(String table_name)
{
  dropSchemaObject("TABLE", table_name);
}

sql::Connection * unit_fixture::getConnection()
{
  if (driver == NULL)
  {
    driver=get_driver_instance();
  }

  return driver->connect(url, user, passwd);
}

void unit_fixture::logMsg(const String message)
{
  TestsListener::theInstance().messagesLog(message);
}

void unit_fixture::logErr(const String message)
{
  TestsListener::theInstance().errorsLog(message);
}

void unit_fixture::logDebug(const String message)
{
  logMsg(message);
}

std::string unit_fixture::exceptionIsOK(sql::SQLException & e)
{
  return exceptionIsOK(e, "HY000", 0);
}

std::string unit_fixture::exceptionIsOK(sql::SQLException &e, const std::string& sql_state, int errno)
{

  std::stringstream reason;
  reason.str("");

  std::string what(e.what());
  if (what.empty())
  {
    reason << "Exception must not have an empty message.";
    logMsg(reason.str());
    return reason.str();
  }

  if (e.getErrorCode() != errno)
  {
    reason << "Expecting error code '" << errno << "' got '" << e.getErrorCode() << "'";
    logMsg(reason.str());
    return reason.str();
  }

  if (e.getSQLState() != sql_state)
  {
    reason << "Expecting sqlstate '" << sql_state << "' got '" << e.getSQLState() << "'";
    logMsg(reason.str());
    return reason.str();
  }

  return reason.str();
}


} /* namespace testsuite */
