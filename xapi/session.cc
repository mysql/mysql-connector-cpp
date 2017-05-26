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

#include <mysql_xapi.h>
#include "mysqlx_cc_internal.h"
#include <algorithm>
#include <string>

mysqlx_session_t::mysqlx_session_struct(const std::string host,
                                        unsigned int port, const string usr,
                                        const std::string *pwd,
                                        const std::string *db, bool is_node_sess)
                                        : m_sess_opt(host, port, usr, pwd, db),
                                          m_session(m_sess_opt.get_multi_source()),
                                          m_stmt(NULL), m_is_node_sess(is_node_sess)
{}

mysqlx_session_t::mysqlx_session_struct(const std::string &conn_str, bool is_node_sess)
  : m_sess_opt(conn_str),
    m_session(m_sess_opt.get_multi_source()),
    m_stmt(NULL), m_is_node_sess(is_node_sess)
{}

mysqlx_session_t::mysqlx_session_struct(mysqlx_session_options_t *opt, bool is_node_sess)
  : m_sess_opt(*opt),
    m_session(m_sess_opt.get_multi_source()),
    m_stmt(NULL), m_is_node_sess(is_node_sess)
{}


mysqlx_stmt_t * mysqlx_session_t::sql_query(const char *query, uint32_t length,
                                            bool enable_sql_x_session)
{
  if (!query || !(*query))
    throw Mysqlx_exception("Query is empty");

  if (!m_is_node_sess && !enable_sql_x_session)
    throw Mysqlx_exception(Mysqlx_exception::MYSQLX_EXCEPTION_INTERNAL, 0,
                          "Executing SQL is not supported for this session type.");
  if (m_stmt)
    delete m_stmt;

  if (length == MYSQLX_NULL_TERMINATED)
    length = (uint32_t)strlen(query);

  m_stmt = new mysqlx_stmt_t(this, query, length);
  return m_stmt;
}

/*
  This method creates a local statement operation that belongs to the session
  or an external one, which belongs to the higher level objects such as
  collection or a table
*/
mysqlx_stmt_t * mysqlx_session_t::stmt_op(const cdk::string schema, const cdk::string obj_name,
                                      mysqlx_op_t op_type, bool session_crud,
                                      mysqlx_stmt_t *parent)
{
  mysqlx_stmt_t *stmt;

  // This will be removed after adding multiple CRUD's per session
  if (session_crud && m_stmt)
    delete m_stmt;

  if (!schema.length() && !m_sess_opt.database())
  {
    if (session_crud)
      m_stmt = NULL;
    throw Mysqlx_exception("The default schema is not specified");
  }

  if (parent)
    stmt = new mysqlx_stmt_t(this, schema.length() ? schema : *m_sess_opt.database(),
                             obj_name, op_type, parent);
  else
    stmt = new mysqlx_stmt_t(this, schema.length() ? schema : *m_sess_opt.database(),
                             obj_name, op_type);

  if (session_crud)
    m_stmt = stmt;
  return stmt;
}

void mysqlx_session_t::reset_stmt(mysqlx_stmt_t*)
{
  if (m_stmt)
    delete m_stmt;
  m_stmt = NULL;
}

mysqlx_error_t * mysqlx_session_t::get_last_error()
{
  // Return session errors from CDK first
  if (m_session.entry_count())
  {
      m_error.set(&m_session.get_error());
  }
  else if (!m_error.message() && !m_error.error_num())
    return NULL;

  return &m_error;
}

const cdk::Error * mysqlx_session_t::get_cdk_error()
{
  if (m_session.entry_count())
    return &m_session.get_error();

  return NULL;
}

void mysqlx_session_t::reset_diagnostic()
{
  m_error.reset();
}

