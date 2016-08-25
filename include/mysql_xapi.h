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

/*! \file mysqlx_cc.h
* MySQL Connector/C 7.0
*/

#ifndef MYSQL_XAPI_H
#define MYSQL_XAPI_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <mysql_classic.h>

///////////////////// COMMON TYPE DECLARATIONS, REMOVE LATER

#ifndef	__cplusplus
  typedef unsigned char bool;
#endif

typedef char object_id[16];
typedef object_id* MYSQLX_GUID;

/*! @brief MYSQLX API
 * @details Macro definition to indicate a function/operation successful end
 */
#define RESULT_OK 0

/*! @brief MYSQLX API
 * @details Macro definition to indicate a function/operation end with an error
            mysqlx_get_crud_error() or mysqlx_get_session_error() should be called
            to get more details
 */
#define RESULT_NULL 16
#define RESULT_INFO 32
#define RESULT_WARNING 64
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

/* Opaque structures */
/*! @brief MYSQLX API
 * @details structure for obtaining the error information from the session
 *          and CRUD operations (see mysqlx_get_session(), mysqlx_session_error()
 *          and mysqlx_crud_error())
*/
typedef struct MYSQLX_ERROR_T MYSQLX_ERROR;

/*! @brief MYSQLX API
 *  @details structure containing the session context after the session is
 *   established (see mysqlx_get_session_s())
 */
typedef struct MYSQLX_SESSION_T MYSQLX_SESSION;

/*! @brief MYSQLX API
 *  @details structure containing the connection options for the session before
 *   the connection is established (see mysqlx_get_session())
 */
typedef struct MYSQLX_SESSION_OPTIONS_T MYSQLX_SESSION_OPTIONS;

/*! @brief MYSQLX API
 *  @details structure containing the context of the CRUD operation. See
 *    mysqlx_sql_query(), mysqlx_table_select_new(), mysqlx_table_insert_new(),
 *    mysqlx_table_update_new(), mysqlx_table_delete_new(),
 *    mysqlx_collection_find_new(), mysqlx_collection_modify_new(), mysqlx_collection_add_new(),
 *    mysqlx_collection_remove_new()
 */
typedef struct MYSQLX_CRUD_T MYSQLX_CRUD;

/*! @brief NOT IMPLEMENTED
 * Using of this structure is temporarily suspended
 */
typedef struct MYSQLX_DOC_T MYSQLX_DOC;

/*! @brief MYSQLX API
 *  @details structure representing a row from a table resultset
 *  (see mysqlx_row_fetch_one())
 */
typedef struct MYSQLX_ROW_T MYSQLX_ROW;


/*! @brief MYSQLX API
 *  @details Structure representing the result context along with the buffered
 *           rows/documents (see mysqlx_crud_execute(), mysqlx_store_result(),
 *           mysqlx_row_fetch_one(), mysqlx_doc_fetch_one(), mysqlx_next_result())
 */
typedef struct MYSQLX_RESULT_T MYSQLX_RESULT;

/*! @brief NOT IMPLEMENTED
 *  @details Structure containig a list of values for INSERT/ADD operations
 */
typedef struct MYSQLX_ROW_VALUE_T MYSQLX_ROW_VALUE;

/*! @brief NOT IMPLEMENTED
 *  @details Structure containig a list of values for UPDATE/MODIFY operations
 */
typedef struct MYSQLX_UPDATE_VALUE_T MYSQLX_UPDATE_VALUE;

/*!
* \enum MYSQLX_DATA_TYPE
* The data type identifiers used in MYSQLX API
*/
typedef enum MYSQLX_DATA_TYPE_T
{
  MYSQLX_TYPE_UNDEFINED = 0,

  /* Coulmn types as defined in protobuf (mysqlx_resultset.proto) */
  MYSQLX_TYPE_SINT     = 1, /*!< 64-bit signed integer number type */
  MYSQLX_TYPE_UINT  = 2,    /*!< 64-bit unsigned integer number type */
  MYSQLX_TYPE_DOUBLE   = 5, /*!< Floating point double number type */
  MYSQLX_TYPE_FLOAT = 6,    /*!< Floating point float number type */
  MYSQLX_TYPE_BYTES    = 7, /*!< Bytes array type */
  MYSQLX_TYPE_TIME  = 10,   /*!< Time type */
  MYSQLX_TYPE_DATETIME = 12,/*!< Datetime type */
  MYSQLX_TYPE_SET  = 15,    /*!< Set type */
  MYSQLX_TYPE_ENUM     = 16,/*!< Enum type */
  MYSQLX_TYPE_BIT  = 17,    /*!< Bit type */
  MYSQLX_TYPE_DECIMAL  = 18,/*!< Decimal type */

  /* Column types from DevAPI (no number constants assigned, just names) */
  MYSQLX_TYPE_BOOL     = 19,/*!< Bool type */
  MYSQLX_TYPE_JSON     = 20,/*!< JSON type */
  MYSQLX_TYPE_STRING   = 21,/*!< String type */
  MYSQLX_TYPE_GEOMETRY = 22,/*!< Geometry type*/
  MYSQLX_TYPE_TIMESTAMP= 23,/*!< Timestamp type*/

  MYSQLX_TYPE_NULL     = 100, /*!< NULL value */
  MYSQLX_TYPE_EXPR     = 101  /*!< Expression type */
} MYSQLX_DATA_TYPE;

#define PARAM_SINT(A) (void*)MYSQLX_TYPE_SINT, (int64_t)A
#define PARAM_UINT(A) (void*)MYSQLX_TYPE_UINT, (uint64_t)A
#define PARAM_FLOAT(A) (void*)MYSQLX_TYPE_FLOAT, (double)A
#define PARAM_DOUBLE(A) (void*)MYSQLX_TYPE_DOUBLE, (double)A
#define PARAM_BYTES(DATA, SIZE) (void*)MYSQLX_TYPE_BYTES, (void*)DATA, (size_t)SIZE
#define PARAM_STRING(A) (void*)MYSQLX_TYPE_STRING, A
#define PARAM_EXPR(A) (void*)MYSQLX_TYPE_EXPR, A
#define PARAM_NULL() (void*)MYSQLX_TYPE_NULL

#define PARAM_END (void*)0

/*!
 * \enum MYSQLX_SORT_DIRECTION
 * Enumerating sort directions in sorting operations such as ORDER BY
*/
typedef enum MYSQLX_SORT_DIRECTION_T
{
  SORT_ORDER_ASC = 1, /*!< Ascending sorting (Default) */
  SORT_ORDER_DESC = 2 /*!< Descending sorting */
} MYSQLX_SORT_DIRECTION;

/*! @brief MYSQLX API
 *
 *  @details Establish a session using string options provided as function parameters
 *
 *
 * @param host       server host address
 * @param port       port number
 * @param user       user name
 * @param password   password
 * @param password   default database name
 * @param[out] out_error if error happens during connect the error message
 *                   is returned through this parameter
 * @param[out] err_code if error happens during connect the error code
 *                   is returned through this parameter
 *
 * @return Pointer to MYSQLX_SESSION structure if connection is success,
 *         otherwise and the error is returned through
 *         the error [OUT] parameter
 *
 * @note The MYSQLX_SESSION pointer returned by the function must be
 *       properly closed using mysqlx_session_close()
 * @note This type of session does not support executing plain SQL queries
 */
MYSQLX_SESSION * STDCALL
mysqlx_get_session_s(const char *host, int port, const char *user,
                     const char *password, const char *database,
                     char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code);

/*! @brief MYSQLX API
 *
 *  @details Establish a session using connection string
 *           as "user:pass@host:port
 *
 * @param conn_string character connection string
 * @param[out] out_error if error happens during connect the error message
 *                   is returned through this parameter
 * @param[out] err_code if error happens during connect the error code
 *                   is returned through this parameter
 *
 * @return Pointer to MYSQLX_SESSION structure if connection is success,
 *         otherwise and the error is returned through
 *         the error [OUT] parameter
 *
 * @note The MYSQLX_SESSION pointer returned by the function must be
 *       properly closed using mysqlx_session_close()
 * @note This type of session does not support executing plain SQL queries
 */
MYSQLX_SESSION * STDCALL
mysqlx_get_session(const char *conn_string,
                     char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code);

/*! @brief MYSQLX API
 *
 *  @details Establish a node session using string options provided as
 *           function parameters. A node session connects only to one
 *           mysqld node at a time
 *
 * @param host       server host address
 * @param port       port number
 * @param user       user name
 * @param password   password
 * @param database   default database name
 * @param[out] out_error if error happens during connect the error message
 *                   is returned through this parameter
 * @param[out] err_code if error happens during connect the error code
 *                   is returned through this parameter
 *
 * @return Pointer to MYSQLX_SESSION structure if connection is success,
 *         otherwise and the error is returned through
 *         the error [OUT] parameter
 *
 * @note The MYSQLX_SESSION pointer returned by the function must be
 *       properly closed using mysqlx_session_close()
 * @note This type of session supports executing plain SQL queries
 */
MYSQLX_SESSION * STDCALL
mysqlx_get_node_session_s(const char *host, int port, const char *user,
                     const char *password, const char *database,
                     char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code);

/*! @brief MYSQLX API
 *
 *  @details Establish a node session using connection string
 *           as "user:pass@host:port. A node session connects only to one
 *           mysqld node at a time.
 *
 * @param conn_string character connection string
 * @param[out] out_error if error happens during connect the error message
 *                   is returned through this parameter
 * @param[out] err_code if error happens during connect the error code
 *                   is returned through this parameter
 *
 * @return Pointer to MYSQLX_SESSION structure if connection is success,
 *         otherwise and the error is returned through
 *         the error [OUT] parameter
 *
 * @note The MYSQLX_SESSION pointer returned by the function must be
 *       properly closed using mysqlx_session_close()
 * @note This type of session supports executing plain SQL queries
 */
