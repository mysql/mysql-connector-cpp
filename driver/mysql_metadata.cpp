/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include <stdlib.h>
#include <memory>
#include <cppconn/exception.h>
#include "mysql_util.h"
#include "mysql_connection.h"
#include "mysql_metadata.h"
#include "mysql_art_resultset.h"
#include "mysql_statement.h"
#include "mysql_prepared_statement.h"
#include "mysql_debug.h"

// For snprintf
#include <stdio.h>

namespace sql
{
namespace mysql
{

struct TypeInfoDef
{
	const char * const typeName;
	int dataType;
	unsigned long long precision;
	const char * const literalPrefix;
	const char * const literalSuffix;
	const char * const createParams;
	short nullable;
	bool caseSensitive;
	short searchable;
	bool isUnsigned;
	bool fixedPrecScale;
	bool autoIncrement;
	const char * localTypeName;
	int minScale;
	int maxScale;
	int sqlDataType;
	int sqlDateTimeSub;
	int numPrecRadix;
};

static const TypeInfoDef mysqlc_types[] = {

	// ------------- MySQL-Type: BIT. DBC-Type: BIT -------------
	{
		"BIT",								// Typename
		sql::DataType::BIT,					// dbc-type
		1,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"BIT",								// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ------------ MySQL-Type: BOOL. DBC-Type: BIT -------------
	{
		"BOOL",								// Typename
		sql::DataType::TINYINT,				// dbc-type
		1,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"BOOL",								// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// --------- MySQL-Type: TINYINT DBC-Type: TINYINT ----------
	{
		"TINYINT",							// Typename
		sql::DataType::TINYINT,				// dbc-type
		3,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)] [ZEROFILL]",					// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"TINYINT",							// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"TINYINT UNSIGNED",					// Typename
		sql::DataType::TINYINT,				// dbc-type
		3,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)] [UNSIGNED] [ZEROFILL]",		// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"TINYINT UNSIGNED",					// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"TINYINT UNSIGNED ZEROFILL",		// Typename
		sql::DataType::TINYINT,				// dbc-type
		3,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)] [UNSIGNED]",					// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"TINYINT UNSIGNED ZEROFILL",		// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: BIGINT DBC-Type: BIGINT ----------
	{
		"BIGINT",							// Typename
		sql::DataType::BIGINT,				// dbc-type
		19,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)] [ZEROFILL]",					// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"BIGINT",							// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"BIGINT UNSIGNED",					// Typename
		sql::DataType::BIGINT,				// dbc-type
		19,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)] [ZEROFILL]",					// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"BIGINT UNSIGNED",					// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"BIGINT UNSIGNED ZEROFILL",			// Typename
		sql::DataType::BIGINT,				// dbc-type
		19,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)]",							// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"BIGINT UNSIGNED ZEROFILL",			// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: LONG VARBINARY DBC-Type: LONGVARBINARY ----------
	{
		"LONG VARBINARY",					// Typename
		sql::DataType::LONGVARBINARY,		// dbc-type
		16777215,							// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		true,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"LONG VARBINARY",					// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: MEDIUMBLOB DBC-Type: LONGVARBINARY ----------
	{
		"MEDIUMBLOB",						// Typename
		sql::DataType::LONGVARBINARY,		// dbc-type
		16777215,							// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		true,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"MEDIUMBLOB",						// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: LONGBLOB DBC-Type: LONGVARBINARY ----------
	{
		"LONGBLOB",							// Typename
		sql::DataType::LONGVARBINARY,		// dbc-type
		0xFFFFFFFF,							// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		true,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"LONGBLOB",							// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: BLOB DBC-Type: LONGVARBINARY ----------
	{
		"BLOB",								// Typename
		sql::DataType::LONGVARBINARY,		// dbc-type
		0xFFFF,								// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		true,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"BLOB",								// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: TINYBLOB DBC-Type: LONGVARBINARY ----------
	{
		"TINYBLOB",							// Typename
		sql::DataType::LONGVARBINARY,		// dbc-type
		0xFF,								// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		true,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"TINYBLOB",							// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: VARBINARY DBC-Type: VARBINARY ----------
	{
		"VARBINARY",						// Typename
		sql::DataType::VARBINARY,			// dbc-type
		0xFF,								// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"(M)",								// Create params
		DatabaseMetaData::typeNullable,		// nullable
		true,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"VARBINARY",						// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: BINARY DBC-Type: BINARY ----------
	{
		"BINARY",							// Typename
		sql::DataType::BINARY,				// dbc-type
		0xFF,								// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"(M)",								// Create params
		DatabaseMetaData::typeNullable,		// nullable
		true,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"BINARY",							// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: LONG VARCHAR DBC-Type: LONG VARCHAR ----------
	{
		"LONG VARCHAR",						// Typename
		sql::DataType::LONGVARCHAR,			// dbc-type
		0xFFFFFF,							// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"LONG VARCHAR",						// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: MEDIUMTEXT DBC-Type: LONG VARCHAR ----------
	{
		"MEDIUMTEXT",						// Typename
		sql::DataType::LONGVARCHAR,			// dbc-type
		0xFFFFFF,							// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"MEDIUMTEXT",						// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: LONGTEXT DBC-Type: LONG VARCHAR ----------
	{
		"LONGTEXT",							// Typename
		sql::DataType::LONGVARCHAR,			// dbc-type
		0xFFFFFF,							// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"LONGTEXT",							// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: TEXT DBC-Type: LONG VARCHAR ----------
	{
		"TEXT",								// Typename
		sql::DataType::LONGVARCHAR,			// dbc-type
		0xFFFF,								// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"TEXT",								// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: TINYTEXT DBC-Type: LONG VARCHAR ----------
	{
		"TINYTEXT",							// Typename
		sql::DataType::LONGVARCHAR,			// dbc-type
		0xFF,								// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"TINYTEXT",							// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: CHAR DBC-Type: CHAR ----------
	{
		"CHAR",								// Typename
		sql::DataType::CHAR,				// dbc-type
		0xFF,								// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"(M)",								// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"CHAR",								// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

// ToDo : The maximum number of digits for DECIMAL or NUMERIC is 65 (64 from MySQL 5.0.3 to 5.0.5).

	// ----------- MySQL-Type: NUMERIC (silently conv. to DECIMAL) DBC-Type: NUMERIC ----------
	{
		"NUMERIC",							// Typename
		sql::DataType::NUMERIC,				// dbc-type
		64,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M[,D])] [ZEROFILL]",				// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"NUMERIC",							// local type name
		-308,								// minimum scale
		308,								// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: DECIMAL  DBC-Type: DECIMAL ----------
	{
		"DECIMAL",							// Typename
		sql::DataType::DECIMAL,				// dbc-type
		64,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M[,D])] [ZEROFILL] [UNSIGNED]",	// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"DECIMAL",							// local type name
		-308,								// minimum scale
		308,								// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"DECIMAL UNSIGNED",					// Typename
		sql::DataType::DECIMAL,				// dbc-type
		64,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M[,D])] [ZEROFILL]",				// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"DECIMAL UNSIGNED",					// local type name
		-308,								// minimum scale
		308,								// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"DECIMAL UNSIGNED ZEROFILL",		// Typename
		sql::DataType::DECIMAL,				// dbc-type
		64,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M[,D])]",						// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"DECIMAL UNSIGNED ZEROFILL",		// local type name
		-308,								// minimum scale
		308,								// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

#ifdef LETS_HAVE_JUST_ONLY_ONE_INT_TYPE
	// ----------- MySQL-Type: INTEGER DBC-Type: INTEGER ----------
	{
		"INTEGER",							// Typename
		sql::DataType::INTEGER,				// dbc-type
		10,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)] [UNSIGNED] [ZEROFILL]",		// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"INTEGER",							// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	{
		"INTEGER UNSIGNED",					// Typename
		sql::DataType::INTEGER,				// dbc-type
		10,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)] [ZEROFILL]",					// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"INTEGER UNSIGNED",					// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
#endif
	// ----------- MySQL-Type: INT DBC-Type: INTEGER ----------
	{
		"INT",							// Typename
		sql::DataType::INTEGER,				// dbc-type
		10,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)] [UNSIGNED] [ZEROFILL]",		// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"INT",								// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"INT UNSIGNED",						// Typename
		sql::DataType::INTEGER,				// dbc-type
		10,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)] [ZEROFILL]",					// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"INT UNSIGNED",						// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"INT UNSIGNED ZEROFILL",			// Typename
		sql::DataType::INTEGER,				// dbc-type
		10,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)]",					// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"INT UNSIGNED ZEROFILL",			// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: MEDIUMINT DBC-Type: MEDIUMINT ----------
	{
		"MEDIUMINT",						// Typename
		sql::DataType::MEDIUMINT,			// dbc-type
		7,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)] [UNSIGNED] [ZEROFILL]",		// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"MEDIUMINT",						// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"MEDIUMINT UNSIGNED",				// Typename
		sql::DataType::MEDIUMINT,			// dbc-type
		7,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)] [ZEROFILL]",					// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"MEDIUMINT UNSIGNED",				// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"MEDIUMINT UNSIGNED ZEROFILL",		// Typename
		sql::DataType::MEDIUMINT,			// dbc-type
		7,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)]",					// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"MEDIUMINT UNSIGNED ZEROFILL",		// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: SMALLINT DBC-Type: SMALLINT ----------
	{
		"SMALLINT",							// Typename
		sql::DataType::SMALLINT,			// dbc-type
		5,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)] [UNSIGNED] [ZEROFILL]",		// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"SMALLINT",							// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"SMALLINT UNSIGNED",				// Typename
		sql::DataType::SMALLINT,			// dbc-type
		5,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)] [ZEROFILL]",					// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"SMALLINT UNSIGNED",				// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"SMALLINT UNSIGNED ZEROFILL",		// Typename
		sql::DataType::SMALLINT,			// dbc-type
		5,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M)]",							// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"SMALLINT UNSIGNED ZEROFILL",		// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	/*
	 * MySQL Type: FLOAT JDBC Type: REAL (this is the SINGLE PERCISION
	 * floating point type)
	 */

	// ----------- MySQL-Type: FLOAT DBC-Type: REAL ----------
	{
		"FLOAT",							// Typename
		sql::DataType::REAL,				// dbc-type
		10,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M,D)] [ZEROFILL] [UNSIGNED]",	// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"FLOAT",							// local type name
		-38,								// minimum scale
		38,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"FLOAT UNSIGNED",					// Typename
		sql::DataType::REAL,				// dbc-type
		10,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M,D)] [ZEROFILL]",				// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"FLOAT UNSIGNED",					// local type name
		-38,								// minimum scale
		38,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"FLOAT UNSIGNED ZEROFILL",			// Typename
		sql::DataType::REAL,				// dbc-type
		10,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M,D)]",							// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"FLOAT UNSIGNED ZEROFILL",			// local type name
		-38,								// minimum scale
		38,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: DOUBLE DBC-Type: DOUBLE ----------
	{
		"DOUBLE",							// Typename
		sql::DataType::DOUBLE,				// dbc-type
		17,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M,D)] [ZEROFILL] [UNSIGNED]",	// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"DOUBLE",							// local type name
		-308,								// minimum scale
		308,								// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"DOUBLE UNSIGNED",						// Typename
		sql::DataType::DOUBLE,				// dbc-type
		17,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M,D)] [ZEROFILL]",				// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"DOUBLE UNSIGNED",					// local type name
		-308,								// minimum scale
		308,								// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"DOUBLE UNSIGNED ZEROFILL",			// Typename
		sql::DataType::DOUBLE,				// dbc-type
		17,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M,D)]",							// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"DOUBLE UNSIGNED ZEROFILL",			// local type name
		-308,								// minimum scale
		308,								// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	/*
	 * MySQL Type: REAL (does not map to Types.REAL) JDBC Type: DOUBLE
	 */
	// ----------- MySQL-Type: REAL DBC-Type: DOUBLE ----------

	{
		"REAL",								// Typename
		sql::DataType::DOUBLE,				// dbc-type
		17,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M,D)] [ZEROFILL] [UNSIGNED]",	// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"REAL",								// local type name
		-308,								// minimum scale
		308,								// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	{
		"REAL UNSIGNED",					// Typename
		sql::DataType::DOUBLE,				// dbc-type
		17,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"[(M,D)] [ZEROFILL]",				// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		true,								// unsigned_attribute
		false,								// fixed_prec_scale
		true,								// auto_increment
		"REAL UNSIGNED",					// local type name
		-308,								// minimum scale
		308,								// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: VARCHAR DBC-Type: VARCHAR ----------
	{
		"VARCHAR",							// Typename
		sql::DataType::VARCHAR,				// dbc-type
		255,								// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"(M)",								// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"VARCHAR",							// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: ENUM DBC-Type: VARCHAR ----------
	{
		"ENUM",								// Typename
		sql::DataType::ENUM,				// dbc-type
		0xFFFF,								// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"ENUM",								// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: SET DBC-Type: VARCHAR ----------
	{
		"SET",								// Typename
		sql::DataType::SET,					// dbc-type
		64,									// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"SET",								// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: DATE DBC-Type: DATE ----------
	{
		"DATE",								// Typename
		sql::DataType::DATE,				// dbc-type
		0,									// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"DATE",								// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: TIME DBC-Type: TIME ----------
	{
		"TIME",								// Typename
		sql::DataType::TIME,				// dbc-type
		0,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"TIME",								// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: DATETIME DBC-Type: TIMESTAMP ----------
	{
		"DATETIME",							// Typename
		sql::DataType::TIMESTAMP,			// dbc-type
		0,									// Precision
		"",									// Literal prefix
		"",									// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"DATETIME",							// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},

	// ----------- MySQL-Type: TIMESTAMP DBC-Type: TIMESTAMP ----------
	{
		"TIMESTAMP",						// Typename
		sql::DataType::TIMESTAMP,			// dbc-type
		0,									// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"[(M)]",							// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"TIMESTAMP",						// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	// ----------- MySQL-Type: DATE DBC-Type: DATE ----------
	{
		"YEAR",								// Typename
		sql::DataType::YEAR,				// dbc-type
		0,									// Precision
		"'",								// Literal prefix
		"'",								// Literal suffix
		"",									// Create params
		DatabaseMetaData::typeNullable,		// nullable
		false,								// case sensitive
		DatabaseMetaData::typeSearchable,	// searchable
		false,								// unsigned_attribute
		false,								// fixed_prec_scale
		false,								// auto_increment
		"YEAR",								// local type name
		0,									// minimum scale
		0,									// maximum scale
		0,									// sql data type (unused)
		0,									// sql datetime sub (unused)
		10									// num prec radix
	},
	// ----------- MySQL-Type: TIMESTAMP DBC-Type: TIMESTAMP ----------

	{
		NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	}
};


/* {{{ my_i_to_a() -I- */
static inline const char * my_i_to_a(char * buf, size_t buf_size, int a)
{
	snprintf(buf, buf_size, "%d", a);
	return buf;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::MySQL_ConnectionMetaData() -I- */
MySQL_ConnectionMetaData::MySQL_ConnectionMetaData(MySQL_Connection * const conn, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l)
  : connection(conn), logger(l? l->getReference():NULL), use_info_schema(true)
{
	CPP_ENTER("MySQL_ConnectionMetaData::MySQL_ConnectionMetaData");
	server_version = mysql_get_server_version(connection->getMySQLHandle());
	lower_case_table_names = connection->getSessionVariable("lower_case_table_names");

	connection->getClientOption("metadata_use_info_schema", (void *) &use_info_schema);
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::~MySQL_ConnectionMetaData() -I- */
MySQL_ConnectionMetaData::~MySQL_ConnectionMetaData()
{
	/* Don't remove the block or we can get into problems with logger */
	{
		CPP_ENTER("MySQL_ConnectionMetaData::~MySQL_ConnectionMetaData");
		CPP_INFO_FMT("this=%p", this);
	}
	logger->freeReference();
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getSchemata() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getSchemata(const std::string& /*catalogName*/)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getSchemata");
	std::auto_ptr<sql::Statement> stmt(connection->createStatement());
	return stmt->executeQuery("SHOW DATABASES");
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getSchemaObjects() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getSchemaObjects(const std::string& /* catalogName */, const std::string& schemaName, const std::string& objectType)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getSchemaObjects");
	// for now catalog name is ignored
	std::string query;

	std::string schemata_where_clause;
	std::string tables_where_clause;
	std::string views_where_clause;
	std::string routines_where_clause;
	std::string triggers_where_clause;

	const std::string schemata_select_items("'schema' AS 'OBJECT_TYPE', CATALOG_NAME as 'CATALOG', SCHEMA_NAME as 'SCHEMA', SCHEMA_NAME as 'NAME'");
	const std::string tables_select_items("'table' AS 'OBJECT_TYPE', TABLE_CATALOG as 'CATALOG', TABLE_SCHEMA as 'SCHEMA', TABLE_NAME as 'NAME'");
	const std::string views_select_items("'view' AS 'OBJECT_TYPE', TABLE_CATALOG as 'CATALOG', TABLE_SCHEMA as 'SCHEMA', TABLE_NAME as 'NAME'");
	const std::string routines_select_items("ROUTINE_TYPE AS 'OBJECT_TYPE', ROUTINE_CATALOG as 'CATALOG', ROUTINE_SCHEMA as 'SCHEMA', ROUTINE_NAME as 'NAME'");
	const std::string triggers_select_items("'trigger' AS 'OBJECT_TYPE', TRIGGER_CATALOG as 'CATALOG', TRIGGER_SCHEMA as 'SCHEMA', TRIGGER_NAME as 'NAME'");

	const std::string schema_ddl_column("Create Database");
	const std::string table_ddl_column("Create Table");
	const std::string view_ddl_column("Create View");
	const std::string procedure_ddl_column("Create Procedure");
	const std::string function_ddl_column("Create Function");
	const std::string trigger_ddl_column("SQL Original Statement");

	if (schemaName.length() > 0) {
		tables_where_clause.append(" WHERE table_type<>'VIEW' AND table_schema = '").append(schemaName).append("' ");
		schemata_where_clause.append(" WHERE schema_name = '").append(schemaName).append("' ");
		views_where_clause.append(" WHERE table_schema = '").append(schemaName).append("' ");
		routines_where_clause.append(" WHERE routine_schema = '").append(schemaName).append("' ");
		triggers_where_clause.append(" WHERE trigger_schema = '").append(schemaName).append("' ");
	}

	if (objectType.length() == 0) {
		query.append("SELECT ").append(tables_select_items)
			.append(" FROM information_schema.tables ").append(tables_where_clause)
			.append("UNION SELECT ").append(views_select_items)
			.append(" FROM information_schema.views ").append(views_where_clause)
			.append("UNION SELECT ").append(routines_select_items)
			.append(" FROM information_schema.routines ").append(routines_where_clause)
			.append("UNION SELECT ").append(triggers_select_items)
			.append(" FROM information_schema.triggers ").append(triggers_where_clause)
			;
	} else {
		if (objectType.compare("schema") == 0) {
			query.append("SELECT ")
				.append(schemata_select_items)
				.append(" FROM information_schema.schemata")
				.append(schemata_where_clause);
		} else if (objectType.compare("table") == 0) {
			query.append("SELECT ")
				.append(tables_select_items)
				.append(" FROM information_schema.tables")
				.append(tables_where_clause);
		} else if (objectType.compare("view") == 0) {
			query.append("SELECT ")
				.append(views_select_items)
				.append(" FROM information_schema.views")
				.append(views_where_clause);
		} else if (objectType.compare("routine") == 0) {
			query.append("SELECT ")
				.append(routines_select_items)
				.append(" FROM information_schema.routines")
				.append(routines_where_clause);
		} else if (objectType.compare("trigger") == 0) {
			query.append("SELECT ")
				.append(triggers_select_items)
				.append(" FROM information_schema.triggers")
				.append(triggers_where_clause);
		} else {
			throw sql::InvalidArgumentException("MySQLMetadata::getSchemaObjects: invalid 'objectType'");
		}
	}

	std::auto_ptr<sql::Statement> stmt1(connection->createStatement());
	std::auto_ptr<sql::ResultSet> native_rs(stmt1->executeQuery(query));

	int objtype_field_index = native_rs->findColumn("OBJECT_TYPE");
	int catalog_field_index = native_rs->findColumn("CATALOG");
	int schema_field_index = native_rs->findColumn("SCHEMA");
	int name_field_index = native_rs->findColumn("NAME");


	std::map<std::string, std::string> trigger_name_map;
	std::map<std::string, std::string> trigger_ddl_map;

	// if we fetch triggers, then build DDL for them
	if ((objectType.compare("trigger") == 0) || objectType.empty()) {
		std::string trigger_ddl_query("SELECT ");
		trigger_ddl_query
			.append(triggers_select_items)
			.append(", EVENT_MANIPULATION, EVENT_OBJECT_SCHEMA, EVENT_OBJECT_TABLE, ACTION_ORDER, "
							"	ACTION_CONDITION, ACTION_STATEMENT, ACTION_ORIENTATION, ACTION_TIMING, DEFINER"
							"	FROM information_schema.triggers ")
			.append(triggers_where_clause);

		std::auto_ptr<sql::Statement> stmt2(connection->createStatement());
		std::auto_ptr<sql::ResultSet> trigger_ddl_rs(stmt2->executeQuery(trigger_ddl_query));

		// trigger specific fields: exclusion from the rule - 'show create trigger' is not supported by verions below 5.1.21
		// reproducing ddl based on metadata
		int event_manipulation_index = trigger_ddl_rs->findColumn("EVENT_MANIPULATION");
		int event_object_schema_index = trigger_ddl_rs->findColumn("EVENT_OBJECT_SCHEMA");
		int event_object_table_index = trigger_ddl_rs->findColumn("EVENT_OBJECT_TABLE");
		int action_statement_index = trigger_ddl_rs->findColumn("ACTION_STATEMENT");
		int action_timing_index = trigger_ddl_rs->findColumn("ACTION_TIMING");
		int definer_index = trigger_ddl_rs->findColumn("DEFINER");

		while (trigger_ddl_rs->next()) {
			std::string trigger_ddl;

			// quote definer, which is stored as unquoted string
			std::string quoted_definer;
			{
				quoted_definer = trigger_ddl_rs->getString(definer_index);
				const char *quot_sym = "`\0";
				size_t i = quoted_definer.find('@');
				if (std::string::npos != i)
				{
					quoted_definer.reserve(quoted_definer.size()+4);
					quoted_definer.insert(i+1, quot_sym);
					quoted_definer.insert(i, quot_sym);
				}
				quoted_definer.insert(0, quot_sym);
				quoted_definer.push_back(quot_sym[0]);
			}

			std::string key;
			key.append("`").append(trigger_ddl_rs->getString("schema"))
				.append("`.`").append(trigger_ddl_rs->getString("name")).append("`");

			{
				trigger_ddl
					.append("CREATE\nDEFINER=").append(quoted_definer)
					.append("\nTRIGGER ").append("`")
					.append(trigger_ddl_rs->getString("schema")).append("`.`").append(trigger_ddl_rs->getString("name")).append("`")
					.append("\n").append(trigger_ddl_rs->getString(action_timing_index))
					.append(" ").append(trigger_ddl_rs->getString(event_manipulation_index))
					.append(" ON `").append(trigger_ddl_rs->getString(event_object_schema_index))
					.append("`.`").append(trigger_ddl_rs->getString(event_object_table_index)).append("`")
					.append("\nFOR EACH ROW\n")
					.append(trigger_ddl_rs->getString(action_statement_index))
					.append("\n");
					trigger_ddl_map[key] = trigger_ddl;
			}

			{
				std::string trigger_name;
				trigger_name
					.append(trigger_ddl_rs->getString(event_object_table_index))
					.append(".")
					.append(trigger_ddl_rs->getString("name"));
					trigger_name_map[key] = trigger_name;
			}
		}
	}

	std::list<std::string> rs_field_data;
	rs_field_data.push_back(std::string("OBJECT_TYPE"));
	rs_field_data.push_back(std::string("CATALOG"));
	rs_field_data.push_back(std::string("SCHEMA"));
	rs_field_data.push_back(std::string("NAME"));
	rs_field_data.push_back(std::string("DDL"));

	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	while (native_rs->next()) {
		MySQL_ArtResultSet::row_t rs_data_row;

		std::string obj_type(native_rs->getString(objtype_field_index));
		std::string schema(native_rs->getString(schema_field_index));
		std::string name(native_rs->getString(name_field_index));

		if ((obj_type.compare("PROCEDURE") == 0) || (obj_type.compare("FUNCTION") == 0)) {
			rs_data_row.push_back("routine");
		} else {
			rs_data_row.push_back(obj_type);
		}
		rs_data_row.push_back(native_rs->getString(catalog_field_index));
		rs_data_row.push_back(schema);


		if (obj_type.compare("trigger") == 0) {
			std::string key;
			key.append("`").append(schema).append("`.`").append(name).append("`");
			rs_data_row.push_back(trigger_name_map[key]);
		} else {
			rs_data_row.push_back(name);
		}

		std::string ddl_query;
		std::string ddl_column;

		if (obj_type.compare("schema") == 0) {
			ddl_column = schema_ddl_column;
			ddl_query.append("SHOW CREATE SCHEMA `").append(name).append("`");
		} else if (obj_type.compare("table") == 0) {
			ddl_column = table_ddl_column;
			ddl_query.append("SHOW CREATE TABLE `")
				.append(schema).append("`.`")
				.append(name).append("`");
		} else if (obj_type.compare("view") == 0) {
			ddl_column = view_ddl_column;
			ddl_query.append("SHOW CREATE VIEW `")
				.append(schema).append("`.`")
				.append(name).append("`");
		} else if (obj_type.compare("PROCEDURE") == 0) {
			ddl_column = procedure_ddl_column;
			ddl_query.append("SHOW CREATE PROCEDURE `")
				.append(schema).append("`.`")
				.append(name).append("`");
		} else if (obj_type.compare("FUNCTION") == 0) {
			ddl_column = function_ddl_column;
			ddl_query.append("SHOW CREATE FUNCTION `")
				.append(schema).append("`.`")
				.append(name).append("`");
		} else if (obj_type.compare("trigger") == 0) {
			/*
			ddl_column= trigger_ddl_column;
			ddl_query.append("SHOW CREATE TRIGGER `")
				.append(schema).append("`.`")
				.append(name).append("`");
			*/
		} else {
			throw sql::InvalidArgumentException("MySQL_DatabaseMetaData::getSchemaObjects: invalid OBJECT_TYPE returned from query");
		}

		// due to bugs in server code some queries can fail.
		// here we want to gather as much info as possible
		try  {
			std::string ddl;

			if (obj_type.compare("trigger") == 0) {
				//ddl
				//	.append("CREATE\nDEFINER=").append(trigger_ddl_rs->getString(definer_index))
				//	.append("\nTRIGGER ").append("`")
				//	.append(schema).append("`.`").append(name).append("`")
				//	.append("\n").append(trigger_ddl_rs->getString(action_timing_index))
				//	.append(" ").append(trigger_ddl_rs->getString(event_manipulation_index))
				//	.append(" ON `").append(trigger_ddl_rs->getString(event_object_schema_index))
				//	.append("`.`").append(trigger_ddl_rs->getString(event_object_table_index)).append("`")
				//	.append("\nFOR EACH ROW\n")
				//	.append(trigger_ddl_rs->getString(action_statement_index))
				//	.append("\n");

				std::string key;
				key.append("`").append(schema).append("`.`").append(name).append("`");

				std::map<std::string, std::string>::const_iterator it = trigger_ddl_map.find(key);
				if (it != trigger_ddl_map.end())
					ddl.append(it->second);
			} else {
				std::auto_ptr<sql::Statement> stmt3(connection->createStatement());
				std::auto_ptr<sql::ResultSet> sql_rs(stmt3->executeQuery(ddl_query));

				sql_rs->next();

				// this is a hack for views listed as tables
				int colIdx = sql_rs->findColumn(ddl_column);
				if ((colIdx == -1) && (obj_type.compare("table") == 0))
					colIdx = sql_rs->findColumn(view_ddl_column);

				ddl = sql_rs->getString(colIdx);
			}
			rs_data_row.push_back(ddl);
		} catch (SQLException) {
			rs_data_row.push_back("");
		}

		rs_data->push_back(rs_data_row);
	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getSchemaObjectTypes() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getSchemaObjectTypes()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getSchemaObjectTypes");
	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	std::list<std::string> rs_field_data;
	rs_field_data.push_back("OBJECT_TYPE");

	{
		MySQL_ArtResultSet::row_t rs_data_row;
		rs_data_row.push_back("table");
		rs_data->push_back(rs_data_row);
	}
	{
		MySQL_ArtResultSet::row_t rs_data_row;
		rs_data_row.push_back("view");
		rs_data->push_back(rs_data_row);
	}
	{
		MySQL_ArtResultSet::row_t rs_data_row;
		rs_data_row.push_back("routine");
		rs_data->push_back(rs_data_row);
	}
	{
		MySQL_ArtResultSet::row_t rs_data_row;
		rs_data_row.push_back("trigger");
		rs_data->push_back(rs_data_row);
	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::allProceduresAreCallable() -I- */
bool
MySQL_ConnectionMetaData::allProceduresAreCallable()
{
	CPP_ENTER("MySQL_ConnectionMetaData::allProceduresAreCallable");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::allTablesAreSelectable() -I- */
bool
MySQL_ConnectionMetaData::allTablesAreSelectable()
{
	CPP_ENTER("MySQL_ConnectionMetaData::allTablesAreSelectable");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::dataDefinitionCausesTransactionCommit() -I- */
bool
MySQL_ConnectionMetaData::dataDefinitionCausesTransactionCommit()
{
	CPP_ENTER("MySQL_ConnectionMetaData::dataDefinitionCausesTransactionCommit");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::dataDefinitionIgnoredInTransactions() -I- */
bool
MySQL_ConnectionMetaData::dataDefinitionIgnoredInTransactions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::dataDefinitionIgnoredInTransactions");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::deletesAreDetected() -I- */
bool
MySQL_ConnectionMetaData::deletesAreDetected(int /*type*/)
{
	CPP_ENTER("MySQL_ConnectionMetaData::deletesAreDetected");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::doesMaxRowSizeIncludeBlobs() -I- */
bool
MySQL_ConnectionMetaData::doesMaxRowSizeIncludeBlobs()
{
	CPP_ENTER("MySQL_ConnectionMetaData::doesMaxRowSizeIncludeBlobs");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getAttributes() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getAttributes(const std::string& /*catalog*/, const std::string& /*schemaPattern*/,
										const std::string& /*typeNamePattern*/, const std::string& /*attributeNamePattern*/)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getAttributes");
	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	std::list<std::string> rs_field_data;

	rs_field_data.push_back("TYPE_CAT");
	rs_field_data.push_back("TYPE_SCHEM");
	rs_field_data.push_back("TYPE_NAME");
	rs_field_data.push_back("ATTR_NAME");
	rs_field_data.push_back("DATA_TYPE");
	rs_field_data.push_back("ATTR_TYPE_NAME");
	rs_field_data.push_back("ATTR_SIZE");
	rs_field_data.push_back("DECIMAL_DIGITS");
	rs_field_data.push_back("NUM_PREC_RADIX");
	rs_field_data.push_back("NULLABLE");
	rs_field_data.push_back("REMARKS");
	rs_field_data.push_back("ATTR_DEF");
	rs_field_data.push_back("SQL_DATA_TYPE");
	rs_field_data.push_back("SQL_DATETIME_SUB");
	rs_field_data.push_back("CHAR_OCTET_LENGTH");
	rs_field_data.push_back("ORDINAL_POSITION");
	rs_field_data.push_back("IS_NULLABLE");
	rs_field_data.push_back("SCOPE_CATALOG");
	rs_field_data.push_back("SCOPE_SCHEMA");
	rs_field_data.push_back("SCOPE_TABLE");
	rs_field_data.push_back("SOURCE_DATA_TYPE");

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getBestRowIdentifier() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getBestRowIdentifier(const std::string& catalog, const std::string& schema,
												const std::string& table, int /* scope */, bool /* nullable */)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getBestRowIdentifier");
	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	std::list<std::string> rs_field_data;
	rs_field_data.push_back("SCOPE");
	rs_field_data.push_back("COLUMN_NAME");
	rs_field_data.push_back("DATA_TYPE");
	rs_field_data.push_back("TYPE_NAME");
	rs_field_data.push_back("COLUMN_SIZE");
	rs_field_data.push_back("BUFFER_LENGTH");
	rs_field_data.push_back("DECIMAL_DIGITS");
	rs_field_data.push_back("PSEUDO_COLUMN");

	std::auto_ptr<sql::ResultSet> rs(getPrimaryKeys(catalog, schema, table));

	while (rs->next()) {
		std::string columnNamePattern(rs->getString(4));

		std::auto_ptr<sql::ResultSet> rsCols(getColumns(catalog, schema, table, columnNamePattern));
		if (rsCols->next()) {
			MySQL_ArtResultSet::row_t rs_data_row;
			rs_data_row.push_back((int64_t) DatabaseMetaData::bestRowSession); // SCOPE
			rs_data_row.push_back(rs->getString(4));		// COLUMN_NAME
			rs_data_row.push_back(rsCols->getString(5));	// DATA_TYPE
			rs_data_row.push_back(rsCols->getString(6));	// TYPE_NAME
			rs_data_row.push_back(rsCols->getString(7));	// COLUMN_SIZE
			rs_data_row.push_back(rsCols->getString(8));	// BUFFER_LENGTH
			rs_data_row.push_back(rsCols->getString(9));	// DECIMAL_DIGITS
			rs_data_row.push_back((int64_t) DatabaseMetaData::bestRowNotPseudo); // PSEUDO_COLUMN

			rs_data->push_back(rs_data_row);
		}
	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getCatalogs() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getCatalogs()
{
	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	std::list<std::string> rs_field_data;

	rs_field_data.push_back("TABLE_CAT");

	{
		MySQL_ArtResultSet::row_t rs_data_row;
		rs_data_row.push_back("def");
		rs_data->push_back(rs_data_row);	
	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getCatalogSeparator() -I- */
const std::string&
MySQL_ConnectionMetaData::getCatalogSeparator()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getCatalogSeparator");
	static const std::string separator("");
	return separator;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getCatalogTerm() -I- */
const std::string&
MySQL_ConnectionMetaData::getCatalogTerm()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getCatalogTerm");
	static const std::string term("n/a");
	return term;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getColumnPrivileges() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getColumnPrivileges(const std::string& /*catalog*/, const std::string& schema,
											  const std::string& table, const std::string& columnNamePattern)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getColumnPrivileges");

	std::list<std::string> rs_field_data;
	rs_field_data.push_back("TABLE_CAT");
	rs_field_data.push_back("TABLE_SCHEM");
	rs_field_data.push_back("TABLE_NAME");
	rs_field_data.push_back("COLUMN_NAME");
	rs_field_data.push_back("GRANTOR");
	rs_field_data.push_back("GRANTEE");
	rs_field_data.push_back("PRIVILEGE");
	rs_field_data.push_back("IS_GRANTABLE");

	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	/* I_S seems currently (20080220) not to work */
	if (use_info_schema && server_version > 69999) {
#if A0
		std::string query("SELECT TABLE_CATALOG AS TABLE_CAT, TABLE_SCHEMA AS TABLE_SCHEM, TABLE_NAME,"
				 		"COLUMN_NAME, NULL AS GRANTOR, GRANTEE, PRIVILEGE_TYPE AS PRIVILEGE, IS_GRANTABLE\n"
						"FROM INFORMATION_SCHEMA.COLUMN_PRIVILEGES\n"
						"WHERE TABLE_SCHEMA LIKE ? AND TABLE_NAME=? AND COLUMN_NAME LIKE ?\n"
						"ORDER BY COLUMN_NAME, PRIVILEGE_TYPE");
		std::auto_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));
		stmt->setString(1, schema);
		stmt->setString(2, table);
		stmt->setString(3, columnNamePattern);

		std::auto_ptr<sql::ResultSet> rs(stmt->executeQuery());

		while (rs->next()) {
			MySQL_ArtResultSet::row_t rs_data_row;

			rs_data_row.push_back(rs->getString(1));	// TABLE_CAT
			rs_data_row.push_back(rs->getString(2));	// TABLE_SCHEM
			rs_data_row.push_back(rs->getString(3));	// TABLE_NAME
			rs_data_row.push_back(rs->getString(4));	// COLUMN_NAME
			rs_data_row.push_back(rs->getString(5));	// KEY_SEQ
			rs_data_row.push_back(rs->getString(6));	// PK_NAME

			rs_data->push_back(rs_data_row);
		}
#endif
	} else {
		size_t idx;
		std::string query("SHOW FULL COLUMNS FROM `");
		query.append(schema).append("`.`").append(table).append("` LIKE '").append(columnNamePattern).append("'");

		std::auto_ptr<sql::Statement> stmt(connection->createStatement());
		std::auto_ptr<sql::ResultSet> res(NULL);
		try {
			res.reset(stmt->executeQuery(query));
		} catch (SQLException &) {
			// schema and/or table doesn't exist. return empty set
			// do nothing here
		}

		while (res.get() && res->next()) {
			size_t pos = 0;
			std::string privs = res->getString(8);
			do {
				MySQL_ArtResultSet::row_t rs_data_row;
				std::string privToken;

				while (privs[pos] == ' ') ++pos; // Eat the whitespace

				idx = privs.find(",", pos);

				if (idx != std::string::npos) {
					privToken = privs.substr(pos, idx - pos);
					pos = idx + 1; /* skip ',' */
				} else {
					privToken = privs.substr(pos, privs.length() - pos);
				}
				rs_data_row.push_back("def");				// TABLE_CAT
				rs_data_row.push_back(schema);				// TABLE_SCHEM
				rs_data_row.push_back(table);				// TABLE_NAME
				rs_data_row.push_back(res->getString(1));	// COLUMN_NAME
				rs_data_row.push_back("");					// GRANTOR
				rs_data_row.push_back(getUserName());		// GRANTEE
				rs_data_row.push_back(privToken);			// PRIVILEGE
				rs_data_row.push_back("");					// IS_GRANTABLE

				rs_data->push_back(rs_data_row);

			} while (idx != std::string::npos);
		}
	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getColumns() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getColumns(const std::string& /*catalog*/, const std::string& schemaPattern,
									 const std::string& tableNamePattern, const std::string& columnNamePattern)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getColumns");
	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	std::list<std::string> rs_field_data;

	rs_field_data.push_back("TABLE_CAT");
	rs_field_data.push_back("TABLE_SCHEM");
	rs_field_data.push_back("TABLE_NAME");
	rs_field_data.push_back("COLUMN_NAME");
	rs_field_data.push_back("DATA_TYPE");
	rs_field_data.push_back("TYPE_NAME");
	rs_field_data.push_back("COLUMN_SIZE");
	rs_field_data.push_back("BUFFER_LENGTH");
	rs_field_data.push_back("DECIMAL_DIGITS");
	rs_field_data.push_back("NUM_PREC_RADIX");
	rs_field_data.push_back("NULLABLE");
	rs_field_data.push_back("REMARKS");
	rs_field_data.push_back("COLUMN_DEF");
	rs_field_data.push_back("SQL_DATA_TYPE");
	rs_field_data.push_back("SQL_DATETIME_SUB");
	rs_field_data.push_back("CHAR_OCTET_LENGTH");
	rs_field_data.push_back("ORDINAL_POSITION");
	rs_field_data.push_back("IS_NULLABLE");
	/* The following are not known by SDBC */
	rs_field_data.push_back("SCOPE_CATALOG");
	rs_field_data.push_back("SCOPE_SCHEMA");
	rs_field_data.push_back("SCOPE_TABLE");
	rs_field_data.push_back("SOURCE_DATA_TYPE");
	rs_field_data.push_back("IS_AUTOINCREMENT");

	if (use_info_schema && server_version > 50020) {
		char buf[5];
		std::string query("SELECT TABLE_CATALOG AS TABLE_CAT, TABLE_SCHEMA AS TABLE_SCHEM, TABLE_NAME, COLUMN_NAME, DATA_TYPE,"
			"CASE "
			"WHEN LOCATE('unsigned', COLUMN_TYPE) != 0 AND LOCATE('unsigned', DATA_TYPE) = 0 THEN "
			"  CASE"
			"    WHEN LOCATE('zerofill', COLUMN_TYPE) != 0 AND LOCATE('zerofill', DATA_TYPE) = 0 THEN CONCAT(UCASE(DATA_TYPE), ' UNSIGNED ZEROFILL')"
			"    ELSE CONCAT(UCASE(DATA_TYPE), ' UNSIGNED')"
			"  END "
//			"ELSE"
//			"  CASE"
//			"    WHEN LCASE(DATA_TYPE)='set' THEN 'VARCHAR'"
//			"    WHEN LCASE(DATA_TYPE)='enum' THEN 'VARCHAR'"
//			"    WHEN LCASE(DATA_TYPE)='year' THEN 'YEAR'"
			"    ELSE UCASE(DATA_TYPE)"
//			"  END "
			"END AS TYPE_NAME,"
 			"CASE "
				"WHEN LCASE(DATA_TYPE)='year' THEN SUBSTRING(COLUMN_TYPE, 6, 1) -- 'year('=5\n"
				"WHEN LCASE(DATA_TYPE)='date' THEN 10 "
				"WHEN LCASE(DATA_TYPE)='time' THEN 8 "
				"WHEN LCASE(DATA_TYPE)='datetime' THEN 19 "
				"WHEN LCASE(DATA_TYPE)='timestamp' THEN 19 "
				"WHEN CHARACTER_MAXIMUM_LENGTH IS NULL THEN NUMERIC_PRECISION "
				"ELSE CHARACTER_MAXIMUM_LENGTH END AS COLUMN_SIZE, "
			"'' AS BUFFER_LENGTH,"
			"NUMERIC_SCALE AS DECIMAL_DIGITS,"
			"10 AS NUM_PREC_RADIX,"
			"CASE WHEN IS_NULLABLE='NO' THEN ");
		query.append(my_i_to_a(buf, sizeof(buf) - 1, columnNoNulls));
		query.append(" ELSE CASE WHEN IS_NULLABLE='YES' THEN ");
		query.append(my_i_to_a(buf, sizeof(buf) - 1, columnNullable));
		query.append(" ELSE ");
		query.append(my_i_to_a(buf, sizeof(buf) - 1, columnNullableUnknown));
		query.append(" END END AS NULLABLE,"
			"COLUMN_COMMENT AS REMARKS,"
			"COLUMN_DEFAULT AS COLUMN_DEF,"
			"0 AS SQL_DATA_TYPE,"
			"0 AS SQL_DATETIME_SUB,"
			"CHARACTER_OCTET_LENGTH,"
			"ORDINAL_POSITION,"
			"IS_NULLABLE,"
			"NULL AS SCOPE_CATALOG,"
			"NULL AS SCOPE_SCHEMA,"
			"NULL AS SCOPE_TABLE,"
			"NULL AS SOURCE_DATA_TYPE,"
			"IF (EXTRA LIKE '%auto_increment%','YES','NO') AS IS_AUTOINCREMENT "
			"FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA LIKE ? AND TABLE_NAME LIKE ? AND COLUMN_NAME LIKE ? "
			"ORDER BY TABLE_SCHEMA, TABLE_NAME, ORDINAL_POSITION");

		std::auto_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));

		stmt->setString(1, schemaPattern);
		stmt->setString(2, tableNamePattern);
		stmt->setString(3, columnNamePattern);

		std::auto_ptr<sql::ResultSet> rs(stmt->executeQuery());

		while (rs->next()) {
			MySQL_ArtResultSet::row_t rs_data_row;

			rs_data_row.push_back(rs->getString(1));	// TABLE_CAT
			rs_data_row.push_back(rs->getString(2));	// TABLE_SCHEM
			rs_data_row.push_back(rs->getString(3));	// TABLE_NAME
			rs_data_row.push_back(rs->getString(4));	// COLUMN_NAME
			rs_data_row.push_back((int64_t) sql::mysql::util::mysql_string_type_to_datatype(rs->getString(5)));	// DATA_TYPE
			rs_data_row.push_back(rs->getString(6));	// TYPE_NAME
			rs_data_row.push_back(rs->getInt64(7));		// COLUMN_SIZE
			rs_data_row.push_back(rs->getInt64(8));		// BUFFER_LENGTH
			rs_data_row.push_back(rs->getInt64(9));		// DECIMAL_DIGITS
			rs_data_row.push_back(rs->getInt64(10));	// NUM_PREC_RADIX
			rs_data_row.push_back(rs->getString(11));	// NULLABLE
			rs_data_row.push_back(rs->getString(12));	// REMARKS
			rs_data_row.push_back(rs->getString(13));	// COLUMN_DEFAULT
			rs_data_row.push_back(rs->getString(14));	// SQL_DATA_TYPE
			rs_data_row.push_back(rs->getString(15));	// SQL_DATETIME_SUB
			rs_data_row.push_back(rs->getString(16));	// CHAR_OCTET_LENGTH
			rs_data_row.push_back(rs->getString(17));	// ORDINAL_POSITION
			rs_data_row.push_back(rs->getString(18));	// IS_NULLABLE
			/* The following are not currently used by C/OOO*/
			rs_data_row.push_back(rs->getString(19));	// SCOPE_CATALOG
			rs_data_row.push_back(rs->getString(20));	// SCOPE_SCHEMA
			rs_data_row.push_back(rs->getString(21));	// SCOPE_TABLE
			rs_data_row.push_back(rs->getString(22));	// SOURCE_DATA_TYPE
			rs_data_row.push_back(rs->getString(23));	// IS_AUTOINCREMENT

			rs_data->push_back(rs_data_row);
		}
	} else {
		/* get schemata */
		std::string query1("SHOW DATABASES LIKE '");
		query1.append(schemaPattern).append("'");

		std::auto_ptr<sql::Statement> stmt1(connection->createStatement());
		std::auto_ptr<sql::ResultSet> rs1(stmt1->executeQuery(query1));

		while (rs1->next()) {
			std::string current_schema(rs1->getString(1));
			std::string query2("SHOW TABLES FROM `");
			query2.append(current_schema).append("` LIKE '").append(tableNamePattern).append("'");

			std::auto_ptr<sql::Statement> stmt2(connection->createStatement());
			std::auto_ptr<sql::ResultSet> rs2(stmt2->executeQuery(query2));

			while (rs2->next()) {
				std::string current_table(rs2->getString(1));
				std::string query3("SELECT * FROM `");
				query3.append(current_schema).append("`.`").append(current_table).append("` WHERE 0=1");

				std::auto_ptr<sql::Statement> stmt3(connection->createStatement());
				std::auto_ptr<sql::ResultSet> rs3(stmt1->executeQuery(query3));
				sql::ResultSetMetaData * rs3_meta = rs3->getMetaData();

				std::string query4("SHOW FULL COLUMNS FROM `");
				query4.append(current_schema).append("`.`").append(current_table).append("` LIKE '").append(columnNamePattern).append("'");
				std::auto_ptr<sql::Statement> stmt4(connection->createStatement());
				std::auto_ptr<sql::ResultSet> rs4(stmt1->executeQuery(query4));

				while (rs4->next()) {
					for (unsigned int i = 1; i <= rs3_meta->getColumnCount(); ++i) {
						/*
						  `SELECT * FROM XYZ WHERE 0=1` will return metadata about all
						  columns but `columnNamePattern` could be set. So, we can have different
						  number of rows/columns in the result sets which doesn't correspond.
						*/
						if (rs3_meta->getColumnName(i) == rs4->getString(1)) {
							MySQL_ArtResultSet::row_t rs_data_row;

							rs_data_row.push_back("def");				// TABLE_CAT
							rs_data_row.push_back(current_schema);		// TABLE_SCHEM
							rs_data_row.push_back(current_table);		// TABLE_NAME
							rs_data_row.push_back(rs4->getString(1));	// COLUMN_NAME
							rs_data_row.push_back((int64_t) rs3_meta->getColumnType(i)); 		// DATA_TYPE
							rs_data_row.push_back(rs3_meta->getColumnTypeName(i));											// TYPE_NAME
							rs_data_row.push_back((int64_t) rs3_meta->getColumnDisplaySize(i));	// COLUMN_SIZE
							rs_data_row.push_back("");					// BUFFER_LENGTH
							rs_data_row.push_back((int64_t) rs3_meta->getScale(i)); // DECIMAL_DIGITS
							rs_data_row.push_back("10");							// NUM_PREC_RADIX
							rs_data_row.push_back((int64_t) rs3_meta->isNullable(i)); // Is_nullable
							rs_data_row.push_back(rs4->getString(9));		// REMARKS
							rs_data_row.push_back(rs4->getString(6));		// COLUMN_DEFAULT
							rs_data_row.push_back("");						// SQL_DATA_TYPE - unused
							rs_data_row.push_back("");						// SQL_DATETIME_SUB - unused
							rs_data_row.push_back((int64_t) rs3_meta->getColumnDisplaySize(i)); // CHAR_OCTET_LENGTH
							rs_data_row.push_back((int64_t) i); // ORDINAL_POSITION
							rs_data_row.push_back(rs3_meta->isNullable(i)? "YES":"NO");		// IS_NULLABLE

							rs_data_row.push_back("");	// SCOPE_CATALOG - unused
							rs_data_row.push_back("");	// SCOPE_SCHEMA - unused
							rs_data_row.push_back("");	// SCOPE_TABLE - unused
							rs_data_row.push_back("");	// SOURCE_DATA_TYPE - unused
							rs_data_row.push_back("");	// IS_AUTOINCREMENT - unused

							rs_data->push_back(rs_data_row);

							/* don't iterate any more, we have found our column */
							break;
						}
					}
				}
			}
		}

	}
	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getConnection() -I- */
Connection *
MySQL_ConnectionMetaData::getConnection()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getConnection");
	return this->connection;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getCrossReference() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getCrossReference(const std::string& primaryCatalog, const std::string& primarySchema,
											const std::string& primaryTable, const std::string& foreignCatalog ,
											const std::string& foreignSchema, const std::string& foreignTable)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getCrossReference");
	CPP_INFO_FMT("p_catalog=%s f_catalog=%s p_schema=%s f_schema=%s p_table=%s f_table=%s",
				primaryCatalog.c_str(), foreignCatalog.c_str(), primarySchema.c_str(), foreignSchema.c_str(),
				primaryTable.c_str(), foreignTable.c_str());

	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	std::list<std::string> rs_field_data;
	rs_field_data.push_back("PKTABLE_CAT");
	rs_field_data.push_back("PKTABLE_SCHEM");
	rs_field_data.push_back("PKTABLE_NAME");
	rs_field_data.push_back("PKCOLUMN_NAME");
	rs_field_data.push_back("FKTABLE_CAT");
	rs_field_data.push_back("FKTABLE_SCHEM");
	rs_field_data.push_back("FKTABLE_NAME");
	rs_field_data.push_back("FKCOLUMN_NAME");
	rs_field_data.push_back("KEY_SEQ");
	rs_field_data.push_back("UPDATE_RULE");
	rs_field_data.push_back("DELETE_RULE");
	rs_field_data.push_back("FK_NAME");
	rs_field_data.push_back("PK_NAME");
	rs_field_data.push_back("DEFERRABILITY");

	/* Not sure which version, let it not be 5.1.0, just something above which is anyway not used anymore */
	if (use_info_schema && server_version >= 50110) {
		/* This just doesn't work */
		/* currently this doesn't work - we have to wait for implementation of REFERENTIAL_CONSTRAINTS */
		char buf[10];
		my_i_to_a(buf, sizeof(buf) - 1, importedKeyCascade);
		std::string importedKeyCascadeStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeySetNull);
		std::string importedKeySetNullStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeySetDefault);
		std::string importedKeySetDefaultStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeyRestrict);
		std::string importedKeyRestrictStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeyNoAction);
		std::string importedKeyNoActionStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeyNotDeferrable);
		std::string importedKeyNotDeferrableStr(buf);

		std::string UpdateRuleClause;
		UpdateRuleClause.append("CASE WHEN R.UPDATE_RULE='CASCADE' THEN ").append(importedKeyCascadeStr).
						append(" WHEN R.UPDATE_RULE='SET NULL' THEN ").append(importedKeySetNullStr).
						append(" WHEN R.UPDATE_RULE='SET DEFAULT' THEN ").append(importedKeySetDefaultStr).
						append(" WHEN R.UPDATE_RULE='RESTRICT' THEN ").append(importedKeyRestrictStr).
						append(" WHEN R.UPDATE_RULE='NO ACTION' THEN ").append(importedKeyNoActionStr).
						append(" ELSE ").append(importedKeyNoActionStr).append(" END ");

		std::string DeleteRuleClause;

		DeleteRuleClause.append("CASE WHEN R.DELETE_RULE='CASCADE' THEN ").append(importedKeyCascadeStr).
						append(" WHEN R.DELETE_RULE='SET NULL' THEN ").append(importedKeySetNullStr).
						append(" WHEN R.DELETE_RULE='SET DEFAULT' THEN ").append(importedKeySetDefaultStr).
						append(" WHEN R.DELETE_RULE='RESTRICT' THEN ").append(importedKeyRestrictStr).
						append(" WHEN R.DELETE_RULE='NO ACTION' THEN ").append(importedKeyNoActionStr).
						append(" ELSE ").append(importedKeyNoActionStr).append(" END ");

		std::string OptionalRefConstraintJoinStr(
					"JOIN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS R ON "
					"(R.CONSTRAINT_NAME = B.CONSTRAINT_NAME AND R.TABLE_NAME = B.TABLE_NAME AND R.CONSTRAINT_SCHEMA = B.TABLE_SCHEMA) ");

		std::string query("SELECT \n");
		query.append("A.TABLE_CATALOG AS PKTABLE_CAT, A.REFERENCED_TABLE_SCHEMA AS PKTABLE_SCHEM, A.REFERENCED_TABLE_NAME AS PKTABLE_NAME,"
					 "A.REFERENCED_COLUMN_NAME AS PKCOLUMN_NAME, A.TABLE_CATALOG AS FKTABLE_CAT, A.TABLE_SCHEMA AS FKTABLE_SCHEM,"
					 "A.TABLE_NAME AS FKTABLE_NAME, A.COLUMN_NAME AS FKCOLUMN_NAME, A.ORDINAL_POSITION AS KEY_SEQ,");
		query.append(UpdateRuleClause);
		query.append(" AS UPDATE_RULE,");
		query.append(DeleteRuleClause);
		query.append(" AS DELETE_RULE, A.CONSTRAINT_NAME AS FK_NAME,"
					 "(SELECT CONSTRAINT_NAME FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS WHERE TABLE_SCHEMA = REFERENCED_TABLE_SCHEMA AND"
					 " TABLE_NAME = A.REFERENCED_TABLE_NAME AND CONSTRAINT_TYPE IN ('UNIQUE','PRIMARY KEY') LIMIT 1) AS PK_NAME,");
		query.append(importedKeyNotDeferrableStr);
		query.append(" AS DEFERRABILITY  FROM\nINFORMATION_SCHEMA.KEY_COLUMN_USAGE A JOIN INFORMATION_SCHEMA.TABLE_CONSTRAINTS B\n"
					 "USING (TABLE_SCHEMA, TABLE_NAME, CONSTRAINT_NAME)\n");
		query.append(OptionalRefConstraintJoinStr);
		query.append("\nWHERE B.CONSTRAINT_TYPE = 'FOREIGN KEY' AND A.REFERENCED_TABLE_SCHEMA LIKE ? AND A.REFERENCED_TABLE_NAME=?\n"
					 "AND A.TABLE_SCHEMA LIKE ? AND A.TABLE_NAME=?\nORDER BY  A.TABLE_SCHEMA, A.TABLE_NAME, A.ORDINAL_POSITION");

		std::auto_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));
		stmt->setString(1, primarySchema);
		stmt->setString(2, primaryTable);
		stmt->setString(3, foreignSchema);
		stmt->setString(4, foreignTable);
		std::auto_ptr<sql::ResultSet> rs(stmt->executeQuery());

		while (rs->next()) {
			MySQL_ArtResultSet::row_t rs_data_row;

			rs_data_row.push_back(rs->getString(1));	// PKTABLE_CAT
			rs_data_row.push_back(rs->getString(2));	// PKTABLE_SCHEMA
			rs_data_row.push_back(rs->getString(3));	// PKTABLE_NAME
			rs_data_row.push_back(rs->getString(4));	// PKCOLUMN_NAME
			rs_data_row.push_back(rs->getString(5));	// FKTABLE_CAT
			rs_data_row.push_back(rs->getString(6));	// FKTABLE_SCHEMA
			rs_data_row.push_back(rs->getString(7));	// FKTABLE_NAME
			rs_data_row.push_back(rs->getString(8));	// FKCOLUMN_NAME
			rs_data_row.push_back(rs->getString(9));	// KEY_SEQ
			rs_data_row.push_back(rs->getString(10));	// UPDATE_RULE
			rs_data_row.push_back(rs->getString(11));	// DELETE_RULE
			rs_data_row.push_back(rs->getString(12));	// FK_NAME
			rs_data_row.push_back(rs->getString(13));	// PK_NAME
			rs_data_row.push_back(rs->getString(14));	// DEFERRABILITY

			rs_data->push_back(rs_data_row);
		}
		MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
		// If there is no exception we can release otherwise on function exit memory will be freed
		rs_data.release();
		return ret;
	} else {
		throw sql::MethodNotImplementedException("MySQL_ConnectionMetaData::getCrossReference");
	}
	return NULL; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getDatabaseMajorVersion() -I- */
unsigned int
MySQL_ConnectionMetaData::getDatabaseMajorVersion()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getDatabaseMajorVersion");
	return server_version / 10000;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getDatabaseMinorVersion() -I- */
unsigned int
MySQL_ConnectionMetaData::getDatabaseMinorVersion()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getDatabaseMinorVersion");
	return (server_version % 10000) / 100;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getDatabasePatchVersion() -I- */
unsigned int
MySQL_ConnectionMetaData::getDatabasePatchVersion()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getDatabasePatchVersion");
	return server_version % 100;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getDatabaseProductName() -I- */
const std::string&
MySQL_ConnectionMetaData::getDatabaseProductName()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getDatabaseProductName");
	static const std::string product_name("MySQL");
	return product_name;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getDatabaseProductVersion() -I- */
std::string
MySQL_ConnectionMetaData::getDatabaseProductVersion()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getDatabaseProductVersion");
	return mysql_get_server_info(connection->getMySQLHandle());
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getDefaultTransactionIsolation() -I- */
int
MySQL_ConnectionMetaData::getDefaultTransactionIsolation()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getDefaultTransactionIsolation");
	if (server_version >= 32336) {
		return TRANSACTION_READ_COMMITTED;
	}
	return TRANSACTION_NONE;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getDriverMajorVersion() -I- */
unsigned int
MySQL_ConnectionMetaData::getDriverMajorVersion()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getDriverMajorVersion");
	return 1;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getDriverMinorVersion() -I- */
unsigned int
MySQL_ConnectionMetaData::getDriverMinorVersion()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getDriverMinorVersion");
	return 0;
}
/* }}} */

/* {{{ MySQL_ConnectionMetaData::getDriverPatchVersion() -I- */
unsigned int
MySQL_ConnectionMetaData::getDriverPatchVersion()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getDriverPatchVersion");
	return 2;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getDriverVersion() -I- */
const std::string&
MySQL_ConnectionMetaData::getDriverVersion()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getDriverVersion");
	static const std::string version("1.0.5-beta");
	return version;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getDriverName() -I- */
const std::string&
MySQL_ConnectionMetaData::getDriverName()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getDriverName");
	static const std::string product_version("MySQL Connector/C++");
	return product_version;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getExportedKeys() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getExportedKeys(const std::string& catalog, const std::string& schema, const std::string& table)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getExportedKeys");
	CPP_INFO_FMT("catalog=%s schema=%s table=%s", catalog.c_str(), schema.c_str(), table.c_str());
	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	std::list<std::string> rs_field_data;
	rs_field_data.push_back("PKTABLE_CAT");
	rs_field_data.push_back("PKTABLE_SCHEM");
	rs_field_data.push_back("PKTABLE_NAME");
	rs_field_data.push_back("PKCOLUMN_NAME");
	rs_field_data.push_back("FKTABLE_CAT");
	rs_field_data.push_back("FKTABLE_SCHEM");
	rs_field_data.push_back("FKTABLE_NAME");
	rs_field_data.push_back("FKCOLUMN_NAME");
	rs_field_data.push_back("KEY_SEQ");
	rs_field_data.push_back("UPDATE_RULE");
	rs_field_data.push_back("DELETE_RULE");
	rs_field_data.push_back("FK_NAME");
	rs_field_data.push_back("PK_NAME");
	rs_field_data.push_back("DEFERRABILITY");

	/* Not sure which version, let it not be 5.1.0, just something above which is anyway not used anymore */
	if (use_info_schema && server_version >= 50110) {
		/* This just doesn't work */
		/* currently this doesn't work - we have to wait for implementation of REFERENTIAL_CONSTRAINTS */
		char buf[10];
		my_i_to_a(buf, sizeof(buf) - 1, importedKeyCascade);
		std::string importedKeyCascadeStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeySetNull);
		std::string importedKeySetNullStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeySetDefault);
		std::string importedKeySetDefaultStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeyRestrict);
		std::string importedKeyRestrictStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeyNoAction);
		std::string importedKeyNoActionStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeyNotDeferrable);
		std::string importedKeyNotDeferrableStr(buf);

		std::string UpdateRuleClause;
		UpdateRuleClause.append("CASE WHEN R.UPDATE_RULE='CASCADE' THEN ").append(importedKeyCascadeStr).
						append(" WHEN R.UPDATE_RULE='SET NULL' THEN ").append(importedKeySetNullStr).
						append(" WHEN R.UPDATE_RULE='SET DEFAULT' THEN ").append(importedKeySetDefaultStr).
						append(" WHEN R.UPDATE_RULE='RESTRICT' THEN ").append(importedKeyRestrictStr).
						append(" WHEN R.UPDATE_RULE='NO ACTION' THEN ").append(importedKeyNoActionStr).
						append(" ELSE ").append(importedKeyNoActionStr).append(" END ");

		std::string DeleteRuleClause;

		DeleteRuleClause.append("CASE WHEN R.DELETE_RULE='CASCADE' THEN ").append(importedKeyCascadeStr).
						append(" WHEN R.DELETE_RULE='SET NULL' THEN ").append(importedKeySetNullStr).
						append(" WHEN R.DELETE_RULE='SET DEFAULT' THEN ").append(importedKeySetDefaultStr).
						append(" WHEN R.DELETE_RULE='RESTRICT' THEN ").append(importedKeyRestrictStr).
						append(" WHEN R.DELETE_RULE='NO ACTION' THEN ").append(importedKeyNoActionStr).
						append(" ELSE ").append(importedKeyNoActionStr).append(" END ");

		std::string OptionalRefConstraintJoinStr(
					"JOIN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS R ON "
					"(R.CONSTRAINT_NAME = B.CONSTRAINT_NAME AND R.TABLE_NAME = B.TABLE_NAME AND R.CONSTRAINT_SCHEMA = B.TABLE_SCHEMA) ");

		std::string query("SELECT \n");
		query.append("A.TABLE_CATALOG AS PKTABLE_CAT, A.REFERENCED_TABLE_SCHEMA AS PKTABLE_SCHEM, A.REFERENCED_TABLE_NAME AS PKTABLE_NAME,\n"
					 "A.REFERENCED_COLUMN_NAME AS PKCOLUMN_NAME, A.TABLE_CATALOG AS FKTABLE_CAT, A.TABLE_SCHEMA AS FKTABLE_SCHEM,\n"
					 "A.TABLE_NAME AS FKTABLE_NAME, A.COLUMN_NAME AS FKCOLUMN_NAME, A.ORDINAL_POSITION AS KEY_SEQ,");
		query.append(UpdateRuleClause);
		query.append(" AS UPDATE_RULE,");
		query.append(DeleteRuleClause);
		query.append(" AS DELETE_RULE, A.CONSTRAINT_NAME AS FK_NAME,"
					 "(SELECT CONSTRAINT_NAME FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS WHERE TABLE_SCHEMA = REFERENCED_TABLE_SCHEMA AND"
					 " TABLE_NAME = A.REFERENCED_TABLE_NAME AND CONSTRAINT_TYPE IN ('UNIQUE','PRIMARY KEY') LIMIT 1) AS PK_NAME,");
		query.append(importedKeyNotDeferrableStr);
		query.append(" AS DEFERRABILITY \n FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE A JOIN  INFORMATION_SCHEMA.TABLE_CONSTRAINTS B\n"
					 "USING (TABLE_SCHEMA, TABLE_NAME, CONSTRAINT_NAME)\n");
		query.append(OptionalRefConstraintJoinStr);
		query.append("\nWHERE B.CONSTRAINT_TYPE = 'FOREIGN KEY' AND A.REFERENCED_TABLE_SCHEMA LIKE ? AND A.REFERENCED_TABLE_NAME=?\n"
				 		"ORDER BY A.TABLE_SCHEMA, A.TABLE_NAME, A.ORDINAL_POSITION");

		std::auto_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));
		stmt->setString(1, schema);
		stmt->setString(2, table);
		std::auto_ptr<sql::ResultSet> rs(stmt->executeQuery());

		while (rs->next()) {
			MySQL_ArtResultSet::row_t rs_data_row;

			rs_data_row.push_back(rs->getString(1));	// PKTABLE_CAT
			rs_data_row.push_back(rs->getString(2));	// PKTABLE_SCHEMA
			rs_data_row.push_back(rs->getString(3));	// PKTABLE_NAME
			rs_data_row.push_back(rs->getString(4));	// PKCOLUMN_NAME
			rs_data_row.push_back(rs->getString(5));	// FKTABLE_CAT
			rs_data_row.push_back(rs->getString(6));	// FKTABLE_SCHEMA
			rs_data_row.push_back(rs->getString(7));	// FKTABLE_NAME
			rs_data_row.push_back(rs->getString(8));	// FKCOLUMN_NAME
			rs_data_row.push_back(rs->getString(9));	// KEY_SEQ
			rs_data_row.push_back(rs->getString(10));	// UPDATE_RULE
			rs_data_row.push_back(rs->getString(11));	// DELETE_RULE
			rs_data_row.push_back(rs->getString(12));	// FK_NAME
			rs_data_row.push_back(rs->getString(13));	// PK_NAME
			rs_data_row.push_back(rs->getString(14));	// DEFERRABILITY

			rs_data->push_back(rs_data_row);
		}
		MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
		// If there is no exception we can release otherwise on function exit memory will be freed
		rs_data.release();
		return ret;
	} else {
		throw sql::MethodNotImplementedException("MySQL_ConnectionMetaData::getExportedKeys");
	}
	return NULL; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getExtraNameCharacters() -I- */
const std::string&
MySQL_ConnectionMetaData::getExtraNameCharacters()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getExtraNameCharacters");
	static const std::string extra("#@");
	return extra;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getIdentifierQuoteString() -I- */
const std::string&
MySQL_ConnectionMetaData::getIdentifierQuoteString()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getIdentifierQuoteString");
	static const std::string empty(" "), tick("`"), quote("\"");

	if (server_version >= 32306) {
		/* Ask the server for sql_mode and decide for a tick or a quote */
		std::string sql_mode(connection->getSessionVariable("SQL_MODE"));

		if (sql_mode.find("ANSI_QUOTES") != std::string::npos) {
			return quote;
		} else {
			return tick;
		}
	}
	return empty;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::parseImportedKeys() -I- */
bool
MySQL_ConnectionMetaData::parseImportedKeys(
		const std::string& def,
		std::string & constraint_name,
		std::map< std::string, std::string > & keywords_names,
		std::map< std::string, std::list< std::string > > & referenced_fields,
		std::map< std::string, int > & update_cascade
	)
{
	CPP_ENTER("MySQL_ConnectionMetaData::parseImportedKeys");
	size_t idx, pos;

	/* check if line contains 'CONSTRAINT' */
	idx = def.find("CONSTRAINT");
	if (idx == std::string::npos) {
		return false;
	}
	pos = idx + sizeof("CONSTRAINT") - 1;

	std::string cQuote(getIdentifierQuoteString());

	{
		{
			size_t end_pos;
			if (cQuote.size()) {
				while (def[pos] != cQuote[0]) ++pos;
				end_pos = ++pos;
				while (def[end_pos] != cQuote[0] && def[end_pos - 1] != '\\') ++end_pos;
			} else {
				while (def[pos] == ' ') ++pos;
				end_pos = ++pos;
				while (def[end_pos] != ' ') ++end_pos;
			}
			constraint_name = def.substr(pos, end_pos - pos);
			pos = end_pos + 1;
		}

		std::list< std::string > keywords;
		keywords.push_back("FOREIGN KEY");
		keywords.push_back("REFERENCES");
		std::list< std::string >::const_iterator keywords_it = keywords.begin();

		for (; keywords_it != keywords.end(); ++keywords_it) {
			idx = def.find(*keywords_it, pos);
			pos = idx + keywords_it->size();

			while (def[pos] == ' ') ++pos;
			// Here comes optional constraint name
			if (def[pos] != '(') {
				if (cQuote.size()) {
					size_t end_pos = ++pos;
					while (def[end_pos] != cQuote[0] && def[end_pos - 1] != '\\') ++end_pos;
					keywords_names[*keywords_it] = def.substr(pos, end_pos - pos);
					pos = end_pos + 1;
				} else {
					size_t end_pos = pos;
					while (def[end_pos] != ' ' && def[end_pos] != '(') ++end_pos;
					keywords_names[*keywords_it] = def.substr(pos, end_pos - pos - 1);
					pos = end_pos + 1;
					// Now find the opening bracket
				}
				// skip to the open bracket
				while (def[pos] != '(') ++pos;
			}
			++pos; // skip the bracket

			// Here come the referenced fields
			{
				size_t end_bracket;
				end_bracket = def.find(")", pos);
				size_t comma_pos;
				do {
					// Look within a range
					// , end_bracket - pos
					comma_pos = def.find("," , pos);
					// there is something in the implementation of find(",", pos, end_bracket - pos) - so I have to emulate it
					if (comma_pos >= end_bracket || comma_pos == std::string::npos) {
						referenced_fields[*keywords_it].push_back(def.substr(pos + cQuote.size(), end_bracket - pos - cQuote.size() * 2));
						break;
					} else {
						referenced_fields[*keywords_it].push_back(def.substr(pos + cQuote.size(), comma_pos - pos - cQuote.size() * 2));
						pos = comma_pos + 1; // skip the comma
						while (def[pos] == ' ') ++pos; // skip whitespace after the comma
					}
				} while (1);
				pos = end_bracket + 1;
			}
		}
	}

	// Check optional (UPDATE | DELETE) CASCADE
	{
		std::list< std::string > keywords;
		keywords.push_back("ON DELETE");
		keywords.push_back("ON UPDATE");
		std::list< std::string >::const_iterator keywords_it = keywords.begin();

		for (; keywords_it != keywords.end(); ++keywords_it) {
			int action = importedKeyNoAction;
			idx = def.find(*keywords_it, pos);
			if (idx != std::string::npos) {
				pos = idx + keywords_it->size();
				while (def[pos] == ' ') ++pos;
				if (def[pos] == 'R') { 		// RESTRICT
					action = importedKeyRestrict;
					pos += sizeof("RESTRICT");
				} else if (def[pos] == 'C') { // CASCADE
					action = importedKeyCascade;
					pos += sizeof("CASCADE");
				} else if (def[pos] == 'S') { // SET NULL
					action = importedKeySetNull;
					pos += sizeof("SET NULL");
				} else if (def[pos] == 'N') { // NO ACTION
					action = importedKeyNoAction;
					pos += sizeof("NO ACTION");
				}
			}
			update_cascade[*keywords_it] = action;
		}
	}
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getImportedKeys() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getImportedKeys(const std::string& catalog, const std::string& schema, const std::string& table)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getImportedKeys");
	CPP_INFO_FMT("catalog=%s schema=%s table=%s", catalog.c_str(), schema.c_str(), table.c_str());
	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());


	std::list<std::string> rs_field_data;
	rs_field_data.push_back("PKTABLE_CAT");
	rs_field_data.push_back("PKTABLE_SCHEM");
	rs_field_data.push_back("PKTABLE_NAME");
	rs_field_data.push_back("PKCOLUMN_NAME");
	rs_field_data.push_back("FKTABLE_CAT");
	rs_field_data.push_back("FKTABLE_SCHEM");
	rs_field_data.push_back("FKTABLE_NAME");
	rs_field_data.push_back("FKCOLUMN_NAME");
	rs_field_data.push_back("KEY_SEQ");
	rs_field_data.push_back("UPDATE_RULE");
	rs_field_data.push_back("DELETE_RULE");
	rs_field_data.push_back("FK_NAME");
	rs_field_data.push_back("PK_NAME");
	rs_field_data.push_back("DEFERRABILITY");

	if (use_info_schema && server_version >= 50116) {
		/* This just doesn't work */
		/* currently this doesn't work - we have to wait for implementation of REFERENTIAL_CONSTRAINTS */
		char buf[10];
		my_i_to_a(buf, sizeof(buf) - 1, importedKeyCascade);
		std::string importedKeyCascadeStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeySetNull);
		std::string importedKeySetNullStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeySetDefault);
		std::string importedKeySetDefaultStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeyRestrict);
		std::string importedKeyRestrictStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeyNoAction);
		std::string importedKeyNoActionStr(buf);
		my_i_to_a(buf, sizeof(buf) - 1, importedKeyNotDeferrable);
		std::string importedKeyNotDeferrableStr(buf);

		std::string UpdateRuleClause;
		UpdateRuleClause.append("CASE WHEN R.UPDATE_RULE='CASCADE' THEN ").append(importedKeyCascadeStr).
						append(" WHEN R.UPDATE_RULE='SET NULL' THEN ").append(importedKeySetNullStr).
						append(" WHEN R.UPDATE_RULE='SET DEFAULT' THEN ").append(importedKeySetDefaultStr).
						append(" WHEN R.UPDATE_RULE='RESTRICT' THEN ").append(importedKeyRestrictStr).
						append(" WHEN R.UPDATE_RULE='NO ACTION' THEN ").append(importedKeyNoActionStr).
						append(" ELSE ").append(importedKeyNoActionStr).append(" END ");

		std::string DeleteRuleClause;

		DeleteRuleClause.append("CASE WHEN R.DELETE_RULE='CASCADE' THEN ").append(importedKeyCascadeStr).
						append(" WHEN R.DELETE_RULE='SET NULL' THEN ").append(importedKeySetNullStr).
						append(" WHEN R.DELETE_RULE='SET DEFAULT' THEN ").append(importedKeySetDefaultStr).
						append(" WHEN R.DELETE_RULE='RESTRICT' THEN ").append(importedKeyRestrictStr).
						append(" WHEN R.DELETE_RULE='NO ACTION' THEN ").append(importedKeyNoActionStr).
						append(" ELSE ").append(importedKeyNoActionStr).append(" END ");

		std::string OptionalRefConstraintJoinStr(
					"JOIN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS R ON "
					"(R.CONSTRAINT_NAME = B.CONSTRAINT_NAME AND R.TABLE_NAME = B.TABLE_NAME AND R.CONSTRAINT_SCHEMA = B.TABLE_SCHEMA) ");

		std::string query("SELECT \n"
					"A.TABLE_CATALOG AS PKTABLE_CAT, A.REFERENCED_TABLE_SCHEMA AS PKTABLE_SCHEM, A.REFERENCED_TABLE_NAME AS PKTABLE_NAME,\n"
					"A.REFERENCED_COLUMN_NAME AS PKCOLUMN_NAME, A.TABLE_CATALOG AS FKTABLE_CAT, A.TABLE_SCHEMA AS FKTABLE_SCHEM,\n"
					"A.TABLE_NAME AS FKTABLE_NAME, A.COLUMN_NAME AS FKCOLUMN_NAME, A.ORDINAL_POSITION AS KEY_SEQ,\n");
		query.append(UpdateRuleClause);
		query.append(" AS UPDATE_RULE,\n");
		query.append(DeleteRuleClause);
		query.append(" AS DELETE_RULE, A.CONSTRAINT_NAME AS FK_NAME,\n"
					"(SELECT TC.CONSTRAINT_NAME FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS AS TC\n"
					"WHERE TABLE_SCHEMA = A.REFERENCED_TABLE_SCHEMA AND TABLE_NAME = A.REFERENCED_TABLE_NAME \n"
					"AND CONSTRAINT_TYPE IN ('UNIQUE','PRIMARY KEY') LIMIT 1) AS PK_NAME,\n");
		query.append(importedKeyNotDeferrableStr);
		query.append(" AS DEFERRABILITY FROM "
					"INFORMATION_SCHEMA.KEY_COLUMN_USAGE A JOIN INFORMATION_SCHEMA.TABLE_CONSTRAINTS B \n"
					"USING(CONSTRAINT_NAME, TABLE_NAME) \n");
		query.append(OptionalRefConstraintJoinStr);
		query.append("WHERE B.CONSTRAINT_TYPE = 'FOREIGN KEY' AND A.TABLE_SCHEMA LIKE ?  AND A.TABLE_NAME=?  AND A.REFERENCED_TABLE_SCHEMA \n"
					"IS NOT NULL\nORDER BY A.REFERENCED_TABLE_SCHEMA, A.REFERENCED_TABLE_NAME, A.ORDINAL_POSITION");

		std::auto_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));
		stmt->setString(1, schema);
		stmt->setString(2, table);
		std::auto_ptr<sql::ResultSet> rs(stmt->executeQuery());

		while (rs->next()) {
			MySQL_ArtResultSet::row_t rs_data_row;

			rs_data_row.push_back(rs->getString(1));	// PKTABLE_CAT
			rs_data_row.push_back(rs->getString(2));	// PKTABLE_SCHEMA
			rs_data_row.push_back(rs->getString(3));	// PKTABLE_NAME
			rs_data_row.push_back(rs->getString(4));	// PKCOLUMN_NAME
			rs_data_row.push_back(rs->getString(5));	// FKTABLE_CAT
			rs_data_row.push_back(rs->getString(6));	// FKTABLE_SCHEMA
			rs_data_row.push_back(rs->getString(7));	// FKTABLE_NAME
			rs_data_row.push_back(rs->getString(8));	// FKCOLUMN_NAME
			rs_data_row.push_back(rs->getString(9));	// KEY_SEQ
			rs_data_row.push_back(rs->getString(10));	// UPDATE_RULE
			rs_data_row.push_back(rs->getString(11));	// DELETE_RULE
			rs_data_row.push_back(rs->getString(12));	// FK_NAME
			rs_data_row.push_back(rs->getString(13));	// PK_NAME
			rs_data_row.push_back(rs->getString(14));	// DEFERRABILITY

			rs_data->push_back(rs_data_row);
		}
	} else {
		std::string query("SHOW CREATE TABLE `");
		query.append(schema).append("`.`").append(table).append("`");

		std::auto_ptr<sql::Statement> stmt(connection->createStatement());
		std::auto_ptr<sql::ResultSet> rs(NULL);
		try {
			rs.reset(stmt->executeQuery(query));
		} catch (SQLException &) {
			// schema and/or table don't exist, return empty set
			// just do nothing and the `if` will be skipped
		}
		if (rs.get() && rs->next()) {
			std::string create_query(rs->getString(2));
			unsigned int kSequence = 0;

			std::string constraint_name;
			std::map< std::string, std::string > keywords_names;
			std::map< std::string, std::list< std::string > > referenced_fields;
			std::map< std::string, int > update_delete_action;

			if (parseImportedKeys(create_query, constraint_name, keywords_names, referenced_fields, update_delete_action)) {
				std::list< std::string >::const_iterator it_references = referenced_fields["REFERENCES"].begin();
				std::list< std::string >::const_iterator it_references_end = referenced_fields["REFERENCES"].end();
				std::list< std::string >::const_iterator it_foreignkey = referenced_fields["FOREIGN KEY"].begin();
				std::list< std::string >::const_iterator it_foreignkey_end = referenced_fields["FOREIGN KEY"].end();
				for (
						;
						it_references != it_references_end && it_foreignkey != it_foreignkey_end;
						++it_references, ++it_foreignkey
					)
				{
					MySQL_ArtResultSet::row_t rs_data_row;

					rs_data_row.push_back("def");						// PK_TABLE_CAT
					rs_data_row.push_back(schema);						// PKTABLE_SCHEM
					rs_data_row.push_back(keywords_names["REFERENCES"]);// PKTABLE_NAME

					// ToDo: Extracting just the first column
					rs_data_row.push_back(*it_references);				// PK_COLUMN_NAME

					rs_data_row.push_back("");							// FKTABLE_CAT

					// ToDo: Is this correct? referencing the same schema. Maybe fully referenced name can appear, need to parse it too
					rs_data_row.push_back(schema);						// FKTABLE_SCHEM
					rs_data_row.push_back(table);						// FKTABLE_NAME

					// ToDo: Extracting just the first column
					rs_data_row.push_back(*it_foreignkey);			// FKCOLUMN_NAME
					rs_data_row.push_back((int64_t) ++kSequence);	// KEY_SEQ


					rs_data_row.push_back((int64_t) update_delete_action["ON UPDATE"]);	// UPDATE_RULE

					rs_data_row.push_back((int64_t) update_delete_action["ON DELETE"]);	// DELETE_RULE

					rs_data_row.push_back(constraint_name);		// FK_NAME
					// ToDo: Should it really be PRIMARY?
					rs_data_row.push_back("");					// PK_NAME
					rs_data_row.push_back((int64_t) importedKeyNotDeferrable);	// DEFERRABILITY

					rs_data->push_back(rs_data_row);
				}
			}
		}
	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getIndexInfo() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getIndexInfo(const std::string& /*catalog*/, const std::string& schema,
										const std::string& table, bool unique, bool /* approximate */)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getIndexInfo");
	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());


	std::list<std::string> rs_field_data;
	rs_field_data.push_back("TABLE_CAT");
	rs_field_data.push_back("TABLE_SCHEM");
	rs_field_data.push_back("TABLE_NAME");
	rs_field_data.push_back("NON_UNIQUE");
	rs_field_data.push_back("INDEX_QUALIFIER");
	rs_field_data.push_back("INDEX_NAME");
	rs_field_data.push_back("TYPE");
	rs_field_data.push_back("ORDINAL_POSITION");
	rs_field_data.push_back("COLUMN_NAME");
	rs_field_data.push_back("ASC_OR_DESC");
	rs_field_data.push_back("CARDINALITY");
	rs_field_data.push_back("PAGES");
	rs_field_data.push_back("FILTER_CONDITION");

	char indexOther[5];

	snprintf(indexOther, sizeof(indexOther), "%d", DatabaseMetaData::tableIndexOther);

	if (use_info_schema && server_version > 50020) {
		char indexHash[5];
		snprintf(indexHash, sizeof(indexHash), "%d", DatabaseMetaData::tableIndexHashed);

		std::string query("SELECT TABLE_CATALOG AS TABLE_CAT, TABLE_SCHEMA AS TABLE_SCHEM, TABLE_NAME, NON_UNIQUE, "
						 "TABLE_SCHEMA AS INDEX_QUALIFIER, INDEX_NAME, CASE WHEN INDEX_TYPE='HASH' THEN ");
		query.append(indexHash).append(" ELSE ").append(indexOther);
		query.append(" END AS TYPE, SEQ_IN_INDEX AS ORDINAL_POSITION, COLUMN_NAME, COLLATION AS ASC_OR_DESC, CARDINALITY,"
					"NULL AS PAGES, NULL AS FILTER_CONDITION "
					"FROM INFORMATION_SCHEMA.STATISTICS WHERE TABLE_SCHEMA LIKE ? AND TABLE_NAME LIKE ?\n");

		if (unique) {
			query.append(" AND NON_UNIQUE=0");
		}
		query.append(" ORDER BY NON_UNIQUE, TYPE, INDEX_NAME, ORDINAL_POSITION");

		std::auto_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));
		stmt->setString(1, schema);
		stmt->setString(2, table);

		std::auto_ptr<sql::ResultSet> rs(stmt->executeQuery());

		while (rs->next()) {
			MySQL_ArtResultSet::row_t rs_data_row;

			rs_data_row.push_back(rs->getString(1));	// TABLE_CAT
			rs_data_row.push_back(rs->getString(2));	// TABLE_SCHEM
			rs_data_row.push_back(rs->getString(3));	// TABLE_NAME
			rs_data_row.push_back(rs->getString(4));	// NON_UNIQUE
			rs_data_row.push_back(rs->getString(5));	// INDEX_QUALIFIER
			rs_data_row.push_back(rs->getString(6));	// INDEX_NAME
			rs_data_row.push_back(rs->getString(7));	// TYPE
			rs_data_row.push_back(rs->getString(8));	// ORDINAL_POSITION
			rs_data_row.push_back(rs->getString(9));	// COLUMN_NAME
			rs_data_row.push_back(rs->getString(10));	// ASC_OR_DESC
			rs_data_row.push_back(rs->getString(11));	// CARDINALITY
			rs_data_row.push_back(rs->getString(12));	// PAGES
			rs_data_row.push_back(rs->getString(13));	// FILTER_CONDITION

			rs_data->push_back(rs_data_row);
		}
	} else {
		std::string query("SHOW INDEX FROM `");
		query.append(schema).append("`.`").append(table).append("`");

		std::auto_ptr<sql::Statement> stmt(connection->createStatement());

		std::auto_ptr<sql::ResultSet> rs(NULL);
		try {
			rs.reset(stmt->executeQuery(query));
		} catch (SQLException &) {
			// schema and/or table doesn't exist. return empty set
			// do nothing here
		}

		while (rs.get() && rs->next()) {
			MySQL_ArtResultSet::row_t rs_data_row;

			rs_data_row.push_back("def");							// TABLE_CAT
			rs_data_row.push_back(schema);							// TABLE_SCHEM
			rs_data_row.push_back(rs->getString("Table"));			// TABLE_NAME
			rs_data_row.push_back(atoi(rs->getString("Non_unique").c_str())? true:false);	// NON_UNIQUE
			rs_data_row.push_back(schema);							// INDEX_QUALIFIER
			rs_data_row.push_back(rs->getString("Key_name"));		// INDEX_NAME
			rs_data_row.push_back((const char *) indexOther);				// TYPE
			rs_data_row.push_back(rs->getString("Seq_in_index"));	// ORDINAL_POSITION
			rs_data_row.push_back(rs->getString("Column_name"));	// COLUMN_NAME
			rs_data_row.push_back(rs->getString("Collation"));		// ASC_OR_DESC
			rs_data_row.push_back(rs->getString("Cardinality"));	// CARDINALITY
			rs_data_row.push_back("0");								// PAGES
			rs_data_row.push_back("");								// FILTER_CONDITION

			rs_data->push_back(rs_data_row);
		}
	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getCDBCMajorVersion() -I- */
unsigned int
MySQL_ConnectionMetaData::getCDBCMajorVersion()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getCDBCMajorVersion");
	return 3;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getCDBCMinorVersion() -I- */
unsigned int
MySQL_ConnectionMetaData::getCDBCMinorVersion()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getCDBCMinorVersion");
	return 0;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxBinaryLiteralLength() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxBinaryLiteralLength()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxBinaryLiteralLength");
	return 16777208L;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxCatalogNameLength() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxCatalogNameLength()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxCatalogNameLength");
	return 32;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxCharLiteralLength() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxCharLiteralLength()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxCharLiteralLength");
	return 16777208;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxColumnNameLength() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxColumnNameLength()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxColumnNameLength");
	return 64;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxColumnsInGroupBy() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxColumnsInGroupBy()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxColumnsInGroupBy");
	return 64;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxColumnsInIndex() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxColumnsInIndex()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxColumnsInIndex");
	return 16;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxColumnsInOrderBy() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxColumnsInOrderBy()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxColumnsInOrderBy");
	return 64;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxColumnsInSelect() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxColumnsInSelect()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxColumnsInSelect");
	return 256;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxColumnsInTable() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxColumnsInTable()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxColumnsInTable");
	return 512;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxConnections() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxConnections()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxConnections");
	return atoi(connection->getSessionVariable("max_connections").c_str());
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxCursorNameLength() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxCursorNameLength()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxCursorNameLength");
	return 64;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxIndexLength() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxIndexLength()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxIndexLength");
	return 256;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxProcedureNameLength() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxProcedureNameLength()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxProcedureNameLength");
	return 64;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxRowSize() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxRowSize()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxRowSize");
	return 2147483647L - 8; // Max buffer size - HEADER
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxSchemaNameLength() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxSchemaNameLength()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxSchemaNameLength");
	return 64;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxStatementLength() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxStatementLength()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxStatementLength");
	return atoi(connection->getSessionVariable("max_allowed_packet").c_str()) - 4;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxStatements() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxStatements()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxStatements");
	return 0;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxTableNameLength() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxTableNameLength()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxTableNameLength");
	return 64;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxTablesInSelect() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxTablesInSelect()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxTablesInSelect");
	return 256;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getMaxUserNameLength() -I- */
unsigned int
MySQL_ConnectionMetaData::getMaxUserNameLength()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getMaxUserNameLength");
	return 16;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getNumericFunctions() -I- */
const std::string &
MySQL_ConnectionMetaData::getNumericFunctions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getNumericFunctions");
	static const std::string funcs("ABS,ACOS,ASIN,ATAN,ATAN2,BIT_COUNT,CEILING,COS,"
							"COT,DEGREES,EXP,FLOOR,LOG,LOG10,MAX,MIN,MOD,PI,POW,"
							"POWER,RADIANS,RAND,ROUND,SIN,SQRT,TAN,TRUNCATE");
	return funcs;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getPrimaryKeys() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getPrimaryKeys(const std::string& catalog, const std::string& schema, const std::string& table)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getPrimaryKeys");
	CPP_INFO_FMT("catalog=%s schema=%s table=%s", catalog.c_str(), schema.c_str(), table.c_str());

	std::list<std::string> rs_field_data;
	rs_field_data.push_back("TABLE_CAT");
	rs_field_data.push_back("TABLE_SCHEM");
	rs_field_data.push_back("TABLE_NAME");
	rs_field_data.push_back("COLUMN_NAME");
	rs_field_data.push_back("KEY_SEQ");
	rs_field_data.push_back("PK_NAME");


	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	/* Bind Problems with 49999, check later why */
	if (use_info_schema && server_version > 49999) {
		const std::string query("SELECT TABLE_CATALOG AS TABLE_CAT, TABLE_SCHEMA AS TABLE_SCHEM, TABLE_NAME, "
					"COLUMN_NAME, SEQ_IN_INDEX AS KEY_SEQ, INDEX_NAME AS PK_NAME FROM INFORMATION_SCHEMA.STATISTICS "
					"WHERE TABLE_SCHEMA LIKE ? AND TABLE_NAME LIKE ? AND INDEX_NAME='PRIMARY' "
					"ORDER BY TABLE_SCHEMA, TABLE_NAME, INDEX_NAME, SEQ_IN_INDEX");

		std::auto_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));
		stmt->setString(1, schema);
		stmt->setString(2, table);

		std::auto_ptr<sql::ResultSet> rs(stmt->executeQuery());

		while (rs->next()) {
			MySQL_ArtResultSet::row_t rs_data_row;

			rs_data_row.push_back(rs->getString(1));	// TABLE_CAT
			rs_data_row.push_back(rs->getString(2));	// TABLE_SCHEM
			rs_data_row.push_back(rs->getString(3));	// TABLE_NAME
			rs_data_row.push_back(rs->getString(4));	// COLUMN_NAME
			rs_data_row.push_back(rs->getString(5));	// KEY_SEQ
			rs_data_row.push_back(rs->getString(6));	// PK_NAME

			rs_data->push_back(rs_data_row);
		}
	} else {
		std::string query("SHOW KEYS FROM `");
		query.append(schema).append("`.`").append(table).append("`");

		std::auto_ptr<sql::Statement> stmt(connection->createStatement());
		std::auto_ptr<sql::ResultSet> rs(NULL);
		try {
			rs.reset(stmt->executeQuery(query));
		} catch (SQLException &) {
			// probably schema and/or table doesn't exist. return empty set
			// do nothing here
		}

		while (rs.get() && rs->next()) {
			std::string key_name = rs->getString("Key_name");
			if (!key_name.compare("PRIMARY") || !key_name.compare("PRI")) {
				MySQL_ArtResultSet::row_t rs_data_row;

				rs_data_row.push_back("def");							// TABLE_CAT
				rs_data_row.push_back(schema);							// TABLE_SCHEM
				rs_data_row.push_back(rs->getString(1));				// TABLE_NAME
				rs_data_row.push_back(rs->getString("Column_name"));	// COLUMN_NAME
				rs_data_row.push_back(rs->getString("Seq_in_index"));	// KEY_SEQ
				rs_data_row.push_back(key_name);						// PK_NAME

				rs_data->push_back(rs_data_row);
			}
		}
	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getProcedures() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getProcedures(const std::string& /*catalog*/, const std::string& schemaPattern, const std::string& procedureNamePattern)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getProcedures");

	std::list<std::string> rs_field_data;
	rs_field_data.push_back("PROCEDURE_CAT");
	rs_field_data.push_back("PROCEDURE_SCHEM");
	rs_field_data.push_back("PROCEDURE_NAME");
	rs_field_data.push_back("RESERVERD_1");
	rs_field_data.push_back("RESERVERD_2");
	rs_field_data.push_back("RESERVERD_3");
	rs_field_data.push_back("REMARKS");
	rs_field_data.push_back("PROCEDURE_TYPE");

	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	char procRetNoRes[5];
	my_i_to_a(procRetNoRes, sizeof(procRetNoRes) - 1, procedureNoResult);
	char procRetRes[5];
	my_i_to_a(procRetRes, sizeof(procRetRes) - 1, procedureReturnsResult);
	char procRetUnknown[5];
	my_i_to_a(procRetUnknown, sizeof(procRetUnknown) - 1, procedureResultUnknown);

	if (use_info_schema && server_version > 49999) {
		std::string query("SELECT ROUTINE_CATALOG AS PROCEDURE_CAT, ROUTINE_SCHEMA AS PROCEDURE_SCHEM, "
						"ROUTINE_NAME AS PROCEDURE_NAME, NULL AS RESERVED_1, NULL AS RESERVERD_2, NULL as RESERVED_3,"
						"ROUTINE_COMMENT AS REMARKS, "
						"CASE WHEN ROUTINE_TYPE = 'PROCEDURE' THEN ");
		query.append(procRetNoRes);
		query.append(" WHEN ROUTINE_TYPE='FUNCTION' THEN ");
		query.append(procRetRes);
		query.append(" ELSE ");
		query.append(procRetUnknown);
		query.append(" END AS PROCEDURE_TYPE\nFROM INFORMATION_SCHEMA.ROUTINES\n"
					"WHERE ROUTINE_SCHEMA LIKE ? AND ROUTINE_NAME LIKE ?\n"
					"ORDER BY ROUTINE_SCHEMA, ROUTINE_NAME");

		std::auto_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));
		stmt->setString(1, schemaPattern);
		stmt->setString(2, procedureNamePattern.size() ? procedureNamePattern : "%");

		std::auto_ptr<sql::ResultSet> rs(stmt->executeQuery());
		while (rs->next()) {
			MySQL_ArtResultSet::row_t rs_data_row;

			rs_data_row.push_back(rs->getString(1));	// PROCEDURE_CAT
			rs_data_row.push_back(rs->getString(2));	// PROCEDURE_SCHEM
			rs_data_row.push_back(rs->getString(3));	// PROCEDURE_NAME
			rs_data_row.push_back(rs->getString(4));	// reserved1
			rs_data_row.push_back(rs->getString(5));	// reserved2
			rs_data_row.push_back(rs->getString(6));	// reserved3
			rs_data_row.push_back(rs->getString(7));	// REMARKS
			rs_data_row.push_back(rs->getString(8));	// PROCEDURE_TYPE

			rs_data->push_back(rs_data_row);
		}
	} else if (server_version > 49999) {
		bool got_exception = false;
		do {
			std::string query("SELECT 'def' AS PROCEDURE_CAT, db as PROCEDURE_SCHEM, "
									"name AS PROCEDURE_NAME, NULL as RESERVERD_1, NULL as RESERVERD_2, "
									"NULL AS RESERVERD_3, comment as REMARKS, ");
			query.append("CASE WHEN TYPE=='FUNCTION' THEN ").append(procRetRes).append("\n");
			query.append("WHEN TYPE='PROCEDURE' THEN").append(procRetNoRes).append("ELSE ").append(procRetUnknown);
			query.append("\n END AS PROCEDURE_TYPE\nFROM mysql.proc WHERE name LIKE ? AND db <=> ? ORDER BY name");

			std::auto_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));
			stmt->setString(1, procedureNamePattern);
			stmt->setString(2, schemaPattern);

			std::auto_ptr<sql::ResultSet> rs(NULL);
			try {
				rs.reset(stmt->executeQuery());
			} catch (SQLException & /*e*/) {
				/* We don't have direct access to the mysql.proc, use SHOW */
				got_exception = true;
				break;
			}
			while (rs->next()) {
				MySQL_ArtResultSet::row_t rs_data_row;

				rs_data_row.push_back(rs->getString(1));	// PROCEDURE_CAT
				rs_data_row.push_back(rs->getString(2));	// PROCEDURE_SCHEM
				rs_data_row.push_back(rs->getString(3));	// PROCEDURE_NAME
				rs_data_row.push_back(rs->getString(4));	// reserved1
				rs_data_row.push_back(rs->getString(5));	// reserved2
				rs_data_row.push_back(rs->getString(6));	// reserved3
				rs_data_row.push_back(rs->getString(7));	// REMARKS
				rs_data_row.push_back(rs->getString(8));	// PROCEDURE_TYPE

				rs_data->push_back(rs_data_row);
			}
		} while (0);
		if (got_exception) {
			std::string query("SHOW PROCEDURE STATUS");

			std::auto_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));				

			std::auto_ptr<sql::ResultSet> rs(stmt->executeQuery());
			while (rs->next()) {
				MySQL_ArtResultSet::row_t rs_data_row;

				rs_data_row.push_back("def");				// PROCEDURE_CAT
				rs_data_row.push_back(rs->getString(1));	// PROCEDURE_SCHEM
				rs_data_row.push_back(rs->getString(2));	// PROCEDURE_NAME
				rs_data_row.push_back("");					// reserved1
				rs_data_row.push_back("");					// reserved2
				rs_data_row.push_back("");					// reserved3
				rs_data_row.push_back(rs->getString(8));	// REMARKS
				rs_data_row.push_back(std::string(!rs->getString(3).compare("PROCEDURE")? procRetNoRes:procRetRes));	// PROCEDURE_TYPE

				rs_data->push_back(rs_data_row);
			}
		}

	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getProcedureTerm() -I- */
