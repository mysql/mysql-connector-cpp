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

/**
  @defgroup xapi XAPI Functions

  XAPI functions and types. See @ref xapi_ref for introduction.

  @{
  @defgroup xapi_sess     Session operations

  @defgroup xapi_coll     Statements operating on document collections

  @defgroup xapi_tbl      Statements operating on tables

  @defgroup xapi_sql      SQL execution

  @defgroup xapi_ddl      DDL statements

  @note To create a table or a view, use reqular SQL statement.

  @defgroup xapi_stmt     Statement execution

  @defgroup xapi_res      Result processing

  @defgroup xapi_md  Meta data access

  @defgroup xapi_diag     Diganostics
  @}
*/


/**
  @file
  The main header for MySQL Connector/C++ XAPI.

  This header should be included by C and C++ code which uses the XAPI
  implemented by MySQL Connector/C++

  @ingroup xapi
*/


#ifndef MYSQL_XAPI_H
#define MYSQL_XAPI_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mysql_common.h"
#include <stdlib.h>
#include <stdint.h>

/**
  @addtogroup xapi
  @{
*/

/*
  On Windows, dependency on the sockets library can be handled using
  #pragma comment directive.
*/

#ifdef _WIN32
 #pragma comment(lib, "ws2_32")
#endif

// FIXME
#define STDCALL

///////////////////// COMMON TYPE DECLARATIONS, REMOVE LATER

#ifndef	__cplusplus
  typedef unsigned char bool;
#endif

typedef char object_id[16];
typedef object_id* MYSQLX_GUID;

/** Return value indicating function/operation success. */

#define RESULT_OK 0

/**
  Return value flag indicating that the last reading operation
  did not finish reading to the end and there is still more data
  to be fetched by functions such as mysqlx_get_bytes()
*/

#define RESULT_MORE_DATA 8

/**
  Return value flag indicating end of data items (documents or
  rows) in a query result. This is used by functions which iterate
  over result data.
*/

#define RESULT_NULL 16

/**
  Return value flag indicating that operation generated
  information diagnostic entries.
*/

#define RESULT_INFO 32

/**  Return value flag indicating that operation generated warnings. */

#define RESULT_WARNING 64

/**  Return value flag indicating function/operation error. */

#define RESULT_ERROR   128


#define MYSQLX_MAX_ERROR_LEN 255
#define MYSQLX_NULL_TERMINATED 0xFFFFFFFF

#define MYSQLX_ERR_UNKNOWN 0xFFFF

#define MYSQLX_COLLATION_UNDEFINED 0

#define DEFAULT_MYSQLX_PORT 33060

/*
  Error codes
*/

#define MYSQLX_ERROR_INDEX_OUT_OF_RANGE 1

/*
  Error messages
*/

#define MYSQLX_ERROR_INDEX_OUT_OF_RANGE_MSG "Index is out of range"
#define MYSQLX_ERROR_MISSING_SCHEMA_NAME_MSG "Missing schema name"
#define MYSQLX_ERROR_MISSING_TABLE_NAME_MSG "Missing table name"
#define MYSQLX_ERROR_MISSING_VIEW_NAME_MSG "Missing view name"
#define MYSQLX_ERROR_MISSING_COLLECTION_NAME_MSG "Missing collection name"
#define MYSQLX_ERROR_MISSING_VIEW_NAME_MSG "Missing view name"
#define MYSQLX_ERROR_MISSING_KEY_NAME_MSG "Missing key name"
#define MYSQLX_ERROR_HANDLE_NULL_MSG "Handle cannot be NULL"
#define MYSQLX_ERROR_VIEW_INVALID_STMT_TYPE "Invalid statement type for View. Only SELECT type is supported"
#define MYSQLX_ERROR_VIEW_TYPE_MSG "Statement must be of VIEW type"
#define MYSQLX_ERROR_OUTPUT_BUFFER_NULL "The output buffer cannot be NULL"
#define MYSQLX_ERROR_OUTPUT_BUFFER_ZERO "The output buffer cannot have zero length"
#define MYSQLX_ERROR_OP_NOT_SUPPORTED "The operation is not supported by the function"

/* Opaque structures*/

/**
  Type of error handles.

  Error handles give access to diagnostic information from the session
  and statement operations.

  @see mysqlx_error()
*/

typedef struct mysqlx_error_struct mysqlx_error_t;


/**
  Type of session handles.

  @see mysqlx_get_session()
*/

typedef struct mysqlx_session_struct mysqlx_session_t;


/**
  Type of handles for session configuration data.

  Session can be created using  previously prepared session configuration
  data. New configuration data is allocated by `mysqlx_session_options_new()`
  and can be manipulated using related functions.

  @see mysqlx_get_session_from_options(), mysqlx_session_options_new(),
  mysqlx_session_option_set(), mysqlx_free_options().
*/

typedef struct mysqlx_session_options_struct mysqlx_session_options_t;


/**
  Type of database schema handles.

  @see mysqlx_get_schema()
*/

typedef struct mysqlx_schema_struct mysqlx_schema_t;


/**
  Type of collection handles.

  @see mysqlx_get_collection()
*/

typedef struct mysqlx_collection_struct mysqlx_collection_t;


/**
  Type of table handles.

  @see mysqlx_get_table()
*/
typedef struct mysqlx_table_struct mysqlx_table_t;


/**
  Type of statement handles.

  Some XAPI functions create statements without executing them. These
  functions return a statement handle which can be used to define statement
  properties and then execute it.

  @see mysqlx_sql_new(), mysqlx_table_select_new(), mysqlx_table_insert_new(),
  mysqlx_table_update_new(), mysqlx_table_delete_new(),
  mysqlx_collection_find_new(), mysqlx_collection_modify_new(),
  mysqlx_collection_add_new(), mysqlx_collection_remove_new()
*/

typedef struct mysqlx_stmt_struct mysqlx_stmt_t;


/**
  Type of row handles.

  @see mysqlx_row_fetch_one()
*/

typedef struct mysqlx_row_struct mysqlx_row_t;


/**
  Type of result handles.

  Functions which produce results return a result handle which is
  then used to examine the result.

  @see mysqlx_execute(), mysqlx_store_result(), mysqlx_row_fetch_one(),
       mysqlx_json_fetch_one(), mysqlx_next_result())
*/

typedef struct mysqlx_result_struct mysqlx_result_t;


/**
  The data type identifiers used in MYSQLX API.
*/

typedef enum mysqlx_data_type_enum
{
  MYSQLX_TYPE_UNDEFINED = 0,

  /* Column types as defined in protobuf (mysqlx_resultset.proto)*/
  MYSQLX_TYPE_SINT     = 1, /**< 64-bit signed integer number type*/
  MYSQLX_TYPE_UINT  = 2,    /**< 64-bit unsigned integer number type*/
  MYSQLX_TYPE_DOUBLE   = 5, /**< Floating point double number type*/
  MYSQLX_TYPE_FLOAT = 6,    /**< Floating point float number type*/
  MYSQLX_TYPE_BYTES    = 7, /**< Bytes array type*/
  MYSQLX_TYPE_TIME  = 10,   /**< Time type*/
  MYSQLX_TYPE_DATETIME = 12,/**< Datetime type*/
  MYSQLX_TYPE_SET  = 15,    /**< Set type*/
  MYSQLX_TYPE_ENUM     = 16,/**< Enum type*/
  MYSQLX_TYPE_BIT  = 17,    /**< Bit type*/
  MYSQLX_TYPE_DECIMAL  = 18,/**< Decimal type*/

  /* Column types from DevAPI (no number constants assigned, just names)*/
  MYSQLX_TYPE_BOOL     = 19,/**< Bool type*/
  MYSQLX_TYPE_JSON     = 20,/**< JSON type*/
  MYSQLX_TYPE_STRING   = 21,/**< String type*/
  MYSQLX_TYPE_GEOMETRY = 22,/**< Geometry type*/
  MYSQLX_TYPE_TIMESTAMP= 23,/**< Timestamp type*/

  MYSQLX_TYPE_NULL     = 100, /**< NULL value*/
  MYSQLX_TYPE_EXPR     = 101  /**< Expression type*/
} mysqlx_data_type_t;

#define PARAM_SINT(A) (void*)MYSQLX_TYPE_SINT, (int64_t)A
#define PARAM_UINT(A) (void*)MYSQLX_TYPE_UINT, (uint64_t)A
#define PARAM_FLOAT(A) (void*)MYSQLX_TYPE_FLOAT, (double)A
#define PARAM_DOUBLE(A) (void*)MYSQLX_TYPE_DOUBLE, (double)A
#define PARAM_BYTES(DATA, SIZE) (void*)MYSQLX_TYPE_BYTES, (void*)DATA, (size_t)SIZE
#define PARAM_STRING(A) (void*)MYSQLX_TYPE_STRING, A
#define PARAM_EXPR(A) (void*)MYSQLX_TYPE_EXPR, A
#define PARAM_NULL() (void*)MYSQLX_TYPE_NULL

#define PARAM_END (void*)0


/**
  Sort directions in sorting operations such as ORDER BY.
*/

typedef enum mysqlx_sort_direction_enum
{
  SORT_ORDER_ASC = 1, /**< Ascending sorting (Default)*/
  SORT_ORDER_DESC = 2 /**< Descending sorting*/
} mysqlx_sort_direction_t;


#define PARAM_SORT_ASC(A) A, SORT_ORDER_ASC
#define PARAM_SORT_DESC(A) A, SORT_ORDER_DESC

/**
  Session options for use with `mysqlx_session_option_get()`
  and `mysqlx_session_option_set()` functions.

  @note Specifying `MYSQLX_OPT_SSL_CA` option implies `MYSQLX_OPT_SSL_ENABLE`.
*/

typedef enum mysqlx_opt_type_enum
{
  MYSQLX_OPT_HOST = 1,        /**< host name or IP address */
  /** DNS name of the host, IPv4 address or IPv6 address */
  MYSQLX_OPT_PORT = 2,
  MYSQLX_OPT_USER = 3,        /**< user name */
  MYSQLX_OPT_PWD = 4,         /**< password */
  MYSQLX_OPT_DB = 5,          /**< default database */
  MYSQLX_OPT_SSL_ENABLE = 6,  /**< use TLS connection */
  /** path to a PEM file specifying trusted root certificates */
  MYSQLX_OPT_SSL_CA = 7,
}
mysqlx_opt_type_t;


/**
  Constants for defining the View algorithm using
  mysqlx_set_view_algorithm() function.
  @see https://dev.mysql.com/doc/refman/en/view-algorithms.html
*/

typedef enum mysqlx_view_algorithm_enum
{
  VIEW_ALGORITHM_UNDEFINED = 0,   /**< Undefined view algorithm */
  VIEW_ALGORITHM_MERGE = 1,       /**< Merge view algorithm */
  VIEW_ALGORITHM_TEMPTABLE = 2    /**< Temptable view algorithm */
} mysqlx_view_algorithm_t;


