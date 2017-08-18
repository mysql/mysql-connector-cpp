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
#include <bitset>
#include <cstdarg>
#include <expr_parser.h>
#include <uri_parser.h>
#include <mysql/cdk/converters.h>
#include "../global.h"
#include "def_internal.h"
#include "ref_internal.h"
#include "error_internal.h"
#include "row_internal.h"
#include "view_internal.h"
#include "crud_internal.h"


#define SKIP_ERROR_COLL_EXISTS 1051

#define FILTER_TABLE 1
#define FILTER_COLLECTION 2
#define FILTER_VIEW 4

using cdk::foundation::string;

class Diag_info_list;
typedef struct mysqlx_session_struct mysqlx_session_t;
typedef struct mysqlx_session_options_struct mysqlx_session_options_t;

typedef struct mysqlx_result_struct : public Mysqlx_diag
{
  /*
    Class for processing the rows obtained from xplugin
  */
  class Row_processor;
  /*
    Class for buffering the column info. It ensures that the data
    is accessible as long as mysqlx_result_t exists

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
  mysqlx_stmt_t &m_crud; // parent CRUD handler
  //mysqlx_row_t m_row;
  bool m_store_result;
  std::vector<mysqlx_row_t*> m_row_set;
  std::vector<mysqlx_doc_t*> m_doc_set;
  cdk::scoped_ptr<mysqlx_error_t> m_current_warning;
  cdk::scoped_ptr<mysqlx_error_t> m_current_error;

  // We need column names in UTF8
  std::vector<Column_info> m_col_info;
  uint32_t m_filter_mask;
  uint32_t m_current_warning_index;
  uint32_t m_current_error_index;

  std::vector<std::string> m_doc_id_list;
  uint64_t m_current_id_index;

public:

  enum col_info_type { COL_INFO_NAME, COL_INFO_ORIG_NAME, COL_INFO_TABLE,
                       COL_INFO_ORIG_TABLE, COL_INFO_SCHEMA, COL_INFO_CATALOG,
                       COL_INFO_TYPE, COL_INFO_COLLATION, COL_INFO_LENGTH,
                       COL_INFO_PRECISION, COL_INFO_FLAGS };

  mysqlx_result_struct(mysqlx_stmt_t &parent_crud, cdk::Reply &reply);

  virtual ~mysqlx_result_struct()
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

  mysqlx_error_t *get_next_warning();

  virtual mysqlx_error_t *get_error();

  /*
    Read the next row from the result set and advance the cursor position
  */
  mysqlx_row_t *read_row();

  mysqlx_doc_t *read_doc();

  const char * read_json(size_t *json_byte_size);

  bool row_filter(mysqlx_row_t *row);

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
  mysqlx_stmt_t &get_crud() { return m_crud; }

} mysqlx_result_t;

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
    Sort_direction::value direction() const { return m_sort_direction; }
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


struct Host_sources : public cdk::ds::Multi_source
{

  inline void add(cdk::ds::TCPIP ds,
                  cdk::ds::TCPIP::Options options,
                  unsigned short prio)
  {
#ifdef WITH_SSL
    std::string host = ds.host();

    cdk::connection::TLS::Options tls = options.get_tls();

    tls.set_verify_cn(
          [host](const std::string& cn)-> bool{
      return cn == host;
    });

    options.set_tls(tls);
#endif

    cdk::ds::Multi_source::add(ds, options, prio);
  }

};