void mysqlx_session_t::admin_collection(const char *cmd,
                                     cdk::string schema,
                                     cdk::string coll_name)
{
  if (!schema.length() && !m_sess_opt.database())
  {
    throw Mysqlx_exception("The default schema is not specified");
  }

  Db_obj_ref tab_ref(schema.length() ? schema : *m_sess_opt.database(), coll_name);
  cdk::Reply reply;
  reply = m_session.admin(cmd, tab_ref);
  reply.wait();
  if (reply.entry_count())
  {
    const cdk::Error &err = reply.get_error();
    if (err.code() != cdk::server_error(SKIP_ERROR_COLL_EXISTS))
      throw err;
  }
}

void mysqlx_session_t::create_schema(const char *schema)
{
  if (!schema || !(*schema))
    throw Mysqlx_exception(MYSQLX_ERROR_MISSING_SCHEMA_NAME_MSG);

  std::stringstream sstr;
  sstr << "CREATE SCHEMA IF NOT EXISTS `" << schema << "`";

  cdk::Reply reply;
  reply = m_session.sql(sstr.str());
  reply.wait();
  if (reply.entry_count())
    throw reply.get_error();
}

mysqlx_schema_t & mysqlx_session_t::get_schema(const char *name, bool check)
{
  if (!name || !(*name))
    throw Mysqlx_exception(MYSQLX_ERROR_MISSING_SCHEMA_NAME_MSG);
  cdk::string schema_name = name;
  Schema_map::iterator it = m_schema_map.find(schema_name);
  // Return existing schema if it was requested before
  if (it != m_schema_map.end())
    return it->second;

  m_schema_map.insert(std::make_pair(schema_name,
                      mysqlx_schema_t(*this, schema_name, check)));
  return m_schema_map.at(schema_name);
}

void mysqlx_session_t::drop_object(cdk::string schema, cdk::string name,
                                 Object_type obj_type)

{
  if (obj_type == COLLECTION)
  {
    admin_collection("drop_collection", schema, name);
    return;
  }

  cdk::Reply reply;
  std::stringstream sstr;

  switch(obj_type)
  {
    case SCHEMA:
      sstr << "DROP SCHEMA "; break;
    case TABLE:
      sstr << "DROP TABLE "; break;
    case VIEW:
      sstr << "DROP VIEW "; break;
    break;
    default:
      throw Mysqlx_exception(Mysqlx_exception::MYSQLX_EXCEPTION_INTERNAL, 0,
                             "Attempt to drop an object of unknown type.");
  }

  sstr << "IF EXISTS ";

  if (schema.length())
    sstr << " `" << schema << "`";

  if (schema.length() && name.length())
    sstr << ".";

  if (name.length())
    sstr << " `" << name << "`";

  reply = m_session.sql(sstr.str());
  reply.wait();

  if (reply.entry_count())
    throw reply.get_error();
}

void mysqlx_session_t::transaction_begin()
{
  m_session.begin();
}

void mysqlx_session_t::transaction_commit()
{
  m_session.commit();
}

void mysqlx_session_t::transaction_rollback()
{
  m_session.rollback();
}

mysqlx_session_t::~mysqlx_session_struct()
{
  try
  {
    reset_stmt(m_stmt);
    m_session.close();
  }
  catch(...)
  {
    // Do not do anything
  }
}

mysqlx_session_options_struct::mysqlx_session_options_struct() :
                               m_source_state(source_state::unknown)
{
#ifdef WITH_SSL
  set_ssl_mode(SSL_MODE_PREFERRED);
#else
  set_ssl_mode(SSL_MODE_DISABLED);
#endif
}

mysqlx_session_options_struct::mysqlx_session_options_struct(
                              const std::string host, unsigned short port,
                              const std::string usr, const std::string *pwd,
                              const std::string *db,
                              unsigned int ssl_mode) :
                              m_source_state(source_state::non_priority),
                              m_tcp_opts(usr, pwd)
{
  if (db)
    set_database(*db);

  set_ssl_mode(ssl_mode);

#ifndef WITH_SSL
  if (ssl_mode > SSL_MODE_DISABLED)
    set_diagnostic(MYSQLX_ERROR_NO_TLS_SUPPORT, 0);
#endif
  TCPIP_t tcp(host, port);
  m_host_list.emplace_back(0, tcp);
}