MYSQLX_SESSION * STDCALL
mysqlx_get_node_session(const char *conn_string,
                     char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code);

/* brief NOT IMPLEMENTED
 *
 * @details Establish the session using MYSQLX_SESSION_OPTIONS
 *
 * @param options      MYSQLX_SESSION_OPTIONS structure with options
 * @param[out] error   if error happens during connect it is returned
 *                     through this parameter, otherwise it is NULL
 *
 * @return Pointer to MYSQLX_SESSION structure if connection is success,
 *         otherwise and the error is returned through
 *         the error [OUT] parameter
 *
 * @note: The MYSQLX_SESSION pointer returned by the function must be
 *       properly closed using mysqlx_session_close()

MYSQLX_SESSION * STDCALL
mysqlx_get_session(const MYSQLX_SESSION_OPTIONS *options,
                   MYSQLX_ERROR *error);
*/

/*! @brief MYSQLX API
 *
 *  @details Closing the session. This function must be called by the user
 *  to prevent memory leaks.
 *
 *  @param session Pointer to MYSQLX_SESSION handler to close
*/
void STDCALL mysqlx_session_close(MYSQLX_SESSION *session);

/* brief NOT IMPLEMENTED
 *
 *  @details Closing the session. This function must be called by the user
 *  to prevent memory leaks.
 *
 *  @param session Pointer to MYSQLX_SESSION handler to close
 *
 *  @return session status
 *
int STDCALL mysqlx_session_status(MYSQLX_SESSION *session);
*/

/*! @brief MYSQLX API
 *
 *  @details Get session error after session-related operation fails
 *
 *  @param session Pointer to MYSQLX_SESSION handler get error from
 *
 *  @return Pointer to the error structure with the requested error details
*/
MYSQLX_ERROR * STDCALL
mysqlx_session_error(MYSQLX_SESSION *session);

/*! @brief MYSQLX API
 *
 * @details Create a CRUD handler for a plain SQL query.
 * The query supports parameters and placeholders that can be
 * added later using mysqlx_crud_bind() function
 *
 * @param sess session handler
 * @param query SQL query
 * @param length length of the query
 *
 * @return
 *   CRUD handler containing the results and/or error.
 *   NULL can be returned only in case when there are problems
 *   allocating memory, which normally should not happen.
 *   It is very unlikely for this function to end with the error
 *   because it does not do any parsing, parameters checking etc.
 *
 * @note To actually execute the SQL query the returned CRUD has to be
 *       given to mysqlx_crud_execute()
 */
MYSQLX_CRUD * STDCALL
mysqlx_sql_query(MYSQLX_SESSION *sess, const char *query,
                 uint32_t length);

/* brief MYSQLX API
 *
 * @details Create a CRUD handler for SQL query with parameters
 *
 * @param sess session handler
 * @param format SQL query format string with parameter markers as in printf()
 * @param length length of the query
 * @param ... parameter values of the types corresponding to the format string
 *
 * @return
 *   CRUD handler containing the results and/or error.
 *   NULL can be returned only in case when there are problems
 *   allocating memory, which normally should not happen.
 *   It is very unlikely for this function to end with the error
 *   because it does not do any parsing, parameters checking etc.
 *
 * @note To actually execute the SQL query the returned CRUD has to be
        given to mysqlx_crud_execute()

MYSQLX_CRUD * STDCALL
mysqlx_sql_query_v(MYSQLX_SESSION *sess, const char *format,
                   uint32_t length, ...);
*/
// Operations

/*! @brief MYSQLX API
 *
 * @details Start new table SELECT operation without actually executing it.
 *
 * @param sess pointer to the current session handlre
 * @param schema schema name. The default database will be used if NULL is specified
 *                            for this parameter.
 * @param table table name
 *
 * @return
 *   CRUD handler to the newly created SELECT operation.
 *   NULL can be returned only in case when there are problems
 *   allocating memory, which normally should not happen.
 *   It is very unlikely for this function to end with the error
 *   because it does not do any parsing, parameters checking etc.
 *
 * @note To actually execute the SQL query the returned CRUD has to be
        given to mysqlx_crud_execute()
*/
MYSQLX_CRUD * STDCALL
mysqlx_table_select_new(MYSQLX_SESSION *sess, const char *schema,
                        const char *table);
/*
  All variadic parameters table functions must close the list of items with NULL value

  mysqlx_set_select_items()
  mysqlx_set_select_order_by()
  mysqlx_set_select_group_by()
  mysqlx_set_insert_columns()
  mysqlx_set_insert_rows()
  mysqlx_set_update_set()
  mysqlx_set_update_order_by()
  mysqlx_set_delete_order_by()

  Same is for the collection functions:
*/

/*! @brief MYSQLX API
 *  @details Setting projections (items to select or find) defined as column
 *           names, values, constants or expressions.
 *           operation. See mysqlx_set_where()
 *
 *  @param crud pointer to CRUD structure for which the projections are set
 *  @param  ... - variable parameters list consisting of character strings
 *         that define projections. The list is terminated by PARAM_END:
 *         proj_1, ..., proj_n, PARAM_END
 *         (PARAM_END marks the end of projectins items list)
 *
 *  @return RESULT_OK - on success; RESULT_ERR - on error
 *
 *  @note This function can be only called for the table SELECT or collection
 *        FIND operations (see mysqlx_table_select_new() and
 *        mysqlsx_collection_find_new())
 * */
int STDCALL mysqlx_set_items(MYSQLX_CRUD *crud, ...);

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting projections for SELECT
 *           operation. See mysqlx_set_items()
 * */
#define mysqlx_set_select_items mysqlx_set_items


/*! @brief MYSQLX API
 *  @details a macro defining a function for setting WHERE for SELECT
 *           operation. See mysqlx_set_where()
 * */
#define mysqlx_set_select_where mysqlx_set_where

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting ORDER BY for SELECT
 *           operation. See mysqlx_set_order_by()
 * */
#define mysqlx_set_select_order_by mysqlx_set_order_by

/* brief MYSQLX API
 *  @details a macro defining a function for setting HAVING for SELECT
 *           operation. See mysqlx_set_having()
 *
#define mysqlx_set_select_having mysqlx_set_having
 */

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting LIMIT for SELECT
 *           operation. See mysqlx_set_limit_and_offset()
 * */
#define mysqlx_set_select_limit_and_offset mysqlx_set_limit_and_offset

/*! @brief MYSQLX API
 *
 * @details
 * Limit given CRUD operation to rows/documents that satisfy given WHERE clause:
 *  - for select/find operations limit the returned rows/documents,
 *  - for update/modify/delete/remove operations limit the rows/documents
 *    affected by the operations.
 *
 * Operations supported by this function:
 *   SELECT, FIND, UPDATE, MODIFY, DELETE, REMOVE
 * Calling it for INSERT or ADD will result in an error
 *
 * @param crud pointer to CRUD structure
 * @param where_expr character string containing WHERE clause,
 *                which will be parsed as required
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *
 * @note this function can be be used directly, but for the convenience
 *       the code can use the specialized macros for a specific operation.
 *       For SELECT operation the user code should use
 *       mysqlx_set_select_where() macros that map the
 *       corresponding mysqlx_set_where() function.
 *       This way the unsupported operations will not be used.
 */
int STDCALL mysqlx_set_where(MYSQLX_CRUD *crud, const char *where_expr);

/*! @brief MYSQLX API
 *
 * @details Set ORDER BY clause for CRUD operation
 * Operations supported by this function:
 * SELECT, FIND, UPDATE, MODIFY, DELETE, REMOVE
 * Calling it for INSERT or ADD will result in an error
 *
 * @param crud - pointer to CRUD structure
 * @param  ... - variable parameters list consisting of (expression, direction) pairs
 *         terminated by PARAM_END:
 *         expr_1, direction_1, ..., expr_n, direction_n, PARAM_END
 *         (PARAM_END marks the end of parameters list)
 *         Each expression computes value used to sort
 *         the rows/documents in ascending or descending order,
 *         as determined by direction constant
 *         (list the direction enum names).
 *         Special attention must be paid to the expression
 *         strings because the empty string "" or NULL will be treated
 *         as the end of sequence
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *
 * @note this function can be be used directly, but for the convenience
 *       the code can use the specialized macros for a specific operation.
 *       For SELECT operation the user code should use
 *       mysqlx_set_select_order_by() macros that map the
 *       corresponding mysqlx_set_order_by() function.
 *       This way the unsupported operations will not be used.
 */
int STDCALL mysqlx_set_order_by(MYSQLX_CRUD *crud, ...);

/* brief NOT IMPLEMENTED
 *
 * @details Set GROUP BY clause for CRUD operation
 * Operations supported by SELECT.
 * Calling it for INSERT or ADD will result in an error
 *
 * @param crud - pointer to CRUD structure
 * @param expr - expression computing value used to group rows/documents
 * @param  ... - variable parameters list consisting of expressions
 *         terminated by PARAM_END:
 *         expr_1, ..., expr_n, PARAM_END
 *         (PARAM_END marks the end of parameters list)
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *
int STDCALL mysqlx_set_select_group_by(MYSQLX_CRUD *crud, const char *expr, ...); // NOT IMPLEMENTED
*/

/* brief NOT IMPLEMENTED
 *
 * @note On the implementation stage the function can change its name,
 *       parameters or return type
 *
int STDCALL mysqlx_set_having(MYSQLX_CRUD *crud, const char *criteria); // NOT IMPLEMENTED
*/