typedef struct mysqlx_session_options_struct
  : public Mysqlx_diag
  , public parser::URI_processor
{
private:

  using TLS_options = cdk::connection::TLS::Options;

  std::bitset<LAST> m_options_used;

  /*
    This struct extends cdk::ds::TCPIP to allow setting
    host and port at any time
  */

  struct TCPIP_t : public cdk::ds::TCPIP
  {
    TCPIP_t() : cdk::ds::TCPIP() {}

    TCPIP_t(const std::string &_host, unsigned short _port) :
      cdk::ds::TCPIP(_host, _port) {}

    template <typename S>
    void set_host(const S host) { m_host = host; }

    void set_port(unsigned short port) { m_port = port; }
  };

  struct TCPIP_Options_t : public cdk::ds::TCPIP::Options
  {
    TCPIP_Options_t() : cdk::ds::TCPIP::Options() {}

    TCPIP_Options_t(const string &usr, const std::string *pwd = NULL) :
      cdk::ds::TCPIP::Options(usr, pwd) {}

    template <typename S>
    void set_user(const S usr) { m_usr = usr; }

    void set_pwd(const char *pwd)
    {
      if (pwd)
      { m_has_pwd = true; m_pwd = std::string(pwd); }
      else
        m_has_pwd = false;
    }

    void set_pwd(const std::string *pwd)
    {
      if (pwd)
      { m_has_pwd = true; m_pwd = *pwd; }
      else
        m_has_pwd = false;
    }

  };

  typedef std::pair<unsigned short, TCPIP_t> Prio_host_pair;
  typedef std::vector<Prio_host_pair> Host_list;

  enum source_state
  { unknown, priority, non_priority }
  m_source_state = unknown;

  TCPIP_Options_t m_tcp_opts;
  TLS_options m_tls_options;

  Host_sources m_ms;
  Host_list    m_host_list;

  bool         m_explicit_mode = false;

  void check_option(mysqlx_opt_type_t);

public:

  mysqlx_session_options_struct(source_state state = source_state::unknown)
      : m_source_state(state)
    {
  #ifdef WITH_SSL
      set_ssl_mode(SSL_MODE_REQUIRED);
  #else
      set_ssl_mode(SSL_MODE_DISABLED);
  #endif

      m_explicit_mode = false;
    }

  mysqlx_session_options_struct(const std::string host, unsigned short port,
                           const std::string usr, const std::string *pwd,
                           const std::string *db,
                           unsigned int ssl_mode =
#ifdef WITH_SSL
                           SSL_MODE_REQUIRED
#else
                           SSL_MODE_DISABLED
#endif
  );

  mysqlx_session_options_struct(const std::string &conn_str)
    : mysqlx_session_options_struct()
  {
    parser::parse_conn_str(conn_str, *this);
  }


  // Implementing URI_Processor interface
  void user(const std::string &usr) override
  { m_tcp_opts.set_user(usr); }

  // Implementing URI_Processor interface
  void password(const std::string &pwd) override
  { m_tcp_opts.set_pwd(&pwd); }

  // Implementing URI_Processor interface
  void host(unsigned short priority, const std::string &host,
            unsigned short port) override;

  // Implementing URI_Processor interface
  void host(unsigned short priority, const std::string &host_name) override;

  // Make sure an option is set for a single data source
  void set_multiple_options(va_list args);

  void set_database(const cdk::string &db) { return m_tcp_opts.set_database(db); }

  cdk::ds::TCPIP::Options &get_tcpip_options() { return m_tcp_opts; }
  cdk::ds::Multi_source &get_multi_source() const;

  const cdk::string* database() { return m_tcp_opts.database(); }
  const std::string get_host();
  unsigned int get_port();
  unsigned int get_priority();
  const std::string get_user();
  const std::string* get_password();
  const cdk::string* get_db() const;

  void set_ssl_ca(const string &ca);
  void set_ssl_mode(mysqlx_ssl_mode_enum ssl_mode);
  unsigned int get_ssl_mode();

  // Implementing URI_Processor interface
  void schema(const std::string &path) override
  { m_tcp_opts.set_database(path); }

  void key_val(const std::string& key) override;
  void key_val(const std::string& key, const std::string& val) override;
  ~mysqlx_session_options_struct();

} mysqlx_session_options_t;


typedef struct mysqlx_schema_struct : public Mysqlx_diag
{
private:
  typedef std::map<cdk::string, mysqlx_collection_t> Collection_map;
  Collection_map m_collection_map;

  typedef std::map<cdk::string, mysqlx_table_t> Table_map;
  Table_map m_table_map;

  mysqlx_session_t &m_session;
  cdk::string m_name;
  mysqlx_stmt_t *m_stmt;

public:
  mysqlx_schema_struct(mysqlx_session_t &session, cdk::string name, bool check);

  bool exists();
  cdk::string &get_name() { return m_name; }
  mysqlx_session_t &get_session() { return m_session; }
  mysqlx_collection_t & get_collection(const char *name, bool check);
  mysqlx_table_t & get_table(const char *name, bool check);
  mysqlx_stmt_t *stmt_op(const cdk::string obj_name, mysqlx_op_t op_type,
                         mysqlx_stmt_t *parent = NULL);

  ~mysqlx_schema_struct();
} mysqlx_schema_t;