mysqlx_session_options_struct::mysqlx_session_options_struct(const mysqlx_session_options_struct &opt) :
  m_source_state(opt.m_source_state),
  m_tcp_opts(opt.m_tcp_opts),
  m_host_list(opt.m_host_list)
{}


mysqlx_session_options_struct::mysqlx_session_options_struct(const std::string &conn_str) :
                               m_source_state(source_state::unknown)
{
#ifdef WITH_SSL
    set_ssl_mode(SSL_MODE_PREFERRED);
#else
    set_ssl_mode(SSL_MODE_DISABLED);
#endif
    parser::parse_conn_str(conn_str, *this);
}

void mysqlx_session_options_struct::set_ssl_mode(unsigned int ssl_mode)
{
  m_tls_options.set_ssl_mode(uint_to_ssl_mode(ssl_mode));
  m_tcp_opts.set_tls(m_tls_options);
}

unsigned int mysqlx_session_options_struct::get_ssl_mode()
{
  return ssl_mode_to_uint(m_tls_options.ssl_mode());
}

cdk::ds::TCPIP::Options &mysqlx_session_options_struct::get_tcpip_options()
{
  return m_tcp_opts;
}

void mysqlx_session_options_struct::set_multiple_options(va_list args)
{
  mysqlx_opt_type_t type;
  unsigned short priority = 0;
  TCPIP_t ds;
  bool host_is_set = false;
  bool port_is_set = false;

  // The type is promoted to int when passing into va_list
  while( (type = (mysqlx_opt_type_t)(va_arg(args, int))) > 0 )
  {
    {
      unsigned int uint_data = 0;
      const char *char_data = NULL;

      if (type != MYSQLX_OPT_HOST &&
          type != MYSQLX_OPT_PORT &&
          type != MYSQLX_OPT_PRIORITY &&
          m_options_used.test(type))
      {
        throw Mysqlx_exception(MYSQLX_ERROR_DUPLICATED_OPTION);
      }

      m_options_used.set(type);

      switch (type)
      {
        case MYSQLX_OPT_HOST:
          char_data = va_arg(args, char*);
          if (char_data == NULL)
          {
            throw Mysqlx_exception(MYSQLX_ERROR_MISSING_HOST_NAME);
          }
          ds.set_host(char_data);
          host_is_set = true;
          break;
        case MYSQLX_OPT_PORT:
          uint_data = (va_arg(args, unsigned int));
          ds.set_port(static_cast<unsigned short>(uint_data));
          port_is_set = true;
          break;
        case MYSQLX_OPT_PRIORITY:
          uint_data = (va_arg(args, unsigned int));
          priority = (unsigned short)uint_data + 1;
          break;
        case MYSQLX_OPT_USER:
          char_data = va_arg(args, char*);
          if (char_data == NULL)
            char_data = "";
          m_tcp_opts.set_user(char_data);
          break;
        case MYSQLX_OPT_PWD:
          char_data = va_arg(args, char*);
          m_tcp_opts.set_pwd(char_data);
          break;
        case MYSQLX_OPT_DB:
          char_data = va_arg(args, char*);
          if (char_data == NULL)
            char_data = "";
          m_tcp_opts.set_database(char_data);
          break;

#ifdef WITH_SSL
        case MYSQLX_OPT_SSL_CA:
          char_data = va_arg(args, char*);
          set_ssl_ca(char_data);
          break;
        case MYSQLX_OPT_SSL_MODE:
          uint_data = va_arg(args, unsigned int);
          set_ssl_mode(uint_data);
          break;
#else
        case MYSQLX_OPT_SSL_MODE:
        case MYSQLX_OPT_SSL_CA:
          throw Mysqlx_exception(MYSQLX_ERROR_NO_TLS_SUPPORT);
          break;
#endif
        default:
          throw Mysqlx_exception("Invalid option value");
      }
    }
  }

  if ((priority > 0 && m_source_state == source_state::non_priority) ||
      (priority == 0 && host_is_set &&  m_source_state == source_state::priority))
  {
    throw Mysqlx_exception(MYSQLX_ERROR_MIX_PRIORITY);
  }

  m_source_state = (priority > 0) ? source_state::priority : source_state::non_priority;

  if ((port_is_set || priority > 0) && !host_is_set)
    throw Mysqlx_exception(MYSQLX_ERROR_MISSING_HOST_NAME);

  if (host_is_set)
  {
    // Host list is updated only if a new host is specified
    m_host_list.emplace_back(priority, ds);
  }
}