/**
  Constants for defining the View security using
  mysqlx_set_view_security() function.
  @see https://dev.mysql.com/doc/refman/en/stored-programs-security.html
*/

typedef enum mysqlx_view_security_enum
{
  VIEW_SECURITY_DEFINER = 1,  /**< Definer view security context */
  VIEW_SECURITY_INVOKER = 2   /**< Invoker view security context */
} mysqlx_view_security_t;


/**
  Constants for defining the View check options
  mysqlx_set_view_security() function.
  @see https://dev.mysql.com/doc/refman/en/view-check-option.html
*/

typedef enum mysqlx_view_check_option_enum
{
  VIEW_CHECK_OPTION_CASCADED = 1,   /**< Cascaded view check option */
  VIEW_CHECK_OPTION_LOCAL = 2       /**< Local view check option */
} mysqlx_view_check_option_t;


/*
  ====================================================================
  Session operations
  ====================================================================
*/

/**
  Create a new session.

  @param host       server host DNS name, IPv4 address or IPv6 address
  @param port       port number
  @param user       user name
  @param password   password
  @param database   default database name
  @param[out] out_error if error happens during connect the error message
                    is returned through this parameter
  @param[out] err_code if error happens during connect the error code
                    is returned through this parameter

  @return session handle if session could be created, otherwise NULL
          is returned and the error information is returned through
          the out_error and err_code output parameters.

  @note The session returned by the function must be properly closed using
        `mysqlx_session_close()`.
  @note This type of session does not support executing plain SQL queries.
  @note This function always establishes connection with SSL enabled

  @ingroup xapi_sess
*/

PUBLIC_API mysqlx_session_t *
mysqlx_get_session(const char *host, int port, const char *user,
                     const char *password, const char *database,
                     char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code);


/**
  Create a session using connection string or URL.

  Connection sting has the form `"user:pass\@host:port/?option&option"`,
  valid URL is like a connection string with a `mysqlx://` prefix. Host is
  specified as either DNS name, IPv4 address of the form "nn.nn.nn.nn" or
  IPv6 address of the form "[nn:nn:nn:...]".

  Possible connection options are:

  - `ssl-enable` : use TLS connection
  - `ssl-ca=`path : path to a PEM file specifying trusted root certificates

  Specifying `ssl-ca` option implies `ssl-enable`.

  @param conn_string    connection string
  @param[out] out_error if error happens during connect the error message
                        is returned through this parameter
  @param[out] err_code  if error happens during connect the error code
                        is returned through this parameter

  @return session handle if session could be created, otherwise NULL
  is returned and the error information is returned through
  the out_error and err_code output parameters.

  @note The session returned by the function must be properly closed using
        `mysqlx_session_close()`.
  @note This type of session does not support executing plain SQL queries.

  @ingroup xapi_sess
*/

PUBLIC_API mysqlx_session_t *
mysqlx_get_session_from_url(const char *conn_string,
                     char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code);


/**
  Create a session using session configuration data.

  @param opt  handle to session configuration data
  @param[out] out_error if error happens during connect the error message
                        is returned through this parameter
  @param[out] err_code if error happens during connect the error code
                        is returned through this parameter

  @return session handle if session could be created, otherwise NULL
  is returned and the error information is returned through
  the out_error and err_code output parameters.

  @note The session returned by the function must be properly closed using
        `mysqlx_session_close()`.
  @note This type of session does not support executing plain SQL queries.

  @ingroup xapi_sess
*/

PUBLIC_API mysqlx_session_t *
mysqlx_get_session_from_options(mysqlx_session_options_t *opt,
                       char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code);


/**
  Create a node session.

  A node session connects only to one mysqld node at a time.

  @param host       server host DNS name, IPv4 address or IPv6 address
  @param port       port number
  @param user       user name
  @param password   password
  @param database   default database name
  @param[out] out_error if error happens during connect the error message
                    is returned through this parameter
  @param[out] err_code if error happens during connect the error code
                    is returned through this parameter

  @return session handle if session could be created, otherwise NULL
  is returned and the error information is returned through the out_error
  and err_code output parameters.

  @note The session returned by the function must be properly closed using
        `mysqlx_session_close()`.
  @note This type of session supports executing plain SQL queries
  @note This function always establishes connection with SSL enabled

  @ingroup xapi_sess
*/

PUBLIC_API mysqlx_session_t *
mysqlx_get_node_session(const char *host, int port, const char *user,
                     const char *password, const char *database,
                     char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code);


/**
  Create a node session using connection string or URL.

  See `mysqlx_get_session_from_url()` for information on connection string
  format.

  A node session connects only to one mysqld node at a time.

  @param conn_string character connection string
  @param[out] out_error if error happens during connect the error message
                        is returned through this parameter
  @param[out] err_code if error happens during connect the error code
                        is returned through this parameter

  @return session handle if session could be created, otherwise NULL
    is returned and the error information is returned through the out_error
    and err_code output parameters.

  @note The session returned by the function must be properly closed using
    `mysqlx_session_close()`.

  @note This type of session supports executing plain SQL queries

  @see `mysqlx_get_session_from_url()`

  @ingroup xapi_sess
*/

PUBLIC_API mysqlx_session_t *
mysqlx_get_node_session_from_url(const char *conn_string,
                     char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code);


/**
  Create a node session using mysqlx_session_options_t structure

  @param opt pointer to mysqlx_session_options_t structure containing
             the connection parameters
  @param[out] out_error if error happens during connect the error message
                    is returned through this parameter
  @param[out] err_code if error happens during connect the error code
                    is returned through this parameter

  @return session handle if session could be created, otherwise NULL
    is returned and the error information is returned through the out_error
    and err_code output parameters.

  @note The session returned by the function must be properly closed using
    `mysqlx_session_close()`.

  @note This type of session supports executing plain SQL queries

  @ingroup xapi_sess
*/

PUBLIC_API mysqlx_session_t *
mysqlx_get_node_session_from_options(mysqlx_session_options_t *opt,
                            char out_error[MYSQLX_MAX_ERROR_LEN],
                            int *err_code);


/**
  Close the session.

  This function must be called by the user to prevent memory leaks.
  Closing session frees all related resources, including those
  allocated by statements and results belonging to the session.

  @param session session handle

  @ingroup xapi_sess
*/

PUBLIC_API void mysqlx_session_close(mysqlx_session_t *session);


/**
  Check the session validity.

  @param sess session handle

  @return 1 - if the session is valid, 0 - if the session is not valid

  @note The function checks only the internal session status without
    communicating with server(s).

  @ingroup xapi_sess
*/

PUBLIC_API int mysqlx_session_valid(mysqlx_session_t *sess);

/**
  Get a list of schemas.

  The result is returned as a set of rows with one column containing schema
  name. The rows can be read with functions such as `mysqlx_row_fetch_one()`,
  `mysqlx_store_result()` etc.

  @param sess session handle
  @param schema_pattern schema name pattern to search, using "%" as a wildcard
         character; if this parameter is NULL then all schemas will be
         returned.

  @return handle to the result with rows containing schema names.
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @ingroup xapi_sess
*/

PUBLIC_API mysqlx_result_t *
mysqlx_get_schemas(mysqlx_session_t *sess, const char *schema_pattern);


/**
  Get a schema object and optionally check if it exists on the server.

  @param sess  session handle
  @param schema_name name of the schema
  @param check flag to verify if the schema with the given name
         exists on the server (1 - check, 0 - do not check)

  @return handle to the schema object or NULL
          if an error occurred or the schema does not exist on the server

  @note Performing existence check involves communication with server(s).
  Without the check, this operation is executed locally. It is then possible
  to create a handle to a non-existent schema. Attempt to use such
  a handle later would eventually trigger an error.

  @ingroup xapi_sess
*/

PUBLIC_API mysqlx_schema_t *
mysqlx_get_schema(mysqlx_session_t *sess, const char *schema_name,
                  unsigned int check);


/**
  Get a list of tables and views in a schema.

  The result is returned as a set of rows with two columns. The first column
  contains table/view name, the second column contains object type, either
  "TABLE" or "VIEW". The rows can be read with functions such as
  `mysqlx_row_fetch_one()`, `mysqlx_store_result()` etc.

  @param schema schema handle
  @param table_pattern table name pattern to search, using "%" as a wildcard
         character; if this parameter is NULL then all tables/views in the
         given schema will be returned.
  @param get_views flag specifying whether view names should be included
         into the result. 0 - do not show views (only table names are in
         the result), 1 - show views (table and view names are in the result)

  @return handle to the result with rows containing table/view names.
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @note this function does not return names of tables that represent
        collections, use `mysqlx_get_collections()` function for getting
        collections.

  @ingroup xapi_sess
*/

PUBLIC_API mysqlx_result_t *
mysqlx_get_tables(mysqlx_schema_t *schema,
                  const char *table_pattern,
                  int get_views);


/**
  Get a table object and optionally check if it exists in the schema

  @param schema schema handle
  @param tab_name name of the table
  @param check flag to verify if the table with the given name
         exists in the schema (1 - check, 0 - do not check)

  @return handle to the table or NULL
          if an error occurred or the table does not exist in the schema

  @note Performing existence check involves communication with server(s).
  Without the check, this operation is executed locally. It is then possible
  to create a handle to a non-existent table. Attempt to use such
  a handle later would eventually trigger an error.

  @ingroup xapi_sess
*/

PUBLIC_API mysqlx_table_t *
mysqlx_get_table(mysqlx_schema_t *schema, const char *tab_name,
                      unsigned int check);


/**
  Get a list of collections in a schema.

  The result is returned as a set of rows with two columns. The first column
  contains collection name and the second column contains string "COLLECTION".
  The rows can be read with functions such as `mysqlx_row_fetch_one()`,
  `mysqlx_store_result()` etc.

  @param schema handle
  @param col_pattern collection name pattern to search, using "%" as a wildcard
         character; if this parameter is NULL then all collections in the given
         schema will be returned.

  @return handle to the result with rows containing collection names.
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @ingroup xapi_sess
*/

PUBLIC_API mysqlx_result_t *
mysqlx_get_collections(mysqlx_schema_t *schema,
                  const char *col_pattern);


/**
  Get a collection object and optionally check if it exists in the schema

  @param schema schema handle
  @param col_name name of the collection
  @param check flag to verify if the collection with the given name
         exists in the schema (1 - check, 0 - do not check)

  @return handle to the collection or NULL
          if an error occurred or the collection does not exist in the schema

  @note Performing existence check involves communication with server(s).
  Without the check, this operation is executed locally. It is then possible
  to create a handle to a non-existent collection. Attempt to use such
  a handle later would eventually trigger an error.

  @ingroup xapi_sess
*/

PUBLIC_API mysqlx_collection_t *
mysqlx_get_collection(mysqlx_schema_t *schema, const char *col_name,
                      unsigned int check);


