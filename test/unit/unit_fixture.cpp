/*
 * Copyright (c) 2008, 2020, Oracle and/or its affiliates.
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



#include "unit_fixture.h"
#include <cstdio>

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
  url=TestsRunner::theInstance().getStartOptions()->getString("dbUrl");
  user=TestsRunner::theInstance().getStartOptions()->getString("dbUser");
  passwd=TestsRunner::theInstance().getStartOptions()->getString("dbPasswd");
  db=TestsRunner::theInstance().getStartOptions()->getString("dbSchema");

  columns.push_back(columndefinition("BIT", "BIT", sql::DataType::BIT, "0", false, 1, 0, true, "", 0, "NO", false));

  columns.push_back(columndefinition("BIT", "BIT NOT NULL", sql::DataType::BIT, "1", false, 1, 0, false, "", 0, "NO", false));
  columns.push_back(columndefinition("BIT", "BIT(5) NOT NULL", sql::DataType::BIT, "0", false, 5, 0, false, "", 0, "NO", false));
  columns.push_back(columndefinition("BIT", "BIT(8)", sql::DataType::BIT, "0", false, 8, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("TINYINT", "TINYINT", sql::DataType::TINYINT, "127", true, 3, 0, true, "", 0, "NO", false, "127"));
  columns.push_back(columndefinition("TINYINT", "TINYINT NOT NULL", sql::DataType::TINYINT, "127", true, 3, 0, false, "", 0, "NO", false, "127"));
  columns.push_back(columndefinition("TINYINT", "TINYINT NOT NULL DEFAULT -1", sql::DataType::TINYINT, "12", true, 3, 0, false, "-1", 0, "NO", false, "12"));
  columns.push_back(columndefinition("TINYINT", "TINYINT(1)", sql::DataType::TINYINT, "3", true, 3, 0, true, "", 0, "NO", false, "3"));
  columns.push_back(columndefinition("TINYINT UNSIGNED", "TINYINT UNSIGNED", sql::DataType::TINYINT, "255", false, 3, 0, true, "", 0, "NO", false, "255"));
  columns.push_back(columndefinition("TINYINT UNSIGNED ZEROFILL", "TINYINT ZEROFILL", sql::DataType::TINYINT, "1", false, 3, 0, true, "", 0, "NO", false));
  // Alias of BOOLEAN
  columns.push_back(columndefinition("TINYINT", "BOOLEAN", sql::DataType::TINYINT, "1", true, 3, 0, true, "", 0, "NO", false, "1"));

  columns.push_back(columndefinition("TINYINT", "BOOLEAN NOT NULL", sql::DataType::TINYINT, "2", true, 3, 0, false, "", 0, "NO", false, "2"));
  columns.push_back(columndefinition("TINYINT", "BOOLEAN DEFAULT 0", sql::DataType::TINYINT, "3", true, 3, 0, true, "0", 0, "NO", false, "3"));
  columns.push_back(columndefinition("SMALLINT", "SMALLINT", sql::DataType::SMALLINT, "-32768", true, 5, 0, true, "", 0, "NO", true, "-32768"));
  columns.push_back(columndefinition("SMALLINT", "SMALLINT NOT NULL", sql::DataType::SMALLINT, "32767", true, 5, 0, false, "", 0, "NO", false, "32767"));
  columns.push_back(columndefinition("SMALLINT", "SMALLINT NOT NULL DEFAULT -1", sql::DataType::SMALLINT, "-32768", true, 5, 0, false, "-1", 0, "NO", false));
  columns.push_back(columndefinition("SMALLINT", "SMALLINT(3)", sql::DataType::SMALLINT, "-32768", true, 5, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("SMALLINT UNSIGNED", "SMALLINT UNSIGNED", sql::DataType::SMALLINT, "65535", false, 5, 0, true, "", 0, "NO", false, "65535"));
  columns.push_back(columndefinition("SMALLINT UNSIGNED ZEROFILL", "SMALLINT ZEROFILL", sql::DataType::SMALLINT, "123", false, 5, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("SMALLINT UNSIGNED ZEROFILL", "SMALLINT UNSIGNED ZEROFILL DEFAULT 101", sql::DataType::SMALLINT, "123", false, 5, 0, true, "00101", 0, "NO", false));
  columns.push_back(columndefinition("MEDIUMINT", "MEDIUMINT", sql::DataType::MEDIUMINT, "-8388608", true, 7, 0, true, "", 0, "NO", true, "-8388608"));
  columns.push_back(columndefinition("MEDIUMINT", "MEDIUMINT NOT NULL", sql::DataType::MEDIUMINT, "-8388608", true, 7, 0, false, "", 0, "NO", true));
  columns.push_back(columndefinition("MEDIUMINT", "MEDIUMINT(1)", sql::DataType::MEDIUMINT, "2", true, 7, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("MEDIUMINT", "MEDIUMINT(2) DEFAULT 12", sql::DataType::MEDIUMINT, "2", true, 7, 0, true, "12", 0, "NO", true));
  columns.push_back(columndefinition("MEDIUMINT UNSIGNED", "MEDIUMINT UNSIGNED", sql::DataType::MEDIUMINT, "16777215", false, 7, 0, true, "", 0, "NO", false, "16777215"));
  columns.push_back(columndefinition("MEDIUMINT UNSIGNED ZEROFILL", "MEDIUMINT UNSIGNED ZEROFILL", sql::DataType::MEDIUMINT, "1677721", false, 7, 0, true, "", 0, "NO", false));
  // Alias of INTEGER
  columns.push_back(columndefinition("INT", "INTEGER", sql::DataType::INTEGER, "2147483647", true, 10, 0, true, "", 0, "NO", false, "2147483647"));
  columns.push_back(columndefinition("INT", "INTEGER NOT NULL", sql::DataType::INTEGER, "2147483647", true, 10, 0, false, "", 0, "NO", false));
  columns.push_back(columndefinition("INT", "INTEGER(1)", sql::DataType::INTEGER, "3", true, 10, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("INT UNSIGNED", "INT UNSIGNED", sql::DataType::INTEGER, "4294967295", false, 10, 0, true, "", 0, "NO", false, "4294967295"));
  // If you specify ZEROFILL for a numeric column, MySQL automatically adds the UNSIGNED  attribute to the column.
  columns.push_back(columndefinition("INT UNSIGNED ZEROFILL", "INT(4) SIGNED ZEROFILL", sql::DataType::INTEGER, "1", false, 10, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("INT", "INT(4) SIGNED DEFAULT -123", sql::DataType::INTEGER, "1", true, 10, 0, true, "-123", 0, "NO", false));
  columns.push_back(columndefinition("BIGINT", "BIGINT", sql::DataType::BIGINT, "-9223372036854775808", true, 19, 0, true, "", 0, "NO", true, "-9223372036854775808"));

  columns.push_back(columndefinition("BIGINT", "BIGINT NOT NULL", sql::DataType::BIGINT, "-9223372036854775808", true, 19, 0, false, "", 0, "NO", true));
  columns.push_back(columndefinition("BIGINT UNSIGNED", "BIGINT UNSIGNED", sql::DataType::BIGINT, "18446744073709551615", false, 20, 0, true, "", 0, "NO", false, "18446744073709551615"));
  columns.push_back(columndefinition("BIGINT UNSIGNED", "BIGINT UNSIGNED", sql::DataType::BIGINT, "18446744073709551615", false, 20, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("BIGINT UNSIGNED ZEROFILL", "BIGINT(4) ZEROFILL DEFAULT 10101", sql::DataType::BIGINT, "2", false, 20, 0, true, "10101", 0, "NO", false));
  columns.push_back(columndefinition("FLOAT", "FLOAT", sql::DataType::REAL, "-1.01", true, 12, 0, true, "", 0, "NO", true));
  columns.push_back(columndefinition("FLOAT", "FLOAT NOT NULL", sql::DataType::REAL, "-1.01", true, 12, 0, false, "", 0, "NO", true));
  columns.push_back(columndefinition("FLOAT UNSIGNED", "FLOAT UNSIGNED", sql::DataType::REAL, "1.01", false, 12, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("FLOAT UNSIGNED ZEROFILL", "FLOAT(5,3) UNSIGNED ZEROFILL", sql::DataType::REAL, "1.01", false, 5, 3, true, "", 0, "NO", false));
  columns.push_back(columndefinition("FLOAT", "FLOAT(6)", sql::DataType::REAL, "1.01", true, 12, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("FLOAT", "FLOAT(9) DEFAULT 1.01", sql::DataType::REAL, "1.01", true, 12, 0, true, "1.01", 0, "NO", false));
  columns.push_back(columndefinition("DOUBLE", "DOUBLE", sql::DataType::DOUBLE, "-1.01", true, 22, 0, true, "", 0, "NO", true));
  columns.push_back(columndefinition("DOUBLE", "DOUBLE NOT NULL", sql::DataType::DOUBLE, "-1.01", true, 22, 0, false, "", 0, "NO", true));
  columns.push_back(columndefinition("DOUBLE UNSIGNED", "DOUBLE UNSIGNED", sql::DataType::DOUBLE, "1.01", false, 22, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("DOUBLE UNSIGNED", "DOUBLE UNSIGNED DEFAULT 1.123", sql::DataType::DOUBLE, "1.01", false, 22, 0, true, "1.123", 0, "NO", true));
  columns.push_back(columndefinition("DOUBLE UNSIGNED ZEROFILL", "DOUBLE(5,3) UNSIGNED ZEROFILL", sql::DataType::DOUBLE, "1.01", false, 5, 3, true, "", 0, "NO", true));
  columns.push_back(columndefinition("DECIMAL", "DECIMAL", sql::DataType::DECIMAL, "-1.01", true, 10, 0, true, "", 0, "NO", true));
  columns.push_back(columndefinition("DECIMAL", "DECIMAL NOT NULL", sql::DataType::DECIMAL, "-1.01", true, 10, 0, false, "", 0, "NO", false));
  columns.push_back(columndefinition("DECIMAL UNSIGNED", "DECIMAL UNSIGNED", sql::DataType::DECIMAL, "1.01", false, 10, 0, true, "", 0, "NO", true));
  columns.push_back(columndefinition("DECIMAL UNSIGNED ZEROFILL", "DECIMAL(5,3) UNSIGNED ZEROFILL", sql::DataType::DECIMAL, "1.01", false, 5, 3, true, "", 0, "NO", false));
  columns.push_back(columndefinition("DECIMAL UNSIGNED ZEROFILL", "DECIMAL(6,3) UNSIGNED ZEROFILL DEFAULT 34.56", sql::DataType::DECIMAL, "1.01", false, 6, 3, true, "034.560", 0, "NO", false));
  columns.push_back(columndefinition("DATE", "DATE", sql::DataType::DATE, "2009-02-09", true, 10, 0, true, "", 0, "NO", false, "2009-02-09"));
  columns.push_back(columndefinition("DATE", "DATE NOT NULL", sql::DataType::DATE, "2009-02-12", true, 10, 0, false, "", 0, "NO", false));
  columns.push_back(columndefinition("DATE", "DATE NOT NULL DEFAULT '2009-02-16'", sql::DataType::DATE, "2009-02-12", true, 10, 0, false, "2009-02-16", 0, "NO", false));
  columns.push_back(columndefinition("DATETIME", "DATETIME", sql::DataType::TIMESTAMP, "2009-02-09 20:05:43", true, 19, 0, true, "", 0, "NO", false, "2009-02-09 20:05:43"));
  columns.push_back(columndefinition("DATETIME", "DATETIME NOT NULL", sql::DataType::TIMESTAMP, "2009-02-12 17:49:21", true, 19, 0, false, "", 0, "NO", false));
  columns.push_back(columndefinition("DATETIME", "DATETIME NOT NULL DEFAULT '2009-02-12 21:36:54'", sql::DataType::TIMESTAMP, "2009-02-12 17:49:21", true, 19, 0, false, "2009-02-12 21:36:54", 0, "NO", false));
  // TODO this might be server dependent!

  columns.push_back(columndefinition("TIMESTAMP", "TIMESTAMP", sql::DataType::TIMESTAMP, "2038-01-09 03:14:07", false, 19, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("TIME", "TIME", sql::DataType::TIME, "-838:59:59", true, 8, 0, true, "", 0, "NO", true));
  columns.push_back(columndefinition("TIME", "TIME NOT NULL", sql::DataType::TIME, "838:59:59", true, 8, 0, false, "", 0, "NO", false));
  columns.push_back(columndefinition("TIME", "TIME DEFAULT '12:39:41'", sql::DataType::TIME, "-838:59:59", true, 8, 0, true, "12:39:41", 0, "NO", true));
  columns.push_back(columndefinition("YEAR", "YEAR", sql::DataType::YEAR, "1901", false, 0, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("YEAR", "YEAR NOT NULL", sql::DataType::YEAR, "1902", false, 0, 0, false, "", 0, "NO", false));
  columns.push_back(columndefinition("YEAR", "YEAR(4)", sql::DataType::YEAR, "2009", false, 0, 0, true, "", 0, "NO", false, "2009"));

  columns.push_back(columndefinition("YEAR", "YEAR(2)", sql::DataType::YEAR, "1", false, 0, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("YEAR", "YEAR(3) DEFAULT '2009'", sql::DataType::YEAR, "1", false, 0, 0, true, "2009", 0, "NO", false));

  columns.push_back(columndefinition("CHAR", "CHAR", sql::DataType::CHAR, "a", true, 1, 0, true, "", 0, "NO", false, "a"));
  columns.push_back(columndefinition("CHAR", "CHAR NOT NULL", sql::DataType::CHAR, "a", true, 1, 0, false, "", 0, "NO", false));
  columns.push_back(columndefinition("CHAR", "CHAR(255)", sql::DataType::CHAR, "abc", true, 255, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("BINARY", "CHAR(255) CHARACTER SET binary", sql::DataType::BINARY, "abc", true, 255, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("CHAR", "CHAR(254) NOT NULL", sql::DataType::CHAR, "abc", true, 254, 0, false, "", 0, "NO", false));
  columns.push_back(columndefinition("CHAR", "CHAR(254) NOT NULL DEFAULT 'abc'", sql::DataType::CHAR, "abc", true, 254, 0, false, "abc", 0, "NO", false));
  // 3byte

  columns.push_back(columndefinition("CHAR", "NATIONAL CHAR(255)", sql::DataType::CHAR, "abc", true, 255, 0, true, "", 765, "NO", false));
  columns.push_back(columndefinition("CHAR", "NATIONAL CHAR(215) NOT NULL", sql::DataType::CHAR, "abc", true, 215, 0, false, "", 645, "NO", false));
  columns.push_back(columndefinition("CHAR", "NATIONAL CHAR(215) NOT NULL DEFAULT 'Ulf'", sql::DataType::CHAR, "abc", true, 215, 0, false, "Ulf", 645, "NO", false));
  columns.push_back(columndefinition("CHAR", "CHAR(255) CHARACTER SET 'utf8'", sql::DataType::CHAR, "abc", true, 255, 0, true, "", 765, "NO", false));
  columns.push_back(columndefinition("CHAR", "CHAR(55) CHARACTER SET 'utf8' NOT NULL ", sql::DataType::CHAR, "abc", true, 55, 0, false, "", 165, "NO", false));
  // TODO this might be server dependent!
  columns.push_back(columndefinition("CHAR", "CHAR(250) CHARACTER SET 'utf8' COLLATE 'utf8_bin'", sql::DataType::CHAR, "abc", true, 250, 0, true, "", 750, "NO", false));
  columns.push_back(columndefinition("CHAR", "CHAR(250) CHARACTER SET 'utf8' COLLATE 'utf8_bin' DEFAULT 'Wendel'", sql::DataType::CHAR, "abc", true, 250, 0, true, "Wendel", 750, "NO", false));
  columns.push_back(columndefinition("CHAR", "CHAR(43) CHARACTER SET 'utf8' COLLATE 'utf8_bin' NOT NULL", sql::DataType::CHAR, "abc", true, 43, 0, false, "", 129, "NO", false));
  columns.push_back(columndefinition("CHAR", "CHAR(123) CHARACTER SET 'ucs2'", sql::DataType::CHAR, "abc", true, 123, 0, true, "", 246, "NO", false));
  // The CHAR BYTE data type is an alias for the BINARY data type. This is a compatibility feature.
  columns.push_back(columndefinition("BINARY", "CHAR(255) BYTE", sql::DataType::BINARY, "abc", true, 255, 0, true, "", 255, "NO", false));
  columns.push_back(columndefinition("BINARY", "CHAR(12) BYTE NOT NULL", sql::DataType::BINARY, "abc", true, 12, 0, false, "", 12, "NO", false));
  columns.push_back(columndefinition("CHAR", "CHAR(14) DEFAULT 'Andrey'", sql::DataType::CHAR, "abc", true, 14, 0, true, "Andrey", 0, "NO", false));
  columns.push_back(columndefinition("BINARY", "CHAR(25) CHARACTER SET 'binary'", sql::DataType::BINARY, "abc", true, 25, 0, true, "", 25, "NO", false));
  columns.push_back(columndefinition("VARCHAR", "VARCHAR(10)", sql::DataType::VARCHAR, "a", true, 10, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("VARBINARY", "VARCHAR(10) CHARACTER SET binary", sql::DataType::VARBINARY, "a", true, 10, 0, true, "", 0, "NO", false));
  columns.push_back(columndefinition("VARCHAR", "VARCHAR(7) NOT NULL", sql::DataType::VARCHAR, "a", true, 7, 0, false, "", 0, "NO", false));
  columns.push_back(columndefinition("VARCHAR", "VARCHAR(255) DEFAULT 'Good night twitter. BTW, go MySQL!'", sql::DataType::VARCHAR, "a", true, 255, 0, true, "Good night twitter. BTW, go MySQL!", 0, "NO", false));
  columns.push_back(columndefinition("VARCHAR", "VARCHAR(11) CHARACTER SET 'utf8'", sql::DataType::VARCHAR, "a", true, 11, 0, true, "", 33, "NO", false));
  columns.push_back(columndefinition("VARCHAR", "VARCHAR(11) CHARACTER SET 'ascii' DEFAULT 'Hristov'", sql::DataType::VARCHAR, "a", true, 11, 0, true, "Hristov", 11, "NO", false));
  columns.push_back(columndefinition("VARCHAR", "VARCHAR(12) CHARACTER SET 'utf8' COLLATE 'utf8_bin'", sql::DataType::VARCHAR, "a", true, 12, 0, true, "", 36, "NO", false));
  columns.push_back(columndefinition("VARBINARY", "VARCHAR(13) BYTE", sql::DataType::VARBINARY, "a", true, 13, 0, true, "", 13, "NO", false));
  columns.push_back(columndefinition("VARBINARY", "VARCHAR(14) BYTE NOT NULL", sql::DataType::VARBINARY, "a", true, 14, 0, false, "", 14, "NO", false));
  columns.push_back(columndefinition("BINARY", "BINARY(1)", sql::DataType::BINARY, "a", true, 1, 0, true, "", 1, "NO", false));
  columns.push_back(columndefinition("VARBINARY", "VARBINARY(1)", sql::DataType::VARBINARY, "a", true, 1, 0, true, "", 1, "NO", false));
  columns.push_back(columndefinition("VARBINARY", "VARBINARY(2) NOT NULL", sql::DataType::VARBINARY, "a", true, 2, 0, false, "", 2, "NO", false));
  columns.push_back(columndefinition("VARBINARY", "VARBINARY(20) NOT NULL DEFAULT 0x4C617772696E", sql::DataType::VARBINARY, "a", true, 20, 0, false, "0x4C617772696E", 20, "NO", false));
  columns.push_back(columndefinition("TINYBLOB", "TINYBLOB", sql::DataType::VARBINARY, "a", true, 255, 0, true, "", 255, "NO", false));
  columns.push_back(columndefinition("TINYTEXT", "TINYTEXT", sql::DataType::VARCHAR, "a", true, 255, 0, true, "", 255, "NO", false, "a"));
  columns.push_back(columndefinition("TINYTEXT", "TINYTEXT NOT NULL", sql::DataType::VARCHAR, "a", true, 255, 0, false, "", 255, "NO", false));
  columns.push_back(columndefinition("TINYTEXT", "TINYTEXT", sql::DataType::VARCHAR, "a", true, 255, 0, true, "", 255, "NO", false));
  columns.push_back(columndefinition("TINYBLOB", "TINYTEXT CHARACTER SET binary", sql::DataType::VARBINARY, "a", true, 255, 0, true, "", 255, "NO", false));
  columns.push_back(columndefinition("TINYTEXT", "TINYTEXT CHARACTER SET 'utf8'", sql::DataType::VARCHAR, "a", true, 255, 0, true, "", 255, "NO", false));
  columns.push_back(columndefinition("TINYTEXT", "TINYTEXT CHARACTER SET 'utf8' COLLATE 'utf8_bin'", sql::DataType::VARCHAR, "a", true, 255, 0, true, "", 255, "NO", false));
  columns.push_back(columndefinition("BLOB", "BLOB", sql::DataType::LONGVARBINARY, "a", true, 65535, 0, true, "", 65536, "NO", false));
  columns.push_back(columndefinition("BLOB", "BLOB NOT NULL", sql::DataType::LONGVARBINARY, "a", true, 65535, 0, false, "", 65535, "NO", false));
  columns.push_back(columndefinition("TEXT", "TEXT", sql::DataType::LONGVARCHAR, "a", true, 65535, 0, true, "", 65536, "NO", false));
  columns.push_back(columndefinition("TEXT", "TEXT NOT NULL", sql::DataType::LONGVARCHAR, "a", true, 65535, 0, false, "", 65535, "NO", false));
  columns.push_back(columndefinition("MEDIUMBLOB", "MEDIUMBLOB", sql::DataType::LONGVARBINARY, "a", true, 16777215, 0, true, "", 16777215, "NO", false));
  columns.push_back(columndefinition("MEDIUMBLOB", "MEDIUMBLOB NOT NULL", sql::DataType::LONGVARBINARY, "a", true, 16777215, 0, false, "", 16777215, "NO", false));
  columns.push_back(columndefinition("MEDIUMTEXT", "MEDIUMTEXT", sql::DataType::LONGVARCHAR, "a", true, 16777215, 0, true, "", 16777215, "NO", false));
  columns.push_back(columndefinition("MEDIUMBLOB", "MEDIUMTEXT CHARACTER SET binary", sql::DataType::LONGVARBINARY, "a", true, 16777215, 0, true, "", 16777215, "NO", false));
  columns.push_back(columndefinition("MEDIUMTEXT", "MEDIUMTEXT NOT NULL", sql::DataType::LONGVARCHAR, "a", true, 16777215, 0, false, "", 16777215, "NO", false));
  columns.push_back(columndefinition("MEDIUMTEXT", "MEDIUMTEXT CHARSET 'utf8'", sql::DataType::LONGVARCHAR, "a", true, 16777215, 0, true, "", 16777215, "NO", false));
  columns.push_back(columndefinition("MEDIUMTEXT", "MEDIUMTEXT CHARSET 'utf8' COLLATE 'utf8_bin'", sql::DataType::LONGVARCHAR, "a", true, 16777215, 0, true, "", 16777215, "NO", false));
  columns.push_back(columndefinition("LONGBLOB", "LONGBLOB", sql::DataType::LONGVARBINARY, "a", true, L64(4294967295), 0, true, "", L64(4294967295), "NO", false));
  columns.push_back(columndefinition("LONGBLOB", "LONGBLOB NOT NULL", sql::DataType::LONGVARBINARY, "a", true, L64(4294967295), 0, false, "", L64(4294967295), "NO", false));
  columns.push_back(columndefinition("LONGTEXT", "LONGTEXT", sql::DataType::LONGVARCHAR, "a", true, L64(4294967295), 0, true, "", L64(4294967295), "NO", false));
  columns.push_back(columndefinition("LONGBLOB", "LONGTEXT CHARACTER SET binary", sql::DataType::LONGVARBINARY, "a", true, L64(4294967295), 0, true, "", L64(4294967295), "NO", false));
  columns.push_back(columndefinition("LONGTEXT", "LONGTEXT NOT NULL", sql::DataType::LONGVARCHAR, "a", true, L64(4294967295), 0, false, "", L64(4294967295), "NO", false));
  columns.push_back(columndefinition("LONGTEXT", "LONGTEXT CHARSET 'utf8'", sql::DataType::LONGVARCHAR, "a", true, L64(4294967295), 0, true, "", L64(4294967295), "NO", false));
  columns.push_back(columndefinition("LONGTEXT", "LONGTEXT CHARSET 'utf8' COLLATE 'utf8_bin'", sql::DataType::LONGVARCHAR, "a", true, L64(4294967295), 0, true, "", L64(4294967295), "NO", false));
  columns.push_back(columndefinition("ENUM", "ENUM('yes', 'no')", sql::DataType::ENUM, "yes", true, 3, 0, true, "", 3, "NO", false));
  columns.push_back(columndefinition("ENUM", "ENUM('yes', 'no') CHARACTER SET 'binary'", sql::DataType::ENUM, "yes", true, 3, 0, true, "", 3, "NO", false));
  columns.push_back(columndefinition("ENUM", "ENUM('yes', 'no') NOT NULL", sql::DataType::ENUM, "yes", true, 3, 0, false, "", 3, "NO", false, "yes"));
  columns.push_back(columndefinition("ENUM", "ENUM('yes', 'no', 'not sure') NOT NULL", sql::DataType::ENUM, "yes", true, 8, 0, false, "", 8, "NO", false));
  columns.push_back(columndefinition("ENUM", "ENUM('yes', 'no', 'buy') NOT NULL DEFAULT 'buy'", sql::DataType::ENUM, "yes", true, 3, 0, false, "buy", 3, "NO", false));
  columns.push_back(columndefinition("SET", "SET('yes', 'no')", sql::DataType::SET, "yes", true, 6, 0, true, "", 6, "NO", false, "yes"));
  columns.push_back(columndefinition("SET", "SET('yes', 'no') CHARACTER SET 'binary'", sql::DataType::SET, "yes", true, 6, 0, true, "", 6, "NO", false, "yes"));
  columns.push_back(columndefinition("SET", "SET('yes', 'no') CHARSET 'ascii'", sql::DataType::SET, "yes", true, 6, 0, true, "", 6, "NO", false));
  columns.push_back(columndefinition("SET", "SET('yes', 'no') CHARSET 'ascii' DEFAULT 'yes'", sql::DataType::SET, "yes", true, 6, 0, true, "yes", 6, "NO", false));
  columns.push_back(columndefinition("SET", "SET('yes', 'no', 'ascii') CHARSET 'ascii' NOT NULL", sql::DataType::SET, "yes", true, 12, 0, false, "", 12, "NO", false));

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

  stmt.reset(con->createStatement());
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


sql::Connection *
unit_fixture::getConnection(sql::ConnectOptionsMap *additional_options)
{
  if (driver == NULL)
  {
    driver=sql::mysql::get_driver_instance();
  }

  sql::ConnectOptionsMap connection_properties;
  connection_properties["hostName"]=url;
  connection_properties["userName"]=user;
  connection_properties["password"]=passwd;

  bool bval= !TestsRunner::getStartOptions()->getBool("dont-use-is");
  connection_properties["metadataUseInfoSchema"]=bval;

  bval=TestsRunner::getStartOptions()->getBool("use-dynamic-load");
  if (bval)
  {
    sql::SQLString clientlib(DYNLOAD_MYSQL_LIB);
    connection_properties["clientlib"]=clientlib;
    logMsg("Connection using dynamic load of clientlib " DYNLOAD_MYSQL_LIB);
  }

  if (additional_options != NULL)
  {
    for (sql::ConnectOptionsMap::const_iterator cit= additional_options->begin();
         cit != additional_options->end(); ++cit)
    {
      connection_properties[cit->first]= cit->second;
    }
  }

  return driver->connect(connection_properties);
}

void unit_fixture::logMsg(const String & message)
{
  TestsListener::messagesLog(message + "\n");
}

void unit_fixture::logErr(const String & message)
{
  TestsListener::errorsLog(message + "\n");
}


void unit_fixture::logDebug(const String & message)
{
  logMsg(message);
}


/* There is not really need to have it as a class method */
int unit_fixture::getMySQLVersion(Connection & con)
{
  DatabaseMetaData * dbmeta=con->getMetaData();
  return dbmeta->getDatabaseMajorVersion() * 10000 + dbmeta->getDatabaseMinorVersion() * 1000 + dbmeta->getDatabasePatchVersion();
}