cdk::ds::Multi_source &mysqlx_session_options_struct::get_multi_source()
{
  if (m_source_state == source_state::unknown)
    throw Mysqlx_exception(MYSQLX_ERROR_MISSING_CONN_INFO);

  m_ms.clear();

  for (Host_list::iterator it = m_host_list.begin();
        it != m_host_list.end(); ++it)
  {
    m_ms.add(static_cast<cdk::ds::TCPIP>(it->second),
              static_cast<cdk::ds::TCPIP::Options>(m_tcp_opts),
              it->first); // prio
  }
  return m_ms;
}

cdk::connection::TLS::Options::SSL_MODE mysqlx_session_options_struct::uint_to_ssl_mode(unsigned int mode)
{
  switch (mode)
  {
    case SSL_MODE_DISABLED:
      return cdk::connection::TLS::Options::SSL_MODE::DISABLED;
    case SSL_MODE_PREFERRED:
      return cdk::connection::TLS::Options::SSL_MODE::PREFERRED;
    case SSL_MODE_REQUIRED:
      return cdk::connection::TLS::Options::SSL_MODE::REQUIRED;
    case SSL_MODE_VERIFY_CA:
      return cdk::connection::TLS::Options::SSL_MODE::VERIFY_CA;
    case SSL_MODE_VERIFY_IDENTITY:
      return cdk::connection::TLS::Options::SSL_MODE::VERIFY_IDENTITY;
    default:
      throw Mysqlx_exception(MYSQLX_ERROR_WRONG_SSL_MODE);
  }
}

unsigned int mysqlx_session_options_struct::ssl_mode_to_uint(cdk::connection::TLS::Options::SSL_MODE mode)
{
  switch (mode)
  {
    case cdk::connection::TLS::Options::SSL_MODE::DISABLED:
      return SSL_MODE_DISABLED;
    case cdk::connection::TLS::Options::SSL_MODE::PREFERRED:
      return SSL_MODE_PREFERRED;
    case cdk::connection::TLS::Options::SSL_MODE::REQUIRED:
      return SSL_MODE_REQUIRED;
    case cdk::connection::TLS::Options::SSL_MODE::VERIFY_CA:
      return SSL_MODE_VERIFY_CA;
    case cdk::connection::TLS::Options::SSL_MODE::VERIFY_IDENTITY:
      return SSL_MODE_VERIFY_IDENTITY;
    default:
      throw Mysqlx_exception(MYSQLX_ERROR_WRONG_SSL_MODE);
  }
}

const std::string mysqlx_session_options_struct::get_host()
{
  if (m_host_list.size() == 0)
    return TCPIP_t().host(); // return the default host name

  return m_host_list[m_host_list.size()-1].second.host();
}

unsigned int mysqlx_session_options_struct::get_priority()
{
  if (m_source_state != source_state::priority || m_host_list.size() == 0)
    throw Mysqlx_exception("Priority is not available");

  unsigned short prio = m_host_list[m_host_list.size() - 1].first;
  return prio ? prio - 1 : 0;
}

unsigned int mysqlx_session_options_struct::get_port()
{
  if (m_host_list.size() == 0)
    throw Mysqlx_exception(MYSQLX_ERROR_MISSING_CONN_INFO);

  return m_host_list.back().second.port();
}