/**
  Begin a transaction for the session.

  @param sess session handle

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note a statement will belong to the transaction when
        it is actually executed after the transaction began (and before
        it is committed or rolled back) even if this statement
        was created before `mysqlx_transaction_begin()` call

  @ingroup xapi_sess
*/

PUBLIC_API int
mysqlx_transaction_begin(mysqlx_session_t *sess);


/**
  Commit a transaction for the session.

  @param sess session handle

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note This will commit all statements that were executed as part of this
        transaction, regardless of when the statements were created (see
        `mysqlx_transaction_begin()`).

  @ingroup xapi_sess
*/

PUBLIC_API int
mysqlx_transaction_commit(mysqlx_session_t *sess);


/**
  Rollback a transaction for the session.

  @param sess session handle

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note This will roll back all statements that were executed as part of this
        transaction, regardless of when the statements were created (see
        `mysqlx_transaction_begin()`).

  @ingroup xapi_sess
*/

PUBLIC_API int
mysqlx_transaction_rollback(mysqlx_session_t *sess);


/**
  Allocate a new session configuration data object.

  @return handle to the newly allocated configuration data

  @note The allocated object must be eventually freed by
        `mysqlx_free_options()` to prevent memory leaks

  @ingroup xapi_sess
*/

PUBLIC_API mysqlx_session_options_t * mysqlx_session_options_new();


/**
  Free a session configuration data object.

  @param opt handle to sessin configuartion data object
             that has to be freed

  @ingroup xapi_sess
*/

PUBLIC_API void mysqlx_free_options(mysqlx_session_options_t *opt);


/**
  Set session configuration options.

  @param opt   handle to session configuration data object
  @param type  option type to set (see `mysqlx_opt_type_t` enum)
  @param ...   option value/values to set, as appropriate for the given
               option type (the function can set more than one value)

  TODO: Point to documentation explaining what data each option expects

  @return `RESULT_OK` if option was successfully set; `RESULT_ERROR`
          is set otherwise (use `mysqlx_error()` to get the error
          information)

  @ingroup xapi_sess
*/

PUBLIC_API int
mysqlx_session_option_set(mysqlx_session_options_t *opt, mysqlx_opt_type_t type, ...);


/**
  Read session configuration options.

  @param opt   handle to session configuration data object
  @param type  option type to get (see `mysqlx_opt_type_t` enum)
  @param[out] ...  pointer to a buffer where to return the requested
                   value

  TODO: Point to documentation explaining what data is returned for
  each option.

  @return `RESULT_OK` if option was successfully read; `RESULT_ERROR`
          is set otherwise (use `mysqlx_error()` to get the error
          information)

  @ingroup xapi_sess
*/

PUBLIC_API int
mysqlx_session_option_get(mysqlx_session_options_t *opt, mysqlx_opt_type_t type,
                          ...);


/*
  ====================================================================
  SQL execution
  ====================================================================
*/

/**
  Execute a plain SQL query.

  @param sess session handle
  @param query SQL query
  @param query_len length of the query. For NULL-terminated query strings
                `MYSQLX_NULL_TERMINATED` can be specified instead of the
                actual length

  @return handle to the query results.
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @ingroup xapi_sql
*/

PUBLIC_API mysqlx_result_t * mysqlx_sql(mysqlx_session_t *sess,
                                        const char *query,
                                        size_t query_len);


/**
  Execute a plain SQL query with parameters.

  @param sess session handle
  @param query SQL query
  @param query_len length of the query. For NULL-terminated query strings
                `MYSQLX_NULL_TERMINATED` can be specified instead of the
                actual length
  @param   ...  variable parameters list consisting of (type, value) pairs
          terminated by `PARAM_END`: type_id1, value1, type_id2, value2, ...,
          type_id_n, value_n, `PARAM_END` (`PARAM_END` marks the end of
          the parameters list).

           type_id is the numeric identifier, which helps to determine the type
           of the value provided as the next parameter. The user code must
           ensure that type_id corresponds to the actual value type. Otherwise,
           the value along with and all sequential types and values are most
           likely to be corrupted.
           Allowed types are listed in `mysqlx_data_type_t` enum.
           The XAPI defines the convenience macros that help to specify
           the types and values: See `PARAM_SINT()`, `PARAM_UINT()`,
           `PARAM_FLOAT()`, `PARAM_DOUBLE()`, `PARAM_BYTES()`,
           `PARAM_STRING()`.

  @return handle to the query results.
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @ingroup xapi_sql
*/

PUBLIC_API mysqlx_result_t * mysqlx_sql_param(mysqlx_session_t *sess,
                                        const char *query,
                                        size_t query_len, ...);


/**
  Create a statement which executes a plain SQL query.

  @note The query can contain `?` placeholders whose values should be
    specified using `mysqlx_stmt_bind()` function before executing
    the statement.

  @param sess session handle
  @param query SQL query
  @param length length of the query

  @return statement handle containing the results and/or error.
    NULL can be returned only in case when there are problems
    allocating memory, which normally should not happen.
    It is very unlikely for this function to end with an error
    because it does not do any parsing, parameter checking etc.

  @note To actually execute the SQL query the returned statement has to be
    given to `mysqlx_execute()`.

  @see mysqlx_stmt_bind()

  @ingroup xapi_sql
*/

PUBLIC_API mysqlx_stmt_t *
mysqlx_sql_new(mysqlx_session_t *sess, const char *query,
                 uint32_t length);


/*
  ====================================================================
  Collection operations
  ====================================================================
*/

/**
  Execute a collection FIND statement with a specific find
  criteria.

  @param collection collection handle
  @param criteria criteria for finding documents; if this parameter is
                  NULL then all documents are returned

  @return handle to the query results.
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @ingroup xapi_coll
*/

PUBLIC_API mysqlx_result_t *
mysqlx_collection_find(mysqlx_collection_t *collection, const char *criteria);


/**
  Add a set of new documents to a collection.

  Each document is defined by a JSON string like
  "{ \"key_1\\": value_1, ..., \"key_N\\": value_N }"

  @param collection collection handle
  @param ... list of parameters containing the character JSON strings
             describing documents to be added. Each parameter
             is a separate document. The list has to be terminated by
             PARAM_END macro

  @return handle to the statement result.
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  Each document must have a unique identifier which is stored in `_id`
  field of the document. Document identifiers are character strings no longer
  than 32 characters. If added document does not have `_id` field, a unique
  identifier will be generated for it. Document identifier generated by given
  collection add operation can be examined using `mysqlx_fetch_doc_id()`
  function. Generated document identifiers are strings of 32 hexadecimal digits,
  like this one `0512020981044082E6119DFA0E4C0584`.

  @note Generated document identifiers are based on UUIDs but they are not
  valid UUIDs (fields are reversed).

  @see `mysqlx_collection_add_new()`

  @ingroup xapi_coll
*/

PUBLIC_API mysqlx_result_t *
mysqlx_collection_add(mysqlx_collection_t *collection, ...);


/**
  Remove documents from a collection.

  @param collection collection handle
  @param criteria criteria selecting documents to be removed; if this parameter
                  is NULL, all documents are removed

  @return handle to the statement result.
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @ingroup xapi_coll
*/

PUBLIC_API mysqlx_result_t *
mysqlx_collection_remove(mysqlx_collection_t *collection, const char*criteria);


/**
  Modify documents in the collection.

  @param collection collection handle
  @param criteria criteria selecting documents to be modified; if this
         parameter is NULL then all documents are modified
  @param ... list of parameters that come as triplets
        <field_path, value_type, value>
        Each triplet specifies a field inside a document that should
        be modified (field_path) and the new value for that field.
        The value_type is the type identifier
        for the data type of value (see `mysqlx_data_type_t` enum)
        The list is terminated by `PARAM_END`.
        For `MYSQLX_TYPE_BYTES` there will be one extra parameter specifying
        the length of the binary data:
        <field_path, MYSQLX_TYPE_BYTES, value, length>
        The XAPI defines the convenience macros that help to specify
        the types and values: See `PARAM_SINT()`, `PARAM_UINT()`,
        `PARAM_FLOAT()`, `PARAM_DOUBLE()`, `PARAM_BYTES()`,
        `PARAM_STRING()`, `PARAM_EXPR()`:

        ..., "a_key", PARAM_STRING("New Text Value"),
             "b_key", PARAM_EXPR("b_key-1000"),
              PARAM_END

  @return handle to the statement result
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @ingroup xapi_coll
*/

PUBLIC_API mysqlx_result_t *
mysqlx_collection_modify_set(mysqlx_collection_t *collection,
                             const char *criteria, ...);


/**
  Unset fields in documents from the collection.

  @param collection collection handle
  @param criteria criteria selecting documents to be modified; if this
         parameter is NULL then all documents are modified
  @param ... list of field paths that should be unset;
             The list end is marked using `PARAM_END`

  @return handle to the statement result.
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @ingroup xapi_coll
*/

PUBLIC_API mysqlx_result_t *
mysqlx_collection_modify_unset(mysqlx_collection_t *collection,
                               const char *criteria, ...);


/*
  Deferred statement execution
  ----------------------------
*/

/**
  Create a statement which finds documents in a collection

  @param collection collection handle

  @return handle for the newly created FIND statement.
    NULL can be returned only in case when there are problems
    allocating memory, which normally should not happen.
    It is very unlikely for this function to end with an error
    because it does not do any parsing, parameter checking etc.

  @note To actually execute the operation, use `mysqlx_execute()`.

  @ingroup xapi_coll
*/

PUBLIC_API mysqlx_stmt_t *
mysqlx_collection_find_new(mysqlx_collection_t *collection);


/**
  Specify a projection for a collection find query

  The projection, if present, specifies mapping from documents found by the
  query to new documents returned in the final result.

  @param stmt statement handle
  @param proj projection specification describing JSON document projections as
    "{ \"proj1\\": expr1, ..., \"projN\\": exprN }". Expressions used
    in the projection can refer to fields in the original document
    using `$.path.to.field` syntax.

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note This function can be only called for the collection FIND statements
  @see mysqlsx_collection_find_new()

  @ingroup xapi_coll
*/

PUBLIC_API int mysqlx_set_find_projection(mysqlx_stmt_t *stmt, const char *proj);


/**
  A macro defining a function for setting criteria for FIND operation.

  @see mysqlx_set_where()

  @ingroup xapi_coll
*/

#define mysqlx_set_find_criteria mysqlx_set_where


/*
A macro defining a function for setting HAVING for FIND operation.

@see mysqlx_set_having()
@ingroup xapi_tbl
*/

#define mysqlx_set_find_having mysqlx_set_having


/**
A macro defining a function for setting GROUP BY for FIND operation.

@see mysqlx_set_group_by()
@ingroup xapi_tbl
*/

#define mysqlx_set_find_group_by mysqlx_set_group_by


/**
  A macro defining a function for setting LIMIT for DELETE operation.

  @see mysqlx_set_limit_and_offset()
  @ingroup xapi_coll
*/

#define mysqlx_set_find_limit_and_offset(STMT, LIM, OFFS) mysqlx_set_limit_and_offset(STMT, LIM, OFFS)