std::string unit_fixture::exceptionIsOK(sql::SQLException & e)
{
  return exceptionIsOK(e, "HY000", 0);
}

std::string unit_fixture::exceptionIsOK(sql::SQLException &e, const std::string& sql_state, int errNo)
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

  if (e.getErrorCode() != errNo)
  {
    reason << "Expecting error code '" << errNo << "' got '" << e.getErrorCode() << "'";
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

void unit_fixture::checkResultSetScrolling(ResultSet &res_ref)
{
  /*
    if (res_ref->getType() == sql::ResultSet::TYPE_FORWARD_ONLY)
      return;
   */
  int before;

  before=static_cast<int> (res_ref->getRow());
  if (!res_ref->last())
  {
    res_ref->absolute(before);
    return;
  }

  int num_rows;
  int i;

  num_rows=(int) res_ref->getRow();

  res_ref->beforeFirst();

  ASSERT(!res_ref->previous());
  ASSERT(res_ref->next());
  ASSERT_EQUALS(1, (int) res_ref->getRow());
  ASSERT(!res_ref->isBeforeFirst());
  ASSERT(!res_ref->isAfterLast());
  if (num_rows > 1)
  {
    ASSERT(res_ref->next());
    ASSERT(res_ref->previous());
    ASSERT_EQUALS(1, (int) res_ref->getRow());
  }

  ASSERT(res_ref->first());
  ASSERT_EQUALS(1, (int) res_ref->getRow());
  ASSERT(res_ref->isFirst());
  ASSERT(!res_ref->isBeforeFirst());
  ASSERT(!res_ref->isAfterLast());
  if (num_rows == 1)
    ASSERT(res_ref->isLast());
  else
    ASSERT(!res_ref->isLast());

  if (num_rows > 1)
  {
    ASSERT(res_ref->next());
    ASSERT(res_ref->previous());
    ASSERT_EQUALS(1, (int) res_ref->getRow());
  }
  ASSERT(!res_ref->previous());

  ASSERT(res_ref->last());
  ASSERT_EQUALS(num_rows, (int) res_ref->getRow());
  ASSERT(res_ref->isLast());
  ASSERT(!res_ref->isBeforeFirst());
  ASSERT(!res_ref->isAfterLast());
  if (num_rows == 1)
    ASSERT(res_ref->isFirst());
  else
    ASSERT(!res_ref->isFirst());

  if (num_rows > 1)
  {
    ASSERT(res_ref->previous());
    ASSERT_EQUALS(num_rows - 1, (int) res_ref->getRow());
    ASSERT(res_ref->next());
    ASSERT_EQUALS(num_rows, (int) res_ref->getRow());
  }
  ASSERT(!res_ref->next());

  res_ref->beforeFirst();
  ASSERT_EQUALS(0, (int) res_ref->getRow());
  ASSERT(res_ref->isBeforeFirst());
  ASSERT(!res_ref->isAfterLast());
  ASSERT(!res_ref->isFirst());
  ASSERT(!res_ref->previous());
  ASSERT(res_ref->next());
  ASSERT_EQUALS(1, (int) res_ref->getRow());
  res_ref->absolute(1);
  ASSERT_EQUALS(1, (int) res_ref->getRow());
  ASSERT(!res_ref->previous());

  res_ref->afterLast();
  ASSERT_EQUALS(num_rows + 1, (int) res_ref->getRow());
  ASSERT(res_ref->isAfterLast());
  ASSERT(!res_ref->isBeforeFirst());
  ASSERT(!res_ref->isFirst());
  ASSERT(res_ref->previous());
  ASSERT_EQUALS(num_rows, (int) res_ref->getRow());
  ASSERT(!res_ref->next());

  i=0;
  res_ref->beforeFirst();
  while (res_ref->next())
  {
    ASSERT(!res_ref->isAfterLast());
    i++;
    ASSERT_EQUALS(i, (int) res_ref->getRow());
  }
  ASSERT_EQUALS(num_rows, i);

  // relative(1) is equivalent to next()
  i=0;
  res_ref->beforeFirst();
  while (res_ref->relative(1))
  {
    ASSERT(!res_ref->isAfterLast());
    i++;
    ASSERT_EQUALS(i, (int) res_ref->getRow());
  }
  ASSERT_EQUALS(num_rows, i);

  i=0;
  res_ref->first();
  do
  {
    ASSERT(!res_ref->isAfterLast());
    i++;
    ASSERT_EQUALS(i, (int) res_ref->getRow());
  }
  while (res_ref->next());
  ASSERT_EQUALS(num_rows, i);

  // relative(1) is equivalent to next()
  i=0;
  res_ref->first();
  do
  {
    ASSERT(!res_ref->isAfterLast());
    i++;
    ASSERT_EQUALS(i, (int) res_ref->getRow());
  }
  while (res_ref->relative(1));
  ASSERT_EQUALS(num_rows, i);

  i=num_rows;
  res_ref->last();
  do
  {
    ASSERT(!res_ref->isBeforeFirst());
    ASSERT_EQUALS(i, (int) res_ref->getRow());
    i--;
  }
  while (res_ref->previous());
  ASSERT_EQUALS(0, i);

  // relative(-1) is equivalent to previous()
  i=num_rows;
  res_ref->last();
  do
  {
    ASSERT(!res_ref->isBeforeFirst());
    ASSERT_EQUALS(i, (int) res_ref->getRow());
    i--;
  }
  while (res_ref->relative(-1));
  ASSERT_EQUALS(0, i);

  i=num_rows;
  res_ref->afterLast();
  while (res_ref->previous())
  {
    ASSERT(!res_ref->isBeforeFirst());
    ASSERT_EQUALS(i, (int) res_ref->getRow());
    i--;
  }
  ASSERT_EQUALS(0, i);

  // relative(-1) is equivalent to previous()

  i=num_rows;
  res_ref->afterLast();
  while (res_ref->relative(-1))
  {
    ASSERT(!res_ref->isBeforeFirst());
    ASSERT_EQUALS(i, (int) res_ref->getRow());
    i--;
  }
  ASSERT_EQUALS(0, i);

  res_ref->last();
  res_ref->relative(0);
  ASSERT(res_ref->isLast());

  res_ref->absolute(before);
  ASSERT_EQUALS(before, (int) res_ref->getRow());
}

} /* namespace testsuite */
