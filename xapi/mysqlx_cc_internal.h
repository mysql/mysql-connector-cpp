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

#include <mysql/cdk.h>

#include "../global.h"
#include "../common/session.h"
#include "../common/settings.h"
#include "../common/db_object.h"

#include "def_internal.h"
#include "error_internal.h"
#include "crud_internal.h"


using common::throw_error;
using mysqlx::common::Schema_ref;
using Db_obj_ref = mysqlx::common::Object_ref;

class Diag_info_list;


struct mysqlx_session_options_struct
  : public Mysqlx_diag
  , common::Settings_impl
{
public:

  using Settings_impl::Option;

  mysqlx_session_options_struct() = default;

  mysqlx_session_options_struct(const std::string host, unsigned short port,
                           const std::string usr, const std::string *pwd,
                           const std::string *db,
                           unsigned int ssl_mode =
#ifdef WITH_SSL
                           SSL_MODE_REQUIRED
#else
                           SSL_MODE_DISABLED
#endif
  )
  {
    Setter set(*this);
    set.host(0, host, port);
    set.user(usr);
    if (pwd)
      set.password(*pwd);
    if (db)
      set.schema(*db);
#ifdef WITH_SSL
    set.key_val(Option::SSL_MODE)->scalar()->num(uint64_t(ssl_mode));
#endif
    set.commit();
  }

  mysqlx_session_options_struct(const std::string &conn_str)
  {
    set_from_uri(conn_str);
  }


  using Settings_impl::get;

  bool has_option(mysqlx_opt_type_enum opt)
  {
    // Note: assumes the same enum values as used in Settings_impl::Option
    return Settings_impl::has_option(Option(opt));
  }

  void clear()
  {
    Mysqlx_diag::clear();
    Settings_impl::clear();
  }

  friend mysqlx_session_struct;
  friend int mysqlx_session_option_set(mysqlx_session_options_struct*, ...);
};


template <class handle_t>
class Db_obj_cache
{
  using map_t = std::map<cdk::string, handle_t>;
  map_t     m_storage;

public:

  template <typename T>
  handle_t* get(T &parent, const char *name)
  {
    cdk::string obj_name(name);

    map_t::iterator it = m_storage.find(obj_name);
    if (it == m_storage.end())
      it = m_storage.emplace(obj_name, handle_t(parent, obj_name)).first;

    return &it->second;
  }

};


// TODO: Do not store entries for non-existing objects (if check was made)

template <class handle_t>
inline
handle_t* check_existence(bool check, handle_t *obj)
{
  assert(obj);
  if (check && !obj->exists())
    throw Mysqlx_exception("Database object does not exist");
  return obj;
}


struct mysqlx_session_struct
  : public Mysqlx_diag
{
  using Stmt_cache = std::forward_list<mysqlx_stmt_struct>;

public:

  std::shared_ptr<mysqlx::common::Session_impl> m_impl;

  Stmt_cache                         m_stmts;
  Db_obj_cache<mysqlx_schema_struct> m_schema_map;

  /*
    Create new statement. This session takes its ownership.
  */

  template <mysqlx_op_t OP, typename... T>
  mysqlx_stmt_struct* new_stmt(const T&... args)
  {
    using Impl = typename stmt_traits<OP>::Impl;

    Impl *impl = new Impl(m_impl, args...);
    m_stmts.emplace_front(this, OP, impl);
    return &m_stmts.front();
  }

  void rm_stmt(const mysqlx_stmt_struct *ptr)
  {
    m_stmts.remove_if(
      [ptr](const mysqlx_stmt_struct &stmt) { return &stmt == ptr; }
    );
  }

public:

  mysqlx_session_struct(
    const std::string &host, unsigned short port,
    const std::string &usr, const std::string *pwd,
    const std::string *db
  );

  mysqlx_session_struct(const std::string &conn_str);
  mysqlx_session_struct(mysqlx_session_options_struct *opt);

  mysqlx_session_struct(mysqlx_session_options_struct &&opt)
    : mysqlx_session_struct(&opt)
  {}


  mysqlx::common::Session_impl& get_impl()
  {
    assert(m_impl);
    return *m_impl.get();
  }

  bool is_valid() {
    return get_impl().m_sess.is_valid() == cdk::option_t::YES;
  }

  const cdk::Error* get_cdk_error();

  cdk::Session &get_session() { return m_impl->m_sess; }

  /*
    Execute a plain SQL query (supports parameters and placeholders)
    PARAMETERS:
      query - SQL query
      length - length of the query

    RETURN:
      CRUD handler containing the results and/or error

  */
  mysqlx_stmt_t *sql_query(const char *query, uint32_t length);

  void reset_diagnostic();

  mysqlx_schema_struct* get_schema(const char *name, bool check)
  {
    assert(name && *name);
    return check_existence(check, m_schema_map.get(*this, name));
  }

  mysqlx_result_struct* get_schemas(const char *pattern_utf8 = nullptr)
  {
    cdk::string pattern(pattern_utf8);
    return new_stmt<OP_LIST_SCHEMAS>(pattern_utf8 ? pattern : L"%")->exec();
  }


  using Object_type = common::Object_type;

  void create_schema(const char *name)
  {
    assert(name && *name);
    Schema_ref schema(name);
    common::create_object<Object_type::SCHEMA>(m_impl, schema);
  }

  void drop_schema(const char *name)
  {
    assert(name && *name);
    Schema_ref schema(name);
    common::drop_object<Object_type::SCHEMA>(m_impl, schema);
  }


  void transaction_begin();
  void transaction_commit();
  void transaction_rollback();

  mysqlx_error_t *get_last_error();

  friend mysqlx_result_struct;
  friend mysqlx_stmt_struct;
};