/**
  A macro defining a function for setting ORDER BY for SELECT operation.

  @see mysqlx_set_order_by()
  @ingroup xapi_coll
*/

#define mysqlx_set_find_order_by mysqlx_set_order_by


/**
  Create a statement which adds documents to a collection

  @param collection collection handle

  @return handle for the newly created ADD statement.
  NULL can be returned only in case when there are problems
  allocating memory, which normally should not happen.
  It is very unlikely for this function to end with an error
  because it does not do any parsing, parameter checking etc.

  @note To actually execute the operation, use `mysqlx_execute()` after
  specifying documents to be added.

  @ingroup xapi_coll
*/

PUBLIC_API mysqlx_stmt_t *
mysqlx_collection_add_new(mysqlx_collection_t *collection);


/**
  Specify a document to be added to a collection.

  The function provides the document data for the ADD statement as
  a JSON string like "{ \"key_1\\": value_1, ..., \"key_N\\": value_N }"
  User code must ensure the validity of the document because it is
  not checked until receiving the query on the server side.

  @note  key names and string values in a JSON string must be given
    in quotes and such quotes need to be escaped.

  @param stmt statement handle
  @param json_doc - the JSON string describing document to add
  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note Each new call provides the values for the new document, which
    can be used for multi-document add operations.
  @note User can provide document id as a value of `_id` field, otherwise
    document id is generated by the add operation. Document id must be
    a string - setting `_id` to a non-string value triggers
    an error.

  @ingroup xapi_coll
*/

PUBLIC_API int
mysqlx_set_add_document(mysqlx_stmt_t *stmt, const char *json_doc);


/**
  Create a statement which removes documents from a collection.

  @param collection collection handle

  @return handle for the newly created REMOVE statement.
          NULL can be returned only in case when there are problems
          allocating memory, which normally should not happen.
          It is very unlikely for this function to end with an error
          because it does not do any parsing, parameter checking etc.

  @note To actually execute the statement, use `mysqlx_execute()`

  @ingroup xapi_coll
*/

PUBLIC_API mysqlx_stmt_t *
mysqlx_collection_remove_new(mysqlx_collection_t *collection);


/**
  A macro defining a function for setting WHERE for REMOVE operation.

  @see mysqlx_set_where()
  @ingroup xapi_coll
*/

#define mysqlx_set_remove_criteria mysqlx_set_where


/**
  A macro defining a function for setting ORDER BY for REMOVE operation.

  @see mysqlx_set_order_by()
  @ingroup xapi_coll
*/

#define mysqlx_set_remove_order_by mysqlx_set_order_by


/**
  A macro defining a function for setting LIMIT for REMOVE operation.

  @see mysqlx_set_limit_and_offset()
  @ingroup xapi_coll
*/

#define mysqlx_set_remove_limit_and_offset mysqlx_set_limit_and_offset


/**
  Create a statement which modifies documents in a collection.

  @param collection collection handle

  @return handle for the newly created MODIFY statement.
          NULL can be returned only in case when there are problems
          allocating memory, which normally should not happen.
          It is very unlikely for this function to end with an error
          because it does not do any parsing, parameter checking etc.

  @note To actually execute the statement, use `mysqlx_execute()` after
        specifying modifications that should be performed.

  @ingroup xapi_coll
*/

PUBLIC_API mysqlx_stmt_t *
mysqlx_collection_modify_new(mysqlx_collection_t *collection);


/**
  Set fields in a document to given values.

  @param stmt handle to MODIFY statement
  @param ... list of parameters that come as triplets
        <field_path, value_type, value>
        Each triplet represents a value inside a document that can
        be located by field_path. The value_type is the type identifier
        for the data type of value (see `mysqlx_data_type_t` enum)
        The list is terminated by `PARAM_END`.
        For `MYSQLX_TYPE_BYTES` there will be one extra parameter specifying
        the length of the binary data:
        <field_path, `MYSQLX_TYPE_BYTES`, value, length>

  @note For the convenience the code can use `PARAM_TTT(val)` macros
        instead of (`MYSQLX_TYPE_TTT`, value) pairs (see `mysqlx_stmt_bind()`).

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @ingroup xapi_coll
*/

PUBLIC_API int
mysqlx_set_modify_set(mysqlx_stmt_t *stmt, ...);


/**
  Unset fields in a document

  @param stmt handle to MODIFY statement
  @param ... list of paths to the documents fields that should be unset. Each
        entry in this list is a character string.
        The list is terminated by `PARAM_END`.

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @ingroup xapi_coll
*/

PUBLIC_API int
mysqlx_set_modify_unset(mysqlx_stmt_t *stmt, ...);


/**
  Insert elements into array fields in a document

  @param stmt handle to MODIFY statement
  @param ... list of parameters that come as triplets
        <field_path, value_type, value>
        Each triplet represents a position in an array field of a document,
        given by field_path, and a value to be inserted in that position.
        The value_type is the type identifier for the data type of value
        (see `mysqlx_data_type_t` enum). The list is terminated by `PARAM_END`.

  @note For the convenience the code can use `PARAM_TTT(val)` macros
        instead of (`MYSQLX_TYPE_TTT`, value) pairs (see `mysqlx_stmt_bind()`).

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @ingroup xapi_coll
*/

PUBLIC_API int
mysqlx_set_modify_array_insert(mysqlx_stmt_t *stmt, ...);


/**
  Append to array fields in a document

  @param stmt handle to MODIFY statement
  @param ... list of parameters that come as triplets
        <field_path, value_type, value>. Each triplet specifies an array
        field in a document, given by field_path, and a value that should
        be appended to that array. The value_type is the type identifier
        for the data type of value (see `mysqlx_data_type_t` enum).
        The list is terminated by `PARAM_END`.

  @note For the convenience the code can use `PARAM_TTT(val)` macros
        instead of (`MYSQLX_TYPE_TTT`, value) pairs (see `mysqlx_stmt_bind()`).

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @ingroup xapi_coll
*/

PUBLIC_API int
mysqlx_set_modify_array_append(mysqlx_stmt_t *stmt, ...);


/**
  Delete elements from array fields in a document

  @param stmt handle to MODIFY statement
  @param ... list of paths to array elements that should be deleted from their
        arrays. The list is terminated by `PARAM_END`.

   @return `RESULT_OK` - on success; `RESULT_ERR` - on error

   @ingroup xapi_coll
*/

PUBLIC_API int mysqlx_set_modify_array_delete(mysqlx_stmt_t *stmt, ...);


/**
  A macro defining a function for setting WHERE for MODIFY operation.

  @see mysqlx_set_where()
  @ingroup xapi_coll
*/

#define mysqlx_set_modify_criteria mysqlx_set_where


/*
  ====================================================================
  Table operations
  ====================================================================
*/


/**
  Execute a table SELECT statement with a WHERE clause.

  All columns will be selected.

  @param table table handle
  @param criteria row selection criteria (WHERE clause); if NULL then
         all rows in the table are returned.

  @return handle to the query results
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @ingroup xapi_tbl
*/

PUBLIC_API mysqlx_result_t *
mysqlx_table_select(mysqlx_table_t *table, const char *criteria);


/**
  Execute a table SELECT statement with a WHERE,
  ORDER BY and LIMIT clauses

  @param table table handle
  @param criteria row selection criteria (WHERE clause); if NULL then all
         rows in the table will be selected.
  @param row_count a number of rows to return (LIMIT clause)
  @param offset number of rows to skip (an offset for the LIMIT clause)
  @param  ... sorting specification - variable parameters list consisting of
          (expression, direction) pairs terminated by `PARAM_END`: expr_1,
          direction_1, ..., expr_n, direction_n, `PARAM_END`.
          Each expression computes a value used to sort
          the rows/documents in ascending or descending order,
          as determined by direction constant
          (TODO: list the direction enum names).
          Special attention must be paid to the expression
          strings because the empty string "" or NULL will be treated
          as the end of sequence

  @return handle to the query results.
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @ingroup xapi_tbl
*/

PUBLIC_API mysqlx_result_t *
mysqlx_table_select_limit(mysqlx_table_t *table, const char *criteria,
                               uint64_t row_count, uint64_t offset, ...);


/**
  Execute a table INSERT statement with one row.

  @param table table handle
  @param ... list of column-value specifications consisting of
             <column_name, value_type, value> triplets. The list
             should be terminated using `PARAM_END`.
             Allowed value types are listed in `mysqlx_data_type_t` enum.
             The XAPI defines the convenience macros that help to specify
             the types and values: See `PARAM_SINT()`, `PARAM_UINT()`,
             `PARAM_FLOAT()`, `PARAM_DOUBLE()`, `PARAM_BYTES()`,
             `PARAM_STRING()`:

             ..., "col_uint", PARAM_UINT(uint_val),
                  "col_blob", PARAM_BYTES(byte_buf, buf_len),
                   PARAM_END

  @return handle to the query results.
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @ingroup xapi_tbl
*/

PUBLIC_API mysqlx_result_t *
mysqlx_table_insert(mysqlx_table_t *table, ...);


/**
  Execute a table DELETE statement with a WHERE clause.

  @param table table handle
  @param criteria expression selecting rows to be deleted; if this
         parameter is NULL all rows are deleted

  @return handle to the query results.
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @ingroup xapi_tbl
*/

PUBLIC_API mysqlx_result_t *
mysqlx_table_delete(mysqlx_table_t *table, const char *criteria);



/**
  Execute a table UPDATE statement.

  @param table table handle
  @param criteria expression selecting rows to be updated (WHERE clause)
  @param ... list of column-value specifications consisting of
             <column_name, value_type, value> triplets. The list
             should be terminated using `PARAM_END`.
             Allowed value types are listed in `mysqlx_data_type_t` enum.
             The XAPI defines the convenience macros that help to specify
             the types and values: See `PARAM_SINT()`, `PARAM_UINT()`,
             `PARAM_FLOAT()`, `PARAM_DOUBLE()`, `PARAM_BYTES()`,
             `PARAM_STRING()`, `PARAM_EXPR()`:

             ..., "col_uint", PARAM_EXPR("col_uint * 100"),
                  "col_blob", PARAM_BYTES(byte_buf, buf_len),
                   PARAM_END

  @return handle to the query results.
          NULL is returned only in case of an error. The error details
          can be obtained using `mysqlx_error()` function

  @ingroup xapi_tbl
*/

PUBLIC_API mysqlx_result_t *
mysqlx_table_update(mysqlx_table_t *table,
                    const char *criteria,
                    ...);


/*
  Deferred statement execution
  ----------------------------
*/

/**
  Create a statement which performs a table SELECT operation.

  @param table table handle

  @return handle to the newly created SELECT statement.
    NULL can be returned only in case when there are problems
    allocating memory, which normally should not happen.
    It is very unlikely for this function to end with an error
    because it does not do any parsing, parameter checking etc.

  @note To actually execute the statement, the returned handle has to be
    given to `mysqlx_execute()`.

  @see mysqlx_set_insert_columns(), mysqlx_set_insert_row()

  @ingroup xapi_tbl
*/

