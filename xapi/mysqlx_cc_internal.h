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

#ifndef __MYSQLX_CC_INTERNAL_H__
#define __MYSQLX_CC_INTERNAL_H__

#include <iostream>
#include <sstream>
#include <stdint.h>
#include <mysql/cdk.h>
#include <cstdarg>
#include <expr_parser.h>
#include <uri_parser.h>
#include <mysql/cdk/converters.h>
#include "def_internal.h"
#include "ref_internal.h"
#include "error_internal.h"
#include "row_internal.h"
#include "crud_internal.h"

#define SKIP_ERROR_COLL_EXISTS 1051

#define FILTER_TABLE 1
#define FILTER_COLLECTION 2
#define FILTER_VIEW 4

using cdk::foundation::string;

class Diag_info_list;
typedef struct MYSQLX_SESSION_T MYSQLX_SESSION;
typedef struct MYSQLX_SESSION_OPTIONS_T MYSQLX_SESSION_OPTIONS;

/*
  Implementation of Schema_ref needed for Db_obj_ref
*/
/*
  TODO: uncomment and use it when Parameters are supported
class Param_source : public cdk::Param_source
{
  struct Param_item
  {
    string name;
    MYSQLX_DATA_TYPE type;
  };

  typedef std::vector<Param_item> param_vector;
  param_vector m_list;

public:
  void process(cdk::api::Doc_processor<cdk::Value_processor> &prc) const;
};
*/


typedef struct MYSQLX_RESULT_T
{
  /*
    Class for processing the rows obtained from xplugin
  */
  class Row_processor;
  /*
    Class for buffering the column info. It ensures that the data
    is accessible as long as MYSQLX_RESULT exists

    The constructor creates the instance with empty info
    (when resized the container vector)

    Any metadata request initiates buffering the metadata for the column
    and all further requests for metadata for this column will use
    buffered info
  */
  class Column_info
  {
    private:
      std::string m_col_name, m_col_orig_name;
      std::string m_table_name, m_table_orig_name;
      std::string m_schema, m_catalog;

      bool m_inited;

    public:

    /*
      The default constructor
    */
    Column_info() : m_inited(false)
    {}

    /*
      Get column metadata and store it in the member variables
    */
    void set_info(const cdk::Column_info &col_info)
    {
        m_inited = true;
        m_col_name = col_info.name();
        m_col_orig_name = col_info.orig_name();
        m_table_name = col_info.table()->name();
        m_table_orig_name = col_info.table()->orig_name();
        m_schema = col_info.table()->schema()->name();
        m_catalog = col_info.table()->schema()->catalog()->name();
    }

    /*
      Column info is fetched on request, so we need to know if it was
      fetched for this particular column
    */
    bool inited() { return m_inited; }

    // Return column name
    const char* name() { return m_col_name.data(); }
    // Return column original name
    const char* orig_name() { return m_col_orig_name.data(); }
    // Return table name
    const char* table_name() { return m_table_name.data(); }
    // Return table original name
    const char* table_orig_name() { return m_table_orig_name.data(); }
    // Return schema name
    const char* schema() { return m_schema.data(); }
    // Return catalog name
    const char* catalog() { return m_catalog.data(); }
  };

private:
  uint64_t m_current_row;
  cdk::Cursor *m_cursor;
  cdk::Reply &m_reply;
  Row_processor *m_row_proc;
  MYSQLX_CRUD &m_crud; // parent CRUD handler
  //MYSQLX_ROW m_row;
  bool m_store_result;
  std::vector<MYSQLX_ROW*> m_row_set;
  std::vector<MYSQLX_DOC*> m_doc_set;
  cdk::scoped_ptr<MYSQLX_ERROR> m_current_warning;

  // We need column names in UTF8
  std::vector<Column_info> m_col_info;
  uint32_t m_filter_mask;
  cdk::foundation::Diagnostic_iterator *m_warning_iter;

  std::vector<std::string> m_doc_id_list;
  uint64_t m_current_id_index;

public:

  enum col_info_type { COL_INFO_NAME, COL_INFO_ORIG_NAME, COL_INFO_TABLE,
                       COL_INFO_ORIG_TABLE, COL_INFO_SCHEMA, COL_INFO_CATALOG,
                       COL_INFO_TYPE, COL_INFO_COLLATION, COL_INFO_LENGTH,
                       COL_INFO_PRECISION, COL_INFO_FLAGS };

  MYSQLX_RESULT_T(MYSQLX_CRUD &parent_crud, cdk::Reply &reply);

  ~MYSQLX_RESULT_T()
  {
    close_cursor();
    clear_rows();
  }

  /*
    Initializes the cursor for the result-set.
    Can also be used for accessing the next result set.
  */
  bool init_result(bool);

  cdk::Cursor* get_cursor() { return m_cursor; }

  void clear_rows();
  void clear_docs();
  void close_cursor();

  /*
    Get metadata information such as column name, table, etc that could
    be represented by character strings
  */
  const char *column_get_info_char(uint32_t pos, col_info_type info_type);

  /*
    Get metadata information such as column precision, flags, etc that could
    be represented by numbers
  */
  uint32_t column_get_info_int(uint32_t pos, col_info_type info_type);

  // Return number of rows affected by the last operation
  uint64_t get_affected_count();

  uint64_t get_auto_increment_value();

  uint32_t get_warning_count();

  MYSQLX_ERROR *get_next_warning();

  /*
    Read the next row from the result set and advance the cursor position
  */
  MYSQLX_ROW *read_row();

  MYSQLX_DOC *read_doc();

  const char * read_json(size_t *json_byte_size);

  void set_diagnostic(const MYSQLX_EXCEPTION &ex);

  void set_diagnostic(const char *msg, unsigned int num);

  bool row_filter(MYSQLX_ROW *row);

  void set_table_list_mask(uint32_t mask);

  /*
    Store all rows in the buffer
  */
  size_t store_result();

  /*
    Get to the next resultset
  */
  bool next_result();

  void copy_doc_ids(Doc_source &doc_src);

  const char *get_next_doc_id();

  // Return the CRUD operation, which produced the current result
  MYSQLX_CRUD &get_crud() { return m_crud; }

} MYSQLX_RESULT;