/*! @brief MYSQLX API
 *
 * @details Set LIMIT and OFFSET for CRUD operations which work on ranges of rows/documents:
 *   - for slect/find operations limit the number of returned rows/documents,
 *   - for update/delete limit the number of documents affected by the operation.
 *
 * Operations supported by this function:
 *   SELECT, FIND - use both LIMIT and OFFSET
 *   UPDATE, MODIFY, DELETE, REMOVE - use only LIMIT
 *
 * Calling it for INSERT or ADD will result in an error
 *
 * @param crud pointer to CRUD structure
 * @param row_count the number of result rows to return
 * @param offset the number of rows to skip before starting counting
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *
 * @note this function can be be used directly, but for the convenience
 *       the code can use the specialized macros for a specific operation.
 *       For SELECT operation the user code should use
 *       mysqlx_set_select_limit_and_offset() macros that map the
 *       corresponding mysqlx_set_limit_and_offset() function.
 *       This way the unsupported operations will not be used.
 *
 * @note Each call to this function replaces previously set LIMIT
*/
int STDCALL
mysqlx_set_limit_and_offset(MYSQLX_CRUD *crud, uint64_t row_count,
                            uint64_t offset);

/*! @brief NOT IMPLEMENTED
 *
 * @details Start new table INSERT operation without actually executing it.
 *
 * @param sess pointer to the current session handlre
 * @param schema schema name. The default database will be used if NULL is specified
 *                            for this parameter.
 * @param table table name
 *
 * @return
 *   CRUD handler to the newly created INSERT operation.
 *   NULL can be returned only in case when there are problems
 *   allocating memory, which normally should not happen.
 *   It is very unlikely for this function to end with the error
 *   because it does not do any parsing, parameters checking etc.
 *
 * @note To actually execute the SQL query the returned CRUD has to be
        given to mysqlx_crud_execute()
*/
MYSQLX_CRUD * STDCALL
mysqlx_table_insert_new(MYSQLX_SESSION *sess,
                        const char *schema,
                        const char *table);

/*! @brief MYSQLX API
 *
 * @details The function provides the column names for the CRUD INSERT.
 *  User code must ensure that the column values are correct
 *  because the names are not validated until receiving the query on
 *  the server side after executing mysqlx_crud_execute().
 *
 * @param crud pointer to CRUD
 * @param   ...  - variable parameters list consisting of column names
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *
 * @note This function can only be called after mysqlx_set_insert_columns()
 *       which defines the column names and their types
 * @note Each new call clears the list of column for a given CRUD
 *       if it was set earlier
*/
int STDCALL
mysqlx_set_insert_columns(MYSQLX_CRUD *crud, ...);

/*! @brief MYSQLX API
 *
 * @details The function provides the row data for the CRUD INSERT.
 *  User code must ensure that the number of values and the order they are specified
 *  in the parameters is the same as the number of columns and their order in
 *  mysqlx_set_insert_columns(), which defines the column names for INSERT.
 *  However, mysqlx_set_insert_columns() can be skipped. In this case the number
 *  of columns and their order must correspond to the same in the table being
 *  inserted.
 *
 * @param crud pointer to CRUD
 * @param   ...  - variable parameters list consisting of (type, value) pairs
 *         terminated by PARAM_END. The pairs must be listed in the order they
 *         appear in the list of columns
 *         For MYSQLX_TYPE_BYTES the function will expect three parameters
 *         instead of two as for all other types:
 *         <MYSQLX_TYPE_BYTES, (void*)byte_data, (size_t)length>
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *
 * @note This function can only be called after mysqlx_set_insert_columns()
 *       which defines the column names and their types
 * @note Each new call provides the row values for the new row, which
 *       can be used for multi-row inserts
*/
int STDCALL
mysqlx_set_insert_row(MYSQLX_CRUD *crud, ...);

/* brief MYSQLX API
 *
 * @details The function provides the columns data for the CRUD INSERT.
 *  User code must ensure that the number of values in the parameters corresponds
 *  to the number of columns in the table because this is not checked
 *  until receiving the query on the server side.
 *
 * @param crud pointer to CRUD
 * @param   ...  - variable parameters list consisting of (name, type) pairs
 *         terminated by PARAM_END:
 *
 *          name_1, type_id1, name_2, type_id2, ..., name_n, type_id_n, PARAM_END
 *          (PARAM_END marks the end of parameters list).
 *
 *          type_id is the numeric identifier, which helps to determine the type
 *          of the value provided as the next parameter. The user code must
 *          ensure that type_id corresponds to the actual value type. Otherwise,
 *          the value along with and all sequential types and values are most
 *          likely to be corrupted.
 *
 *          allowed types are listed in MYSQLX_DATA_TYPE enum.
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *
 * @note Each new call resets the row values set by the previous call to
 *       mysqlx_set_insert_columns()
 *
int STDCALL
mysqlx_set_insert_columns(MYSQLX_CRUD *crud, ...); // NOT IMPLEMENTED
*/

/*! @brief MYSQLX API
 *
 * @details Start new table UPDATE operation without actually executing it.
 *
 * @param sess pointer to the current session handlre
 * @param schema schema name. The default database will be used if NULL is specified
 *                            for this parameter.
 * @param table table name
 *
 * @return
 *   CRUD handler to the newly created UPDATE operation.
 *   NULL can be returned only in case when there are problems
 *   allocating memory, which normally should not happen.
 *   It is very unlikely for this function to end with the error
 *   because it does not do any parsing, parameters checking etc.
 *
 * @note To actually execute the SQL query the returned CRUD has to be
 *       given to mysqlx_crud_execute()
*/
MYSQLX_CRUD * STDCALL
mysqlx_table_update_new(MYSQLX_SESSION *sess,
                        const char *schema, const char *table);

/*! @brief MYSQLX API
 *
 * @details Set values for the columns in the UPDATE statement.
 *
 * @param crud - pointer to CRUD structure
 * @param  ... - variable parameters list consisting of triplets
 *         <column_name, value_type, value_or_expression>
 *         representing column names, value types and values as
 *         expressions.The list is terminated by PARAM_END:
 *         column_1, type_1, val_1, ..., column_n, type_n, val_n, PARAM_END
 *         (PARAM_END marks the end of parameters list)
 *         The value type is defined in MYSQLX_DATA_TYPE enum.
 *         If the value is to be computed on the server side the type
 *         has to be set to MYSQLX_TYPE_EXPR. The value (expression)
 *         should be specified as a character string expression.
 *         For MYSQLX_TYPE_BYTES the function will expect four parameters
 *         instead of three as for all other types:
 *         <column_name, MYSQLX_TYPE_BYTES, (void*)byte_data, (size_t)length>
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *
 * @note All fields and their corresponding expressions must be set in one call
 *       otherwise the next call to this function will reset all parameters to
 *       their new values.
 */
int STDCALL mysqlx_set_update_values(MYSQLX_CRUD *crud, ...);

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting WHERE clause for UPDATE
 *           operation. See mysqlx_set_where()
 * */
#define mysqlx_set_update_where mysqlx_set_where

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting LIMIT for UPDATE
 *           operation. See mysqlx_set_limit_and_offset()
 * */
#define mysqlx_set_update_limit(CRUD, LIM) mysqlx_set_limit_and_offset(CRUD, LIM, 0)

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting ORDER BY clause for UPDATE
 *           operation. See mysqlx_set_oder_by()
 * */
#define mysqlx_set_update_order_by mysqlx_set_order_by

// Functios for DELETE (some functions from SELECT are reused)

/*! @brief MYSQLX API
 *
 * @details Start new table DELETE operation without actually executing it.
 *
 * @param sess pointer to the current session handlre
 * @param schema schema name. The default database will be used if NULL is specified
 *                            for this parameter.
 * @param table table name
 *
 * @return
 *   CRUD handler to the newly created DELETE operation.
 *   NULL can be returned only in case when there are problems
 *   allocating memory, which normally should not happen.
 *   It is very unlikely for this function to end with the error
 *   because it does not do any parsing, parameters checking etc.
 *
 * @note To actually execute the SQL query the returned CRUD has to be
 *       given to mysqlx_crud_execute()
*/
MYSQLX_CRUD * STDCALL
mysqlx_table_delete_new(MYSQLX_SESSION *sess,
                        const char *schema, const char *table);

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting WHERE clause for DELETE
 *           operation. See mysqlx_set_where()
 * */
#define mysqlx_set_delete_where mysqlx_set_where

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting LIMIT for DELETE
 *           operation. See mysqlx_set_limit_and_offset()
 * */
#define mysqlx_set_delete_limit(CRUD, LIM) mysqlx_set_limit_and_offset(CRUD, LIM, 0)

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting ORDER BY for DELETE
 *           operation. See mysqlx_set_order_by()
 * */
#define mysqlx_set_delete_order_by mysqlx_set_order_by

/*! @brief MYSQLX API
 *
 * @details Execute a CRUD statement created by mysqlx_table_select_new(),
 *          mysqlx_table_insert_new(), mysqlx_table_update_new(),
 *          mysqlx_table_delete_new(), mysqlx_sql_query(), etc.
 *
 * @param crud pointer to CRUD structure
 *
 * @return  A MYSQL_RESULT handle that can be used to access results
 *          of the operation. Returned handle is valid until the CRUD
 *          handle is freed (when session is closed or explicitly with
 *          mysqlx_crud_free()) or until another call to mysqlx_crud_execute()
 *          on the same CRUD handle is made. It is also possible to close
 *          a RESULT hanlde and free all resources used by it earlier with
 *          mysqlx_result_free() call.
 *          On error NULL is returned. The error is set for CRUD handler.
 */
MYSQLX_RESULT * STDCALL
mysqlx_crud_execute(MYSQLX_CRUD *crud);