const std::string &
MySQL_ConnectionMetaData::getProcedureTerm()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getProcedureTerm");
	static const std::string term("procedure");
	return term;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getResultSetHoldability() -I- */
int
MySQL_ConnectionMetaData::getResultSetHoldability()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getResultSetHoldability");
	return sql::ResultSet::HOLD_CURSORS_OVER_COMMIT;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getSchemas() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getSchemas()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getSchemas");
	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());


	std::list<std::string> rs_field_data;
	rs_field_data.push_back("TABLE_SCHEM");
	rs_field_data.push_back("TABLE_CATALOG");

	std::auto_ptr<sql::Statement> stmt(connection->createStatement());
	std::auto_ptr<sql::ResultSet> rs(
		stmt->executeQuery(use_info_schema && server_version > 49999?
				"SELECT SCHEMA_NAME AS TABLE_SCHEM, CATALOG_NAME AS TABLE_CATALOG FROM INFORMATION_SCHEMA.SCHEMATA ORDER BY SCHEMA_NAME":
				"SHOW DATABASES"));

	while (rs->next()) {
		MySQL_ArtResultSet::row_t rs_data_row;

		rs_data_row.push_back(rs->getString(1));
		if (use_info_schema && server_version > 49999) {
			rs_data_row.push_back(rs->getString(2));
		} else {
			rs_data_row.push_back("");
		}

		rs_data->push_back(rs_data_row);
	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getSchemaTerm() -I- */
const std::string&
MySQL_ConnectionMetaData::getSchemaTerm()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getSchemaTerm");
	static const std::string term("database");
	return term;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getSearchStringEscape() -I- */
const std::string&
MySQL_ConnectionMetaData::getSearchStringEscape()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getSearchStringEscape");
	static const std::string escape("\\");
	return escape;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getSQLKeywords() -I- */
const std::string&
MySQL_ConnectionMetaData::getSQLKeywords()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getSQLKeywords");
	static const std::string keywords(
				"ACCESSIBLE, ADD, ALL,"\
				"ALTER, ANALYZE, AND, AS, ASC, ASENSITIVE, BEFORE,"\
				"BETWEEN, BIGINT, BINARY, BLOB, BOTH, BY, CALL,"\
				"CASCADE, CASE, CHANGE, CHAR, CHARACTER, CHECK,"\
				"COLLATE, COLUMN, CONDITION, CONNECTION, CONSTRAINT,"\
				"CONTINUE, CONVERT, CREATE, CROSS, CURRENT_DATE,"\
				"CURRENT_TIME, CURRENT_TIMESTAMP, CURRENT_USER, CURSOR,"\
				"DATABASE, DATABASES, DAY_HOUR, DAY_MICROSECOND,"\
				"DAY_MINUTE, DAY_SECOND, DEC, DECIMAL, DECLARE,"\
				"DEFAULT, DELAYED, DELETE, DESC, DESCRIBE,"\
				"DETERMINISTIC, DISTINCT, DISTINCTROW, DIV, DOUBLE,"\
				"DROP, DUAL, EACH, ELSE, ELSEIF, ENCLOSED,"\
				"ESCAPED, EXISTS, EXIT, EXPLAIN, FALSE, FETCH,"\
				"FLOAT, FLOAT4, FLOAT8, FOR, FORCE, FOREIGN, FROM,"\
				"FULLTEXT, GRANT, GROUP, HAVING, HIGH_PRIORITY,"\
				"HOUR_MICROSECOND, HOUR_MINUTE, HOUR_SECOND, IF,"\
				"IGNORE, IN, INDEX, INFILE, INNER, INOUT,"\
				"INSENSITIVE, INSERT, INT, INT1, INT2, INT3, INT4,"\
				"INT8, INTEGER, INTERVAL, INTO, IS, ITERATE, JOIN,"\
				"KEY, KEYS, KILL, LEADING, LEAVE, LEFT, LIKE,"\
				"LOCALTIMESTAMP, LOCK, LONG, LONGBLOB, LONGTEXT,"\
				"LOOP, LOW_PRIORITY, MATCH, MEDIUMBLOB, MEDIUMINT,"\
				"MEDIUMTEXT, MIDDLEINT, MINUTE_MICROSECOND,"\
				"MINUTE_SECOND, MOD, MODIFIES, NATURAL, NOT,"\
				"NO_WRITE_TO_BINLOG, NULL, NUMERIC, ON, OPTIMIZE,"\
				"OPTION, OPTIONALLY, OR, ORDER, OUT, OUTER,"\
				"OUTFILE, PRECISION, PRIMARY, PROCEDURE, PURGE,"\
				"RANGE, READ, READS, READ_ONLY, READ_WRITE, REAL,"\
				"REFERENCES, REGEXP, RELEASE, RENAME, REPEAT,"\
				"REPLACE, REQUIRE, RESTRICT, RETURN, REVOKE, RIGHT,"\
				"RLIKE, SCHEMA, SCHEMAS, SECOND_MICROSECOND, SELECT,"\
				"SENSITIVE, SEPARATOR, SET, SHOW, SMALLINT, SPATIAL,"\
				"SPECIFIC, SQL, SQLEXCEPTION, SQLSTATE, SQLWARNING,"\
				"SQL_BIG_RESULT, SQL_CALC_FOUND_ROWS, SQL_SMALL_RESULT,"\
				"SSL, STARTING, STRAIGHT_JOIN, TABLE, TERMINATED,"\
				"THEN, TINYBLOB, TINYINT, TINYTEXT, TO, TRAILING,"\
				"TRIGGER, TRUE, UNDO, UNION, UNIQUE, UNLOCK,"\
				"UNSIGNED, UPDATE, USAGE, USE, USING, UTC_DATE,"\
				"UTC_TIME, UTC_TIMESTAMP, VALUES, VARBINARY, VARCHAR,"\
				"VARCHARACTER, VARYING, WHEN, WHERE, WHILE, WITH,"\
				"WRITE, X509, XOR, YEAR_MONTH, ZEROFILL");

	return keywords;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getSQLStateType() -I- */
int
MySQL_ConnectionMetaData::getSQLStateType()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getSQLStateType");
	return sqlStateSQL99;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getStringFunctions() -I- */
const std::string&
MySQL_ConnectionMetaData::getStringFunctions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getStringFunctions");
	static const std::string funcs(
		"ASCII,BIN,BIT_LENGTH,CHAR,CHARACTER_LENGTH,CHAR_LENGTH,CONCAT,"
		"CONCAT_WS,CONV,ELT,EXPORT_SET,FIELD,FIND_IN_SET,HEX,INSERT,"
		"INSTR,LCASE,LEFT,LENGTH,LOAD_FILE,LOCATE,LOCATE,LOWER,LPAD,"
		"LTRIM,MAKE_SET,MATCH,MID,OCT,OCTET_LENGTH,ORD,POSITION,"
		"QUOTE,REPEAT,REPLACE,REVERSE,RIGHT,RPAD,RTRIM,SOUNDEX,"
		"SPACE,STRCMP,SUBSTRING,SUBSTRING,SUBSTRING,SUBSTRING,"
		"SUBSTRING_INDEX,TRIM,UCASE,UPPER");
	return funcs;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getSuperTables() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getSuperTables(const std::string& /*catalog*/, const std::string& /*schemaPattern*/, const std::string& /*tableNamePattern*/)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getSuperTables");
	std::list<std::string> rs_field_data;

	rs_field_data.push_back("TABLE_CAT");
	rs_field_data.push_back("TABLE_SCHEM");
	rs_field_data.push_back("TABLE_NAME");
	rs_field_data.push_back("SUPERTABLE_NAME");

	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getSuperTypes() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getSuperTypes(const std::string& /*catalog*/, const std::string& /*schemaPattern*/, const std::string& /*typeNamePattern*/)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getSuperTypes");
	std::list<std::string> rs_field_data;

	rs_field_data.push_back("TYPE_CAT");
	rs_field_data.push_back("TYPE_SCHEM");
	rs_field_data.push_back("TYPE_NAME");
	rs_field_data.push_back("SUPERTYPE_CAT");
	rs_field_data.push_back("SUPERTYPE_SCHEM");
	rs_field_data.push_back("SUPERTYPE_NAME");

	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getSystemFunctions() -I- */
const std::string&
MySQL_ConnectionMetaData::getSystemFunctions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getSystemFunctions");
	static const std::string funcs(
			"DATABASE,USER,SYSTEM_USER,"
			"SESSION_USER,PASSWORD,ENCRYPT,LAST_INSERT_ID,VERSION");
	return funcs;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getTablePrivileges() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getTablePrivileges(const std::string& catalog, const std::string& schemaPattern, const std::string& tableNamePattern)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getTablePrivileges");

	std::auto_ptr<sql::Statement> stmt(connection->createStatement());
	std::auto_ptr<sql::ResultSet> rs(stmt->executeQuery("SHOW GRANTS"));

	std::list< std::string > aPrivileges, aSchemas, aTables;

	std::string strAllPrivs("ALTER, DELETE, DROP, INDEX, INSERT, LOCK TABLES, SELECT, UPDATE");

	std::string cQuote(getIdentifierQuoteString());

	while (rs->next() ) {
		std::string aGrant = rs->getString(1);
		aGrant = aGrant.replace(0, 6, "");

		size_t pos = aGrant.find("ALL PRIVILEGES");

		if (pos != std::string::npos) {
			aGrant = aGrant.replace(pos, sizeof("ALL PRIVILEGES") - 1, strAllPrivs);
		}

		pos = aGrant.find("ON");

		//ASSERT(pos != std::string::npos);

		aPrivileges.push_back(aGrant.substr(0, pos - 1)); /* -1 for trim */

		aGrant = aGrant.substr(pos + 3); /* remove "ON " */
		if (aGrant[0] != '*') {
			pos = 1;
			do {
				pos = aGrant.find(cQuote, pos);
			} while (pos != std::string::npos && aGrant[pos - 1] == '\\');
			aSchemas.push_back(aGrant.substr(1, pos - 1)); /* From pos 1, without the quoting */
			aGrant = aGrant.replace(0, pos + 1 + 1, ""); // remove the quote, the dot too
		} else {
			aSchemas.push_back("*");
			aGrant = aGrant.replace(0, 1 + 1, ""); // remove the star, the dot too
		}

		/* first char is the quotestring, the last too "`xyz`." Dot is at 5, copy from 1, 5 - 1 - 1 = xyz */

		if (aGrant[0] != '*') {
			// What if the names are not quoted. They should be, no?
			int idx = 1;
			pos = idx;
			do {
				pos = aGrant.find(cQuote, pos);
			} while (pos != std::string::npos && aGrant[pos - 1] == '\\');
			aTables.push_back(aGrant.substr(1, pos - 1));
		} else {
			aTables.push_back("*");
		}
		/*
		  `aaa`.`xyz`  - jump over the dot and the quote
		  . = 5
		  ` = 6
		  x = 7 = idx
		  ` = 10
		  ` - x = 10 - 7 = 3 -> xyz
		*/
	}
	std::list<std::string> rs_field_data;
	rs_field_data.push_back("TABLE_CAT");
	rs_field_data.push_back("TABLE_SCHEM");
	rs_field_data.push_back("TABLE_NAME");
	rs_field_data.push_back("GRANTOR");
	rs_field_data.push_back("GRANTEE");
	rs_field_data.push_back("PRIVILEGE");
	rs_field_data.push_back("IS_GRANTABLE");

	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());


	std::list< std::string > tableTypes;
	tableTypes.push_back(std::string("TABLE"));

	std::auto_ptr<sql::ResultSet> tables(getTables(catalog, schemaPattern, tableNamePattern, tableTypes));
	std::string schema, table;
	while (tables->next()) {
		schema = tables->getString(2);
		table = tables->getString(3);
		std::list<std::string>::const_iterator it_priv, it_schemas, it_tables;
		it_priv = aPrivileges.begin();
		it_schemas = aSchemas.begin();
		it_tables = aTables.begin();

		for (; it_priv != aPrivileges.end(); ++it_priv, ++it_schemas, ++it_tables) {
			/* skip usage */
			if (it_priv->compare("USAGE") && matchTable(*it_schemas, *it_tables, schema, table)) {
				size_t pos, idx;
				pos = 0;
				do {
					while ((*it_priv)[pos] == ' ') ++pos; // Eat the whitespace

					idx = it_priv->find(",", pos);
					std::string privToken;
					// check for std::string::npos
					if (idx != std::string::npos) {
						privToken = it_priv->substr(pos, idx - pos);
						pos = idx + 1; /* skip ',' */
					} else {
						privToken = it_priv->substr(pos, it_priv->length() - pos);
					}
					// ToDo: Why?
					if (privToken.find_first_of('/') == std::string::npos) {
						MySQL_ArtResultSet::row_t rs_data_row;

						rs_data_row.push_back("def");			// TABLE_CAT
						rs_data_row.push_back(schema);			// TABLE_SCHEM
						rs_data_row.push_back(table);			// TABLE_NAME
						rs_data_row.push_back("");				// GRANTOR
						rs_data_row.push_back(getUserName());	// GRANTEE
						rs_data_row.push_back(privToken);		// PRIVILEGE
						rs_data_row.push_back("");				// IS_GRANTABLE - ToDo maybe here WITH GRANT OPTION??

						rs_data->push_back(rs_data_row);
					}
				} while (idx != std::string::npos);
				break;
			}
		}
	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getTables() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getTables(const std::string& /* catalog */, const std::string& schemaPattern,
									const std::string& tableNamePattern, std::list<std::string> &types)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getTables");
	CPP_INFO_FMT("schemaPattern=%s tablePattern=%s", schemaPattern.c_str(), tableNamePattern.c_str());
	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	std::list<std::string> rs_field_data;
	rs_field_data.push_back("TABLE_CAT");
	rs_field_data.push_back("TABLE_SCHEM");
	rs_field_data.push_back("TABLE_NAME");
	rs_field_data.push_back("TABLE_TYPE");
	rs_field_data.push_back("REMARKS");

	/* Bind Problems with 49999, check later why */
	if (use_info_schema && server_version > 49999) {
		const std::string query("SELECT TABLE_CATALOG AS TABLE_CAT, TABLE_SCHEMA AS TABLE_SCHEM, TABLE_NAME,"
							"IF(STRCMP(TABLE_TYPE,'BASE TABLE'), TABLE_TYPE, 'TABLE') AS TABLE_TYPE, TABLE_COMMENT AS REMARKS\n"
							"FROM INFORMATION_SCHEMA.TABLES\nWHERE TABLE_SCHEMA  LIKE ? AND TABLE_NAME LIKE ?\n"
							"ORDER BY TABLE_TYPE, TABLE_SCHEMA, TABLE_NAME");

		std::auto_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));
		stmt->setString(1, schemaPattern);
		stmt->setString(2, tableNamePattern);

		std::auto_ptr<sql::ResultSet> rs(stmt->executeQuery());

		while (rs->next()) {
			std::list<std::string>::const_iterator it = types.begin();
			for (; it != types.end(); ++it) {
				if (*it == rs->getString(4)) {
					MySQL_ArtResultSet::row_t rs_data_row;

					rs_data_row.push_back(rs->getString(1)); // TABLE_CAT
					rs_data_row.push_back(rs->getString(2)); // TABLE_SCHEM
					rs_data_row.push_back(rs->getString(3)); // TABLE_NAME
					rs_data_row.push_back(rs->getString(4)); // TABLE_TYPE
					rs_data_row.push_back(rs->getString(5)); // REMARKS

					rs_data->push_back(rs_data_row);
					break;
				}
			}
		}
	} else {
		std::string query1("SHOW DATABASES LIKE '");
		query1.append(schemaPattern).append("'");

		std::auto_ptr<sql::Statement> stmt1(connection->createStatement());
		std::auto_ptr<sql::ResultSet> rs1(stmt1->executeQuery(query1));
		while (rs1->next()) {
			std::auto_ptr<sql::Statement> stmt2(connection->createStatement());
			std::string current_schema(rs1->getString(1));
			std::string query2("SHOW TABLES FROM `");
			query2.append(current_schema).append("` LIKE '").append(tableNamePattern).append("'");

			std::auto_ptr<sql::ResultSet> rs2(stmt2->executeQuery(query2));

			while (rs2->next()) {
				std::list< std::string >::const_iterator it = types.begin();
				for (; it != types.end(); ++it) {
					/* < 49999 knows only TABLE, no VIEWS */
					/* TODO: Optimize this everytime checking, put it outside of the loop */
					if (!it->compare("TABLE")) {
						MySQL_ArtResultSet::row_t rs_data_row;

						CPP_INFO_FMT("[][%s][%s][TABLE][]", current_schema.c_str(), rs2->getString(1).c_str());
						rs_data_row.push_back("def");				// TABLE_CAT
						rs_data_row.push_back(current_schema);		// TABLE_SCHEM
						rs_data_row.push_back(rs2->getString(1));	// TABLE_NAME
						rs_data_row.push_back("TABLE");				// TABLE_TYPE
						rs_data_row.push_back("");					// REMARKS

						rs_data->push_back(rs_data_row);
						break;
					}
				}
			}
		}
	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getTableTypes() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getTableTypes()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getTableTypes");
	static const char * const table_types[] = {"TABLE", "VIEW", "LOCAL TEMPORARY"};
	static unsigned int requiredVersion[] = {32200, 50000, 32200};

	std::list<std::string> rs_field_data;
	rs_field_data.push_back("TABLE_TYPE");

	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	for (unsigned int i = 0; i < 3; ++i) {
		if (server_version >= requiredVersion[i]) {
			MySQL_ArtResultSet::row_t rs_data_row;
			rs_data_row.push_back(table_types[i]);

			rs_data->push_back(rs_data_row);
		}
	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getTimeDateFunctions() -I- */
const std::string&
MySQL_ConnectionMetaData::getTimeDateFunctions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getTimeDateFunctions");
	static const std::string funcs(
		"DAYOFWEEK,WEEKDAY,DAYOFMONTH,DAYOFYEAR,MONTH,DAYNAME,"
		"MONTHNAME,QUARTER,WEEK,YEAR,HOUR,MINUTE,SECOND,PERIOD_ADD,"
		"PERIOD_DIFF,TO_DAYS,FROM_DAYS,DATE_FORMAT,TIME_FORMAT,"
		"CURDATE,CURRENT_DATE,CURTIME,CURRENT_TIME,NOW,SYSDATE,"
		"CURRENT_TIMESTAMP,UNIX_TIMESTAMP,FROM_UNIXTIME,"
		"SEC_TO_TIME,TIME_TO_SEC");
	return funcs;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getTypeInfo() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getTypeInfo()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getTypeInfo");

	std::list<std::string> rs_field_data;
	rs_field_data.push_back("TYPE_NAME");
	rs_field_data.push_back("DATA_TYPE");
	rs_field_data.push_back("PRECISION");
	rs_field_data.push_back("LITERAL_PREFIX");
	rs_field_data.push_back("LITERAL_SUFFIX");
	rs_field_data.push_back("CREATE_PARAMS");
	rs_field_data.push_back("NULLABLE");
	rs_field_data.push_back("CASE_SENSITIVE");
	rs_field_data.push_back("SEARCHABLE");
	rs_field_data.push_back("UNSIGNED_ATTRIBUTE");
	rs_field_data.push_back("FIXED_PREC_SCALE");
	rs_field_data.push_back("AUTO_INCREMENT");
	rs_field_data.push_back("LOCAL_TYPE_NAME");
	rs_field_data.push_back("MINIMUM_SCALE");
	rs_field_data.push_back("MAXIMUM_SCALE");
	rs_field_data.push_back("SQL_DATA_TYPE");
	rs_field_data.push_back("SQL_DATETIME_SUB");
	rs_field_data.push_back("NUM_PREC_RADIX");

	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	int i = 0;
	while (mysqlc_types[i].typeName) {
		MySQL_ArtResultSet::row_t rs_data_row;
		const TypeInfoDef * const curr = &mysqlc_types[i];

		rs_data_row.push_back(curr->typeName);
		rs_data_row.push_back((int64_t) curr->dataType);
		rs_data_row.push_back((int64_t) curr->precision);
		rs_data_row.push_back(curr->literalPrefix);
		rs_data_row.push_back(curr->literalSuffix);
		rs_data_row.push_back(curr->createParams);
		rs_data_row.push_back((int64_t) curr->nullable);
		rs_data_row.push_back((int64_t) curr->caseSensitive);
		rs_data_row.push_back((int64_t) curr->searchable);
		rs_data_row.push_back((int64_t) curr->isUnsigned);
		rs_data_row.push_back((int64_t) curr->fixedPrecScale);
		rs_data_row.push_back((int64_t) curr->autoIncrement);
		rs_data_row.push_back(curr->localTypeName);
		rs_data_row.push_back((int64_t) curr->minScale);
		rs_data_row.push_back((int64_t) curr->maxScale);
		rs_data_row.push_back((int64_t) curr->sqlDataType);
		rs_data_row.push_back((int64_t) curr->sqlDateTimeSub);
		rs_data_row.push_back((int64_t) curr->numPrecRadix);

		rs_data->push_back(rs_data_row);
		++i;
	}

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getUDTs() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getUDTs(const std::string& /*catalog*/, const std::string& /*schemaPattern*/,
								  const std::string& /*typeNamePattern*/, std::list<int> & /*types*/)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getUDTs");
	std::list<std::string> rs_field_data;

	rs_field_data.push_back("TYPE_CAT");
	rs_field_data.push_back("TYPE_SCHEM");
	rs_field_data.push_back("TYPE_NAME");
	rs_field_data.push_back("CLASS_NAME");
	rs_field_data.push_back("DATA_TYPE");
	rs_field_data.push_back("REMARKS");

	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getUserName() -I- */
std::string
MySQL_ConnectionMetaData::getUserName()
{
	CPP_ENTER("MySQL_ConnectionMetaData::getUserName");
	std::auto_ptr<sql::Statement> stmt(connection->createStatement());
	std::auto_ptr<sql::ResultSet> rset(stmt->executeQuery("SELECT USER()"));
	if (rset->next()) {
		return std::string(rset->getString(1));
	}
	return "";
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::getVersionColumns() -I- */
sql::ResultSet *
MySQL_ConnectionMetaData::getVersionColumns(const std::string& /*catalog*/, const std::string& /*schema*/, const std::string& /*table*/)
{
	CPP_ENTER("MySQL_ConnectionMetaData::getVersionColumns");
	std::list<std::string> rs_field_data;

	rs_field_data.push_back("SCOPE");
	rs_field_data.push_back("COLUMN_NAME");
	rs_field_data.push_back("DATA_TYPE");
	rs_field_data.push_back("TYPE_NAME");
	rs_field_data.push_back("COLUMN_SIZE");
	rs_field_data.push_back("BUFFER_LENGTH");
	rs_field_data.push_back("DECIMAL_DIGITS");
	rs_field_data.push_back("PSEUDO_COLUMN");

	std::auto_ptr< MySQL_ArtResultSet::rset_t > rs_data(new MySQL_ArtResultSet::rset_t());

	MySQL_ArtResultSet * ret = new MySQL_ArtResultSet(rs_field_data, rs_data.get(), logger);
	// If there is no exception we can release otherwise on function exit memory will be freed
	rs_data.release();
	return ret;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::insertsAreDetected() -I- */
bool
MySQL_ConnectionMetaData::insertsAreDetected(int /* type */)
{
	CPP_ENTER("MySQL_ConnectionMetaData::insertsAreDetected");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::isCatalogAtStart() -I- */
bool
MySQL_ConnectionMetaData::isCatalogAtStart()
{
	CPP_ENTER("MySQL_ConnectionMetaData::isCatalogAtStart");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::isReadOnly() -I- */
bool
MySQL_ConnectionMetaData::isReadOnly()
{
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::nullPlusNonNullIsNull() -I- */
bool
MySQL_ConnectionMetaData::nullPlusNonNullIsNull()
{
	CPP_ENTER("MySQL_ConnectionMetaData::nullPlusNonNullIsNull");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::nullsAreSortedAtEnd() -I- */
bool
MySQL_ConnectionMetaData::nullsAreSortedAtEnd()
{
	CPP_ENTER("MySQL_ConnectionMetaData::nullsAreSortedAtEnd");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::nullsAreSortedAtStart() -I- */
bool
MySQL_ConnectionMetaData::nullsAreSortedAtStart()
{
	CPP_ENTER("MySQL_ConnectionMetaData::nullsAreSortedAtStart");
	return server_version > 40001 && server_version < 40011;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::nullsAreSortedHigh() -I- */
bool
MySQL_ConnectionMetaData::nullsAreSortedHigh()
{
	CPP_ENTER("MySQL_ConnectionMetaData::nullsAreSortedHigh");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::nullsAreSortedLow() -I- */
bool
MySQL_ConnectionMetaData::nullsAreSortedLow()
{
	CPP_ENTER("MySQL_ConnectionMetaData::nullsAreSortedLow");
	return !nullsAreSortedHigh();
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::othersDeletesAreVisible() -I- */
bool
MySQL_ConnectionMetaData::othersDeletesAreVisible(int /* type */)
{
	CPP_ENTER("MySQL_ConnectionMetaData::othersDeletesAreVisible");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::othersInsertsAreVisible() -I- */
bool
MySQL_ConnectionMetaData::othersInsertsAreVisible(int /* type */)
{
	CPP_ENTER("MySQL_ConnectionMetaData::othersInsertsAreVisible");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::othersUpdatesAreVisible() -I- */
bool
MySQL_ConnectionMetaData::othersUpdatesAreVisible(int /* type */)
{
	CPP_ENTER("MySQL_ConnectionMetaData::othersUpdatesAreVisible");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::ownDeletesAreVisible() -I- */
bool
MySQL_ConnectionMetaData::ownDeletesAreVisible(int /* type */)
{
	CPP_ENTER("MySQL_ConnectionMetaData::ownDeletesAreVisible");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::ownInsertsAreVisible() -I- */
bool
MySQL_ConnectionMetaData::ownInsertsAreVisible(int /* type */)
{
	CPP_ENTER("MySQL_ConnectionMetaData::ownInsertsAreVisible");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::ownUpdatesAreVisible() -I- */
bool
MySQL_ConnectionMetaData::ownUpdatesAreVisible(int /* type */)
{
	CPP_ENTER("MySQL_ConnectionMetaData::ownUpdatesAreVisible");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::storesLowerCaseIdentifiers() -I- */
bool
MySQL_ConnectionMetaData::storesLowerCaseIdentifiers()
{
	CPP_ENTER("MySQL_ConnectionMetaData::storesLowerCaseIdentifiers");
	return ((lower_case_table_names.compare("1") || lower_case_table_names.compare("2")));
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::storesLowerCaseQuotedIdentifiers() -I- */
bool
MySQL_ConnectionMetaData::storesLowerCaseQuotedIdentifiers()
{
	CPP_ENTER("MySQL_ConnectionMetaData::storesLowerCaseQuotedIdentifiers");
	return ((lower_case_table_names.compare("1") || lower_case_table_names.compare("2")));
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::storesMixedCaseIdentifiers() -I- */
bool
MySQL_ConnectionMetaData::storesMixedCaseIdentifiers()
{
	CPP_ENTER("MySQL_ConnectionMetaData::storesMixedCaseIdentifiers");
	return !((lower_case_table_names.compare("1") || lower_case_table_names.compare("2")));
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::storesMixedCaseQuotedIdentifiers() -I- */
bool
MySQL_ConnectionMetaData::storesMixedCaseQuotedIdentifiers()
{
	CPP_ENTER("MySQL_ConnectionMetaData::storesMixedCaseQuotedIdentifiers");
	return !((lower_case_table_names.compare("1") || lower_case_table_names.compare("2")));
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::storesUpperCaseIdentifiers() -I- */
bool
MySQL_ConnectionMetaData::storesUpperCaseIdentifiers()
{
	CPP_ENTER("MySQL_ConnectionMetaData::storesUpperCaseIdentifiers");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::storesUpperCaseQuotedIdentifiers() -I- */
bool
MySQL_ConnectionMetaData::storesUpperCaseQuotedIdentifiers()
{
	CPP_ENTER("MySQL_ConnectionMetaData::storesUpperCaseQuotedIdentifiers");
	return true; // not actually true, but required by JDBC spec!?
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsAlterTableWithAddColumn() -I- */
bool
MySQL_ConnectionMetaData::supportsAlterTableWithAddColumn()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsAlterTableWithAddColumn");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsAlterTableWithDropColumn() -I- */
bool
MySQL_ConnectionMetaData::supportsAlterTableWithDropColumn()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsAlterTableWithDropColumn");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsANSI92EntryLevelSQL -I- */
bool
MySQL_ConnectionMetaData::supportsANSI92EntryLevelSQL()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsANSI92EntryLevelSQL");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsANSI92FullSQL() -I- */
bool
MySQL_ConnectionMetaData::supportsANSI92FullSQL()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsANSI92FullSQL");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsANSI92IntermediateSQL() -I- */
bool
MySQL_ConnectionMetaData::supportsANSI92IntermediateSQL()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsANSI92IntermediateSQL");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsBatchUpdates() -I- */
bool
MySQL_ConnectionMetaData::supportsBatchUpdates()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsBatchUpdates");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsCatalogsInDataManipulation() -I- */
bool
MySQL_ConnectionMetaData::supportsCatalogsInDataManipulation()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsCatalogsInDataManipulation");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsCatalogsInIndexDefinitions() -I- */
bool
MySQL_ConnectionMetaData::supportsCatalogsInIndexDefinitions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsCatalogsInIndexDefinitions");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsCatalogsInPrivilegeDefinitions() -I- */
bool
MySQL_ConnectionMetaData::supportsCatalogsInPrivilegeDefinitions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsCatalogsInPrivilegeDefinitions");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsCatalogsInProcedureCalls() -I- */
bool
MySQL_ConnectionMetaData::supportsCatalogsInProcedureCalls()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsCatalogsInProcedureCalls");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsCatalogsInTableDefinitions() -I- */
bool
MySQL_ConnectionMetaData::supportsCatalogsInTableDefinitions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsCatalogsInTableDefinitions");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsColumnAliasing() -I- */
bool
MySQL_ConnectionMetaData::supportsColumnAliasing()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsColumnAliasing");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsConvert() -I- */
bool
MySQL_ConnectionMetaData::supportsConvert()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsConvert");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsConvert() -I- */
bool
MySQL_ConnectionMetaData::supportsConvert(int fromType, int toType)
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsConvert");
	switch (fromType) {
		// The char/binary types can be converted to pretty much anything.
		case sql::DataType::CHAR:
		case sql::DataType::VARCHAR:
		case sql::DataType::LONGVARCHAR:
		case sql::DataType::BINARY:
		case sql::DataType::VARBINARY:
		case sql::DataType::LONGVARBINARY:
		{
			switch (toType) {
				case sql::DataType::DECIMAL:
				case sql::DataType::NUMERIC:
				case sql::DataType::REAL:
				case sql::DataType::TINYINT:
				case sql::DataType::SMALLINT:
				case sql::DataType::INTEGER:
				case sql::DataType::BIGINT:
				case sql::DataType::DOUBLE:
				case sql::DataType::CHAR:
				case sql::DataType::VARCHAR:
				case sql::DataType::LONGVARCHAR:
				case sql::DataType::BINARY:
				case sql::DataType::VARBINARY:
				case sql::DataType::LONGVARBINARY:
				case sql::DataType::DATE:
				case sql::DataType::TIME:
				case sql::DataType::TIMESTAMP:
					return true;

				default:
					return false;
			}
		}

		// We don't handle the BIT type yet.
		case sql::DataType::BIT:
			return false;

		// The numeric types. Basically they can convert among themselves, and
		case sql::DataType::DECIMAL:
		case sql::DataType::NUMERIC:
		case sql::DataType::REAL:
		case sql::DataType::TINYINT:
		case sql::DataType::SMALLINT:
		case sql::DataType::INTEGER:
		case sql::DataType::BIGINT:
		case sql::DataType::DOUBLE:
		{
			switch (toType) {
				case sql::DataType::DECIMAL:
				case sql::DataType::NUMERIC:
				case sql::DataType::REAL:
				case sql::DataType::TINYINT:
				case sql::DataType::SMALLINT:
				case sql::DataType::INTEGER:
				case sql::DataType::BIGINT:
				case sql::DataType::DOUBLE:
				case sql::DataType::CHAR:
				case sql::DataType::VARCHAR:
				case sql::DataType::LONGVARCHAR:
				case sql::DataType::BINARY:
				case sql::DataType::VARBINARY:
				case sql::DataType::LONGVARBINARY:
					return true;

				default:
					return false;
			}
		}

		// MySQL doesn't support a NULL type
		case sql::DataType::SQLNULL:
			return false;

		// Dates can be converted to char/binary types
		case sql::DataType::DATE:
		{
			switch (toType) {
				case sql::DataType::CHAR:
				case sql::DataType::VARCHAR:
				case sql::DataType::LONGVARCHAR:
				case sql::DataType::BINARY:
				case sql::DataType::VARBINARY:
				case sql::DataType::LONGVARBINARY:
					return true;

				default:
					return false;
			}
		}
		// Time can be converted to char/binary types
		case sql::DataType::TIME:
		{
			switch (toType) {
				case sql::DataType::CHAR:
				case sql::DataType::VARCHAR:
				case sql::DataType::LONGVARCHAR:
				case sql::DataType::BINARY:
				case sql::DataType::VARBINARY:
				case sql::DataType::LONGVARBINARY:
					return true;

				default:
					return false;
			}
		}
		// Timestamp can be converted to char/binary types and date/time types (with loss of precision).
		case sql::DataType::TIMESTAMP:
		{
			switch (toType) {
				case sql::DataType::CHAR:
				case sql::DataType::VARCHAR:
				case sql::DataType::LONGVARCHAR:
				case sql::DataType::BINARY:
				case sql::DataType::VARBINARY:
				case sql::DataType::LONGVARBINARY:
				case sql::DataType::TIME:
				case sql::DataType::DATE:
					return true;

				default:
					return false;
			}
		}
		// We shouldn't get here!
		default:
			return false; // not sure
	}
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsCoreSQLGrammar() -I- */
bool
MySQL_ConnectionMetaData::supportsCoreSQLGrammar()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsCoreSQLGrammar");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsCorrelatedSubqueries() -I- */
bool
MySQL_ConnectionMetaData::supportsCorrelatedSubqueries()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsCorrelatedSubqueries");
	return server_version >= 40100;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsDataDefinitionAndDataManipulationTransactions() -I- */
bool
MySQL_ConnectionMetaData::supportsDataDefinitionAndDataManipulationTransactions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsDataDefinitionAndDataManipulationTransactions");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsDataManipulationTransactionsOnly() -I- */
bool
MySQL_ConnectionMetaData::supportsDataManipulationTransactionsOnly()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsDataManipulationTransactionsOnly");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsDifferentTableCorrelationNames() -I- */
bool
MySQL_ConnectionMetaData::supportsDifferentTableCorrelationNames()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsDifferentTableCorrelationNames");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsExpressionsInOrderBy() -I- */
bool
MySQL_ConnectionMetaData::supportsExpressionsInOrderBy()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsExpressionsInOrderBy");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsExtendedSQLGrammar() -I- */
bool
MySQL_ConnectionMetaData::supportsExtendedSQLGrammar()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsExtendedSQLGrammar");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsFullOuterJoins() -I- */
bool
MySQL_ConnectionMetaData::supportsFullOuterJoins()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsFullOuterJoins");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsGetGeneratedKeys() -I- */
bool
MySQL_ConnectionMetaData::supportsGetGeneratedKeys()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsGetGeneratedKeys");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsGroupBy() -I- */
bool
MySQL_ConnectionMetaData::supportsGroupBy()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsGroupBy");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsGroupByBeyondSelect() -I- */
bool
MySQL_ConnectionMetaData::supportsGroupByBeyondSelect()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsGroupByBeyondSelect");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsGroupByUnrelated() -I- */
bool
MySQL_ConnectionMetaData::supportsGroupByUnrelated()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsGroupByUnrelated");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsLikeEscapeClause() -I- */
bool
MySQL_ConnectionMetaData::supportsLikeEscapeClause()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsLikeEscapeClause");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsLimitedOuterJoins() -I- */
bool
MySQL_ConnectionMetaData::supportsLimitedOuterJoins()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsLimitedOuterJoins");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsMinimumSQLGrammar() -I- */
bool
MySQL_ConnectionMetaData::supportsMinimumSQLGrammar()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsMinimumSQLGrammar");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsMixedCaseIdentifiers() -I- */
bool
MySQL_ConnectionMetaData::supportsMixedCaseIdentifiers()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsMixedCaseIdentifiers");
	return !((lower_case_table_names.compare("1") || lower_case_table_names.compare("2")));
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsMixedCaseQuotedIdentifiers() -I- */
bool
MySQL_ConnectionMetaData::supportsMixedCaseQuotedIdentifiers()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsMixedCaseQuotedIdentifiers");
	return !((lower_case_table_names.compare("1") || lower_case_table_names.compare("2")));
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsMultipleOpenResults() -I- */
bool
MySQL_ConnectionMetaData::supportsMultipleOpenResults()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsMultipleOpenResults");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsMultipleResultSets() -I- */
bool
MySQL_ConnectionMetaData::supportsMultipleResultSets()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsMultipleResultSets");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsMultipleTransactions() -I- */
bool
MySQL_ConnectionMetaData::supportsMultipleTransactions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsMultipleTransactions");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsNamedParameters() -I- */
bool
MySQL_ConnectionMetaData::supportsNamedParameters()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsNamedParameters");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsNonNullableColumns() -I- */
bool
MySQL_ConnectionMetaData::supportsNonNullableColumns()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsNonNullableColumns");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsOpenCursorsAcrossCommit() -I- */
bool
MySQL_ConnectionMetaData::supportsOpenCursorsAcrossCommit()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsOpenCursorsAcrossCommit");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsOpenCursorsAcrossRollback() -I- */
bool
MySQL_ConnectionMetaData::supportsOpenCursorsAcrossRollback()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsOpenCursorsAcrossRollback");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsOpenStatementsAcrossCommit() -I- */
bool
MySQL_ConnectionMetaData::supportsOpenStatementsAcrossCommit()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsOpenStatementsAcrossCommit");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsOpenStatementsAcrossRollback() -I- */
bool
MySQL_ConnectionMetaData::supportsOpenStatementsAcrossRollback()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsOpenStatementsAcrossRollback");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsOrderByUnrelated() -I- */
bool
MySQL_ConnectionMetaData::supportsOrderByUnrelated()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsOrderByUnrelated");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsOuterJoins() -I- */
bool
MySQL_ConnectionMetaData::supportsOuterJoins()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsOuterJoins");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsPositionedDelete() -I- */
bool
MySQL_ConnectionMetaData::supportsPositionedDelete()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsPositionedDelete");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsPositionedUpdate() -I- */
bool
MySQL_ConnectionMetaData::supportsPositionedUpdate()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsPositionedUpdate");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsResultSetHoldability() -I- */
bool
MySQL_ConnectionMetaData::supportsResultSetHoldability(int holdability)
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsResultSetHoldability");
	return (holdability == sql::ResultSet::HOLD_CURSORS_OVER_COMMIT);
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsResultSetType() -I- */
bool
MySQL_ConnectionMetaData::supportsResultSetType(int type)
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsResultSetType");
	return (type == sql::ResultSet::TYPE_SCROLL_INSENSITIVE);
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsSavepoints() -I- */
bool
MySQL_ConnectionMetaData::supportsSavepoints()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsSavepoints");
	return (server_version >= 40014 || server_version >= 40101);
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsSchemasInDataManipulation() -I- */
bool
MySQL_ConnectionMetaData::supportsSchemasInDataManipulation()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsSchemasInDataManipulation");
	return server_version >= 32200;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsSchemasInIndexDefinitions() -I- */
bool
MySQL_ConnectionMetaData::supportsSchemasInIndexDefinitions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsSchemasInIndexDefinitions");
	return server_version >= 32200;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsSchemasInPrivilegeDefinitions() -I- */
bool
MySQL_ConnectionMetaData::supportsSchemasInPrivilegeDefinitions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsSchemasInPrivilegeDefinitions");
	return server_version > 32200;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsSchemasInProcedureCalls() -I- */
bool
MySQL_ConnectionMetaData::supportsSchemasInProcedureCalls()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsSchemasInProcedureCalls");
	return server_version >= 32200;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsSchemasInTableDefinitions() -I- */
bool
MySQL_ConnectionMetaData::supportsSchemasInTableDefinitions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsSchemasInTableDefinitions");
	return server_version >= 32200;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsSelectForUpdate() -I- */
bool
MySQL_ConnectionMetaData::supportsSelectForUpdate()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsSelectForUpdate");
	return server_version >= 40000;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsStatementPooling() -I- */
bool
MySQL_ConnectionMetaData::supportsStatementPooling()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsStatementPooling");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsStoredProcedures() -I- */
bool
MySQL_ConnectionMetaData::supportsStoredProcedures()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsStoredProcedures");
	return server_version >= 50000;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsSubqueriesInComparisons() -I- */
bool
MySQL_ConnectionMetaData::supportsSubqueriesInComparisons()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsSubqueriesInComparisons");
	return server_version >= 40100;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsSubqueriesInExists() -I- */
bool
MySQL_ConnectionMetaData::supportsSubqueriesInExists()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsSubqueriesInExists");
	return server_version >= 40100;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsSubqueriesInIns() -I- */
bool
MySQL_ConnectionMetaData::supportsSubqueriesInIns()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsSubqueriesInIns");
	return server_version >= 40100;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsSubqueriesInQuantifieds() -I- */
bool
MySQL_ConnectionMetaData::supportsSubqueriesInQuantifieds()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsSubqueriesInQuantifieds");
	return server_version >= 40100;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsTableCorrelationNames() -I- */
bool
MySQL_ConnectionMetaData::supportsTableCorrelationNames()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsTableCorrelationNames");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsTransactionIsolationLevel() -I- */
bool
MySQL_ConnectionMetaData::supportsTransactionIsolationLevel(int /* level */)
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsTransactionIsolationLevel");
	return server_version >= 32336;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsTransactions() -I- */
bool
MySQL_ConnectionMetaData::supportsTransactions()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsTransactions");
	return server_version >= 32315;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsTypeConversion() -I- */
bool
MySQL_ConnectionMetaData::supportsTypeConversion()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsTypeConversion");
	return true;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsUnion() -I- */
bool
MySQL_ConnectionMetaData::supportsUnion()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsUnion");
	return server_version >= 40000;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::supportsUnionAll() -I- */
bool
MySQL_ConnectionMetaData::supportsUnionAll()
{
	CPP_ENTER("MySQL_ConnectionMetaData::supportsUnionAll");
	return server_version >= 40000;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::updatesAreDetected() -I- */
bool
MySQL_ConnectionMetaData::updatesAreDetected(int /* type */)
{
	CPP_ENTER("MySQL_ConnectionMetaData::updatesAreDetected");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::usesLocalFilePerTable() -I- */
bool
MySQL_ConnectionMetaData::usesLocalFilePerTable()
{
	CPP_ENTER("MySQL_ConnectionMetaData::usesLocalFilePerTable");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::usesLocalFiles() -I- */
bool
MySQL_ConnectionMetaData::usesLocalFiles()
{
	CPP_ENTER("MySQL_ConnectionMetaData::usesLocalFiles");
	return false;
}
/* }}} */


/* {{{ MySQL_ConnectionMetaData::matchTable() -I- */
bool
MySQL_ConnectionMetaData::matchTable(const std::string & sPattern, const std::string & tPattern,
									 const std::string & schema, const std::string & table)
{
	CPP_ENTER("MySQL_ConnectionMetaData::matchTable");
	return (!sPattern.compare("*") || !sPattern.compare(schema)) && (!tPattern.compare("*") || !tPattern.compare(table));
}


} /* namespace mysql */
} /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