PUBLIC_API mysqlx_stmt_t *
mysqlx_table_select_new(mysqlx_table_t *table);


/**
  A macro defining a function for setting projections for SELECT operation.

  @see mysqlx_set_items()
  @ingroup xapi_tbl
*/

#define mysqlx_set_select_items mysqlx_set_items


/**
  A macro defining a function for setting WHERE for SELECT operation.

  @see mysqlx_set_where()
  @ingroup xapi_tbl
*/

#define mysqlx_set_select_where mysqlx_set_where


/**
  A macro defining a function for setting ORDER BY for SELECT
  operation.

  @see mysqlx_set_order_by()
  @ingroup xapi_tbl
*/

#define mysqlx_set_select_order_by mysqlx_set_order_by


/*
  A macro defining a function for setting HAVING for SELECT operation.

  @see mysqlx_set_having()
  @ingroup xapi_tbl
*/

#define mysqlx_set_select_having mysqlx_set_having


/**
A macro defining a function for setting GROUP BY for SELECT operation.

@see mysqlx_set_group_by()
@ingroup xapi_tbl
*/

#define mysqlx_set_select_group_by mysqlx_set_group_by


/**
  A macro defining a function for setting LIMIT for SELECT operation.

  @see mysqlx_set_limit_and_offset()
  @ingroup xapi_tbl
*/

#define mysqlx_set_select_limit_and_offset mysqlx_set_limit_and_offset


/**
  Create a statement executing a table INSERT operation.

  @param table table handle

  @return statement handle for the newly created INSERT operation.
    NULL can be returned only in case when there are problems
    allocating memory, which normally should not happen.
    It is very unlikely for this function to end with an error
    because it does not do any parsing, parameter checking etc.

  @note To actually execute the SQL query the returned Statement has to be
    given to `mysqlx_execute()`

  @ingroup xapi_tbl
*/

PUBLIC_API mysqlx_stmt_t *
mysqlx_table_insert_new(mysqlx_table_t *table);


/**
  Specify column names for an INSERT statement.

  The function specifies the names of the columns into which the statement
  will insert data. User code must ensure that the column values are correct
  because the names are not validated until receiving the query on
  the server side after executing with `mysqlx_execute()`.

  @param stmt statement handle
  @param ...  variable parameters list consisting of column names; the list is
              terminated by PARAM_END.
  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note Each new call clears the list of column for a given statement
        if it was set earlier.
  @note If column names are not specified for an insert statement, it will
        insert data into all columns of the table.

  @ingroup xapi_tbl
*/

PUBLIC_API int
mysqlx_set_insert_columns(mysqlx_stmt_t *stmt, ...);


/**
  Specify a row to be added by an INSERT statement.

  The function provides the row data for an INSERT statement.
  User code must ensure that the number of values and their order matches
  the list of columns specified for the operation. If column names were not
  explicitly specified, the values must match the columns of the table.

  @param stmt statement handle
  @param ...  variable parameters list consisting of (type, value) pairs
          terminated by PARAM_END. The pairs must be listed in the order they
          appear in the list of columns
          For MYSQLX_TYPE_BYTES the function will expect three parameters
          instead of two as for all other types:
          <MYSQLX_TYPE_BYTES, (void*)byte_data, (size_t)length>
  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note Each new call provides the row values for the new row, which
        can be used for multi-row inserts

  @ingroup xapi_stmt
*/

PUBLIC_API int
mysqlx_set_insert_row(mysqlx_stmt_t *stmt, ...);


/**
  Create a statement executing a table DELETE operation.

  @param table table handle

  @return handle for the newly created DELETE statement.
    NULL can be returned only in case when there are problems
    allocating memory, which normally should not happen.
    It is very unlikely for this function to end with an error
    because it does not do any parsing, parameter checking etc.

  @note To actually execute the statement, use `mysqlx_execute()`.

  @see mysqlx_set_delete_where(), mysqlx_set_delete_limit(),
    mysqlx_set_delete_order_by()

  @ingroup xapi_tbl
*/

PUBLIC_API mysqlx_stmt_t *
mysqlx_table_delete_new(mysqlx_table_t *table);


/**
  A macro defining a function for setting WHERE clause for DELETE operation.

  @see mysqlx_set_where()
  @ingroup xapi_tbl
*/

#define mysqlx_set_delete_where mysqlx_set_where

/**
  A macro defining a function for setting LIMIT for DELETE operation.

  @see mysqlx_set_limit_and_offset()
  @ingroup xapi_tbl
*/

#define mysqlx_set_delete_limit(STMT, LIM) mysqlx_set_limit_and_offset(STMT, LIM, 0)

/**
  A macro defining a function for setting ORDER BY for DELETE operation.

  @see mysqlx_set_order_by()
  @ingroup xapi_tbl
*/

#define mysqlx_set_delete_order_by mysqlx_set_order_by


/**
  Create a statement executing a table UPDATE operation.

  @param table table handle

  @return handle for the newly created UPDATE statement.
    NULL can be returned only in case when there are problems
    allocating memory, which normally should not happen.
    It is very unlikely for this function to end with an error
    because it does not do any parsing, parameter checking etc.

  @note To actually execute the statement, use `mysqlx_execute()` after
    specifying what updates should it perform.

  @see mysqlx_set_update_values(), mysqlx_set_update_where(),
       mysqlx_set_update_limit(), mysqlx_set_update_order_by()

  @ingroup xapi_tbl
*/

PUBLIC_API mysqlx_stmt_t *
mysqlx_table_update_new(mysqlx_table_t *table);


/**
  Set values for the columns in the UPDATE statement.

  @param stmt statement handle
  @param  ... variable parameters list consisting of triplets
          <column_name, value_type, value_or_expression>
          representing column names, value types and values as
          expressions. The list is terminated by `PARAM_END`:
          column_1, type_1, val_1, ..., column_n, type_n, val_n, `PARAM_END`.
          The value type is defined in `mysqlx_data_type_t` enum.
          If the value is to be computed on the server side the type
          has to be set to `MYSQLX_TYPE_EXPR`. The value (expression)
          should be specified as a character string expression.
          For `MYSQLX_TYPE_BYTES` the function will expect four parameters
          instead of three as for all other types:
          <column_name, `MYSQLX_TYPE_BYTES`, (void*)byte_data, (size_t)length>

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note All fields and their corresponding expressions must be set in one call
        otherwise the next call to this function will reset all parameters to
        their new values.

  @ingroup xapi_tbl
*/

PUBLIC_API int mysqlx_set_update_values(mysqlx_stmt_t *stmt, ...);


/**
  A macro defining a function for setting WHERE clause for UPDATE operation.

  @see mysqlx_set_where()
  @ingroup xapi_tbl
*/

#define mysqlx_set_update_where mysqlx_set_where


/**
  A macro defining a function for setting LIMIT for UPDATE operation.

  @see mysqlx_set_limit_and_offset()
  @ingroup xapi_tbl
*/

#define mysqlx_set_update_limit(STMT, LIM) mysqlx_set_limit_and_offset(STMT, LIM, 0)


/**
  A macro defining a function for setting ORDER BY clause for UPDATE
  operation.

  @see mysqlx_set_oder_by()
  @ingroup xapi_tbl
*/

#define mysqlx_set_update_order_by mysqlx_set_order_by


/*
  ====================================================================
  Statement execution
  ====================================================================
*/

/**
  Execute a statement

  Executes statement created by `mysqlx_table_select_new()`,
  `mysqlx_table_insert_new()`, `mysqlx_table_update_new()`,
  `mysqlx_table_delete_new()`, `mysqlx_sql_new()`, etc.

  @param stmt statement handle

  @return  handle that can be used to access results
           of the operation. Returned handle is valid until the statement
           handle is freed (when session is closed or explicitly with
           `mysqlx_free()`) or until another call to `mysqlx_execute()`
           on the same statement handle is made. It is also possible to close
           a result handle and free all resources used by it earlier with
           `mysqlx_result_free()` call.
           On error NULL is returned. The statement is set to an error state and
           errors can be examined using the statement handle.

  @ingroup xapi_stmt
*/

PUBLIC_API mysqlx_result_t *
mysqlx_execute(mysqlx_stmt_t *stmt);


/**
  Bind values for parametrized statements.

  This function binds values of either `?` placeholders in an SQL statement
  or of named parameters that can be used in other statements.

  User code must ensure that the number of values in bind is the same
  as the number of parameters in the query because this is not checked
  until receiving the query on the server side.

  @param stmt statement handle
  @param ... variable parameters list, which has different structure for SQL
           statements that use placeholders and for other statements that use
           named parameters.

           For SQL statements it is consisting of (type, value) pairs
           terminated by `PARAM_END`: type_id1, value1, type_id2, value2, ...,
           type_id_n, value_n, `PARAM_END`.

           For SELECT, INSERT, UPDATE, DELETE, FIND, ADD, MODIFY and REMOVE
           statements, the parameters come as triplets (param_name, type,
           value): name1, type_id1, value1, name2, type_id2, value2, ...,
           name_n, type_id_n, value_n, `PARAM_END` (`PARAM_END` marks the end
           of the parameters list).

           type_id is the numeric identifier, which helps to determine the type
           of the value provided as the next parameter. The user code must
           ensure that type_id corresponds to the actual value type. Otherwise,
           the value along with and all sequential types and values are most
           likely to be corrupted.

           It is recommended to use `PARAM_TTT()` macros to keep the list
           integrity: `PARAM_UINT()`, `PARAM_SINT()`, `PARAM_FLOAT()`,
           `PARAM_DOUBLE()`, `PARAM_STRING()`, `PARAM_BYTES()`, `PARAM_EXPR()`
           for different data types instead of (`MYSQLX_TYPE_TTT`, value) pairs.

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note Each new call resets the binds set by the previous call to
        `mysqlx_stmt_bind()`

  @ingroup xapi_stmt
*/

PUBLIC_API int mysqlx_stmt_bind(mysqlx_stmt_t *stmt, ...);


/**
  Specify a table query projection.

  Using projection, rows found by the query can be mapped to a new set of
  rows which is returned in the final result. Projection is given by a list
  of expressions determining values of fields in the resulting rows. These
  expressions can refer to the fields in the original row (via column names
  of the original table).

   @param stmt handle to the statement for which the projection is set
   @param  ... variable parameters list consisting of character strings
          containing expressions: proj_1, ..., proj_n, PARAM_END
          (PARAM_END marks the end of projection's item list)

   @return `RESULT_OK` - on success; `RESULT_ERR` - on error

   @note This function can be only called for table SELECT statements
   @see mysqlx_table_select_new()

  @ingroup xapi_stmt
*/

PUBLIC_API int mysqlx_set_items(mysqlx_stmt_t *stmt, ...);