const std::string mysqlx_session_options_struct::get_user()
{
  return m_tcp_opts.user();
}

const std::string* mysqlx_session_options_struct::get_password()
{
  return m_tcp_opts.password();
}

const cdk::string* mysqlx_session_options_struct::get_db()
{
  return m_tcp_opts.database();
}

#define PRIORITY_CHECK if ((priority > 0 && m_source_state == source_state::non_priority) || \
                           (priority == 0 && m_source_state == source_state::priority)) \
 throw Mysqlx_exception(MYSQLX_ERROR_MIX_PRIORITY); \
else \
 m_source_state = (priority > 0) ? source_state::priority : source_state::non_priority


// Implementing URI_Processor interface
void mysqlx_session_options_struct::host(unsigned short priority,
          const std::string &host,
          unsigned short port)
{
  PRIORITY_CHECK;
  if (!port)
    throw Mysqlx_exception("Wrong value for port");

  m_host_list.emplace_back(priority, TCPIP_t(host, port));
}

// Implementing URI_Processor interface
void mysqlx_session_options_struct::host(unsigned short priority,
          const std::string &host_name)
{
  PRIORITY_CHECK;

  TCPIP_t tcp;
  tcp.set_host(host_name);
  m_host_list.emplace_back(priority, tcp);
}


void mysqlx_session_options_struct::key_val(const std::string&)
{
  // So far there is no supported options as "?key"
  set_diagnostic("Wrong connection option", 0);
}

void mysqlx_session_options_struct::key_val(const std::string& key, const std::string& val)
{
  std::string lc_key=key;
  lc_key.resize(key.size());
  std::transform(key.begin(), key.end(), lc_key.begin(), ::tolower);

  if (lc_key.find("ssl-", 0) == 0)
  {
#ifdef WITH_SSL
    if (lc_key == "ssl-ca")
    {
      if (m_options_used.test(MYSQLX_OPT_SSL_CA))
      {
        throw Mysqlx_exception("Option ssl-ca defined twice");
      }

      m_options_used.set(MYSQLX_OPT_SSL_CA);

      set_ssl_ca(val);
    }
    else if (lc_key  == "ssl-mode")
    {
      if (m_options_used.test(MYSQLX_OPT_SSL_MODE))
      {
        throw Mysqlx_exception("Option ssl-mode defined twice");
      }

      m_options_used.set(MYSQLX_OPT_SSL_MODE);

      std::string lc_val;
      lc_val.resize(val.size());
      std::transform(val.begin(), val.end(), lc_val.begin(), ::tolower);

      if (lc_val == "disabled")
      {
        m_tls_options.set_ssl_mode(cdk::connection::TLS::Options::SSL_MODE::DISABLED);
      }
      else if (lc_val == "preferred")
      {
        m_tls_options.set_ssl_mode(cdk::connection::TLS::Options::SSL_MODE::PREFERRED);
      }
      else if (lc_val == "required")
      {
        m_tls_options.set_ssl_mode(cdk::connection::TLS::Options::SSL_MODE::REQUIRED);
      }
      else if (lc_val == "verify_ca")
      {
        m_tls_options.set_ssl_mode(cdk::connection::TLS::Options::SSL_MODE::VERIFY_CA);
      }
      else if (lc_val == "verify_identity")
      {
        m_tls_options.set_ssl_mode(cdk::connection::TLS::Options::SSL_MODE::VERIFY_IDENTITY);
      }
      else
      {
        std::stringstream err;
        err << "Unexpected key " << key << "=" << val << " in URI";
        throw Mysqlx_exception(err.str().c_str());
      }
    }
    else
    {
      std::stringstream err;
      err << "Wrong option " << key << " in URI";
      throw Mysqlx_exception(err.str().c_str());
    }

    m_tcp_opts.set_tls(m_tls_options);
#else
    set_diagnostic(MYSQLX_ERROR_NO_TLS_SUPPORT, 0);
#endif
  }
}


mysqlx_session_options_struct::~mysqlx_session_options_struct()
{}