/*! @brief MYSQLX_API
 *
 * @details Rows/documents contained in a result must be fetched in a timely fashion.
 *          Failing to do that can result in an error and lost access to the
 *          remaining part of the result. This function can store complete result
 *          in memory so it can be accessed at any time, as long as MYSQLX_RESULT
 *          handle is valid.
 *
 * @param result result handler used for obtaining and buffering the result
 * @param[out] num number of records buffered
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error. If the error occurred
 *         it can be retrieved by mysqlx_crud_error() function.
 *
 * @note Even in case of an error some rows/documents might be buffered if they
 *       were retrieved before the error occurred.
 */
int STDCALL
mysqlx_store_result(MYSQLX_RESULT *result, size_t *num);

/*! @brief MYSQLX API
 *
 * @details Create a schema
 *
 * @param sess session handler
 * @param schema the name of the schema to be created
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *         The error handle can be obtained from the session
 *         using mysqlx_session_error() function
*/
int STDCALL
mysqlx_schema_create(MYSQLX_SESSION *sess, const char *schema);

/*! @brief MYSQLX API
 *
 * @details Drop a schema
 *
 * @param sess session handler
 * @param schema the name of the schema to be dropped
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *         The error handle can be obtained from the session
 *         using mysqlx_session_error() function
*/
int STDCALL
mysqlx_schema_drop(MYSQLX_SESSION *sess, const char *schema);

/*! @brief MYSQLX API
 *
 * @details Drop a table
 *
 * @param sess session handler
 * @param schema the name of the schema containing table to drop
 * @param table the name of the table to drop
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *         The error handle can be obtained from the session
 *         using mysqlx_session_error() function
*/
int STDCALL
mysqlx_table_drop(MYSQLX_SESSION *sess, const char *schema,
                  const char *table);


/*! @brief MYSQLX API
 *
 * @details Drop a view
 *
 * @param sess session handler
 * @param schema the name of the schema containing table to drop
 * @param table the name of the table to drop
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *         The error handle can be obtained from the session
 *         using mysqlx_session_error() function
*/
int STDCALL
mysqlx_view_drop(MYSQLX_SESSION *sess, const char *schema,
                  const char *view);

/*! @brief MYSQLX API
 *
 * @details Create a new collection in a specified schema
 *
 * @param sess session handler
 * @param schema schema in which to create the collection
 * @param collection collection name
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *         The error handle can be obtained from the session
 *         using mysqlx_session_error() function
*/
int STDCALL
mysqlx_collection_create(MYSQLX_SESSION *sess,
                      const char *schema, const char *collection);

/*! @brief MYSQLX API
 *
 * @details Drop an existing collection in a specified schema
 *
 * @param sess session handler
 * @param schema schema in which to drop the collection
 * @param collection collection name
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *         The error handle can be obtained from the session
 *         using mysqlx_session_error() function
*/
int STDCALL
mysqlx_collection_drop(MYSQLX_SESSION *sess,
                      const char *schema, const char *collection);

/*! @brief MYSQLX API
 *
 * @details Create a new CRUD operation for adding a new collection
 *
 * @param sess session handler
 * @param schema schema in which to add the collection
 * @param collection collection name
 *
 * @return
 *   CRUD handler to the newly created ADD operation.
 *   NULL can be returned only in case when there are problems
 *   allocating memory, which normally should not happen.
 *   It is very unlikely for this function to end with the error
 *   because it does not do any parsing, parameters checking etc.
 *
 * @note To actually execute the operation the returned CRUD has to be
 *       given to mysqlx_crud_execute()
*/
MYSQLX_CRUD * STDCALL
mysqlx_collection_add_new(MYSQLX_SESSION *sess,
                      const char *schema, const char *collection);

/*! @brief MYSQLX API
 *
 * @details The function provides the document data for the CRUD ADD as
 * JSON document like "{ key_1: value_1, ..., key_N: value_N }"
 *  User code must ensure the validity of the document because it is
 * not checked until receiving the query on the server side.
 *
 * @param crud pointer to CRUD
 * @param json_doc - the character string describing JSON document to add
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *
 * @note This function can only be called after mysqlx_collection_add_new()
 *       which creates a new CRUD operation
 * @note Each new call provides the values for the new document, which
 *       can be used for multi-document add operations
*/
int STDCALL
mysqlx_set_add_document(MYSQLX_CRUD *crud, const char *json_doc);

/*! @brief MYSQLX API
 *
 * @details Find a collection
 *
 * @param sess session handler
 * @param schema schema name in which the collection is to be searched.
 *               The default database will be used if NULL is specified
 *               for this parameter.
 * @param collection collection name
 *
 * @return
 *   CRUD handler to the newly created FIND operation.
 *   NULL can be returned only in case when there are problems
 *   allocating memory, which normally should not happen.
 *   It is very unlikely for this function to end with the error
 *   because it does not do any parsing, parameters checking etc.
 *
 * @note To actually execute the operation the returned CRUD has to be
 *       given to mysqlx_crud_execute()
*/
MYSQLX_CRUD * STDCALL
mysqlx_collection_find_new(MYSQLX_SESSION *sess,
                       const char *schema,
                       const char *collection);

/*! @brief MYSQLX API
 *  @details Setting projections (items to select or find) defined as column
 *           names, values, constants or expressions.
 *           operation. See mysqlx_set_where()
 *
 *  @param crud pointer to CRUD structure for which the projections are set
 *  @param proj projection specification describing JSON document projections as
 *  "{proj1: expr1, proj2: expr2}".
 *
 *  @return RESULT_OK - on success; RESULT_ERR - on error
 *
 *  @note This function can be only called for the collection
 *        FIND operations (see mysqlsx_collection_find_new())
 * */
int STDCALL mysqlx_set_find_projection(MYSQLX_CRUD *crud, const char *proj);

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting criteria for FIND
 *           operation. See mysqlx_set_where()
 * */
#define mysqlx_set_find_criteria mysqlx_set_where

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting LIMIT for DELETE
 *           operation. See mysqlx_set_limit_and_offset()
 * */
#define mysqlx_set_find_limit_and_offset(CRUD, LIM, OFFS) mysqlx_set_limit_and_offset(CRUD, LIM, OFFS)

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting ORDER BY for SELECT
 *           operation. See mysqlx_set_order_by()
 * */
#define mysqlx_set_find_order_by mysqlx_set_order_by

/*! @brief MYSQLX API
 *
 * @details for binding values for parametrized queries.
 *  User code must ensure that the number of values in bind is the same
 *  as the number of parameters in the query because this is not checked
 *  until receiving the query on the server side.
 *
 * @param crud pointer to CRUD
 * @param   ... variable parameters list, which has different structure for SQL
 *          no-SQL (CRUD) operations.
 *          For SQL operation it is consisting of (type, value) pairs
 *          terminated by PARAM_END:
 *
 *          type_id1, value1, type_id2, value2, ..., type_id_n, value_n, PARAM_END
 *          (PARAM_END marks the end of parameters list).
 *
 *          For CRUD SELECT, INSERT, UPDATE, DELETE, FIND, ADD, MODIFY and REMOVE
 *          operations the parameters come as triplets (param_name, type, value).
 *
 *          name1, type_id1, value1, name2, type_id2, value2, ...,
 *          namen, type_id_n, value_n, PARAM_END
 *          (PARAM_END marks the end of parameters list).
 *
 *          type_id is the numeric identifier, which helps to determine the type
 *          of the value provided as the next parameter. The user code must
 *          ensure that type_id corresponds to the actual value type. Otherwise,
 *          the value along with and all sequential types and values are most
 *          likely to be corrupted.
 *          It is recommended to use PARAM_<TYPE> macros to keep the list integrity:
 *          PARAM_UINT(), PARAM_SINT(), PARAM_FLOAT(), PARAM_DOUBLE(), PARAM_STRING(),
 *          PARAM_BYTES(), PARAM_EXPR() for different data types instead of
 *          (MYSQLX_TYPE_<TYPE>, value) pairs.
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *
 * @note Each new call resets the binds set by the previous call to
 *       mysqlx_crud_bind()
*/
int STDCALL mysqlx_crud_bind(MYSQLX_CRUD *crud, ...);

/*! @brief MYSQLX API
 *
 * @details Create a Collection MODIFY operation
 *
 * @param sess session handler
 * @param schema schema name in which the collection is to be modified.
 *               The default database will be used if NULL is specified
 *               for this parameter.
 * @param collection collection name
 *
 * @return
 *   CRUD handler to the newly created FIND operation.
 *   NULL can be returned only in case when there are problems
 *   allocating memory, which normally should not happen.
 *   It is very unlikely for this function to end with the error
 *   because it does not do any parsing, parameters checking etc.
 *
 * @note To actually execute the MODIFY query the returned CRUD has to be
 *       given to mysqlx_crud_execute()
*/
MYSQLX_CRUD * STDCALL
mysqlx_collection_modify_new(MYSQLX_SESSION *sess,
                         const char *schema,
                         const char *collection);

/*! @brief MYSQLX API
 * @details Set fields in a document to the designated JSON values.
 *
 * @param crud pointer to CRUD initated for Collectin MODIFY operation
 * @ ... list of parameters that come as triplets
 *       <field_path, value_type, value>
 *       Each triplet represents a value inside a document that can
 *       be located by field_path. The value_type is the type identifier
 *       for the data type of value (see MYSQLX_DATA_TYPE enum)
 *       The list is terminated by PARAM_END.
 *       For MYSQLX_TYPE_BYTES there will be one extra parameter specifying
 *       the length of the binary data:
 *       <field_path, MYSQLX_TYPE_BYTES, value, length>
 *
 * @note For the convenience the code can use PARAM_XXXX(val) macros
 *       instead of MYSQLX_TYPE_XXXX, val.
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 */
int STDCALL
mysqlx_set_modify_set(MYSQLX_CRUD *crud, ...);