/**
  Specify selection criteria for a statement.

  Restrict the statement to rows/documents that satisfy
  given selection criteria:
    - for select/find operations limit the returned rows/documents,
    - for update/modify/delete/remove operations limit
      the rows/documents affected by the operations.

  Statements supported by this function: SELECT, FIND, UPDATE, MODIFY, DELETE,
  REMOVE. Calling it for INSERT or ADD will result in an error

  @param stmt statement handle
  @param where_expr character string containing Boolean expression
                    like in SQL WHERE clause

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note this function can be be used directly, but for the convenience
        the code can use the specialized macros for a specific operation.
        For SELECT operation the user code should use
        `mysqlx_set_select_where()` macros that map the
        corresponding `mysqlx_set_where()` function.
        This way the unsupported operations will not be used.

  @ingroup xapi_stmt
*/

PUBLIC_API int mysqlx_set_where(mysqlx_stmt_t *stmt, const char *where_expr);


/**
  Specify filter conditions for a group of rows/documents or aggregates
  such as GROUP BY

  Restrict the statement to rows/documents that satisfy
  given selection criteria:
    - for select/find operations limit the returned rows/documents,

  Statements supported by this function: SELECT, FIND.
  Calling it for UPDATE, MODIFY, DELETE, REMOVE, INSERT or ADD
  will result in an error

  @param stmt statement handle
  @param having_expr character string containing Boolean expression
                     like in SQL HAVING clause

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note this function can be be used directly, but for the convenience
        the code can use the specialized macros for a specific operation.
        For SELECT operation the user code should use
        `mysqlx_set_select_having()` macros that map the
        corresponding `mysqlx_set_having()` function.
        This way the unsupported operations will not be used.

  @ingroup xapi_stmt
*/

PUBLIC_API int mysqlx_set_having(mysqlx_stmt_t *stmt, const char *having_expr);


/**
  Specify one or more columns/values to group the result in conjunction
  with the aggregate functions.

  Statements supported by this function: SELECT, FIND.
  Calling it for UPDATE, MODIFY, DELETE, REMOVE, INSERT or ADD
  will result in an error

  @param stmt statement handle
  @param  ... variable parameters list consisting of character strings
  containing expressions specifying grouping:
  expr_1, ..., expr_n, PARAM_END
  (PARAM_END marks the end of projection's item list)

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note this function can be be used directly, but for the convenience
        the code can use the specialized macros for a specific operation.
        For SELECT operation the user code should use
        `mysqlx_set_select_group_by()` macros that map the
        corresponding `mysqlx_set_group_by()` function.
        This way the unsupported operations will not be used.

  @ingroup xapi_stmt
*/

PUBLIC_API int mysqlx_set_group_by(mysqlx_stmt_t *stmt, ...);

/**
  Specify ordering for a statement.

  Operations supported by this function:
  SELECT, FIND, UPDATE, MODIFY, DELETE, REMOVE
  Calling it for INSERT or ADD will result in an error

  @param stmt statement handle
  @param  ... variable parameters list consisting of (expression, direction)
          pairs terminated by `PARAM_END`: expr_1, direction_1, ..., expr_n,
          direction_n, `PARAM_END`.
          Each expression computes a value used to sort
          the rows/documents in ascending or descending order,
          as determined by direction constant
          (list the direction enum names).
          Special attention must be paid to the expression
          strings because the empty string "" or NULL will be treated
          as the end of sequence

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note this function can be be used directly, but for the convenience
        the code can use the specialized macros for a specific operation.
        For SELECT operation the user code should use
        `mysqlx_set_select_order_by()` macros that map the
        corresponding `mysqlx_set_order_by()` function.
        This way the unsupported operations will not be used.

  @ingroup xapi_stmt
*/

PUBLIC_API int mysqlx_set_order_by(mysqlx_stmt_t *stmt, ...);


/**
  Set limit and offset information for a statement.

  Set LIMIT and OFFSET for statement operations which work on ranges of
      rows/documents: for select/find operations limit the number of returned
      rows/documents, for update/delete limit the number of documents affected
      by the operation.

  Operations supported by this function:
    SELECT, FIND - use both LIMIT and OFFSET
    UPDATE, MODIFY, DELETE, REMOVE - use only LIMIT

  Calling it for INSERT or ADD will result in an error

  @param stmt statement handle
  @param row_count the number of result rows to return
  @param offset the number of rows to skip before starting counting

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error

  @note this function can be be used directly, but for the convenience
        the code can use the specialized macros for a specific operation.
        For SELECT operation the user code should use
        `mysqlx_set_select_limit_and_offset()` macros that map the
        corresponding `mysqlx_set_limit_and_offset()` function.
        This way the unsupported operations will not be used.

  @note Each call to this function replaces previously set LIMIT

  @ingroup xapi_stmt
*/

PUBLIC_API int
mysqlx_set_limit_and_offset(mysqlx_stmt_t *stmt, uint64_t row_count,
                            uint64_t offset);


/**
  Free the statement handle explicitly.

  @note Statement handles are also freed automatically when
  statement's session is closed.

  @param stmt statement handle

  @ingroup xapi_stmt
*/

PUBLIC_API void mysqlx_free(mysqlx_stmt_t *stmt);


/*
  ====================================================================
  Result handling
  ====================================================================
*/

/**
  Fetch one row from the result

  The result is advanced to the next row (if any).

  @param res result handle

  @return row handle or NULL if no more rows left or if an error
          occurred. In case of an error it can be retrieved from
          the result using `mysqlx_error()` or `mysqlx_error_message()`.

  @note The previously fetched row and its data will become invalid.

  @ingroup xapi_res
*/

PUBLIC_API mysqlx_row_t * mysqlx_row_fetch_one(mysqlx_result_t *res);


/**
  Fetch one document as a JSON string

  @param res result handle

  @param[out] out_length the total number of bytes in the JSON string;
              can be NULL, in that case nothing is returned through
              this parameter and user must ensure the data is correctly
              interpreted

  @return pointer to character JSON string or NULL if no more documents left
          in the result. No need to free this data as it is tied and freed
          with the result handle.

  @ingroup xapi_res
*/

PUBLIC_API const char * mysqlx_json_fetch_one(mysqlx_result_t *res, size_t *out_length);


/**
  Proceed to the next result set in the reply.

  This function is used to process replies containing multiple result sets.
  After a successful call to this function, given result handle will be moved
  to access the next result set from the reply.

  @note Any data from the previous result set that has not yet been fetched
  is no more accessible after moving to the next result set.

  @param res result handle

  @return `RESULT_OK` - on success; `RESULT_NULL` when there is no more results;
          `RESULT_ERR` - on error

  @ingroup xapi_res
*/

PUBLIC_API int mysqlx_next_result(mysqlx_result_t *res);


/**
  Get number of rows affected by a statement.

  @param res result handle

  @return the number of rows affected by the statement that produced the result

  @note The returned number is meaningful only for results of statements which
  modify data stored in a table or collection.

  @ingroup xapi_res
*/

PUBLIC_API uint64_t
mysqlx_get_affected_count(mysqlx_result_t *res);


/**
  Store result data in an internal buffer

  Rows/documents contained in a result must be fetched in a timely fashion.
  Failing to do that can result in an error and lost access to the
  remaining part of the result. This function can store complete result
  in memory so it can be accessed at any time, as long as the result
  handle is valid.

  @param result result handle
  @param[out] num number of records buffered

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error. If the error
          occurred it can be retrieved by `mysqlx_error()` function.

  @note Even in case of an error some rows/documents might be buffered if they
        were retrieved before the error occurred.

  @ingroup xapi_res
*/

PUBLIC_API int
mysqlx_store_result(mysqlx_result_t *result, size_t *num);


/**
  Get identifiers of the documents added to the collection.

  This function returns both generated document ids and document ids specified
  by user in `_id` field.

  The function can be used for the multi-document inserts. In this case each
  call to `mysqlx_fetch_doc_id()` returns identifier of the next document,
  until NULL is returned.

  @param result handle to a result of a statement which adds documents to
         a collection

  @return character string containing an identifier of a document added by the
          statement;  NULL - if all UUIDs for all added documents have been
          returned

  @note The returned string is valid as long as the result handle is valid.
        Starting a new operation will invalidate it.

  @ingroup xapi_res
*/

PUBLIC_API const char *
mysqlx_fetch_doc_id(mysqlx_result_t *result);


/**
  Get auto increment value generated by a statement that inserts rows
  into a table with auto increment column.

  @param res handle to a result of INSERT statement

  @return the generated auto increment value

  @note with multi-row inserts the function returns the value generated
        for the first row

  @ingroup xapi_res
*/

PUBLIC_API uint64_t
mysqlx_get_auto_increment_value(mysqlx_result_t *res);


/**
  Read bytes stored in a row into a pre-allocated buffer

  @param row row handle
  @param col zero-based column number
  @param offset the number of bytes to skip before reading them from source row
  @param[out] buf the buffer allocated on the user side into which to write data
  @param[in,out] buf_len pointer to a variable holding the length of the buffer
                  [IN], the number of bytes actually written into the
                  buffer [OUT]

  @return `RESULT_OK` - on success; `RESULT_NULL` when the value in the
          requested column is NULL; `RESULT_MORE_DATA` if not all data was
          fetched after the last call to the function;
          `RESULT_ERR` - on error

  @ingroup xapi_res
*/

PUBLIC_API int
mysqlx_get_bytes(mysqlx_row_t* row, uint32_t col,
                 uint64_t offset, void *buf, size_t *buf_len);


/**
  Get an unsigned integer number from a row.

  It is important to pay attention to the signed/unsigned type of the column.
  Attempting to call this function for a column whose type is different from
  `MYSQLX_TYPE_UINT` will result in wrong data being retrieved.

  @param row row handle
  @param col zero-based column number
  @param[out] val the pointer to a variable of the 64-bit unsigned integer
                  type in which to write the data

  @return `RESULT_OK` - on success; `RESULT_NULL` when the column is NULL;
          `RESULT_ERR` - on error

  @ingroup xapi_res
*/

PUBLIC_API int
mysqlx_get_uint(mysqlx_row_t* row, uint32_t col, uint64_t* val);


/**
  Get a signed integer number from a row.

  It is important to pay attention to the signed/unsigned type of the column.
  Attempting to call this function for a column whose type is different from
  `MYSQLX_TYPE_SINT` will result in wrong data being retrieved.

  @param row row handle
  @param col zero-based column number
  @param[out] val the pointer to a variable of the 64-bit signed integer
              type in which to write the data

  @return `RESULT_OK` - on success; `RESULT_NULL` when the column is NULL;
          `RESULT_ERR` - on error

  @ingroup xapi_res
*/

PUBLIC_API int
mysqlx_get_sint(mysqlx_row_t* row, uint32_t col, int64_t* val);