inline
bool mysqlx_stmt_struct::session_valid()
{
  return m_session.is_valid();
}


struct mysqlx_schema_struct
  : public Mysqlx_diag
  , public common::Schema_ref
{
private:

  using Object_type = common::Object_type;

  Db_obj_cache<mysqlx_collection_struct> m_collection_map;
  Db_obj_cache<mysqlx_table_struct>      m_table_map;

  mysqlx_session_struct &m_session;

public:

  mysqlx_schema_struct(mysqlx_session_struct &session, cdk::string name)
    : common::Schema_ref(name)
    , m_session(session)
  {}

  bool exists()
  {
    return common::check_schema_exists(m_session.m_impl, *this);
  }

  mysqlx_session_struct& get_session() { return m_session; }

  mysqlx_collection_struct* get_collection(const char *name, bool check)
  {
    assert(name && *name);
    return check_existence(check, m_collection_map.get(*this, name));
  }

  mysqlx_table_struct* get_table(const char *name, bool check)
  {
    assert(name && *name);
    return check_existence(check, m_table_map.get(*this, name));
  }

  mysqlx_result_struct* get_tables(const char *pattern_utf8, bool include_views)
  {
    cdk::string pattern(pattern_utf8);

    return m_session.new_stmt<OP_LIST_TABLES>(
            *this,
            pattern_utf8 ? pattern : L"%",
            include_views
           )->exec();
  }

  mysqlx_result_struct* get_collections(const char *pattern_utf8)
  {
    cdk::string pattern(pattern_utf8);

    return m_session.new_stmt<OP_LIST_COLLECTIONS>(
              *this,
              pattern_utf8 ? pattern : L"%"
           )->exec();
  }


  void create_collection(const char *name, bool reuse)
  {
    assert(name && *name);
    Db_obj_ref coll(this->name(), name);
    common::create_object<Object_type::COLLECTION>(
      m_session.m_impl, coll, reuse
    );
  }

  template <Object_type T>
  void drop_object(const char *name)
  {
    assert(name && *name);
    Db_obj_ref obj(this->name(), name);
    common::drop_object<T>(m_session.m_impl, obj);
  }

  void drop_collection(const char *name)
  {
    drop_object<Object_type::COLLECTION>(name);
  }

};


struct mysqlx_collection_struct
  : public Mysqlx_diag
  , public common::Object_ref
{
private:

  mysqlx_session_struct &m_sess;

public:

  mysqlx_collection_struct(mysqlx_schema_struct &schema, cdk::string name)
    : common::Object_ref(schema.name(), name)
    , m_sess(schema.get_session())
  {}

  bool exists()
  {
    return
      common::check_object_exists<common::Object_type::COLLECTION>(
        m_sess.m_impl, *this
      );
  }

  mysqlx_session_t &get_session()
  {
    return m_sess;
  }

};


struct mysqlx_table_struct
  : public Mysqlx_diag
  , public common::Object_ref
{
private:

  mysqlx_session_struct &m_sess;

public:

  mysqlx_table_struct(mysqlx_schema_struct &schema, cdk::string name)
    : common::Object_ref(schema.name(), name)
    , m_sess(schema.get_session())
  {}

  bool exists()
  {
    return
      common::check_object_exists<common::Object_type::TABLE>(
        m_sess.m_impl, *this
      );
  }


  mysqlx_session_struct &get_session()
  {
    return m_sess;
  }

};


struct mysqlx_row_struct
  : public Mysqlx_diag
  , public mysqlx::common::Row_impl<>
{
  using mysqlx::common::Row_impl<>::Row_impl;
};


#endif /* __MYSQLX_CC_INTERNAL_H__ */
