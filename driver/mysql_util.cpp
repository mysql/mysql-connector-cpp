
/* Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

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


#include <cppconn/datatype.h>
#include <string>
#include <memory>
#include "mysql_util.h"

namespace sql {
namespace mysql {
namespace util {


#define cppconn_mbcharlen_big5		NULL
#define check_mb_big5				NULL
#define cppconn_mbcharlen_ujis		NULL
#define check_mb_ujis				NULL
#define cppconn_mbcharlen_sjis		NULL
#define check_mb_sjis				NULL
#define cppconn_mbcharlen_euckr		NULL
#define check_mb_euckr				NULL
#define cppconn_mbcharlen_gb2312	NULL
#define check_mb_gb2312				NULL
#define cppconn_mbcharlen_gbk		NULL
#define check_mb_gbk				NULL
#define cppconn_mbcharlen_utf8		NULL
#define check_mb_utf8_valid			NULL
#define cppconn_mbcharlen_ucs2		NULL
#define check_mb_ucs2				NULL
#define cppconn_mbcharlen_cp932		NULL
#define check_mb_cp932				NULL
#define cppconn_mbcharlen_eucjpms	NULL
#define check_mb_eucjpms			NULL
#define cppconn_mbcharlen_utf8		NULL
#define check_mb_utf8_valid			NULL

/* {{{ our_charsets60 */
const OUR_CHARSET our_charsets60[] =
{
	{   1, "big5","big5_chinese_ci", 1, 2, "", cppconn_mbcharlen_big5, check_mb_big5},
	{   3, "dec8", "dec8_swedisch_ci", 1, 1, "", NULL, NULL},
	{   4, "cp850", "cp850_general_ci", 1, 1, "", NULL, NULL},
	{   6, "hp8", "hp8_english_ci", 1, 1, "", NULL, NULL},
	{   7, "koi8r", "koi8r_general_ci", 1, 1, "", NULL, NULL},
	{   8, "latin1", "latin1_swedish_ci", 1, 1, "", NULL, NULL},
	{   9, "latin2", "latin2_general_ci", 1, 1, "", NULL, NULL},
	{  10, "swe7", "swe7_swedish_ci", 1, 1, "", NULL, NULL},
	{  11, "ascii", "ascii_general_ci", 1, 1, "", NULL, NULL},
	{  12, "ujis", "ujis_japanese_ci", 1, 3, "", cppconn_mbcharlen_ujis, check_mb_ujis},
	{  13, "sjis", "sjis_japanese_ci", 1, 2, "", cppconn_mbcharlen_sjis, check_mb_sjis},
	{  16, "hebrew", "hebrew_general_ci", 1, 1, "", NULL, NULL},
	{  18, "tis620", "tis620_thai_ci", 1, 1, "", NULL, NULL},
	{  19, "euckr", "euckr_korean_ci", 1, 2, "", cppconn_mbcharlen_euckr, check_mb_euckr},
	{  22, "koi8u", "koi8u_general_ci", 1, 1, "", NULL, NULL},
	{  24, "gb2312", "gb2312_chinese_ci", 1, 2, "", cppconn_mbcharlen_gb2312, check_mb_gb2312},
	{  25, "greek", "greek_general_ci", 1, 1, "", NULL, NULL},	
	{  26, "cp1250", "cp1250_general_ci", 1, 1, "", NULL, NULL},
	{  28, "gbk", "gbk_chinese_ci", 1, 2, "", cppconn_mbcharlen_gbk, check_mb_gbk},
	{  30, "latin5", "latin5_turkish_ci", 1, 1, "", NULL, NULL},
	{  32, "armscii8", "armscii8_general_ci", 1, 1, "", NULL, NULL},
	{  33, "utf8", "utf8_general_ci", 1, 2, "UTF-8 Unicode", cppconn_mbcharlen_utf8,  check_mb_utf8_valid},
	{  35, "ucs2", "ucs2_general_ci", 2, 2, "UCS-2 Unicode", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{  36, "cp866", "cp866_general_ci", 1, 1, "", NULL, NULL},
	{  37, "keybcs2", "keybcs2_general_ci", 1, 1, "", NULL, NULL},
	{  38, "macce", "macce_general_ci", 1, 1, "", NULL, NULL},
	{  39, "macroman", "macroman_general_ci", 1, 1, "", NULL, NULL},
	{  40, "cp852", "cp852_general_ci", 1, 1, "", NULL, NULL},
	{  41, "latin7", "latin7_general_ci", 1, 1, "", NULL, NULL},
	{  51, "cp1251", "cp1251_general_ci", 1, 1, "", NULL, NULL},
	{  57, "cp1256", "cp1256_general_ci", 1, 1, "", NULL, NULL},
	{  59, "cp1257", "cp1257_general_ci", 1, 1, "", NULL, NULL},
	{  63, "binary", "binary", 1, 1, "", NULL, NULL},
	{  92, "geostd8", "geostd8_general_ci", 1, 1, "", NULL, NULL},
	{  95, "cp932", "cp932_japanese_ci", 1, 2, "", cppconn_mbcharlen_cp932, check_mb_cp932},
	{  97, "eucjpms", "eucjpms_japanese_ci", 1, 3, "", cppconn_mbcharlen_eucjpms, check_mb_eucjpms},
	{   2, "latin2", "latin2_czech_cs", 1, 1, "", NULL, NULL},
	{   5, "latin1", "latin1_german_ci", 1, 1, "", NULL, NULL},
	{  14, "cp1251", "cp1251_bulgarian_ci", 1, 1, "", NULL, NULL},
	{  15, "latin1", "latin1_danish_ci", 1, 1, "", NULL, NULL},
	{  17, "filename", "filename", 1, 5, "", NULL, NULL},
	{  20, "latin7", "latin7_estonian_cs", 1, 1, "", NULL, NULL},
	{  21, "latin2", "latin2_hungarian_ci", 1, 1, "", NULL, NULL},
	{  23, "cp1251", "cp1251_ukrainian_ci", 1, 1, "", NULL, NULL},
	{  27, "latin2", "latin2_croatian_ci", 1, 1, "", NULL, NULL},
	{  29, "cp1257", "cp1257_lithunian_ci", 1, 1, "", NULL, NULL},
	{  31, "latin1", "latin1_german2_ci", 1, 1, "", NULL, NULL},
	{  34, "cp1250", "cp1250_czech_cs", 1, 1, "", NULL, NULL},
	{  42, "latin7", "latin7_general_cs", 1, 1, "", NULL, NULL},
	{  43, "macce", "macce_bin", 1, 1, "", NULL, NULL},
	{  44, "cp1250", "cp1250_croatian_ci", 1, 1, "", NULL, NULL},
	{  47, "latin1", "latin1_bin", 1, 1, "", NULL, NULL},
	{  48, "latin1", "latin1_general_ci", 1, 1, "", NULL, NULL},
	{  49, "latin1", "latin1_general_cs", 1, 1, "", NULL, NULL},
	{  50, "cp1251", "cp1251_bin", 1, 1, "", NULL, NULL},
	{  52, "cp1251", "cp1251_general_cs", 1, 1, "", NULL, NULL},
	{  53, "macroman", "macroman_bin", 1, 1, "", NULL, NULL},
	{  58, "cp1257", "cp1257_bin", 1, 1, "", NULL, NULL},
	{  60, "armascii8", "armascii8_bin", 1, 1, "", NULL, NULL},
	{  65, "ascii", "ascii_bin", 1, 1, "", NULL, NULL},
	{  66, "cp1250", "cp1250_bin", 1, 1, "", NULL, NULL},
	{  67, "cp1256", "cp1256_bin", 1, 1, "", NULL, NULL},
	{  68, "cp866", "cp866_bin", 1, 1, "", NULL, NULL},
	{  69, "dec8", "dec8_bin", 1, 1, "", NULL, NULL},
	{  70, "greek", "greek_bin", 1, 1, "", NULL, NULL},
	{  71, "hebew", "hebrew_bin", 1, 1, "", NULL, NULL},
	{  72, "hp8", "hp8_bin", 1, 1, "", NULL, NULL},
	{  73, "keybcs2", "keybcs2_bin", 1, 1, "", NULL, NULL},
	{  74, "koi8r", "koi8r_bin", 1, 1, "", NULL, NULL},
	{  75, "koi8u", "koi8u_bin", 1, 1, "", NULL, NULL},
	{  77, "latin2", "latin2_bin", 1, 1, "", NULL, NULL},
	{  78, "latin5", "latin5_bin", 1, 1, "", NULL, NULL},
	{  79, "latin7", "latin7_bin", 1, 1, "", NULL, NULL},
	{  80, "cp850", "cp850_bin", 1, 1, "", NULL, NULL},
	{  81, "cp852", "cp852_bin", 1, 1, "", NULL, NULL},
	{  82, "swe7", "swe7_bin", 1, 1, "", NULL, NULL},
	{  93, "geostd8", "geostd8_bin", 1, 1, "", NULL, NULL},
	{  83, "utf8", "utf8_bin", 1, 2, "UTF-8 Unicode", cppconn_mbcharlen_utf8,  check_mb_utf8_valid},
	{  84, "big5", "big5_bin", 1, 2, "", cppconn_mbcharlen_big5, check_mb_big5},
	{  85, "euckr", "euckr_bin", 1, 2, "", cppconn_mbcharlen_euckr, check_mb_euckr},
	{  86, "gb2312", "gb2312_bin", 1, 2, "", cppconn_mbcharlen_gb2312, check_mb_gb2312},
	{  87, "gbk", "gbk_bin", 1, 2, "", cppconn_mbcharlen_gbk, check_mb_gbk},
	{  88, "sjis", "sjis_bin", 1, 2, "", cppconn_mbcharlen_sjis, check_mb_sjis},
	{  89, "tis620", "tis620_bin", 1, 1, "", NULL, NULL},
	{  90, "ucs2", "ucs2_bin", 2, 2, "UCS-2 Unicode", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{  91, "ujis", "ujis_bin", 1, 3, "", cppconn_mbcharlen_ujis, check_mb_ujis},
	{  94, "latin1", "latin1_spanish_ci", 1, 1, "", NULL, NULL},
	{  96, "cp932", "cp932_bin", 1, 2, "", cppconn_mbcharlen_cp932, check_mb_cp932},
	{  99, "cp1250", "cp1250_polish_ci", 1, 1, "", NULL, NULL},
	{  98, "eucjpms", "eucjpms_bin", 1, 3, "", cppconn_mbcharlen_eucjpms, check_mb_eucjpms},
	{ 128, "ucs2", "ucs2_unicode_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 129, "ucs2", "ucs2_icelandic_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 130, "ucs2", "ucs2_latvian_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 131, "ucs2", "ucs2_romanian_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 132, "ucs2", "ucs2_slovenian_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 133, "ucs2", "ucs2_polish_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 134, "ucs2", "ucs2_estonian_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 135, "ucs2", "ucs2_spanish_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 136, "ucs2", "ucs2_swedish_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 137, "ucs2", "ucs2_turkish_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 138, "ucs2", "ucs2_czech_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 139, "ucs2", "ucs2_danish_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 140, "ucs2", "ucs2_lithunian_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 141, "ucs2", "ucs2_slovak_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 142, "ucs2", "ucs2_spanish2_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 143, "ucs2", "ucs2_roman_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 144, "ucs2", "ucs2_persian_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 145, "ucs2", "ucs2_esperanto_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 146, "ucs2", "ucs2_hungarian_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 147, "ucs2", "ucs2_sinhala_ci", 2, 2, "", cppconn_mbcharlen_ucs2, check_mb_ucs2},
	{ 192, "utf8mb3", "utf8mb3_general_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 193, "utf8mb3", "utf8mb3_icelandic_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 194, "utf8mb3", "utf8mb3_latvian_ci", 1, 3, "", cppconn_mbcharlen_utf8,  check_mb_utf8_valid},
	{ 195, "utf8mb3", "utf8mb3_romanian_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 196, "utf8mb3", "utf8mb3_slovenian_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 197, "utf8mb3", "utf8mb3_polish_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 198, "utf8mb3", "utf8mb3_estonian_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 119, "utf8mb3", "utf8mb3_spanish_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 200, "utf8mb3", "utf8mb3_swedish_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 201, "utf8mb3", "utf8mb3_turkish_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 202, "utf8mb3", "utf8mb3_czech_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 203, "utf8mb3", "utf8mb3_danish_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid },
	{ 204, "utf8mb3", "utf8mb3_lithunian_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid },
	{ 205, "utf8mb3", "utf8mb3_slovak_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 206, "utf8mb3", "utf8mb3_spanish2_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 207, "utf8mb3", "utf8mb3_roman_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 208, "utf8mb3", "utf8mb3_persian_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 209, "utf8mb3", "utf8mb3_esperanto_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 210, "utf8mb3", "utf8mb3_hungarian_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 211, "utf8mb3", "utf8mb3_sinhala_ci", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 224, "utf8", "utf8_unicode_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 225, "utf8", "utf8_icelandic_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 226, "utf8", "utf8_latvian_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 227, "utf8", "utf8_romanian_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 228, "utf8", "utf8_slovenian_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 229, "utf8", "utf8_polish_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 230, "utf8", "utf8_estonian_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 231, "utf8", "utf8_spanish_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 232, "utf8", "utf8_swedish_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 233, "utf8", "utf8_turkish_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 234, "utf8", "utf8_czech_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 235, "utf8", "utf8_danish_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 236, "utf8", "utf8_lithuanian_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 237, "utf8", "utf8_slovak_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 238, "utf8", "utf8_spanish2_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 239, "utf8", "utf8_roman_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 240, "utf8", "utf8_persian_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 241, "utf8", "utf8_esperanto_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 242, "utf8", "utf8_hungarian_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 243, "utf8", "utf8_sinhala_ci", 1, 4, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{ 254, "utf8mb3", "utf8mb3_general_cs", 1, 3, "", cppconn_mbcharlen_utf8, check_mb_utf8_valid},
	{   0, NULL, NULL, 0, 0, NULL, NULL, NULL}
};
/* }}} */


/* {{{ find_charset */
const OUR_CHARSET * find_charset(unsigned int charsetnr)
{
	const OUR_CHARSET * c = our_charsets60;

	do {
		if (c->nr == charsetnr) {
			return c;
		}
		++c;
	} while (c[0].nr != 0);
	return NULL;
}
/* }}} */


#define MAGIC_BINARY_CHARSET_NR 63

/* {{{ mysql_to_datatype() -I- */
int
mysql_type_to_datatype(const MYSQL_FIELD * const field)
{
	switch (field->type) {
		case MYSQL_TYPE_BIT:
			return sql::DataType::BIT;
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
			return sql::DataType::DECIMAL;
		case MYSQL_TYPE_TINY:
			return sql::DataType::TINYINT;
		case MYSQL_TYPE_SHORT:
			return sql::DataType::SMALLINT;
		case MYSQL_TYPE_INT24:
			return sql::DataType::MEDIUMINT;
		case MYSQL_TYPE_LONG:
			return sql::DataType::INTEGER;
		case MYSQL_TYPE_LONGLONG:
			return sql::DataType::BIGINT;
		case MYSQL_TYPE_FLOAT:
			return sql::DataType::REAL;
		case MYSQL_TYPE_DOUBLE:
			return sql::DataType::DOUBLE;
		case MYSQL_TYPE_NULL:
			return sql::DataType::SQLNULL;
		case MYSQL_TYPE_TIMESTAMP:
			return sql::DataType::TIMESTAMP;
		case MYSQL_TYPE_DATE:
			return sql::DataType::DATE;
		case MYSQL_TYPE_TIME:
			return sql::DataType::TIME;
		case MYSQL_TYPE_YEAR:
			return sql::DataType::DATE;
		case MYSQL_TYPE_DATETIME:
			return sql::DataType::TIMESTAMP;
		case MYSQL_TYPE_TINY_BLOB:
			if ((field->flags & BINARY_FLAG) && field->charsetnr == MAGIC_BINARY_CHARSET_NR) {
				return sql::DataType::VARBINARY;
			}
			return sql::DataType::VARCHAR;
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
			if ((field->flags & BINARY_FLAG) && field->charsetnr == MAGIC_BINARY_CHARSET_NR) {
				return sql::DataType::LONGVARBINARY;
			}
			return sql::DataType::LONGVARCHAR;
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
			if ((field->flags & BINARY_FLAG) && field->charsetnr == MAGIC_BINARY_CHARSET_NR) {
				return sql::DataType::VARBINARY;
			}
			return sql::DataType::VARCHAR;
		case MYSQL_TYPE_STRING:
			if ((field->flags & BINARY_FLAG) && field->charsetnr == MAGIC_BINARY_CHARSET_NR) {
				return sql::DataType::BINARY;
			}
			return sql::DataType::CHAR;
		case MYSQL_TYPE_ENUM:
			/* This hould never happen - MYSQL_TYPE_ENUM is not sent over the wire, just used in the server */
			return sql::DataType::VARCHAR;
		case MYSQL_TYPE_SET:
			/* This hould never happen - MYSQL_TYPE_SET is not sent over the wire, just used in the server */
			return sql::DataType::VARCHAR;
		case MYSQL_TYPE_GEOMETRY:
			return sql::DataType::GEOMETRY;
		default:
			return sql::DataType::UNKNOWN;
	}
}
/* }}} */


/* {{{ mysql_to_datatype() -I- */
int
mysql_string_type_to_datatype(const std::string & name)
{
	/*
	  I_S.COLUMNS is buggy, because it deflivers (float|double) unsigned
	  but not (tinyint|smallint|mediumint|int|bigint) unsigned
	*/
	if (!name.compare("bit")) {
		return sql::DataType::BIT;
	} else if (!name.compare("decimal") || !name.compare("decimal unsigned")) {
		return sql::DataType::DECIMAL;
	} else if (!name.compare("tinyint") || !name.compare("tinyint unsigned")) {
		return sql::DataType::TINYINT;
	} else if (!name.compare("smallint") || !name.compare("smallint unsigned")) {
		return sql::DataType::SMALLINT;
	} else if (!name.compare("mediumint") || !name.compare("mediumint unsigned")) {
		return sql::DataType::MEDIUMINT;
	} else if (!name.compare("int") || !name.compare("int unsigned")) {
		return sql::DataType::INTEGER;
	} else if (!name.compare("bigint") || !name.compare("bigint unsigned")) {
		return sql::DataType::BIGINT;
	} else if (!name.compare("float") || !name.compare("float unsigned")) {
		return sql::DataType::REAL;
	} else if (!name.compare("double") || !name.compare("double unsigned")) {
		return sql::DataType::DOUBLE;
	} else if (!name.compare("timestamp")) {
		return sql::DataType::TIMESTAMP;
	} else if (!name.compare("date")) {
		return sql::DataType::DATE;
	} else if (!name.compare("time")) {
		return sql::DataType::TIME;
	} else if (!name.compare("year")) {
		return sql::DataType::DATE;
	} else if (!name.compare("datetime")) {
		return sql::DataType::TIMESTAMP;
	} else if (!name.compare("tinytext")) {
		return sql::DataType::VARCHAR;
	} else if (!name.compare("mediumtext") || !name.compare("text") || !name.compare("longtext")) {
		return sql::DataType::LONGVARCHAR;
	} else if (!name.compare("tinyblob")) {
		return sql::DataType::VARBINARY;
	} else if (!name.compare("mediumblob") || !name.compare("blob") || !name.compare("longblob")) {
		return sql::DataType::LONGVARBINARY;
	} else if (!name.compare("char")) {
		return sql::DataType::CHAR;
	} else if (!name.compare("binary")) {
		return sql::DataType::BINARY;
	} else if (!name.compare("varchar")) {
		return sql::DataType::VARCHAR;
	} else if (!name.compare("varbinary")) {
		return sql::DataType::VARBINARY;
	} else if (!name.compare("enum")) {
		return sql::DataType::VARCHAR;
	} else if (!name.compare("set")) {
		return sql::DataType::VARCHAR;
	} else if (!name.compare("geometry")) {
		return sql::DataType::GEOMETRY;
	} else {
		return sql::DataType::UNKNOWN;
	}
}
/* }}} */


/* {{{ mysql_to_datatype() -I- */
const char *
mysql_type_to_string(const MYSQL_FIELD * const field)
{
	bool isUnsigned = (field->flags & UNSIGNED_FLAG) != 0;
	bool isZerofill = (field->flags & ZEROFILL_FLAG) != 0;
	switch (field->type) {
		case MYSQL_TYPE_BIT:
			return "BIT";
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
			return isUnsigned ? (isZerofill? "DECIMAL UNSIGNED ZEROFILL" : "DECIMAL UNSIGNED"): "DECIMAL";
		case MYSQL_TYPE_TINY:
			return isUnsigned ? (isZerofill? "TINYINT UNSIGNED ZEROFILL" : "TINYINT UNSIGNED"): "TINYINT";
		case MYSQL_TYPE_SHORT:
			return isUnsigned ? (isZerofill? "SMALLINT UNSIGNED ZEROFILL" : "SMALLINT UNSIGNED"): "SMALLINT";
		case MYSQL_TYPE_LONG:
			return isUnsigned ? (isZerofill? "INT UNSIGNED ZEROFILL" : "INT UNSIGNED"): "INT";
		case MYSQL_TYPE_FLOAT:
			return isUnsigned ? (isZerofill? "FLOAT UNSIGNED ZEROFILL" : "FLOAT UNSIGNED"): "FLOAT";
		case MYSQL_TYPE_DOUBLE:
			return isUnsigned ? (isZerofill? "DOUBLE UNSIGNED ZEROFILL" : "DOUBLE UNSIGNED"): "DOUBLE";
		case MYSQL_TYPE_NULL:
			return "NULL";
		case MYSQL_TYPE_TIMESTAMP:
			return "TIMESTAMP";
		case MYSQL_TYPE_LONGLONG:
			return isUnsigned ? (isZerofill? "BIGINT UNSIGNED ZEROFILL" : "BIGINT UNSIGNED") : "BIGINT";
		case MYSQL_TYPE_INT24:
			return isUnsigned ? (isZerofill? "MEDIUMINT UNSIGNED ZEROFILL" : "MEDIUMINT UNSIGNED") : "MEDIUMINT";
		case MYSQL_TYPE_DATE:
			return "DATE";
		case MYSQL_TYPE_TIME:
			return "TIME";
		case MYSQL_TYPE_DATETIME:
			return "DATETIME";
		case MYSQL_TYPE_TINY_BLOB:
			return "TINYBLOB";
		case MYSQL_TYPE_MEDIUM_BLOB:
			return "MEDIUMBLOB";
		case MYSQL_TYPE_LONG_BLOB:
			return "LONGBLOB";
		case MYSQL_TYPE_BLOB:
			if (field->charsetnr == MAGIC_BINARY_CHARSET_NR) {
				return "BLOB";
			}
			return "TEXT";
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
			if (field->charsetnr == MAGIC_BINARY_CHARSET_NR) {
				return "VARBINARY";
			}
			return "VARCHAR";
		case MYSQL_TYPE_STRING:
			if (field->flags & ENUM_FLAG) {
				return "VARCHAR";
			}
			if (field->flags & SET_FLAG) {
				return "VARCHAR";
			}
			if ((field->flags & BINARY_FLAG) && field->charsetnr == MAGIC_BINARY_CHARSET_NR) {
				return "BINARY";
			}
			return "CHAR";
		case MYSQL_TYPE_ENUM:
			/* This should never happen */
			return "CHAR";
		case MYSQL_TYPE_YEAR:
			return "DATE";
		case MYSQL_TYPE_SET:
			/* This should never happen */
			return "CHAR";
		case MYSQL_TYPE_GEOMETRY:
			return "GEOMETRY";
		default:
			return "UNKNOWN";
	}
}
/* }}} */


}; /* namespace util */
}; /* namespace mysql */
}; /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