/*! @brief MYSQLX API
 * @details Unset fields in a document
 *
 * @param crud pointer to CRUD initated for Collectin MODIFY operation
 * @param ... list of the documents fields paths that should be unset. Each
 *       entry in this list is a character string.
 *       The list is terminated by PARAM_END.
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 */
int STDCALL
mysqlx_set_modify_unset(MYSQLX_CRUD *crud, ...);

/*! @brief MYSQLX API
 * @details Insert elements into an array in a document
 *
 * @param crud pointer to CRUD initated for Collectin MODIFY operation
 * @param ... list of parameters that come as triplets
 *       <field_path, value_type, value>
 *       Each triplet represents a value inside a document that can
 *       be located by field_path. The value_type is the type identifier
 *       for the data type of value (see MYSQLX_DATA_TYPE enum)
 *       The list is terminated by PARAM_END.
 *
 * @note For the convenience the code can use PARAM_XXXX(val) macros
 *       instead of MYSQLX_TYPE_XXXX, val.
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 */
int STDCALL
mysqlx_set_modify_array_insert(MYSQLX_CRUD *crud, ...);

/*! @brief MYSQLX API
 * @details Append to an array in a document
 *
 * @param crud pointer to CRUD initated for Collectin MODIFY operation
 * @param ... list of parameters that come as triplets
 *       <field_path, value_type, value>
 *       The list is terminated by PARAM_END.
 *
 * @note For the convenience the code can use PARAM_XXXX(val) macros
 *       instead of MYSQLX_TYPE_XXXX, val.
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 */
int STDCALL
mysqlx_set_modify_array_append(MYSQLX_CRUD *crud, ...);

/*! @brief MYSQLX API
 * @details Delete element in an array in a document
 *
 * @param crud pointer to CRUD initated for Collectin MODIFY operation
 * @param ... list of array elements paths that should be deleted from their arrays
 *       The list is terminated by PARAM_END.
 *
 *  @return RESULT_OK - on success; RESULT_ERR - on error
 */
int mysqlx_set_modify_array_delete(MYSQLX_CRUD *crud, ...);

#define mysqlx_set_modify_criteria mysqlx_set_where

/*! @brief MYSQLX API
 * @details Remove a document from a collection
 *
 * @param sess session handler
 * @param schema schema name in which a collection will have
 *        a document removed. The default database will be used if NULL is specified
 *        for this parameter.
 * @param collection collection name
 *
 * @return
 *   CRUD handler to the newly created REMOVE operation.
 *   NULL can be returned only in case when there are problems
 *   allocating memory, which normally should not happen.
 *   It is very unlikely for this function to end with the error
 *   because it does not do any parsing, parameters checking etc.
 *
 * @note To actually execute the REMOVE query the returned CRUD has to be
 *       given to mysqlx_crud_execute()
 */
MYSQLX_CRUD * STDCALL
mysqlx_collection_remove_new(MYSQLX_SESSION *sess,
                         const char *schema,
                         const char *collection);

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting WHERE for REMIVE
 *           operation. See mysqlx_set_where()
 * */
#define mysqlx_set_remove_criteria mysqlx_set_where

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting ORDER BY for REMOVE
 *           operation. See mysqlx_set_order_by()
 * */
#define mysqlx_set_remove_order_by mysqlx_set_order_by

/*! @brief MYSQLX API
 *  @details a macro defining a function for setting LIMIT for REMOVE
 *           operation. See mysqlx_set_limit_and_offset()
 * */
#define mysqlx_set_remove_limit_and_offset mysqlx_set_limit_and_offset

// Rows and documents

/*! @brief MYSQLX API
 * @details Fetch one row from the result and advance to the next row
 *
 * @param res pointer to the result structure
 *
 * @return pointer to MYSQLX_ROW or NULL if no more rows left or if an error
 *         occurred. In case of an error it can be retrieved using
 *         mysqlx_crud_error() or mysqlx_crud_error_message().
 *
 * @note The previously fetched row and its data will become invalid.
*/
MYSQLX_ROW * STDCALL mysqlx_row_fetch_one(MYSQLX_RESULT *res);

/*! @brief MYSQLX API
 * @details Fetch one JSON document as a character string
 *
 * @param res pointer to the result structure
 * @param[out] out_length the total number of bytes in the json string
 *             can be NULL, in that case nothing is returned through
 *             this parameter and user must ensure the data is correctly
 *             interpreted
 *
 * @return pointer to character JSON string or NULL if no more rows left.
 *         No need to free this data as it is tied and freed with the row
 *         handler.
 *
 */
const char * STDCALL mysqlx_json_fetch_one(MYSQLX_RESULT *res, size_t *out_length);

/*
 * @brief Fetch one document from the result and advance to
 * the next document
 *
 * @param res pointer to the result structure
 *
 * @return pointer to MYSQLX_DOC or NULL if no more rows left
 */
//MYSQLX_DOC * STDCALL mysqlx_doc_fetch_one(MYSQLX_RESULT *res);

/* brief MYSQLX API
 *
 * @details Get a 64-bit unsigned int number from a document. It is important to
 *          pay attention to the signed/unsigned type. Attemptining to call this
 *          function for data of the wrong type will result in error or
 *          wrong data being retrieved
 *
 * @param doc pointer to the MYSQLX_DOC structure returned by mysqlx_doc_fetch_one()
 *  to get data from
 * @param key character string key to find the right value
 * @param[out] val the pointer to a variable of the 64-bit unsigned integer
 *  type in which to write the data
 *
 * @return RESULT_OK - on success; RESULT_NULL when the value is NULL;
 *         RESULT_ERR - on error
*/
//int STDCALL mysqlx_doc_get_uint(MYSQLX_DOC *doc, const char *key, uint64_t *out);

/* brief MYSQLX API
 *
 * @details Get a 64-bit signed int number from a document. It is important to
 *          pay attention to the signed/unsigned type. Attemptining to call this
 *          function for data of the wrong type will result in error or
 *          wrong data being retrieved
 *
 * @param doc pointer to the MYSQLX_DOC structure returned by mysqlx_doc_fetch_one()
 *  to get data from
 * @param key character string key to find the right value
 * @param[out] val the pointer to a variable of the 64-bit signed integer
 *  type in which to write the data
 *
 * @return RESULT_OK - on success; RESULT_NULL when the value is NULL;
 *         RESULT_ERR - on error
*/
//int STDCALL mysqlx_doc_get_sint(MYSQLX_DOC *doc, const char *key, int64_t *out);

/* brief MYSQLX API
 *
 * @details Get a float number from a document. It is important to
 *          pay attention to the data type. Attemptining to call this
 *          function for data of the wrong type will result in error or
 *          wrong data being retrieved
 *
 * @param doc pointer to the MYSQLX_DOC structure returned by mysqlx_doc_fetch_one()
 *  to get data from
 * @param key character string key to find the right value
 * @param[out] val the pointer to a variable of the float
 *  type in which to write the data
 *
 * @return RESULT_OK - on success; RESULT_NULL when the value is NULL;
 *         RESULT_ERR - on error
*/
//int STDCALL mysqlx_doc_get_float(MYSQLX_DOC *doc, const char *key, float *out);

/* brief MYSQLX API
 *
 * @details Get a double number from a document. It is important to
 *          pay attention to the data type. Attemptining to call this
 *          function for data of the wrong type will result in error or
 *          wrong data being retrieved
 *
 * @param doc pointer to the MYSQLX_DOC structure returned by mysqlx_doc_fetch_one()
 *  to get data from
 * @param key character string key to find the right value
 * @param[out] val the pointer to a variable of the double
 *  type in which to write the data
 *
 * @return RESULT_OK - on success; RESULT_NULL when the value is NULL;
 *         RESULT_ERR - on error
*/
//int STDCALL mysqlx_doc_get_double(MYSQLX_DOC *doc, const char *key, double *out);

/* brief MYSQLX API
 *
 * @details Write resulting bytes into a pre-allocated buffer
 *
 * @param doc pointer to the MYSQLX_DOC structure returned by mysqlx_doc_fetch_one()
 *  to get data from
 * @param key character string key to find the right value
 * @param offset the number of bytes to skip before reading them from source document
 * @param[out] buf the buffer allocated on the user side in which to write data
 * @param[in,out] buf_len pointer to variable holding the length of the buffer [IN],
 *                    the number of bytes actually written into the buffer [OUT]
 *
 * @return RESULT_OK - on success; RESULT_NULL when the value is NULL;
 *         RESULT_ERR - on error
*/
//int STDCALL mysqlx_doc_get_bytes(MYSQLX_DOC *doc, const char *key, uint64_t offset,
//                                 void *out, size_t *buf_len);

/* brief MYSQLX API
 *
 * @details Write resulting character string into a pre-allocated buffer
 *
 * @param row pointer to the MYSQLX_DOC structure returned by mysqlx_doc_fetch_one()
 *  to get data from
 * @param key character string key to find the right value
 * @param offset the number of bytes to skip before reading them from source document
 * @param[out] buf the buffer allocated on the user side in which to write data
 * @param[in,out] buf_len pointer to variable holding the length of the buffer [IN],
 *                    the number of bytes actually written into the buffer [OUT]
 *
 * @return RESULT_OK - on success; RESULT_NULL when the value is NULL;
 *         RESULT_ERR - on error
*/
//int STDCALL mysqlx_doc_get_str(MYSQLX_DOC *doc, const char *key, uint64_t offset,
//                                  char *out, size_t *buf_len);

