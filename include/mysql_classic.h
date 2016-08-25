/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FLOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

/*
  This header has the declarations needed for the classic C API
  compatibility layer.
*/
#ifndef __MYSQL_CLASSIC_H__
#define __MYSQL_CLASSIC_H__

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define STDCALL __stdcall
#else
#define STDCALL
#endif


#define CR_UNKNOWN_ERROR	2000


typedef struct MYSQL_T MYSQL;
typedef struct MYSQL_RES_T MYSQL_RES;
typedef char **MYSQL_ROW;		/* return data as array of strings */

#define NOT_NULL_FLAG	1		/* Field can't be NULL */
#define PRI_KEY_FLAG	2		/* Field is part of a primary key */
#define UNIQUE_KEY_FLAG 4		/* Field is part of a unique key */
#define MULTIPLE_KEY_FLAG 8		/* Field is part of a key */
#define BLOB_FLAG	16		/* Field is a blob */
#define UNSIGNED_FLAG	32		/* Field is unsigned */
#define ZEROFILL_FLAG	64		/* Field is zerofill */
#define BINARY_FLAG	128		/* Field is binary   */

/* The following are only sent to new clients */
#define ENUM_FLAG	256		/* field is an enum */
#define AUTO_INCREMENT_FLAG 512		/* field is a autoincrement field */
#define TIMESTAMP_FLAG	1024		/* Field is a timestamp */
#define SET_FLAG	2048		/* field is a set */
#define NO_DEFAULT_VALUE_FLAG 4096	/* Field doesn't have default value */
#define ON_UPDATE_NOW_FLAG 8192         /* Field is set to NOW on UPDATE */
#define NUM_FLAG	32768		/* Field is num (for clients) */
#define PART_KEY_FLAG	16384		/* Intern; Part of some key */
#define GROUP_FLAG	32768		/* Intern: Group field */
#define UNIQUE_FLAG	65536		/* Intern: Used by sql_yacc */
#define BINCMP_FLAG	131072		/* Intern: Used by sql_yacc */
#define GET_FIXED_FIELDS_FLAG (1 << 18) /* Used to get fields in item tree */
#define FIELD_IN_PART_FUNC_FLAG (1 << 19)/* Field part of partition func */


typedef enum enum_field_types {
  MYSQL_TYPE_DECIMAL, MYSQL_TYPE_TINY,
  MYSQL_TYPE_SHORT,  MYSQL_TYPE_LONG,
  MYSQL_TYPE_FLOAT,  MYSQL_TYPE_DOUBLE,
  MYSQL_TYPE_NULL,   MYSQL_TYPE_TIMESTAMP,
  MYSQL_TYPE_LONGLONG,MYSQL_TYPE_INT24,
  MYSQL_TYPE_DATE,   MYSQL_TYPE_TIME,
  MYSQL_TYPE_DATETIME, MYSQL_TYPE_YEAR,
  MYSQL_TYPE_NEWDATE, MYSQL_TYPE_VARCHAR,
  MYSQL_TYPE_BIT,
  MYSQL_TYPE_TIMESTAMP2,
  MYSQL_TYPE_DATETIME2,
  MYSQL_TYPE_TIME2,
  MYSQL_TYPE_JSON=245,
  MYSQL_TYPE_NEWDECIMAL=246,
  MYSQL_TYPE_ENUM=247,
  MYSQL_TYPE_SET=248,
  MYSQL_TYPE_TINY_BLOB=249,
  MYSQL_TYPE_MEDIUM_BLOB=250,
  MYSQL_TYPE_LONG_BLOB=251,
  MYSQL_TYPE_BLOB=252,
  MYSQL_TYPE_VAR_STRING=253,
  MYSQL_TYPE_STRING=254,
  MYSQL_TYPE_GEOMETRY=255
} enum_field_types;

typedef struct st_mysql_field {
  char *name;                 /* Name of column */
  char *org_name;             /* Original column name, if an alias */
  char *table;                /* Table of column if column was a field */
  char *org_table;            /* Org table name, if table was an alias */
  char *db;                   /* Database for table */
  char *catalog;	      /* Catalog for table */
  char *def;                  /* Default value (set by mysql_list_fields) */
  unsigned long length;       /* Width of column (create length) */
  unsigned long max_length;   /* Max width for selected set */
  unsigned int name_length;
  unsigned int org_name_length;
  unsigned int table_length;
  unsigned int org_table_length;
  unsigned int db_length;
  unsigned int catalog_length;
  unsigned int def_length;
  unsigned int flags;         /* Div flags */
  unsigned int decimals;      /* Number of decimals in field */
  unsigned int charsetnr;     /* Character set */
  enum enum_field_types type; /* Type of field. See mysql_com.h for types */
  void *extension;
} MYSQL_FIELD;


void STDCALL mysql_close(MYSQL *sock);
MYSQL *	STDCALL mysql_init(MYSQL *mysql);
MYSQL *	STDCALL mysql_real_connect(MYSQL *mysql, const char *host,
					   const char *user,
					   const char *passwd,
					   const char *db,
					   unsigned int port,
					   const char *unix_socket,
					   unsigned long clientflag);
int		STDCALL mysql_real_query(MYSQL *mysql, const char *q,
					unsigned long length);


const char * STDCALL mysql_error(MYSQL *mysql);
MYSQL_FIELD * STDCALL mysql_fetch_field(MYSQL_RES *result);
MYSQL_ROW	STDCALL mysql_fetch_row(MYSQL_RES *result);
unsigned long * STDCALL mysql_fetch_lengths(MYSQL_RES *result);
unsigned int STDCALL mysql_num_fields(MYSQL_RES *res);
MYSQL_RES * STDCALL mysql_use_result(MYSQL *mysql);
MYSQL_RES * STDCALL mysql_store_result(MYSQL *mysql);
void STDCALL mysql_free_result(MYSQL_RES *result);

#ifdef	__cplusplus
}
#endif

#endif /* __MYSQL_CLASSIC_H__ */