/*
  Client-side implementation of Limit
*/
class Limit : public cdk::Limit
{
private:
  const row_count_t m_row_count, m_offset;
  bool m_offset_set;
public:

  Limit(row_count_t row_count) : m_row_count(row_count), m_offset(0), m_offset_set(false) {}

  Limit(row_count_t row_count, row_count_t offset) : m_row_count(row_count), m_offset(offset),
                                                m_offset_set(true) {}

  row_count_t get_row_count() const { return m_row_count; }

  const row_count_t* get_offset() const
  {
    if(m_offset_set)
      return &m_offset;
    return NULL;
  }

  // Adding a virtual destructor to deal with warnings
  virtual ~Limit() {}
};

/*
  Trivial Format_info class that is used to report opaque blob values.
*/
struct Blob_format_info : public cdk::Format_info
{
  bool for_type(cdk::Type_info ti) const { return cdk::TYPE_BYTES == ti; }
  void get_info(cdk::Format<cdk::TYPE_BYTES>&) const {}

  // bring in the rest of overloads that would be hidden otherwise
  // (avoid compiler warning)
  using cdk::Format_info::get_info;
};

/*
  Trivial Format_info class that is used to report opaque JSON values.
*/
struct JSON_format_info : public cdk::Format_info
{
  bool for_type(cdk::Type_info ti) const { return cdk::TYPE_DOCUMENT == ti; }
  void get_info(cdk::Format<cdk::TYPE_BYTES>&) const {}

  // bring in the rest of overloads that would be hidden otherwise
  // (avoid compiler warning)
  using cdk::Format_info::get_info;
};


/*
  Internal class representing the list of items for ORDER BY
*/
class Order_by : public cdk::Order_by
{
  parser::Parser_mode::value m_parser_mode;

  /*
    Internal class representing one item in ORDER BY
  */
  class Order_by_item : public cdk::Expression
  {
    parser::Parser_mode::value m_mode;
    const char *m_expr;
    Sort_direction::value m_sort_direction;
  public:

    Order_by_item(const char *expr, Sort_direction::value sort_direction,
                  parser::Parser_mode::value mode) :
		    m_mode(mode), m_expr(expr),
        m_sort_direction(sort_direction)
    {}

    void process(cdk::Expression::Processor &prc) const
    {
      Expression_parser parser(m_mode, m_expr);
      parser.process(prc);
    }


    // Get sort direction for the current instance
    const Sort_direction::value direction() const { return m_sort_direction; }
  };

  typedef std::vector<Order_by_item> Order_item_list;
  Order_item_list m_list;
public:
  Order_by(parser::Parser_mode::value mode) : m_parser_mode(mode)
  {}

  /*
    Add one item to ORDER BY
  */
  void add_item(const char *expr, Sort_direction::value sort_direction)
  {
    m_list.push_back(Order_by_item(expr, sort_direction, m_parser_mode));
  }

  /*
    Clear the list of ORDER BY items
  */
  void clear() { m_list.clear(); }