/* brief MYSQLX API
 *
 * @details Get character string keys from the current document. The keys are fetched
 *          one by one. Each call to this function gets the next key until all keys
 *          are returned. The keys sequence is not ordered.
 *
 * @param doc pointer to the MYSQLX_DOC structure returned by mysqlx_doc_fetch_one()
 *  to get data from
 *
 * @return NULL-terminated character string key or NULL if all keys had been
 *  already returned by the previous calls
*/
//char* STDCALL mysqlx_doc_get_key(MYSQLX_DOC *doc);

/* brief MYSQLX API
 *
 * @details Checks if the specific key exists in the document
 *
 * @param doc pointer to the MYSQLX_DOC structure to check for keys
 * @param key character string key to check
 *
 * @return true if the key exists, false otherwise
*/
//bool STDCALL mysqlx_doc_key_exists(MYSQLX_DOC *doc, const char *key);

/* brief MYSQLX API
 *
 * @details Return the type identifier for the value with the specified
 *  key
 *
 * @param doc pointer to the MYSQLX_DOC structure to check for values
 * @param key character string key to find the value
 *
 * @return 16-bit unsigned int number with the column type identifier
 * (see MYSQLX_DATA_TYPE enum). If the value with the specified key
 * does not exist or the error happens the function returns
 * MYSQLX_TYPE_UNDEFINED
*/
//uint16_t STDCALL mysqlx_doc_get_type(MYSQLX_DOC *doc, const char *key);


/*! @brief MYSQLX_API
 *
 * @details This function is used to process replies containing multiple result sets.
 *          Each time it is called the function reads the next result set.
 *
 * @param result result handle containing the statement/CRUD execute results
 *
 * @return RESULT_OK - on success; RESULT_NULL when there is no more results;
 *         RESULT_ERR - on error
 *
 */
int STDCALL mysqlx_next_result(MYSQLX_RESULT *res);

/*! @brief NOT IMPLEMENTED
 * @note On the implementation stage the function can change its name,
 *       parameters or return type
uint64_t STDCALL
mysqlx_get_last_insert_id(MYSQLX_RESULT *res);
 */

/*! @brief NOT IMPLEMENTED
 * @note On the implementation stage the function can change its name,
 *       parameters or return type
MYSQLX_GUID STDCALL
mysqlx_get_last_document_id(MYSQLX_RESULT *res);
 */

/* brief NOT IMPLEMENTED
 * @note On the implementation stage the function can change its name,
 *       parameters or return type
 *
uint32_t STDCALL
mysqlx_warning_count(MYSQLX_RESULT *res);
*/

/* brief NOT IMPLEMENTED
 * @note On the implementation stage the function can change its name,
 *       parameters or return type
 *
MYSQLX_ROW * STDCALL
mysqlx_get_warnings(MYSQLX_RESULT *res);
*/

/*! @brief MYSQLX API
 *
 * @details Get number of rows affected by the last operation
 *
 * @param res pointer to the result structure returned by mysqlx_crud_execute()
 *
 * @return 64-bit unsigned int number containing the number of affected rows
*/
uint64_t STDCALL
mysqlx_get_affected_count(MYSQLX_RESULT *res);

// Metadata

/*! @brief MYSQLX API
 *
 * @details Get column type identifier
 *
 * @param res pointer to the result structure returned by mysqlx_crud_execute()
 * @param pos zero-based column number
 *
 * @return 16-bit unsigned int number with the column type identifier
 * (see MYSQLX_DATA_TYPE enum)
*/
uint16_t STDCALL
mysqlx_column_get_type(MYSQLX_RESULT *res, uint32_t pos);

/*! @brief MYSQLX API
 *
 * @details Get column collation number
 *
 * @param res pointer to the result structure returned by mysqlx_crud_execute()
 * @param pos zero-based column number
 *
 * @return 16-bit unsigned int number with the column collation number
*/
uint16_t STDCALL
mysqlx_column_get_collation(MYSQLX_RESULT *res, uint32_t pos);

/*! @brief MYSQLX API
 *
 * @details Get column length
 *
 * @param res pointer to the result structure returned by mysqlx_crud_execute()
 * @param pos zero-based column number
 *
 * @return 32-bit unsigned int number indicating the maximum data length
*/
uint32_t STDCALL
mysqlx_column_get_length(MYSQLX_RESULT *res, uint32_t pos);

/*! @brief MYSQLX API
 *
 * @details Get column precision
 *
 * @param res pointer to the result structure returned by mysqlx_crud_execute()
 * @param pos zero-based column number
 *
 * @return 16-bit unsigned int number of digits after the decimal point
*/
uint16_t STDCALL
mysqlx_column_get_precision(MYSQLX_RESULT *res, uint32_t pos);

/*! @brief MYSQLX API
 *
 * @details Get column flags
 *
 * @param res pointer to the result structure returned by mysqlx_crud_execute()
 * @param pos zero-based column number
 *
 * @return 32-bit unsigned int number containing column flags
*/
uint32_t STDCALL
mysqlx_column_get_flags(MYSQLX_RESULT *res, uint32_t pos);

/*! @brief MYSQLX API
 *
 * @details Get the number of columns in the result
 *
 * @param res pointer to the result structure returned by mysqlx_crud_execute()
 *
 * @return the number of columns
*/
uint32_t STDCALL
mysqlx_column_get_count(MYSQLX_RESULT *res);

/*! @brief MYSQLX API
 *
 * @details Get column name
 *
 * @param res pointer to the result structure returned by mysqlx_crud_execute()
 * @param pos zero-based column number
 *
 * @return character string containing the column name
*/
const char * STDCALL
mysqlx_column_get_name(MYSQLX_RESULT *res, uint32_t pos);

/*! @brief MYSQLX API
 *
 * @details Get column original name
 *
 * @param res pointer to the result structure returned by mysqlx_crud_execute()
 * @param pos zero-based column number
 *
 * @return character string containing the column original name
*/
const char * STDCALL
mysqlx_column_get_original_name(MYSQLX_RESULT *res, uint32_t pos);

/*! @brief MYSQLX API
 *
 * @details Get column table
 *
 * @param res pointer to the result structure returned by mysqlx_crud_execute()
 * @param pos zero-based column number
 *
 * @return character string containing the column table name
*/
const char * STDCALL
mysqlx_column_get_table(MYSQLX_RESULT *res, uint32_t pos);

/*! @brief MYSQLX API
 *
 * @details Get column original table
 *
 * @param res pointer to the result structure returned by mysqlx_crud_execute()
 * @param pos zero-based column number
 *
 * @return character string containing the column original table
*/
const char * STDCALL
mysqlx_column_get_original_table(MYSQLX_RESULT *res, uint32_t pos);

/*! @brief MYSQLX API
 *
 * @details Get column schema
 *
 * @param res pointer to the result structure returned by mysqlx_crud_execute()
 * @param pos zero-based column number
 *
 * @return character string containing the column schema
*/
const char * STDCALL
mysqlx_column_get_schema(MYSQLX_RESULT *res, uint32_t pos);

/*! @brief MYSQLX API
 *
 * @details Get column name
 *
 * @param res pointer to the result structure returned by mysqlx_crud_execute()
 * @param pos zero-based column number
 *
 * @return character string containing the column name
*/
const char * STDCALL
mysqlx_column_get_catalog(MYSQLX_RESULT *res, uint32_t pos);

/*! @brief MYSQLX API
 *
 * @details Write resulting bytes into a pre-allocated buffer
 *
 * @param row pointer to the row structure returned by mysqlx_row_fetch_one()
 *  to get bytes from
 * @param col zero-based column number
 * @param offset the number of bytes to skip before reading them from source row
 * @param[out] buf the buffer allocated on the user side in which to write data
 * @param[in,out] buf_len pointer to variable holding the length of the buffer [IN],
 *                    the number of bytes actually written into the buffer [OUT]
 *
 * @return RESULT_OK - on success; RESULT_NULL when the value is NULL;
 *         RESULT_ERR - on error
*/
int STDCALL
mysqlx_get_bytes(MYSQLX_ROW* row, uint32_t col,
                 uint64_t offset, void *buf, size_t *buf_len);

/*! @brief MYSQLX API
 *
 * @details Get a 64-bit unsigned int number. It is important to pay attention
 *          to the signed/unsigned type of the column. Attemptining to call this
 *          function for a column with the signed integer type will result
 *          in wrong data being retrieved
 *
 * @param row pointer to the row structure returned by mysqlx_row_fetch_one()
 *  to get bytes from
 * @param col zero-based column number
 * @param[out] val the pointer to a variable of the 64-bit unsigned integer
 *  type in which to write the data
 *
 * @return RESULT_OK - on success; RESULT_NULL when the value is NULL;
 *         RESULT_ERR - on error
*/
int STDCALL
mysqlx_get_uint(MYSQLX_ROW* row, uint32_t col, uint64_t* val);

/*! @brief MYSQLX API
 *
 * @details Get a 64-bit signed int number. It is important to pay attention
 *          to the signed/unsigned type of the column. Attemptining to call this
 *          function for a column with the unsigned integer type will result
 *          in wrong data being retrieved
 *
 * @param row pointer to the row structure returned by mysqlx_row_fetch_one()
 *  to get bytes from
 * @param col zero-based column number
 * @param[out] val the pointer to a variable of the 64-bit signed integer
 *  type in which to write the data
 *
 * @return RESULT_OK - on success; RESULT_NULL when the value is NULL;
 *         RESULT_ERR - on error
*/
int STDCALL
mysqlx_get_sint(MYSQLX_ROW* row, uint32_t col, int64_t* val);

