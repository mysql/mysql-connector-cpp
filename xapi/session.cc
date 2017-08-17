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

const unsigned max_priority = 100;


mysqlx_session_struct::mysqlx_session_struct(
  const mysqlx_session_options_t &opt
)
  : m_session(opt.get_multi_source()),
    m_stmt(NULL)
{
  const string *db = opt.get_db();
  if (db)
    m_default_db = *db;
}

mysqlx_session_struct::mysqlx_session_struct(
  const std::string &host, unsigned short port,
  const string &usr, const std::string *pwd,
  const std::string *db
)
  : mysqlx_session_struct(mysqlx_session_options_t(host, port, usr, pwd, db))
{}

mysqlx_session_struct::mysqlx_session_struct(
  const std::string &conn_str
)
  : mysqlx_session_struct(mysqlx_session_options_t(conn_str))
{}



mysqlx_stmt_t * mysqlx_session_t::sql_query(const char *query, uint32_t length)
{
  if (!query || !(*query))
    throw Mysqlx_exception("Query is empty");

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

  if (!schema.length() && m_default_db.empty())
  {
    if (session_crud)
      m_stmt = NULL;
    throw Mysqlx_exception("The default schema is not specified");
  }

  if (parent)
    stmt = new mysqlx_stmt_t(this, schema.length() ? schema : m_default_db,
                             obj_name, op_type, parent);
  else
    stmt = new mysqlx_stmt_t(this, schema.length() ? schema : m_default_db,
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
  if (!schema.length() && m_default_db.empty())
  {
    throw Mysqlx_exception("The default schema is not specified");
  }

  Db_obj_ref tab_ref(schema.length() ? schema : m_default_db, coll_name);
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

using cdk::foundation::connection::TLS;


TLS::Options::SSL_MODE uint_to_ssl_mode(unsigned int mode)
{
  switch (mode)
  {
    case SSL_MODE_DISABLED:
      return TLS::Options::SSL_MODE::DISABLED;
    case SSL_MODE_REQUIRED:
      return TLS::Options::SSL_MODE::REQUIRED;
    case SSL_MODE_VERIFY_CA:
      return TLS::Options::SSL_MODE::VERIFY_CA;
    case SSL_MODE_VERIFY_IDENTITY:
      return TLS::Options::SSL_MODE::VERIFY_IDENTITY;
    default:
      assert(false);
      // Quiet compile warnings
      return TLS::Options::SSL_MODE::DISABLED;
  }
}

unsigned int ssl_mode_to_uint(TLS::Options::SSL_MODE mode)
{
  switch (mode)
  {
    case TLS::Options::SSL_MODE::DISABLED:
      return SSL_MODE_DISABLED;
    case TLS::Options::SSL_MODE::REQUIRED:
      return SSL_MODE_REQUIRED;
    case TLS::Options::SSL_MODE::VERIFY_CA:
      return SSL_MODE_VERIFY_CA;
    case TLS::Options::SSL_MODE::VERIFY_IDENTITY:
      return SSL_MODE_VERIFY_IDENTITY;
    default:
      assert(false);
      // Quiet compile warnings
      return 0;
  }
}

const char* opt_name(mysqlx_opt_type_t opt)
{
  switch (opt)
  {
  case MYSQLX_OPT_HOST: return "hoar";
  case MYSQLX_OPT_PORT: return "port";
  case MYSQLX_OPT_USER: return "user";
  case MYSQLX_OPT_PWD: return "pwd";
  case MYSQLX_OPT_DB: return "db";
  case MYSQLX_OPT_SSL_MODE: return "ssl-mode";
  case MYSQLX_OPT_SSL_CA: return "ssl-ca";
  case MYSQLX_OPT_PRIORITY: return "priority";
  default: return "<unknown>";
  }
}

const char* ssl_mode_name(mysqlx_ssl_mode_t m)
{
  switch (m)
  {
  case SSL_MODE_DISABLED: return "DISABLED";
  case SSL_MODE_REQUIRED: return "REQUIRED";
  case SSL_MODE_VERIFY_CA: return "VERIFY_CA";
  case SSL_MODE_VERIFY_IDENTITY: return "VERIFY_IDENTITY";
  default: return "<unknown>";
  }
}

struct Error_bad_option : public Mysqlx_exception
{
  Error_bad_option()
    : Mysqlx_exception("Unrecognized connection option")
  {}

  Error_bad_option(const std::string &opt) : Error_bad_option()
  {
    m_message += ": " + opt;
  }

  Error_bad_option(unsigned int opt) : Error_bad_option()
  {
    std::ostringstream buf;
    buf << opt;
    m_message += " (" + buf.str() + ")";
  }
};

struct Error_dup_option : public Mysqlx_exception
{
  Error_dup_option(mysqlx_opt_type_t opt)
  {
    m_message = "Option ";
    m_message += opt_name(opt);
    m_message += " defined twice";
  }
};

struct Error_bad_mode : public Mysqlx_exception
{
  Error_bad_mode(const std::string &m)
  {
    m_message = "Unrecognized ssl-mode: " + m;
  }
};

struct Error_ca_mode : public Mysqlx_exception
{
  Error_ca_mode()
    : Mysqlx_exception("The ssl-ca option is not compatible with ssl-mode")
  {}

  Error_ca_mode(mysqlx_ssl_mode_t m) : Error_ca_mode()
  {
    m_message += " ";
    m_message += ssl_mode_name(m);
  }

  Error_ca_mode(TLS::Options::SSL_MODE m)
    : Error_ca_mode(mysqlx_ssl_mode_t(ssl_mode_to_uint(m)))
  {}
};

void  mysqlx_session_options_struct::check_option(mysqlx_opt_type_t opt)
{
  switch (opt)
  {
  case MYSQLX_OPT_HOST:
  case MYSQLX_OPT_PORT:
  case MYSQLX_OPT_PRIORITY:
    return;

  default:
    if (!(opt < mysqlx_opt_type_t::LAST))
      throw Error_bad_option(opt);
    if (m_options_used.test(opt))
      throw Error_dup_option(opt);
    m_options_used.set(opt);
  }
};

mysqlx_session_options_struct::mysqlx_session_options_struct(
                              const std::string host, unsigned short port,
                              const std::string usr, const std::string *pwd,
                              const std::string *db,
                              unsigned int ssl_mode)
  : mysqlx_session_options_struct(source_state::non_priority)
{
  this->host(0, host, (port ? port : DEFAULT_MYSQLX_PORT));
  this->user(usr);

  if (pwd)
    this->password(*pwd);

  if (db)
    set_database(*db);

  set_ssl_mode(mysqlx_ssl_mode_t(ssl_mode));

#ifndef WITH_SSL
  if (ssl_mode != SSL_MODE_DISABLED)
    set_diagnostic(MYSQLX_ERROR_NO_TLS_SUPPORT, 0);
#endif

  m_explicit_mode = false;
}


void mysqlx_session_options_t::set_ssl_ca(const string &ca)
{

  if (m_explicit_mode)
  {
    switch (auto m = m_tls_options.ssl_mode())
    {
    case TLS::Options::SSL_MODE::DISABLED:
    case TLS::Options::SSL_MODE::REQUIRED:
      throw Error_ca_mode(m);

    case TLS::Options::SSL_MODE::PREFERRED:
      assert(false);

    case TLS::Options::SSL_MODE::VERIFY_CA:
    case TLS::Options::SSL_MODE::VERIFY_IDENTITY:
      break;
    }
  }
  else
  {
    m_tls_options.set_ssl_mode(TLS::Options::SSL_MODE::VERIFY_CA);
  }

  m_tls_options.set_ca(ca);
  m_tcp_opts.set_tls(m_tls_options);
}


void mysqlx_session_options_struct::set_ssl_mode(mysqlx_ssl_mode_t ssl_mode)
{
  auto m = uint_to_ssl_mode(ssl_mode);

  if (!m_tls_options.get_ca().empty())
  {
    switch (ssl_mode)
    {
    case SSL_MODE_DISABLED:
    case SSL_MODE_REQUIRED:
      throw Error_ca_mode(ssl_mode);

    case SSL_MODE_VERIFY_CA:
    case SSL_MODE_VERIFY_IDENTITY:
      break;
    }
  }

  m_tls_options.set_ssl_mode(m);
  m_tcp_opts.set_tls(m_tls_options);
  m_explicit_mode = true;
}

unsigned int mysqlx_session_options_struct::get_ssl_mode()
{
  return ssl_mode_to_uint(m_tls_options.ssl_mode());
}


#define PRIO_CHECK if ((add_list.priority > 0 && m_source_state == source_state::non_priority) || \
(add_list.priority == 0 && ds &&  m_source_state == source_state::priority)) \
  throw Mysqlx_exception(MYSQLX_ERROR_MIX_PRIORITY)


void mysqlx_session_options_struct::set_multiple_options(va_list args)
{
  mysqlx_opt_type_t type;
  enum {TCPIP_type, UNIX_type} selected_type = TCPIP_type;
  Ds_variant ds;
  m_options_used.reset();



#ifndef _WIN32
  Add_list add_list(m_host_list, m_last_tcpip, m_last_socket);
#else
  Add_list add_list(m_host_list, m_last_tcpip);
#endif

  // The type is promoted to int when passing into va_list
  while( (type = (mysqlx_opt_type_t)(va_arg(args, int))) > 0 )
  {
    {
      unsigned int uint_data = 0;
      const char *char_data = NULL;

      check_option(type);

      switch (type)
      {
        case MYSQLX_OPT_HOST:
          if (ds)
          {
            PRIO_CHECK;

            ds.visit(add_list);

            add_list.priority = 0;
          }

          char_data = va_arg(args, char*);
          if (char_data == NULL)
            throw Mysqlx_exception(MYSQLX_ERROR_MISSING_HOST_NAME);

          ds = TCPIP_t(char_data);
          selected_type = TCPIP_type;
          break;
        case MYSQLX_OPT_PORT:
          uint_data = (va_arg(args, unsigned int));
          if (!ds)
            throw Mysqlx_exception(MYSQLX_ERROR_MISSING_HOST_NAME);

          if (selected_type != TCPIP_type)
            throw Mysqlx_exception(MYSQLX_ERROR_MISSING_HOST_NAME);

          const_cast<TCPIP_t&>(ds.get<TCPIP_t>()).set_port(static_cast<unsigned short>(uint_data));

          break;
#ifndef _WIN32
        case MYSQLX_OPT_SOCKET:
        {
          if (ds)
          {
            PRIO_CHECK;
            ds.visit(add_list);

            add_list.priority = 0;
          }
          char_data = va_arg(args, char*);

          ds = cdk::ds::Unix_socket(char_data);
          selected_type = UNIX_type;
        }
          break;
#endif //_WIN32
        case MYSQLX_OPT_PRIORITY:
          if (m_source_state == source_state::non_priority)
            throw Mysqlx_exception(MYSQLX_ERROR_MIX_PRIORITY);

          if (!ds)
            throw Mysqlx_exception(MYSQLX_ERROR_MISSING_HOST_NAME);

          uint_data = (va_arg(args, unsigned int));
          if (uint_data > max_priority)
            throw Mysqlx_exception(MYSQLX_ERROR_MAX_PRIORITY);

          add_list.priority = (unsigned short)uint_data + 1;
          m_source_state = source_state::priority;
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
          m_has_ssl = true;
          break;
        case MYSQLX_OPT_SSL_MODE:
          uint_data = va_arg(args, unsigned int);
          set_ssl_mode(mysqlx_ssl_mode_t(uint_data));
          m_has_ssl = true;
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

  PRIO_CHECK;

  if (ds)
  {
    m_source_state = (add_list.priority > 0) ? source_state::priority : source_state::non_priority;
    // Host list is updated only if a new host is specified
    ds.visit(add_list);
  }
}

cdk::ds::Multi_source&
mysqlx_session_options_struct::get_multi_source() const
{
  auto *self = const_cast<mysqlx_session_options_struct*>(this);

  if (mysqlx_error_t *err = self->get_error())
    throw Mysqlx_exception(
      Mysqlx_exception::MYSQLX_EXCEPTION_INTERNAL,
      err->error_num(), err->message());

  if (m_source_state == source_state::unknown)
    throw Mysqlx_exception(MYSQLX_ERROR_MISSING_CONN_INFO);

  self->m_ms.clear();


  struct Sources_add
  {
    Host_sources& m_sources;
    cdk::ds::TCPIP::Options &m_tcp_opt;
#ifndef _WIN32
    cdk::ds::Unix_socket::Options &m_unix_opt;
#endif //_WIN32

    unsigned short m_priority;
    bool m_socket_only = true;

    Sources_add(Host_sources& sources
                ,cdk::ds::TCPIP::Options &tcp_opt
            #ifndef _WIN32
                ,cdk::ds::Unix_socket::Options &unix_opt
            #endif //_WIN32
                )
      : m_sources(sources)
      , m_tcp_opt(tcp_opt)
  #ifndef _WIN32
      , m_unix_opt(unix_opt)
  #endif //_WIN32
    {}

    void operator()(const cdk::ds::mysqlx::TCPIP& to_add)
    {
      m_sources.add(to_add, m_tcp_opt, m_priority);
      m_socket_only = false;
    }

#ifndef _WIN32
    void operator()(const cdk::ds::mysqlx::Unix_socket& to_add)
    {
      m_sources.add(to_add,
                    m_unix_opt, m_priority);
    }
#endif //_WIN32

    void set_priority(unsigned short priority)
    {
      m_priority = priority;
    }

  };

#ifndef _WIN32
  cdk::ds::Unix_socket::Options socket_opts( m_tcp_opts.user(),
                                           m_tcp_opts.password());

  if (m_tcp_opts.database())
    socket_opts.set_database(*m_tcp_opts.database());
#endif //_WIN32

  Sources_add sources(self->m_ms
                      , self->m_tcp_opts
                    #ifndef _WIN32
                      , socket_opts
                    #endif //_WIN32
                      );

  for (Host_list::iterator it = self->m_host_list.begin();
        it != self->m_host_list.end(); ++it)
  {

    sources.set_priority(it->first);

    it->second.visit(sources);

  }
#ifndef _WIN32
  if (sources.m_socket_only && m_has_ssl)
    throw Mysqlx_exception("TLS connections over Unix domain socket are not supported");
#endif
  return self->m_ms;
}


const std::string mysqlx_session_options_struct::get_host()
{
  if (m_last_tcpip != m_host_list.end())
    return m_last_tcpip->second.get<TCPIP_t>().host();

  return TCPIP_t().host(); // return the default host name
}

#ifndef _WIN32
const std::string mysqlx_session_options_struct::get_socket()
{
  if (m_last_socket == m_host_list.end())
    throw Mysqlx_exception("No socket defined");

  return m_last_socket->second.get<cdk::ds::Unix_socket>().path();
}
#endif

unsigned int mysqlx_session_options_struct::get_priority()
{
  if (m_source_state != source_state::priority || m_last_tcpip == m_host_list.end())
    throw Mysqlx_exception("Priority is not available");

  unsigned short prio = m_last_tcpip->first;
  return prio ? prio - 1 : 0;
}

unsigned int mysqlx_session_options_struct::get_port()
{
  if (m_last_tcpip == m_host_list.end())
    throw Mysqlx_exception(MYSQLX_ERROR_MISSING_CONN_INFO);

  return m_last_tcpip->second.get<TCPIP_t>().port();
}

const std::string mysqlx_session_options_struct::get_user()
{
  return m_tcp_opts.user();
}

const std::string* mysqlx_session_options_struct::get_password()
{
  return m_tcp_opts.password();
}

const cdk::string* mysqlx_session_options_struct::get_db() const
{
  return m_tcp_opts.database();
}

#define PRIORITY_CHECK if (priority > (max_priority+1)) \
                         throw Mysqlx_exception(MYSQLX_ERROR_MAX_PRIORITY);\
                       if ((priority > 0 && m_source_state == source_state::non_priority) || \
                           (priority == 0 && m_source_state == source_state::priority)) \
 throw Mysqlx_exception(MYSQLX_ERROR_MIX_PRIORITY); \
else \
 m_source_state = (priority > 0) ? source_state::priority : source_state::non_priority\


// Implementing URI_Processor interface
void mysqlx_session_options_struct::host(unsigned short priority,
          const std::string &host,
          unsigned short port)
{
  PRIORITY_CHECK;
  if (!port)
    throw Mysqlx_exception("Wrong value for port");

  m_last_tcpip = m_host_list.emplace(priority, TCPIP_t(host, port));
}

// Implementing URI_Processor interface
void mysqlx_session_options_struct::host(unsigned short priority,
          const std::string &host)
{
  PRIORITY_CHECK;

  m_last_tcpip = m_host_list.emplace(priority, TCPIP_t(host));

}

#ifndef _WIN32
void mysqlx_session_options_struct::socket(unsigned short priority,
                                           const std::string &path)
{
  PRIORITY_CHECK;

  m_last_socket =
      m_host_list.emplace(priority, cdk::ds::Unix_socket(path));

}
#endif




void mysqlx_session_options_struct::key_val(const std::string &key)
{
  // So far there is no supported options as "?key"
  throw Error_bad_option(key);
}

void mysqlx_session_options_struct::key_val(const std::string& key, const std::string& val)
{
  std::string lc_key=key;
  lc_key.resize(key.size());
  std::transform(key.begin(), key.end(), lc_key.begin(), ::tolower);

  if (lc_key.find("ssl-", 0) == 0)
  {
    m_has_ssl = true;
#ifdef WITH_SSL
    if (lc_key == "ssl-ca")
    {
      check_option(MYSQLX_OPT_SSL_CA);
      set_ssl_ca(val);
    }
    else if (lc_key  == "ssl-mode")
    {
      check_option(MYSQLX_OPT_SSL_MODE);

      static std::map<std::string, mysqlx_ssl_mode_enum> mode_map =
      {
        { "disabled", SSL_MODE_DISABLED },
        { "required", SSL_MODE_REQUIRED },
        { "verify_ca", SSL_MODE_VERIFY_CA },
        { "verify_identity", SSL_MODE_VERIFY_IDENTITY },
      };

      std::string lc_val;
      lc_val.resize(val.size());
      std::transform(val.begin(), val.end(), lc_val.begin(), ::tolower);

      try {
        set_ssl_mode(mode_map.at(lc_val));
      }
      catch (const std::out_of_range&)
      {
        throw Error_bad_mode(val);
      }
    }
    else
    {
      throw Error_bad_option(key);
    }

    m_tcp_opts.set_tls(m_tls_options);
#else
    set_diagnostic(MYSQLX_ERROR_NO_TLS_SUPPORT, 0);
#endif
  }
}


mysqlx_session_options_struct::~mysqlx_session_options_struct()
{}