  uint32_t count() const { return m_list.size(); }

  void process(Processor &prc) const
  {
    prc.list_begin();
    for (Order_item_list::const_iterator it = m_list.begin();
         it != m_list.end(); ++it)
    {
      // Avoid compiler warnings in OSX
      Processor *pprc = &prc;
      if (pprc)
      {
        Processor::Element_prc *list_el_ptr = prc.list_el();
        if (list_el_ptr)
          it->process_if(list_el_ptr->sort_key(it->direction()));
      }
    }
    prc.list_end();
  }

  //virtual const Expr_type& get(uint32_t pos) const { return m_list[pos].expr(); }
  Sort_direction::value get_direction(uint32_t pos) const {return m_list[pos].direction(); }
};


typedef struct MYSQLX_SESSION_OPTIONS_T : public parser::URI_processor,
                                          public cdk::ds::Options,
                                          public cdk::ds::TCPIP
{
public:

  MYSQLX_SESSION_OPTIONS_T()
  {}

  MYSQLX_SESSION_OPTIONS_T(const std::string host, unsigned short port,
                           const std::string usr, const std::string *pwd,
                           const std::string *db) :
                           cdk::ds::Options(usr, pwd),
                           cdk::ds::TCPIP(host, port ? port : DEFAULT_MYSQLX_PORT)
  {
    if (db)
      set_database(*db);
  }


  MYSQLX_SESSION_OPTIONS_T(const std::string &conn_str)
  { parser::parse_conn_str(conn_str, *this); }

  // Implementing URI_Processor interface
  void user(const std::string &usr)
  { m_usr = usr; }

  // Implementing URI_Processor interface
  void password(const std::string &pwd)
  {
    m_pwd = pwd;
    m_has_pwd = true;
  }

  // Implementing URI_Processor interface
  void host(const std::string &host)
  { m_host = host; }

  // Implementing URI_Processor interface
  void port(unsigned short port)
  { m_port = port; }

  // Implementing URI_Processor interface
  void path(const std::string &path)
  { set_database(path); }

} MYSQLX_SESSION_OPTIONS;


typedef struct MYSQLX_SESSION_T
{
private:

  MYSQLX_SESSION_OPTIONS m_sess_opt;
  cdk::Session m_session;
  MYSQLX_CRUD *m_crud;
  MYSQLX_ERROR m_error;
  bool m_is_node_sess;

public:

  enum Object_type { SCHEMA, TABLE, COLLECTION, VIEW };

  MYSQLX_SESSION_T(const std::string host, unsigned int port, const string usr,
                   const std::string *pwd, const std::string *db, bool is_node_sess);

  MYSQLX_SESSION_T(const std::string &conn_str, bool is_node_sess);

  bool is_valid() { return m_session.is_valid(); }

  const cdk::Error* get_cdk_error();

  cdk::Session &get_session() { return m_session; }

  /*
    Execute a plain SQL query (supports parameters and placeholders)
    PARAMETERS:
      query - SQL query
      length - length of the query

    RETURN:
      CRUD handler containing the results and/or error

  */
  MYSQLX_CRUD *sql_query(const char *query, uint32_t length);

  /*
    Create a new CRUD operation (SELECT, INSERT, UPDATE, DELETE)
    PARAMETERS:
      schema - schema name for the operation
      obj_name - table/collection name for the operation
      op_type - operation type. The allowed values are:
                OP_SELECT, OP_INSERT, OP_UPDATE, OP_DELETE
    RETURN:
      CRUD handler containing the results and/or error

    TODO: error must be set if op_type goes out of the allowed range
  */
  MYSQLX_CRUD *crud_op(const char *schema, const char *obj_name,
                       MYSQLX_OP op_type);

  /*
    Delete the CRUD object and reset the pointer
  */
  void reset_crud(MYSQLX_CRUD*);

  void set_diagnostic(const MYSQLX_EXCEPTION &ex);

  void set_diagnostic(const char *msg, unsigned int num);

  void reset_diagnostic();

  void create_schema(const char *schema);

  void drop_object(const char *schema, const char *name,
                   Object_type obj_type);

  void admin_collection(const char *cmd, const char *schema,
                       const char *coll_name);

  void transaction_begin();
  void transaction_commit();
  void transaction_rollback();

  MYSQLX_ERROR *get_last_error();

  ~MYSQLX_SESSION_T();

} MYSQLX_SESSION;

typedef struct MYSQLX_UPDATE_VALUE_T
{
                 // Not used
  void *context; // TODO: add something meaningful
}MYSQLX_UPDATE_VALUE;

#endif /* __MYSQLX_CC_INTERNAL_H__ */