/*! @brief MYSQLX API
 *
 * @details Get a float number. It is important to pay attention
 *          to the type of the column. Attemptining to call this
 *          function for a column with the type different from float will result
 *          in wrong data being retrieved
 *
 * @param row pointer to the row structure returned by mysqlx_row_fetch_one()
 *  to get bytes from
 * @param col zero-based column number
 * @param[out] val the pointer to a variable of the float
 *  type in which to write the data
 *
 * @return RESULT_OK - on success; RESULT_NULL when the value is NULL;
 *         RESULT_ERR - on error
*/
int STDCALL
mysqlx_get_float(MYSQLX_ROW* row, uint32_t col, float* val);

/*! @brief MYSQLX API
 *
 * @details Get a double number. It is important to pay attention
 *          to the type of the column. Attemptining to call this
 *          function for a column with the type different from double will result
 *          in wrong data being retrieved
 *
 * @param row pointer to the row structure returned by mysqlx_row_fetch_one()
 *  to get bytes from
 * @param col zero-based column number
 * @param[out] val the pointer to a variable of the double
 *  type in which to write the data.
 *
 * @return RESULT_OK - on success; RESULT_NULL when the value is NULL;
 *         RESULT_ERR - on error
*/
int STDCALL
mysqlx_get_double(MYSQLX_ROW* row, uint32_t col, double *val);

/*! @brief MYSQLX API
 *
 * @details Free the result explicitly, otherwise it will be done automatically
 * when CRUD handler is destroyed.
 *
 * @param res the result handler, which should be freed
 *
 * @note make sure this function is called if the result handler is going to be
 *       re-used
*/
void STDCALL mysqlx_result_free(MYSQLX_RESULT *res);
// TODO: multi results

/*! @brief MYSQLX API
 *
 * @details Get the last error related to the CRUD operation.
 *
 * @param crud the CRUD handler to extract the error information from
 *
 * @return the error handler or NULL if there is no errors.
*/
MYSQLX_ERROR * STDCALL mysqlx_crud_error(MYSQLX_CRUD *crud);

/*! @brief MYSQLX API
 *
 * @details Get the last error message string related to the CRUD operation.
 *
 * @param crud the CRUD handler to extract the error information from
 *
 * @return the character string or NULL if there is no errors.
*/
const char * STDCALL mysqlx_crud_error_message(MYSQLX_CRUD *crud);

/*! @brief MYSQLX API
 *
 * @details Get the error message from MYSQLX_ERROR
 *
 * @param error the MYSQLX_ERROR handler to extract the error information from
 *
 * @return the character string or NULL if there is no errors.
*/
const char * STDCALL mysqlx_error_message(MYSQLX_ERROR *error);

/*! @brief MYSQLX API
 *
 * @details Get the error code from MYSQLX_ERROR
 *
 * @param error the MYSQLX_ERROR handler to extract the error information from
 *
 * @return the error number or 0 if no error
*/
unsigned int STDCALL mysqlx_error_num(MYSQLX_ERROR *error);

/*! @brief MYSQLX API
 * @details Free the CRUD handler explicitly, otherwise it will be done automatically
 * when CRUD the session is closed.
 *
 * @param crud the CRUD handler, which should be freed
 *
 * @note make sure this function is called if the crud handler is going to be
 *       re-used

 * @todo
 *   implement book-keeping to be able to free the lower-level objects
 *   by freeing the higher-level ones
*/
void STDCALL mysqlx_crud_free(MYSQLX_CRUD *crud);

/*! @brief MYSQLX API
 *
 * @details Execute a plain SQL query.
 *
 * @param sess session handle
 * @param query SQL query
 * @param length length of the query. For NULL-terminated query strings
 *               MYSQLX_NULL_TERMINATED can be specified instead of the
 *               actual length
 *
 * @return
 *   Result handle containing the results of the query.
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
 */
MYSQLX_RESULT * STDCALL mysqlx_sql_exec(MYSQLX_SESSION *sess,
                                        const char *query,
                                        size_t query_len);

/*! @brief MYSQLX API
 *
 * @details Execute a plain SQL query with parameters
 *
 * @param sess session handle
 * @param query SQL query
 * @param length length of the query. For NULL-terminated query strings
 *               MYSQLX_NULL_TERMINATED can be specified instead of the
 *               actual length
 * @param   ...  - variable parameters list consisting of (type, value) pairs
 *         terminated by PARAM_END:
 *
 *          type_id1, value1, type_id2, value2, ..., type_id_n, value_n, PARAM_END
 *          (PARAM_END marks the end of parameters list).
 *
 *          type_id is the numeric identifier, which helps to determine the type
 *          of the value provided as the next parameter. The user code must
 *          ensure that type_id corresponds to the actual value type. Otherwise,
 *          the value along with and all sequential types and values are most
 *          likely to be corrupted.
 *          Allowed types are listed in MYSQLX_DATA_TYPE enum.
 *          The MYSQLX API defines the convenience macros that help to specify
 *          the types and values: See PARAM_SINT, PARAM_UINT, PARAM_FLOAT,
 *          PARAM_DOUBLE, PARAM_BYTES, PARAM_STRING.
 *
 * @return Result handle containing the results of the query.
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
 */

MYSQLX_RESULT * STDCALL mysqlx_sql_exec_param(MYSQLX_SESSION *sess,
                                        const char *query,
                                        size_t query_len, ...);

/*! @brief MYSQLX API
 *
 * @details Execute a table SELECT CRUD operation with a WHERE clause.
 *          All columns will be selected.
 *
 * @param sess pointer to the current session handle
 * @param schema schema name. The default database will be used if NULL is specified
 *                            for this parameter.
 * @param table table name
 * @param criteria a WHERE clause for SELECT
 *
 * @return Result handle containing the results of the query.
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
*/
MYSQLX_RESULT * STDCALL
mysqlx_table_select_exec(MYSQLX_SESSION *sess, const char *schema,
                        const char *table, const char *criteria);

/*! @brief MYSQLX API
 *
 * @details Start and execute a table SELECT CRUD operation with a WHERE,
 *          ORDER BY and LIMIT clauses
 *
 * @param sess pointer to the current session handle
 * @param schema schema name. The default database will be used if NULL is specified
 *                            for this parameter.
 * @param table table name
 * @param criteria a WHERE clause for SELECT
 * @param row_count a number of rows for LIMIT
 * @param offset an offset for LIMIT
 * @param  ... - variable parameters list consisting of (expression, direction) pairs
 *         terminated by PARAM_END:
 *         expr_1, direction_1, ..., expr_n, direction_n, PARAM_END
 *         (PARAM_END marks the end of parameters list)
 *         Each expression computes value used to sort
 *         the rows/documents in ascending or descending order,
 *         as determined by direction constant
 *         (list the direction enum names).
 *         Special attention must be paid to the expression
 *         strings because the empty string "" or NULL will be treated
 *         as the end of sequence
 *
 * @return Result handle containing the results of the query.
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
*/
MYSQLX_RESULT * STDCALL
mysqlx_table_select_exec_limit(MYSQLX_SESSION *sess, const char *schema,
                               const char *table, const char *criteria,
                               uint64_t row_count, uint64_t offset, ...);

/*! @brief MYSQLX API
 *
 * @details Start and execute a table INSERT operation one row at a time
 *
 * @param sess pointer to the current session handle
 * @param schema schema name. The default database will be used if NULL is specified
 *                            for this parameter.
 * @param table table name
 * @param ... list of column-value specifications consisting of
 *            <column_name, value_type, value> triplets. The list
 *            should be terminated using PARAM_END.
 *            Allowed value types are listed in MYSQLX_DATA_TYPE enum.
 *            The MYSQLX API defines the convenience macros that help to specify
 *            the types and values: See PARAM_SINT, PARAM_UINT, PARAM_FLOAT,
 *            PARAM_DOUBLE, PARAM_BYTES, PARAM_STRING:
 *            ..., "col_uint", PARAM_UINT(uint_val),
 *                 "col_blob", PARAM_BYTES(byte_buf, buf_len),
 *                  PARAM_END
 *
 * @return Result handle containing the result.
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
*/
MYSQLX_RESULT * STDCALL
mysqlx_table_insert_exec(MYSQLX_SESSION *sess,
                        const char *schema,
                        const char *table, ...);

/*! @brief MYSQLX API
 *
 * @details Start and execute a table UPDATE operation
 *
 * @param sess pointer to the current session handle
 * @param schema schema name. The default database will be used if NULL is specified
 *                            for this parameter.
 * @param table table name
 * @param criterie the WHERE clause for UPDATE
 * @param ... list of column-value specifications consisting of
 *            <column_name, value_type, value> triplets. The list
 *            should be terminated using PARAM_END.
 *            Allowed value types are listed in MYSQLX_DATA_TYPE enum.
 *            The MYSQLX API defines the convenience macros that help to specify
 *            the types and values: See PARAM_SINT, PARAM_UINT, PARAM_FLOAT,
 *            PARAM_DOUBLE, PARAM_BYTES, PARAM_STRING, PARAM_EXPR:
 *            ..., "col_uint", PARAM_EXPR("col_uint * 100"),
 *                 "col_blob", PARAM_BYTES(byte_buf, buf_len),
 *                  PARAM_END
 *
 * @return Result handle containing the result.
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
*/
MYSQLX_RESULT * STDCALL
mysqlx_table_update_exec(MYSQLX_SESSION *sess,
                        const char *schema,
                        const char *table,
                        const char *criteria,
                        ...);

/*! @brief MYSQLX API
 *
 * @details Execute a table DELETE CRUD operation with a WHERE clause
 *
 * @param sess pointer to the current session handle
 * @param schema schema name. The default database will be used if NULL is specified
 *                            for this parameter.
 * @param table table name
 * @param criteria a WHERE clause for DELETE
 *
 * @return Result handle containing the result.
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
*/
MYSQLX_RESULT * STDCALL
mysqlx_table_delete_exec(MYSQLX_SESSION *sess, const char *schema,
                        const char *table, const char *criteria);