typedef struct mysqlx_collection_struct : public Mysqlx_diag
{
private:
  mysqlx_schema_t &m_schema;
  cdk::string m_name;
  mysqlx_stmt_t *m_stmt;

public:
  mysqlx_collection_struct(mysqlx_schema_t &schema, cdk::string name, bool check);
  mysqlx_session_t &get_session() { return m_schema.get_session(); }
  bool exists();
  mysqlx_stmt_t *stmt_op(mysqlx_op_t op_type);
  ~mysqlx_collection_struct();

}mysqlx_collection_t;


typedef struct mysqlx_table_struct : public Mysqlx_diag
{
private:
  mysqlx_schema_t &m_schema;
  cdk::string m_name;
  mysqlx_stmt_t *m_stmt;

public:
  mysqlx_table_struct(mysqlx_schema_t &schema, cdk::string name, bool check);

  mysqlx_session_t &get_session() { return m_schema.get_session(); }
  bool exists();
  mysqlx_stmt_t *stmt_op(mysqlx_op_t op_type);
  ~mysqlx_table_struct();

}mysqlx_table_t;


typedef struct mysqlx_session_struct : public Mysqlx_diag
{
private:

  cdk::Session m_session;
  string       m_default_db;
  mysqlx_stmt_t *m_stmt;

  typedef std::map<cdk::string, mysqlx_schema_t> Schema_map;
  Schema_map m_schema_map;

public:

  enum Object_type { SCHEMA, TABLE, COLLECTION, VIEW };

  mysqlx_session_struct(
    const std::string &host, unsigned short port,
    const string &usr, const std::string *pwd,
    const std::string *db
  );

  mysqlx_session_struct(const std::string &conn_str);
  mysqlx_session_struct(const mysqlx_session_options_t &opt);

  bool is_valid() { return m_session.is_valid() == cdk::option_t::YES; }

  const cdk::Error* get_cdk_error();

  cdk::Session &get_session() { return m_session; }

  bool cert_validation(const std::string &cn);

  /*
    Execute a plain SQL query (supports parameters and placeholders)
    PARAMETERS:
      query - SQL query
      length - length of the query

    RETURN:
      CRUD handler containing the results and/or error

  */
  mysqlx_stmt_t *sql_query(const char *query, uint32_t length);

  /*
    Create a new CRUD operation (SELECT, INSERT, UPDATE, DELETE)
    PARAMETERS:
      schema - schema name for the operation
      obj_name - table/collection name for the operation
      op_type - operation type. The allowed values are:
                OP_SELECT, OP_INSERT, OP_UPDATE, OP_DELETE
      delete_crud - flag indicates if created statement is on session level
                or above. Determining ownership of the object
                (either caller or the session) is a side aspect of this.
      parent - parent statement. Some statement types such as VIEW need to get
               the context of parent SELECT or FIND statements.
    RETURN:
      CRUD handler containing the results and/or error

    TODO: error must be set if op_type goes out of the allowed range
  */
  mysqlx_stmt_t *stmt_op(const cdk::string schema, const cdk::string obj_name,
                       mysqlx_op_t op_type, bool delete_crud = true,
                       mysqlx_stmt_t *parent = NULL);

  /*
    Delete the CRUD object and reset the pointer
  */
  void reset_stmt(mysqlx_stmt_t*);

  void reset_diagnostic();

  mysqlx_schema_t & get_schema(const char *name, bool check);

  void create_schema(const char *schema);

  void drop_object(cdk::string schema, cdk::string name,
                   Object_type obj_type);

  void admin_collection(const char *cmd, cdk::string schema,
                       cdk::string coll_name);

  void transaction_begin();
  void transaction_commit();
  void transaction_rollback();

  mysqlx_error_t *get_last_error();

  ~mysqlx_session_struct();

} mysqlx_session_t;

#endif /* __MYSQLX_CC_INTERNAL_H__ */