/**
  Get a float number from a row.

  It is important to pay attention to the type of the column.
  Attempting to call this function for a column whose type is different from
  `MYSQLX_TYPE_FLOAT` will result in wrong data being retrieved.

  @param row row handle
  @param col zero-based column number
  @param[out] val the pointer to a variable of the float
              type in which to write the data

  @return `RESULT_OK` - on success; `RESULT_NULL` when the column is NULL;
          `RESULT_ERR` - on error

  @ingroup xapi_res
*/

PUBLIC_API int
mysqlx_get_float(mysqlx_row_t* row, uint32_t col, float* val);


/**
  Get a double number from a row.

  It is important to pay attention to the type of the column.
  Attempting to call this function for a column whose type is different from
  `MYSQLX_TYPE_DOUBLE` will result in wrong data being retrieved.

  @param row row handle
  @param col zero-based column number
  @param[out] val the pointer to a variable of the double
                  type in which to write the data.

  @return `RESULT_OK` - on success; `RESULT_NULL` when the column is NULL;
          `RESULT_ERR` - on error

  @ingroup xapi_res
*/

PUBLIC_API int
mysqlx_get_double(mysqlx_row_t* row, uint32_t col, double *val);


/**
  Free the result explicitly.

  @note Results are also freed automatically when the corresponding
  statement handle is freed.

  @param res the result handle

  @ingroup xapi_res
*/

PUBLIC_API void mysqlx_result_free(mysqlx_result_t *res);


/*
  Result metadata
  ---------------
*/

/**
  Get column type identifier.

  @param res result handle
  @param pos zero-based column number

  @return column type identifier (see `mysqlx_data_type_t` enum)

  @ingroup xapi_md
*/

PUBLIC_API uint16_t
mysqlx_column_get_type(mysqlx_result_t *res, uint32_t pos);


/**
  Get column collation number.

  @param res result handle
  @param pos zero-based column number

  @return column collation number. TODO: specify these

  @ingroup xapi_md
*/

PUBLIC_API uint16_t
mysqlx_column_get_collation(mysqlx_result_t *res, uint32_t pos);


/**
  Get column length.

  @param res result handle
  @param pos zero-based column number

  @return maximum length of data in the column (as reported by server)

  @ingroup xapi_md
*/

PUBLIC_API uint32_t
mysqlx_column_get_length(mysqlx_result_t *res, uint32_t pos);


/**
  Get column precision.

  @param res result handle
  @param pos zero-based column number

  @return number of digits after the decimal point

  @ingroup xapi_md
*/

PUBLIC_API uint16_t
mysqlx_column_get_precision(mysqlx_result_t *res, uint32_t pos);


/**
  Get column flags.

  @param res result handle
  @param pos zero-based column number

  @return 32-bit unsigned integer containing column flags reported by
  server. TODO: Document these

  @ingroup xapi_md
*/

PUBLIC_API uint32_t
mysqlx_column_get_flags(mysqlx_result_t *res, uint32_t pos);


/**
  Get the number of columns in the result.

  @param res result handle

  @return the number of columns in the result

  @note If the result does not contain rows, 0 columns are reported.
  @note For a result with multiple result sets, the number of columns for
  the current result set is reported (see `mysqlx_next_result()`).

  @ingroup xapi_md
*/

PUBLIC_API uint32_t
mysqlx_column_get_count(mysqlx_result_t *res);


/**
  Get column name.

  @param res result handle
  @param pos zero-based column number

  @return character string containing the column name

  @ingroup xapi_md
*/

PUBLIC_API const char *
mysqlx_column_get_name(mysqlx_result_t *res, uint32_t pos);


/**
  Get column original name.

  @param res result handle
  @param pos zero-based column number

  @return character string containing the column original name

  @ingroup xapi_md
*/

PUBLIC_API const char *
mysqlx_column_get_original_name(mysqlx_result_t *res, uint32_t pos);


/**
  Get column's table name.

  @param res result handle
  @param pos zero-based column number

  @return character string containing the column table name

  @ingroup xapi_md
*/

PUBLIC_API const char *
mysqlx_column_get_table(mysqlx_result_t *res, uint32_t pos);


/**
  Get column's original table name.

  @param res result handle
  @param pos zero-based column number

  @return character string containing the column original table

  @ingroup xapi_md
*/

PUBLIC_API const char *
mysqlx_column_get_original_table(mysqlx_result_t *res, uint32_t pos);


/**
  Get column's schema name.

  @param res result handle
  @param pos zero-based column number

  @return character string containing the column schema

  @ingroup xapi_md
*/

PUBLIC_API const char *
mysqlx_column_get_schema(mysqlx_result_t *res, uint32_t pos);


/**
  Get column's catalog name.

  @param res result handle
  @param pos zero-based column number

  @return character string containing the column name

  @ingroup xapi_md
*/

PUBLIC_API const char *
mysqlx_column_get_catalog(mysqlx_result_t *res, uint32_t pos);


/*
  ====================================================================
  DDL statements
  ====================================================================
*/

/**
  Create a schema

  @param sess session handle
  @param schema the name of the schema to be created

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error
          The error handle can be obtained from the session
          using `mysqlx_error()` function.

  @ingroup xapi_ddl
*/

PUBLIC_API int
mysqlx_schema_create(mysqlx_session_t *sess, const char *schema);


/**
  Drop a schema

  @param sess session handle
  @param schema the name of the schema to be dropped

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error
          The error handle can be obtained from the session
          using `mysqlx_error()` function.

  @ingroup xapi_ddl
*/

PUBLIC_API int
mysqlx_schema_drop(mysqlx_session_t *sess, const char *schema);


/**
  Drop a table

  @param schema schema handle
  @param table the name of the table to drop

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error
          The error handle can be obtained from the session
          using `mysqlx_error()` function.

  @ingroup xapi_ddl
*/

PUBLIC_API int
mysqlx_table_drop(mysqlx_schema_t *schema, const char *table);


/**
  Drop a view

  @param schema schema handle
  @param view the name of the view to drop

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error
          The error handle can be obtained from the session
          using `mysqlx_error()` function.

  @ingroup xapi_ddl
*/

PUBLIC_API int
mysqlx_view_drop(mysqlx_schema_t *schema, const char *view);


/**
  Create a new collection in a specified schema

  @param schema schema handle
  @param collection collection name to create

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error
          The error handle can be obtained from the session
          using `mysqlx_error()` function.

  @ingroup xapi_ddl
*/

PUBLIC_API int
mysqlx_collection_create(mysqlx_schema_t *schema, const char *collection);


/**
  Drop an existing collection in a specified schema

  @param schema schema handle
  @param collection collection name to drop

  @return `RESULT_OK` - on success; `RESULT_ERR` - on error
          The error handle can be obtained from the session
          using `mysqlx_error()` function

  @ingroup xapi_ddl
*/

PUBLIC_API int
mysqlx_collection_drop(mysqlx_schema_t *schema, const char *collection);


/*
  ====================================================================
  Diagnostics
  ====================================================================
*/


/**
  Get the last error from the object.

  @param obj handle to the object to extract the error information from.
             Supported handle types are `mysqlx_session_t`,
             `mysqlx_session_options_t`, `mysqlx_schema_t`,
             `mysqlx_collection_t`, `mysqlx_table_t`, `mysqlx_stmt_t`,
             `mysqlx_result_t`, `mysqlx_row_t`, `mysqlx_error_t`

  @return the error handle or NULL if there is no errors.

  @ingroup xapi_diag
*/

PUBLIC_API mysqlx_error_t * mysqlx_error(void *obj);


/**
  Get the error message from the object.

  @param obj handle to the object to extract the error information from.
             Supported handle types are `mysqlx_session_t`,
             `mysqlx_session_options_t`, `mysqlx_schema_t`,
             `mysqlx_collection_t`, `mysqlx_table_t`, `mysqlx_stmt_t`,
             `mysqlx_result_t`, `mysqlx_row_t`, `mysqlx_error_t`

  @return the character string or NULL if there is no errors.

  @ingroup xapi_diag
*/

PUBLIC_API const char * mysqlx_error_message(void *obj);


/**
  Get the error number from the object.

  @param obj handle to the object to extract the error information from.
             Supported handle types are `mysqlx_session_t`,
             `mysqlx_session_options_t`, `mysqlx_schema_t`,
             `mysqlx_collection_t`, `mysqlx_table_t`, `mysqlx_stmt_t`,
             `mysqlx_result_t`, `mysqlx_row_t`, `mysqlx_error_t`

  @return the error number or 0 if no error

  @ingroup xapi_diag
*/

PUBLIC_API unsigned int mysqlx_error_num(void *obj);


/**
  Get the number of warnings generated by a statement.

  @param res result handle

  @return the number of warnings stored in the result
  @ingroup xapi_diag
*/

PUBLIC_API unsigned int mysqlx_result_warning_count(mysqlx_result_t *res);


/**
  Get the next warning from the result.

  This function returns a handle to a warning which can be examined using
  the same functions used for errors: `mysqlx_error_num()` and
  `mysqlx_error_message()`.

  @param res result handle

  @return handle to the next warning from the result or
          NULL if there is no more warnings left to return.

  @note The warning handle returned by a previous call is invalidated.

  @ingroup xapi_diag
*/

PUBLIC_API mysqlx_error_t *
mysqlx_result_next_warning(mysqlx_result_t *res);


#define VIEW_OPTION_ALGORITHM     100
#define VIEW_OPTION_SECURITY      101
#define VIEW_OPTION_CHECK_OPTION  102
#define VIEW_OPTION_DEFINER       103
#define VIEW_OPTION_COLUMNS       104

/**  @name Macros used to declare view attributes */
/**@{*/

/**
  Specify algorithm to be used for view processing.
  @see `mysqlx_view_algorithm_t`
*/
#define VIEW_ALGORITHM(A)    (void*)VIEW_OPTION_ALGORITHM, A

/**
  Specify view security context.
  @see `mysqlx_view_security_t`
*/
#define VIEW_SECURITY(S)     (void*)VIEW_OPTION_SECURITY, S

/**
  Specify definer of a view.

  The definer is used to determine access rights for the view. It is specified
  as a valid MySQL account name of the form "user@host".

  @see https://dev.mysql.com/doc/refman/en/stored-programs-security.html
*/
#define VIEW_DEFINER(D)      (void*)VIEW_OPTION_DEFINER, D

/**
  Specify checks to be performed for an updatable view.
  @see `mysqlx_view_check_option_t`
*/
#define VIEW_CHECK_OPTION(C) (void*)VIEW_OPTION_CHECK_OPTION, C

/**
  Specify names for view columns.

  The number of column names must match the number of columns of view's
  query.
*/
#define VIEW_COLUMNS(...)    (void*)VIEW_OPTION_COLUMNS, __VA_ARGS__

/**@}*/