/*! @brief MYSQLX API
 *
 * @details Execute a collection FIND CRUD operation with a specific find
 *          criteria.
 *
 * @param sess pointer to the current session handle
 * @param schema schema name
 * @param collection collection name
 * @param criteria criteria for finding documents
 *
 * @return Result handle containing the results of FIND.
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
*/
MYSQLX_RESULT * STDCALL
mysqlx_collection_find_exec(MYSQLX_SESSION *sess, const char *schema,
                        const char *collection, const char *criteria);

/*! @brief MYSQLX API
 *
 * @details Execute adding a set of new documents into a collection.
 *          The document is defined by a JSON string like
 *          "{ key_1: value_1, ..., key_N: value_N }"
 *
 * @param sess pointer to the current session handle
 * @param schema schema name
 * @param collection collection name
 * @param ... list of parameters containing the character JSON strings
 *            describing documents to be added. Each parameter
 *            is a separate document. The list has to be terminated by
 *            PARAM_END macro
 *
 * @return Result handle containing the result of ADD.
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
*/
MYSQLX_RESULT * STDCALL
mysqlx_collection_add_exec(MYSQLX_SESSION *sess, const char *schema,
                        const char *collection, ...);

/*! @brief MYSQLX API
 * @details Set JSON values in a document using collection MODIFY operation.
 *
 * @param sess pointer to the current session handle
 * @param schema schema name
 * @param collection collection name
 * @param criteria criteria for modifying documents
 * @param ... list of parameters that come as triplets
 *       <field_path, value_type, value>
 *       Each triplet represents a value inside a document that can
 *       be located by field_path. The value_type is the type identifier
 *       for the data type of value (see MYSQLX_DATA_TYPE enum)
 *       The list is terminated by PARAM_END.
 *       For MYSQLX_TYPE_BYTES there will be one extra parameter specifying
 *       the length of the binary data:
 *       <field_path, MYSQLX_TYPE_BYTES, value, length>
  *      The MYSQLX API defines the convenience macros that help to specify
 *       the types and values: See PARAM_SINT, PARAM_UINT, PARAM_FLOAT,
 *       PARAM_DOUBLE, PARAM_BYTES, PARAM_STRING, PARAM_EXPR:
 *       ..., "a_key", PARAM_STRING("New Text Value"),
 *            "b_key", PARAM_EXPR("b_key-1000"),
 *             PARAM_END
 *
 * @return Result handle containing the result of the operation
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
 */
MYSQLX_RESULT * STDCALL
mysqlx_collection_modify_set_exec(MYSQLX_SESSION *sess, const char *schema,
                                  const char *collection, const char *criteria,
                                  ...);

/*! @brief MYSQLX API
 * @details Unset a field in a document using MODIFY operation
 *
 * @param sess pointer to the current session handle
 * @param schema schema name
 * @param collection collection name
 * @param criteria criteria for modifying documents
 * @param ... list of field paths that should be unset.
 *            The list end is marked using PARAM_END
 *
 * @return Result handle containing the result of the operation
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
 */
MYSQLX_RESULT * STDCALL
mysqlx_collection_modify_unset_exec(MYSQLX_SESSION *sess, const char *schema,
                                  const char *collection, const char *criteria,
                                  ...);

/*! @brief MYSQLX API
 * @details Remove a document from a collection that satisfies a given
 *          criteria
 *
 * @param sess pointer to the current session handle
 * @param schema schema name
 * @param collection collection name
 * @param criteria criteria for removing documents
 *
 * @return Result handle containing the result of the operation
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
 */
MYSQLX_RESULT * STDCALL
mysqlx_collection_remove_exec(MYSQLX_SESSION *sess, const char *schema,
                              const char *collection, const char*criteria);

/*! @brief MYSQLX API
 *
 * @details Get a list of schemas
 *
 * @param sess pointer to the current session handle
 * @param schema_pattern schema name pattern to search.
 *        Giving NULL for this parameter is equivalent to "%" and will
 *        result in searching for all schemas
 *
 * @return Result handle containing the result.
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
 * @note the list of schema names is returned as a set of rows. Therefore,
 *       the functins such as mysqlx_store_result() and mysqlx_row_fetch_one()
 *       could be used.
*/
MYSQLX_RESULT * STDCALL
mysqlx_get_schemas(MYSQLX_SESSION *sess, const char *schema_pattern);

/*! @brief MYSQLX API
 *
 * @details Get a list of tables and views
 *
 * @param sess pointer to the current session handle
 * @param schema name of the schema to list tables and views
 * @param table_pattern table name pattern to search.
 *        Giving NULL for this parameter is equivalent to "%" and will
 *        result in searching for all tables in the given schema
 * @param get_views flag specifying whether view names should be included
 *        into the result. 0 - do not show views (only table names are in
 *        the result), 1 - show views (table and view names are in the result)
 *
 * @return Result handle containing the result.
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
 *
 * @note the list of table/view names is returned as a set of rows. Therefore,
 *       the functins such as mysqlx_store_result() and mysqlx_row_fetch_one()
 *       could be used.
 *
 * @note this function does not return table names that represent collections.
 *       use mysqlx_get_collections() function for getting collections.
*/
MYSQLX_RESULT * STDCALL
mysqlx_get_tables(MYSQLX_SESSION *sess,
                  const char *schema,
                  const char *table_pattern,
                  int get_views);

/*! @brief MYSQLX API
 *
 * @details Get a list of collections
 *
 * @param sess pointer to the current session handle
 * @param schema name of the schema to list tables and views
 * @param col_pattern collection name pattern to search.
 *        Giving NULL for this parameter is equivalent to "%" and will
 *        result in searching for all collections in the given schema
 *
 * @return Result handle containing the result.
 *   NULL is returned only in case of an error. The error details
 *   can be obtained using mysqlx_session_error() function
 *
 * @note mysqlx_crud_execute() is not needed to execute the query
 * @note the list of table/view names is returned as a set of rows. Therefore,
 *       the functins such as mysqlx_store_result() and mysqlx_row_fetch_one()
 *       could be used.
*/
MYSQLX_RESULT * STDCALL
mysqlx_get_collections(MYSQLX_SESSION *sess,
                  const char *schema,
                  const char *col_pattern);


/*! @brief MYSQLX API
 *
 *  @details Get the number of warnings generated by an operation
 *
 *  @param res Pointer to MYSQLX_RESULT handle to get warnings from
 *
 *  @return the number of warnings returned in the result structure
*/
unsigned int STDCALL
mysqlx_result_warning_count(MYSQLX_RESULT *res);

/*! @brief MYSQLX API
 *
 *  @details Get the next warning from the result. The warning pointer returned
 *           by a previous call is invalidated.
 *
 *  @param res Pointer to MYSQLX_RESULT handle to get warnings from
 *
 *  @return the MYSQLX_ERROR structure containing info about a warning or
 *          NULL if there is no more warnings left to return.
*/
MYSQLX_ERROR * STDCALL
mysqlx_result_next_warning(MYSQLX_RESULT *res);


/*! @brief MYSQLX API
 *
 *  @details Get the value generated by the last insert in the table
 *           with auto_increment primary key
 *
 *  @param res Pointer to MYSQLX_RESULT handle to get the auto increment
 *
 *  @return the auto incremented value
 *
 *  @note with multi-row inserts the function returns the value generated
 *        for the first row
*/
uint64_t STDCALL
mysqlx_get_auto_increment_value(MYSQLX_RESULT *res);

/*! @brief MYSQLX API
 *
 * @details Begin a transaction for the session
 *
 * @param sess Pointer to MYSQLX_SESSION structure
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *
 * @note CRUD operation will belong to the transaction when
 *       it is actually executed after the transaction began, but before
 *       it is committed or rolled back even if this CRUD operation
 *       was created before mysqlx_transaction_begin() call
*/
int STDCALL
mysqlx_transaction_begin(MYSQLX_SESSION *sess);

/*! @brief MYSQLX API
 *
 * @details Commit a transaction for the session
 *
 * @param sess Pointer to MYSQLX_SESSION structure
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *
 * @note CRUD operation will belong to the transaction when
 *       it is actually executed after the transaction began, but before
 *       it is committed or rolled back even if this CRUD operation
 *       was created before mysqlx_transaction_begin() call
*/
int STDCALL
mysqlx_transaction_commit(MYSQLX_SESSION *sess);

/*! @brief MYSQLX API
 *
 * @details Rollback a transaction for the session
 *
 * @param sess Pointer to MYSQLX_SESSION structure
 *
 * @return RESULT_OK - on success; RESULT_ERR - on error
 *
 * @note CRUD operation will belong to the transaction when
 *       it is actually executed after the transaction began, but before
 *       it is committed or rolled back even if this CRUD operation
 *       was created before mysqlx_transaction_begin() call
*/
int STDCALL
mysqlx_transaction_rollback(MYSQLX_SESSION *sess);

/*! @brief MYSQLX API
 *
 * @details Get UUIDs for the documents added to the collection
 *          by the last ADD operation. The function can be used for
 *          the multi-document inserts. In this case each new generated
 *          UUID is returned by a new call to mysqlx_fetch_doc_id().
 *
 * @param result Pointer to MYSQLX_RESULT structure returned after
 *        executing the ADD operation
 *
 * @return Character string containing a generated UUID corresponding
 *         to a document previously added to a collection; NULL - if
 *         all UUIDs for all added documents have been returned
 *
 * @note The UUID result string is valid as long as the result handle is valid.
 *       Starting a new operation will invalidate it.
*/
const char * STDCALL
mysqlx_fetch_doc_id(MYSQLX_RESULT *result);

#ifdef	__cplusplus
}
#endif

#endif /* __MYSQLX_H__ */