/**
  Create a new View

  Creates a new View in a given schema based on a previously
  defined select statement.

  An attempt to call this function if a view with a specified name
  already exists will result in an error.

  @param schema schema handle

  @param name   view name

  @param select_stmt statement representing a table select query
         which will be used for the view (see `mysqlx_table_select_new()`)

  @param ... variable list of view definition options terminated by `PARAM_END`.
          Each option starts with a `VIEW_OPTION_X` constant followed
          by option's value(s). See below for more details.

  @return handle containing the result of the view create operation or
          NULL if an error has happened. This result can be used to examine
          reported warnings, if any.

  There are convenience macros `VIEW_ALGORITHM()`, `VIEW_SECURITY()`,
  `VIEW_CHECK_OPTION()`, `VIEW_DEFINER()` and `VIEW_COLUMNS()`which can be used
  to specify view options. For instance:
  ~~~~~~
  mysqlx_view_create(schema, "view_1", select_stmt,
                      VIEW_DEFINER("root"),
                      VIEW_ALGORITHM(VIEW_ALGORITHM_TEMPTABLE),
                      VIEW_COLUMNS("col_1", "col_2"), PARAM_END);
  ~~~~~~
  The order of parameters in the list is not fixed, but if
  `VIEW_COLUMNS()` (`VIEW_OPTION_COLUMNS`) is used it must be the
  last parameter before terminating the list with `PARAM_END`. See documentation
  of the convenience macros for more details on possible view definition
  options


  @note In case of error the error details can be obtained from the schema
        object using `mysqlx_error()` or `mysqlx_error_message()`.

  @ingroup xapi_ddl
*/

PUBLIC_API mysqlx_result_t *
mysqlx_view_create(mysqlx_schema_t *schema, const char *name,
                   mysqlx_stmt_t *select_stmt, ...);


/**
  Return a new statement which creates a view

  Creates a statement which will be used for creating a new view
  in a given schema based on a previously defined select statement. Before
  executing this statement with `mysqlx_execute()` additional view options
  can be specified using `mysqlx_set_view_x()` family of functions.

  An attempt to execute the returned statement if a view with a specified name
  already exists will result in an error.

  @param schema schema handle

  @param name view name

  @param select_stmt statement representing a table select query
         which will be used for the view (see `mysqlx_table_select_new()`)

  @return statement handle for view create operation.
          NULL can be returned only in case when there are problems
          allocating memory, which normally should not happen.
          It is very unlikely for this function to end with an error
          because it does not do any parsing, parameter checking etc.

  @note To actually execute the statement, the returned handle has to be
        given to `mysqlx_execute()`.

  @note In case of error the error details can be obtained from the schema
        object using `mysqlx_error()` or `mysqlx_error_message()`.

  @note Changing the select statement after the `mysqlx_view_create_new()`
        call has no effect on the returned view create statement, which uses
        the state of the select statement at the time of the
        `mysqlx_view_create_new()` call.

  @see `mysqlx_set_view_algorithm()`, `mysqlx_set_view_security()`,
       `mysqlx_set_view_check_option()`, `mysqlx_set_view_definer()`,
       `mysqlx_set_view_columns()`

  @ingroup xapi_ddl
*/

PUBLIC_API mysqlx_stmt_t *
mysqlx_view_create_new(mysqlx_schema_t *schema, const char *name,
                       mysqlx_stmt_t *select_stmt);


/**
  Modify an existing view

  Modifies a view in a given schema based on a previously defined select
  statement.

  An attempt to call this function if a view with the specified name
  does not exist will result in an error.

  @param schema schema handle

  @param name view name

  @param select_stmt statement representing a table select query
         which will be used for the view (see `mysqlx_table_select_new()`)

  @param ... variable list of view definition options terminated by `PARAM_END`.
          Each option starts with a `VIEW_OPTION_X` constant followed
          by option's value(s). See `mysqlx_view_create()` for more details
          on how view options are specified.

  @return handle containing result of view modify operation or NULL if an error
          has happened. This result can be used to examine
          reported warnings, if any.

  @note In case of error the error details can be obtained from the schema
        object using `mysqlx_error()` or `mysqlx_error_message()`.

  @note A new select statement must be specified when updating a view. It is not
  possible to change other view options without changing the main query.

  @ingroup xapi_ddl
*/

PUBLIC_API mysqlx_result_t *
mysqlx_view_modify(mysqlx_schema_t *schema, const char *name,
                   mysqlx_stmt_t *select_stmt, ...);


/**
  Return a new statement which modifies an existing view

  Creates a statement which will modify an existing view in a given schema
  based on a previously defined select statement.  Before
  executing this statement with `mysqlx_execute()` additional view options
  can be specified using `mysqlx_set_view_x()` family of functions.

  An attempt to execute the returned statement if a view with a specified name
  does not exist will result in an error.

  @param schema schema handle

  @param name view name

  @param select_stmt statement representing a table select query
         which will be used for the view (see `mysqlx_table_select_new()`)

  @return statement handle for view modify operation.
          NULL can be returned only in case when there are problems
          allocating memory, which normally should not happen.
          It is very unlikely for this function to end with an error
          because it does not do any parsing, parameter checking etc.

  @note To actually execute the statement, the returned handle has to be
        given to `mysqlx_execute()`.

  @note In case of error the error details can be obtained from the schema
        object using `mysqlx_error()` or `mysqlx_error_message()`.

  @note Changing the select statement after the `mysqlx_view_modify_new()`
        call has no effect on the returned view modify statement, which uses
        the state of the select statement at the time of the
        `mysqlx_view_modify_new()` call.

  @see `mysqlx_set_view_algorithm()`, `mysqlx_set_view_security()`,
       `mysqlx_set_view_check_option()`, `mysqlx_set_view_definer()`,
       `mysqlx_set_view_columns()`

  @ingroup xapi_ddl
*/

PUBLIC_API mysqlx_stmt_t *
mysqlx_view_modify_new(mysqlx_schema_t *schema, const char *name,
                       mysqlx_stmt_t *select_stmt);


/**
  Modify an existing view or create a new one

  Modifies an existing view or creates a new one in a given schema
  based on a previously defined select statement.

  @param schema schema handle

  @param name view name

  @param select_stmt statement representing a table select query
         which will be used for the view (see `mysqlx_table_select_new()`)

  @param ... variable list of view definition options terminated by `PARAM_END`.
          Each option starts with a `VIEW_OPTION_X` constant followed
          by option's value(s). See `mysqlx_view_create()` for more details
          on how view options are specified.

  @return handle containing result of view replace operation or
          NULL if the error has happened. This result can be used to examine
          reported warnings, if any.

  @note In case of error the error details can be obtained from the schema
        object using `mysqlx_error()` or `mysqlx_error_message()`.

  @ingroup xapi_ddl
*/

PUBLIC_API mysqlx_result_t *
mysqlx_view_replace(mysqlx_schema_t *schema, const char *name,
                    mysqlx_stmt_t *select_stmt, ...);


/**
  Return a new statement which modifies an existing view or creates a new one

  Creates a statement which will create or modify an existing view in the given
  schema based on a previously defined select statement. Before
  executing this statement with `mysqlx_execute()` additional view options
  can be specified using `mysqlx_set_view_x()` family of functions.

  @param schema schema handle

  @param name view name

  @param select_stmt statement representing a table select query
         which will be used for the view (see `mysqlx_table_select_new()`)

  @return statement handle for view replace operation.
          NULL can be returned only in case when there are problems
          allocating memory, which normally should not happen.
          It is very unlikely for this function to end with an error
          because it does not do any parsing, parameter checking etc.

  @note To actually execute the statement, the returned handle has to be
        given to `mysqlx_execute()`.

  @note In case of error the error details can be obtained from the schema
        object using `mysqlx_error()` or `mysqlx_error_message()`.

  @note Changing the select statement after the `mysqlx_view_replace_new()`
        call has no effect on the returned view replace statement, which uses
        the state of the select statement at the time of the
        `mysqlx_view_replace_new()` call.

  @see `mysqlx_set_view_algorithm()`, `mysqlx_set_view_security()`,
       `mysqlx_set_view_check_option()`, `mysqlx_set_view_definer()`,
       `mysqlx_set_view_columns()`

  @ingroup xapi_ddl
*/

PUBLIC_API mysqlx_stmt_t *
mysqlx_view_replace_new(mysqlx_schema_t *schema, const char *name,
                        mysqlx_stmt_t *select_stmt);


/**
  Set an algorithm for a view

  @param view_stmt view statement handle

  @param algorithm a `mysqlx_view_algorithm_t` constant specifying
         the algorithm.

  @return RESULT_OK if a call was successful, RESULT_ERROR otherwise

  @note In case of error the error details can be obtained from the
        view statement object using `mysqlx_error()` or
        `mysqlx_error_message()`.

  @see `mysqlx_view_algorithm_t`

  @ingroup xapi_ddl
*/

PUBLIC_API int
mysqlx_set_view_algorithm(mysqlx_stmt_t *view_stmt, int algorithm);


/**
  Set a security context for a view

  @param view_stmt view statement handle

  @param security a `mysqlx_view_security_t` constant specifying the security
         context.

  @return RESULT_OK if a call was successful, RESULT_ERROR otherwise

  @note In case of error the error details can be obtained from the
        view statement object using `mysqlx_error()` or
        `mysqlx_error_message()`.

  @see `mysqlx_view_security_t`

  @ingroup xapi_ddl
*/

PUBLIC_API int
mysqlx_set_view_security(mysqlx_stmt_t *view_stmt, int security);


/**
  Set a definer for a view

  @param view_stmt view statement handle

  @param user a valid MySQL account name of the form "user@host"

  @return RESULT_OK if a call was successful, RESULT_ERROR otherwise

  @note In case of error the error details can be obtained from the
        view statement object using `mysqlx_error()` or
        `mysqlx_error_message()`.

  @see https://dev.mysql.com/doc/refman/en/stored-programs-security.html

  @ingroup xapi_ddl
*/

PUBLIC_API int
mysqlx_set_view_definer(mysqlx_stmt_t *view_stmt, const char *user);


/**
  Set a check option for a view

  @param view_stmt view statement handle

  @param option a `mysqlx_view_check_option_t` constant specifying the
         check option

  @return RESULT_OK if a call was successful, RESULT_ERROR otherwise

  @note In case of error the error details can be obtained from the
        view statement object using `mysqlx_error()` or
        `mysqlx_error_message()`.

  @see `mysqlx_view_check_option_t`

  @ingroup xapi_ddl
*/

PUBLIC_API int
mysqlx_set_view_check_option(mysqlx_stmt_t *view_stmt, int option);


/**
  Set column names for a view

  @param view_stmt view statement handle

  @param ... variable parameters list containing strings that specify
             the column names for a View. The list has to be terminated
             by PARAM_END

  @return RESULT_OK if a call was successful, RESULT_ERROR otherwise

  @note In case of error the error details can be obtained from the
        view statement object using `mysqlx_error()` or
        `mysqlx_error_message()`.

  @note The number of column names must match the number of columns of
  view's query.

  @ingroup xapi_ddl
*/

PUBLIC_API int
mysqlx_set_view_columns(mysqlx_stmt_t *view_stmt, ...);

#ifdef	__cplusplus
}
#endif

/**@}*/

#endif /* __MYSQLX_H__*/
