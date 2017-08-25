/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C++ is licensed under the terms of the GPLv2
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

#include <mysql_devapi.h>
#include <mysql/cdk.h>
#include <uri_parser.h>

#include <iostream>
#include <sstream>
#include <list>

#include "impl.h"


const unsigned max_priority = 100;

using namespace ::mysqlx;


template<>
Value& internal::Settings_detail<internal::Settings_traits>::find(SessionOption opt)
{
  for (auto &key_val : m_options)
  {
    if (key_val.first == opt)
      return key_val.second;
  }

  std::stringstream error;
  error << "SessionSettings option " << SessionOptionName(opt) << " not found";
  throw Error(error.str().c_str());
}


template<>
void internal::Settings_detail<internal::Settings_traits>::do_add(SessionOption opt, Value &&v)
{
  // Sanity checks on option values.

  switch (opt)
  {
  case SessionOption::PORT:
    if (v.get<unsigned>() > 65535U)
      throw_error("Port value out of range");
    break;

  case SessionOption::SSL_MODE:
    if (m_option_used.test(size_t(SessionOption::SSL_CA)))
    {
      SSLMode m = SSLMode(v.get<unsigned>());
      switch (m)
      {
      case SSLMode::VERIFY_CA:
      case SSLMode::VERIFY_IDENTITY:
        break;

      default:
      {
        std::string msg = "SSL mode ";
        msg += SSLModeName(m);
        msg += " is not compatible with ssl-ca setting";
        throw_error(msg.c_str());
      }
      }
    }
    break;

  case SessionOption::SSL_CA:
    if (m_option_used.test(size_t(SessionOption::SSL_MODE)))
    {
      SSLMode m = SSLMode(find(SessionOption::SSL_MODE).get<unsigned>());
      switch (m)
      {
      case SSLMode::VERIFY_CA:
      case SSLMode::VERIFY_IDENTITY:
        break;

      default:
      {
        std::string msg = "Setting ssl-ca is not compatible with SSL mode ";
        msg += SSLModeName(m);
        throw_error(msg.c_str());
      }
      }
    }
    break;

  default:
    break;
  }

  /*
    Store option value, checking for duplicates etc.

    Note: Only HOST and PRIORITY and host connection options
    can repeat, the others are unique.
  */

  if (opt == SessionOption::HOST
      || opt == SessionOption::PORT
      || opt == SessionOption::PRIORITY
#ifndef _WIN32
      || opt == SessionOption::SOCKET
#endif
      )
  {
    m_options.emplace_back(opt, std::move(v));
  }
  else
  {
    auto it = m_options.begin();
    for (; it != m_options.end(); ++it)
    {
      if (it->first == opt)
      {
        it->second = std::move(v);
        break;
      }
    }

    if (it == m_options.end())
    {
      m_options.emplace_back(opt, std::move(v));
    }
  }
  m_option_used.set(size_t(opt));
}


#ifdef WITH_SSL

SSLMode  get_ssl_mode(const std::string &name)
{
  #define map_ssl(x) { #x, SSLMode::x },
  static std::map<std::string, SSLMode>
    ssl_modes{ SSL_MODE_TYPES(map_ssl) };

  try {
    return ssl_modes.at(name);
  }
  catch (const std::out_of_range&)
  {
    std::string msg = "Invalid ssl-mode value: " + std::string(name);
    throw_error(msg.c_str());
    // Quiet compiler warnings
    return SSLMode::DISABLED;
  }
}


void set_ssl_mode(cdk::connection::TLS::Options &tls_opt,
                  SSLMode mode)
{
  switch (mode)
  {
  case SSLMode::DISABLED:
    tls_opt.set_ssl_mode(
          cdk::connection::TLS::Options::SSL_MODE::DISABLED
          );
    break;
  case SSLMode::REQUIRED:
    tls_opt.set_ssl_mode(
          cdk::connection::TLS::Options::SSL_MODE::REQUIRED
          );
    break;
  case SSLMode::VERIFY_CA:
    tls_opt.set_ssl_mode(
          cdk::connection::TLS::Options::SSL_MODE::VERIFY_CA
          );
    break;
  case SSLMode::VERIFY_IDENTITY:
    tls_opt.set_ssl_mode(
          cdk::connection::TLS::Options::SSL_MODE::VERIFY_IDENTITY
          );
    break;
  }
}

#endif //WITH_SSL


cdk::ds::mysqlx::Protocol_options::auth_method_t get_auth_method(const std::string &name)
{
#define map_auth(x) { #x, cdk::ds::mysqlx::Protocol_options::x },
  static std::map<std::string, cdk::ds::mysqlx::Protocol_options::auth_method_t>
    auth_methods{ AUTH_METHODS(map_auth) };

  try {
    return auth_methods.at(name);
  }
  catch (const std::out_of_range&)
  {
    std::string msg = "Invalid auth value: " + std::string(name);
    throw_error(msg.c_str());
    // Quiet compiler warnings
    return cdk::ds::mysqlx::Protocol_options::MYSQL41;
  }
}


struct Host_sources : public cdk::ds::Multi_source
{

  inline void add(const cdk::ds::TCPIP &ds,
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

#ifndef _WIN32
  inline void add(const cdk::ds::Unix_socket &ds,
                  cdk::ds::Unix_socket::Options options,
                  unsigned short prio)
  {

    cdk::ds::Multi_source::add(ds, options, prio);
  }
#endif

};


using TLS_Options = cdk::connection::TLS::Options;


struct URI_parser
  : public cdk::ds::TCPIP::Options
  , public parser::URI_processor
{
  Host_sources m_source;

  typedef
  cdk::foundation::variant<cdk::ds::TCPIP
#ifndef _WIN32
  , cdk::ds::Unix_socket
#endif //_WIN32
  >  Ds_variant;

  std::multimap<unsigned short, Ds_variant> m_sources;
  std::bitset<size_t(SessionOption::LAST)>  m_options_used;
  bool m_has_ssl = false;

#ifdef WITH_SSL
  TLS_Options m_tls_opt;
#endif

  URI_parser(const std::string &uri)
  {
    parser::parse_conn_str(uri, *this);
#ifdef WITH_SSL
    set_tls(m_tls_opt);
#endif
  }

  struct Add_list
  {
    Host_sources &m_list;
    cdk::ds::TCPIP::Options m_tcp_opt;
#ifndef _WIN32
    cdk::ds::Unix_socket::Options m_socket_opt;
#endif
    unsigned short priority = 0;
    bool socket_only = true;

    Add_list(Host_sources &list
             ,const cdk::ds::TCPIP::Options& tcp_opt
         #ifndef _WIN32
             ,const cdk::ds::Unix_socket::Options& socket_opt
         #endif
             )
      : m_list(list)
      , m_tcp_opt(tcp_opt)
  #ifndef _WIN32
      , m_socket_opt(socket_opt)
  #endif
    {}

    void operator() (const cdk::ds::TCPIP &ds_tcp)
    {
      m_list.add(ds_tcp, m_tcp_opt, priority);
      socket_only = false;
    }

  #ifndef _WIN32
    void operator() (const cdk::ds::Unix_socket &ds_socket)
    {
      m_list.add(ds_socket, m_socket_opt, priority);
    }
  #endif //_WIN32

  };

  cdk::ds::Multi_source& get_data_source()
  {
    m_source.clear();
    Add_list add_list(m_source
                      ,*this
#ifndef _WIN32
                      ,*this
#endif
                      );
    for (auto el = m_sources.begin();
         el != m_sources.end();
         ++el)
    {
      add_list.priority = el->first;
      el->second.visit(add_list);
    }
#ifndef _WIN32
      if (add_list.socket_only && m_has_ssl)
        throw Error("TLS connections over Unix domain socket are not supported");
#endif
    return m_source;
  }

  void user(const std::string &usr) override
  {
    m_usr = usr;
  }

  void password(const std::string &pwd) override
  {
    m_pwd = pwd;
    m_has_pwd = true;
  }

  void host(unsigned short priority,
            const std::string &host,
            unsigned short port) override
  {
    if (priority > (max_priority+1))
      throw_error("Priority should be a value between 0 and 100");

    cdk::ds::TCPIP endpoint(host, port);
    m_sources.emplace(priority, endpoint);
  }

  void host(unsigned short priority,
            const std::string &host) override
  {
    if (priority > (max_priority+1))
      throw_error("Priority should be a value between 0 and 100");

    m_sources.emplace(priority, cdk::ds::TCPIP(host));
  }

#ifndef _WIN32
  // Report Unix socket path.
  virtual void socket(unsigned short priority,
                      const std::string &socket_path) override
  {
    if (priority > (max_priority+1))
      throw_error("Priority should be a value between 0 and 100");

    m_sources.emplace(priority, cdk::ds::Unix_socket(socket_path));

  }
#endif //_WIN32

  virtual void schema(const std::string &db) override
  {
    set_database(db);
  }

  void key_val(const std::string &key, const std::string &val) override
  {
    std::string lc_key = key;
    lc_key.resize(key.size());
    std::transform(key.begin(), key.end(), lc_key.begin(), ::tolower);

    if (lc_key == "ssl-mode")
    {
      m_has_ssl = true;
#ifdef WITH_SSL
      if (m_options_used.test(size_t(SessionOption::SSL_MODE)))
      {
        throw Error("Option ssl-mode defined twice");
      }

      m_options_used.set(size_t(SessionOption::SSL_MODE));

      std::string mode;
      mode.resize(val.size());
      std::transform(val.begin(), val.end(), mode.begin(), ::toupper);

      set_ssl_mode(m_tls_opt, get_ssl_mode(mode));

#else
      throw_error(
            "Can not create TLS session - this connector is built"
            " without TLS support."
            );
#endif
    } else if (lc_key == "ssl-ca")
    {
      m_has_ssl = true;
#ifdef WITH_SSL

      if (m_options_used.test(size_t(SessionOption::SSL_CA)))
      {
        throw Error("Option ssl-ca defined twice");
      }

      m_options_used.set(size_t(SessionOption::SSL_CA));

      m_tls_opt.set_ca(val);
#else
      throw_error(
          "Can not create TLS session - this connector is built"
          " without TLS support."
          );
#endif
    } else if (lc_key == "auth")
    {
      std::string auth;
      auth.resize(val.size());
      std::transform(val.begin(), val.end(), auth.begin(), ::toupper);

      set_auth_method(get_auth_method(auth));
    } else
    {
      std::stringstream err;
      err << "Unexpected key " << key << "=" << val << " on URI";
      throw_error(err.str().c_str());
    }
  }

};


/*
  Session implementation
  ======================
*/


Session::Session(SessionSettings settings)
{
  try {

    /*
      If URI option is specified, get settings from the connetion string.
      TODO: Allow further modifying of URI settings by individual options.
    */

    if (settings.has_option(SessionOption::URI))
    {
      URI_parser parser(
            settings.find(SessionOption::URI).get<string>()
          );

      m_impl = std::make_shared<Impl>(parser.get_data_source());
      return;
    }

    /*
      Parse common options that apply to all hosts in a multi-host setting.
    */

    std::string pwd_str;
    bool has_pwd = false;
    bool has_ssl = false;

#ifdef WITH_SSL
    TLS_Options opt_ssl = TLS_Options::SSL_MODE::REQUIRED;
#endif // WITH_SSL

    if (settings.has_option(SessionOption::PWD) &&
        settings.find(SessionOption::PWD).isNull() == false)
    {
      has_pwd = true;
      pwd_str = settings.find(SessionOption::PWD).get<string>();
    }

    string user;
    string database;
    bool   has_db = false;

    if (settings.has_option(SessionOption::USER))
    {
      user = settings.find(SessionOption::USER);
    }
    else
    {
      throw Error("User not defined!");
    }

    if (settings.has_option(SessionOption::DB))
    {
      has_db = true;
      database = settings.find(SessionOption::DB).get<string>();
    }

    AuthMethod auth_method = AuthMethod::MYSQL41;  // avoid compiler warning
    bool       has_auth = settings.has_option(SessionOption::AUTH);

    if (has_auth)
    {
      auth_method =
        AuthMethod(settings.find(SessionOption::AUTH).get<unsigned>());
    }


    /*
      Set common cdk session options based what was found above.
    */

    auto set_common_options
      = [&has_db, &database,&has_auth,&auth_method]
        (cdk::ds::mysqlx::Options &opt, bool secure)
    {
      if (has_db)
        opt.set_database(database);

      if (has_auth)
      {
        switch(auth_method)
        {
        case AuthMethod::PLAIN:
          opt.set_auth_method(cdk::ds::mysqlx::Options::PLAIN);
        break;
        case AuthMethod::MYSQL41:
          opt.set_auth_method(cdk::ds::mysqlx::Options::MYSQL41);
        break;
        case AuthMethod::EXTERNAL:
          opt.set_auth_method(cdk::ds::mysqlx::Options::EXTERNAL);
        break;
        }
      }
      else
      {
        opt.set_auth_method(secure ? cdk::ds::mysqlx::Options::PLAIN
                                   : cdk::ds::mysqlx::Options::MYSQL41);
      }

    };


    if (settings.has_option(SessionOption::SSL_MODE) ||
        settings.has_option(SessionOption::SSL_CA))
    {
      has_ssl = true;
#ifdef WITH_SSL

      if (settings.has_option(SessionOption::SSL_CA))
      {
        opt_ssl.set_ca(settings.find(SessionOption::SSL_CA).get<string>());
        if (!settings.has_option(SessionOption::SSL_MODE))
          set_ssl_mode(opt_ssl, SSLMode::VERIFY_CA);
      }

      if (settings.has_option(SessionOption::SSL_MODE))
      {
        SSLMode m
          = SSLMode(settings.find(SessionOption::SSL_MODE).get<unsigned>());

        switch (m)
        {
        case SSLMode::VERIFY_CA:
        case SSLMode::VERIFY_IDENTITY:

          if (!settings.has_option(SessionOption::SSL_CA))
          {
            std::string msg = "SSL mode ";
            msg += internal::Settings_traits::get_mode_name(m);
            msg += " requires ssl-ca setting";
            throw_error(msg.c_str());
          }

        default:
          break;
        }

        set_ssl_mode(opt_ssl, m);
      }
#else
      throw_error(
            "Can not create TLS session - this connector is built"
            " without TLS support."
            );
#endif
    }

    /*
      Go through the list of options and build data sources list.
    */

    Host_sources source;

    std::string host = "localhost";
    std::string socket;
    unsigned port = DEFAULT_MYSQLX_PORT;
    unsigned priority = 0;

    auto it = settings.begin();

    bool initialized = false;
    bool add = false;

    //HOST can be skipped only on single host scenarios
    bool singlehost = false;
    bool socket_only = true;

    /*
      Check for PORT and PRIORITY settings and if present,
      store values in port and priority variables.
    */

    auto port_prio
      = [&it, &settings, &port, &priority](bool with_port)
    {
      if (with_port &&
          it != settings.end() &&
          it->first ==SessionOption::PORT)
      {
        port = it->second;
        if (port > 65535U)
          throw_error("Port value out of range");
        ++it;
      }

      if (it != settings.end() && it->first == SessionOption::PRIORITY)
      {
        priority = static_cast<unsigned>(it->second);

        if (priority > max_priority)
          throw_error("Priority should be a value between 0 and 100");

        ++priority;

        if (priority > 65535U)
          throw_error("Priority value out of range");
      }
      else
      {
        --it;
      }
    };

    do
    {
#ifndef _WIN32
      if (it != settings.end() && it->first ==SessionOption::SOCKET)
      {
        if (singlehost)
          throw Error("On multiple hosts, HOST option can't be skipped.");

        add = true;
        priority = 0;

        socket = it->second.get<string>();

        ++it;

        port_prio(false);

      }else
#endif //_WIN32

      if (it != settings.end() && it->first ==SessionOption::HOST)
      {
        if (singlehost)
          throw Error("On multiple hosts, HOST option can't be skipped.");

        add = true;
        port = DEFAULT_MYSQLX_PORT;
        priority = 0;

        host = it->second.get<string>();

        if (host.empty())
          throw Error("Empty host.");

        ++it;

        port_prio(true);

      }
      else if (it->first ==SessionOption::PORT)
      {
        if (!initialized)
        {
          add = true;
          singlehost = true;
          port_prio(true);
        }
        else
        {
          throw Error("Incorrect settings order! Passed Port without previously passing HOST");
        }

      }
      else if (it->first ==SessionOption::PRIORITY)
      {
        throw Error("Incorrect settings order! Passed Priority without previously passing HOST");
      }

      ++it;

      if (add || (it == settings.end() && !initialized))
      {
        if (socket.empty())
        {
          socket_only = false;

          cdk::ds::TCPIP::Options opt(user, has_pwd ? &pwd_str : NULL );

          bool is_secure = false;
#ifdef WITH_SSL
          opt.set_tls(opt_ssl);
          is_secure = cdk::ds::TCPIP::Options::TLS_options::SSL_MODE::DISABLED
                      != opt_ssl.ssl_mode();
#endif

          set_common_options(opt, is_secure);

          source.add(cdk::ds::TCPIP( host, static_cast<unsigned short>(port)),
                      opt,
                      static_cast<unsigned short>(priority));
        }
#ifndef _WIN32
        else
        {
          cdk::ds::Unix_socket::Options opt(user, has_pwd ? &pwd_str : NULL );

          set_common_options(opt, true);

          source.add(cdk::ds::Unix_socket(socket),
                      opt,
                      static_cast<unsigned short>(priority));

          socket.clear();
        }
#endif //_WIN32

        initialized = true;

        if (it == settings.end())
          break;

        add = false;
      }

    } while (it != settings.end());

    if (socket_only && has_ssl)
      throw Error("TLS connections over Unix domain socket are not supported");

    m_impl = std::make_shared<Impl>(source);

  }
  CATCH_AND_WRAP
}

Session::Session(Session* master)
{
  assert(master);
  m_impl = master->m_impl;
  master->add_child(this);
}


void internal::Session_detail::Impl::register_result(Result_impl *result)
{
  if (m_current_result && m_current_result != result)
    m_current_result->deregister();

  m_current_result = result;
}

void internal::Session_detail::Impl::deregister_result(Result_impl *result)
{
  if (m_current_result == result)
    m_current_result = NULL;
}


cdk::Session& internal::Session_detail::get_cdk_session()
{
  if (!m_impl)
    throw Error("Session closed");

  return m_impl->m_sess;
}


void internal::Session_detail::prepare_for_cmd()
{
  assert(m_impl);
  m_impl->register_result(nullptr);
}


void internal::Session_detail::close()
{
  if (m_parent_session)
  {
    m_parent_session->remove_child(this);
  }
  else
  {
    // This is master session, notify child session that it is being
    // closed.

    for (auto sess : m_child_sessions)
    {
      sess->parent_close_notify();
    }

    get_cdk_session().rollback();
  }

  m_impl.reset();
}



// ---------------------------------------------------------------------
/*
  Transactions.
*/

void Session::startTransaction()
{
  try {
    get_cdk_session().begin();
  }
  CATCH_AND_WRAP
}


void Session::commit()
{
  try {
    get_cdk_session().commit();
  }
  CATCH_AND_WRAP
}


void Session::rollback()
{
  try {
    get_cdk_session().rollback();
  }
  CATCH_AND_WRAP
}



// ---------------------------------------------------------------------


/*
  Class which acts as query parameter source, passing an arbitrary list of
  string parameters
*/

class Args
    : public cdk::Any_list
{

  typedef cdk::Any_list Any_list;

  std::vector<string> m_args;


  void add_arg(const string& arg)
  {
    m_args.push_back(arg);
  }

  template <typename... A>
  void add_arg(const string& arg, const A&... rest)
  {
    add_arg(arg);
    add_arg(rest...);
  }

public:

  Args() = default;

  template <typename... A>
  Args(const string& arg, A... args)
  {
    add_arg(arg, args...);
  }

  void process(Any_list::Processor &lp) const
  {
    cdk::Safe_prc<Any_list::Processor> sp(lp);
    sp->list_begin();
    // NOTE: uses utf8
    for (auto arg : m_args)
    {
      sp->list_el()->scalar()->str(arg);
    }
    sp->list_end();
  }
};


// ---------------------------------------------------------------------


/*
  Code to fetch list of SCHEMA, TABLE and COLLECTION.

  Checks are done by querying INFORMATION_SCHEMA database, or using SQL or admin
  commands. Sending appropriate query is implemented by list_query<T> class
  which derives from cdk::Reply (T is the type of object to check).
  Each list_query object implements a static method add_data() which receives
  data from cdk::Reply processor
*/

enum obj_type { TABLE, SCHEMA, COLLECTION };

template <obj_type> struct List_query;

template <typename E>
struct List_query_base
    : public cdk::Row_processor
{
  /*
    It will be called for each row with the defined data for storing it on the
    next item to be added to the list.
    If class implementing method returns false, the rest of row data is skipped
    and the element not added to the list.
  */
  virtual bool field_data(size_t col, cdk::string&&) = 0;


  template <typename I>
  List_query_base(I& init)
    : m_reply(init)
  {
    m_reply.wait();
    m_cursor.reset(new cdk::Cursor(m_reply));

    m_list_it = m_list.before_begin();
  }


  cdk::Reply m_reply;
  cdk::scoped_ptr<cdk::Cursor> m_cursor;

  bool m_skip_line = false;
  E m_elem;
  std::forward_list<E> m_list;
  typename std::forward_list<E>::iterator m_list_it;


  /*
    Method to be called to retrieve list of the queried elements
  */

  std::forward_list<E> execute()
  {
    m_cursor->get_rows(*this);
    m_cursor->wait();
    return std::move(m_list);
  }


  /*
    cdk::Row_processor implementation
  */

  bool row_begin(row_count_t)
  {
    m_skip_line = false;
    return true;
  }

  void row_end(row_count_t)
  {
    if (!m_skip_line)
      m_list_it = m_list.emplace_after(m_list_it, m_elem);
  }

  size_t field_begin(col_count_t, size_t s) { return s; }

  void field_end(col_count_t) {}

  void field_null(col_count_t) {}

  size_t field_data(col_count_t col, bytes b)
  {
    // TODO: the data should be saved here and only processed on field_end()
    cdk::Codec<cdk::TYPE_STRING> codec(m_cursor->format(col));

    cdk::string data;
    codec.from_bytes(b, data);

    if (!m_skip_line)
      m_skip_line = !field_data(col, std::move(data));

    return 1024;
  }
  void end_of_data() {}

};


template<>
struct List_query<obj_type::SCHEMA>
    : Args
    , List_query_base<mysqlx::string>
{

  List_query(cdk::Session &sess)
    : List_query_base<mysqlx::string>(sess.sql(L"SHOW SCHEMAS"))
  {
  }

  List_query(cdk::Session &sess, const string& schema)
    : Args(schema)
    , List_query_base<mysqlx::string>(
        sess.sql(L"SHOW SCHEMAS LIKE ?", this))
  {
  }

  /*
    if returns false, skip current row
  */

  bool field_data(size_t col, cdk::string&& data) override
  {
    if (0 == col)
      m_elem = std::move(data);
    return true;
  }
};


/*
  COLLECTION and TABLE list_query will use list_objects admin command.

  list_objects return a list of TABLE, VIEW or COLLECTION.

  As arguments, we can pass the schema and optionally can pass a filter to the
  objects names.

  The retrieved data contains 2 columns
  - Object name
  - Type (TABLE/VIEW/COLLECTION)
*/

template<>
struct List_query<obj_type::COLLECTION>
    : Args
    , List_query_base<mysqlx::string>
{


  List_query(cdk::Session &sess, const string& schema)
    : Args(schema)
    , List_query_base<mysqlx::string>(sess.admin("list_objects", *this))
  {
  }

  List_query(cdk::Session &sess, const string& schema, const string& collection)
    : Args(schema, collection)
    , List_query_base<mysqlx::string>(sess.admin("list_objects", *this))
  {
  }

  /*
    if returns false, skip current row
  */
  bool field_data(size_t col, cdk::string&& data) override
  {
    switch (col)
    {
      case 0: m_elem = std::move(data); break;
      case 1: return data.compare(L"COLLECTION") == 0;
    }
    return true;
  }

};


template<>
struct List_query<obj_type::TABLE>
    : Args
    , List_query_base<std::pair<mysqlx::string,bool>>
{

  List_query(cdk::Session &sess, const string& schema, const string& table = string())
    : Args(schema, table)
    , List_query_base<std::pair<mysqlx::string,bool>>(sess.admin("list_objects", *this))
  {
  }

  // if returns false, skip current row
  bool field_data(size_t col, cdk::string&& data) override
  {
    switch (col)
    {
      // col 0 = name
      case 0:
        m_elem.first = std::move(data);
        break;
      // col 1 = type (view/table)
      case 1:
        m_elem.second = (data == (L"VIEW"));
        return (data.compare(L"TABLE") == 0) ||
               (data.compare(L"VIEW") == 0);
        break;
    }
    return true;
  }

};


/*
  Helper class to execute SQL queries which count Collection/Table rows.
  It assumes that SQL query returns a result set consisting of s single row with
  one column containing integer number.

  This number is returned by execute() method.
*/

struct Obj_row_count
    : Args

{

  Obj_row_count(cdk::Session &sess, const string& qry_count)
    : m_reply(sess.sql(qry_count))
  {
    m_reply.wait();
  }

  cdk::Reply m_reply;

  struct
      : public std::string
      , public cdk::Row_processor
  {

    bool row_begin(row_count_t) { return true; }

    void row_end(row_count_t) {}

    size_t field_begin(col_count_t, size_t s)
    {
      clear();
      reserve(s);
      return s;
    }

    void field_end(col_count_t) {}

    void field_null(col_count_t) {}

    size_t field_data(col_count_t, bytes b)
    {
      insert(end(), b.begin(), b.end());

      return 1024;
    }

    void end_of_data()
    {}

  } m_buffer;

  /*
    Method to be called to retrieve number of rows.
  */

  uint64_t execute()
  {
    cdk::Cursor cursor(m_reply);
    cursor.get_rows(m_buffer);
    cursor.wait();

    // Only expecting 1 column
    cdk::Codec<cdk::TYPE_INTEGER> codec(cursor.format(0));

    uint64_t count;

    codec.from_bytes(cdk::bytes(m_buffer), count);

    return count;
  }

};




// ---------------------------------------------------------------------


Schema Session::createSchema(const string &name, bool reuse)
{
  try {
    std::stringstream query;
    query << "Create Schema `" << name << "`";
    cdk::Reply r(get_cdk_session().sql(query.str()));

    r.wait();

    if (0 < r.entry_count())
    {
      const cdk::Error &err= r.get_error();
      // 1007 = schema already exists
      if (!reuse || cdk::server_error(1007) != err.code())
        err.rethrow();
    }
    return Schema(*this, name);
  }
  CATCH_AND_WRAP
}


/*
  Helper function to wait on reply and throw errors
*/

void check_reply_skip_error_throw(cdk::Reply&& r, int skip_server_error)
{
  r.wait();

  if (0 < r.entry_count())
  {
    const cdk::Error &err= r.get_error();
    if (err.code() != cdk::server_error(skip_server_error))
      err.rethrow();
  }
}


void Session::dropSchema(const string &name)
{
  try{
    prepare_for_cmd();
    std::stringstream qry;
    qry << "Drop Schema `" << name << "`";
    //skip server error 1008 = schema doesn't exist
    check_reply_skip_error_throw(get_cdk_session().sql(qry.str()), 1008);
  }
  CATCH_AND_WRAP
}


void Schema::dropTable(const string& table)
{
  /*
    Note: We simply forward to dropCollection() because xplugin does not
    have a dedicated command for dropping tables and it does not distinguish
    tables from collections.
  */
  dropCollection(table);
}


void Schema::dropCollection(const mysqlx::string& collection)
{
  try{
    m_sess->prepare_for_cmd();
    Args args(m_name, collection);
    // Doesn't throw if collection doesn't exit (server error 1051)
    check_reply_skip_error_throw(
          m_sess->get_cdk_session().admin("drop_collection", args),
          1051
          );
  }
  CATCH_AND_WRAP
}


void Schema::dropView(const mysqlx::string& name)
{
  Table_ref view(getName(),name);

  try {
    m_sess->prepare_for_cmd();
    /*
      Note: false argument to view_drop() means that we do not check for
      the existence of the view being dropped.
      We do not throw error 1347 (object is not a view) treating it the same
      as the case when the view does not exist.
    */
    check_reply_skip_error_throw(
          m_sess->get_cdk_session().view_drop(view, false),
          1347
    );
  }
  CATCH_AND_WRAP
}


Schema Session::getDefaultSchema()
{
  if (m_impl->m_default_db.empty())
    throw Error("No default schema set for the session");
  return Schema(*this, m_impl->m_default_db);
}

string Session::getDefaultSchemaName()
{
  return m_impl->m_default_db;
}


Schema Session::getSchema(const string &name, bool check)
{
  try {

    Schema schema(*this, name);

    if (check)
    {
      if (!schema.existsInDatabase())
        // TODO: Better error (schema name)
        throw Error("No such schema");
    }

    return schema;
  }
  CATCH_AND_WRAP
}


internal::List_init<Schema> Session::getSchemas()
{
  try {

    auto schemas_names = List_query<SCHEMA>(get_cdk_session()).execute();

    std::forward_list<Schema> schemas_list;
    std::forward_list<Schema>::iterator schema_it =  schemas_list.before_begin();

    for (auto el : schemas_names)
    {
      schema_it = schemas_list.emplace_after(schema_it, Schema(*this, el));
    }

    return std::move(schemas_list);
  }
  CATCH_AND_WRAP
}


/*
  Schema
  ======
*/

bool Schema::existsInDatabase() const
{
  try {

    auto schemas_names = List_query<SCHEMA>(m_sess->get_cdk_session(),
                                            m_name).execute();

    return !schemas_names.empty();

  }
  CATCH_AND_WRAP
}


Collection Schema::getCollection(const string &name, bool check)
{
  try {

    Collection coll(*this, name);

    if (check)
    {

      if (!coll.existsInDatabase())
      {
        // TODO: Better error (collection name)
        throw Error("No such collection");
      }
    }
    return coll;
  }
  CATCH_AND_WRAP
}


Collection Schema::createCollection(const string &name, bool reuse)
{
  try {
    Args args(m_name, name);
    cdk::Reply r(m_sess->get_cdk_session().admin("create_collection", args));
    r.wait();
    if (0 < r.entry_count())
    {
      const cdk::Error &err= r.get_error();
      // 1050 = table already exists
      if (!reuse || cdk::server_error(1050) != err.code())
        err.rethrow();
    }
    return Collection(*this, name);
  }
  CATCH_AND_WRAP
}


Table Schema::getTable(const string &name, bool check)
{
  try {
    Table tbl(*this, name);

    if (check)
    {

      if (!tbl.existsInDatabase())
        // TODO: Better error (collection name)
        throw Error("No such table");

    }
    return tbl;
  }
  CATCH_AND_WRAP
}


internal::List_init<Collection> Schema::getCollections()
{
  try{
    std::forward_list<Collection> list;
    std::forward_list<Collection>::iterator list_it = list.before_begin();

    std::forward_list<string> list_name = getCollectionNames();

    for (auto name : list_name)
    {
      list.emplace_after(list_it,Collection(*this, name));
    }

    return std::move(list);
  }
  CATCH_AND_WRAP
}

internal::List_init<string> Schema::getCollectionNames()
{
  try{
    return List_query<COLLECTION>(
          m_sess->get_cdk_session()
          , m_name).execute();
  }
  CATCH_AND_WRAP
}

internal::List_init<Table> Schema::getTables()
{
  try {
    std::forward_list<Table> list;
    std::forward_list<Table>::iterator list_it = list.before_begin();

    auto tables_list = List_query<TABLE>(m_sess->get_cdk_session()
                                         , m_name).execute();

    for (auto& prop : tables_list)
    {
      list_it = list.emplace_after(list_it,
                                   Table(*this, prop.first, prop.second));
    }

    return std::move(list);
  }
  CATCH_AND_WRAP
}

internal::List_init<string> Schema::getTableNames()
{
  try {
    std::forward_list<string> list;
    std::forward_list<string>::iterator list_it = list.before_begin();
    auto tables_list = List_query<TABLE>(m_sess->get_cdk_session()
                                         , m_name).execute();

    for (auto& el : tables_list)
    {
      list.emplace_after(list_it, std::move(el.first));
    }

    return std::move(list);
  }
  CATCH_AND_WRAP
}


Table Schema::getCollectionAsTable(const string& name, bool check_exists)
{
  try {
  //Check if collection exists
  if (check_exists)
    getCollection(name, true);

  return Table(*this, name);
  }
  CATCH_AND_WRAP
}


/*
  Collection
  ==========
*/

bool Collection::existsInDatabase() const
{
  try {

    auto collection_names = List_query<COLLECTION>(m_sess->get_cdk_session(),
                                                   m_schema.getName(),
                                                   m_name).execute();

    return !collection_names.empty();

  }
  CATCH_AND_WRAP
}


uint64_t Collection::count()
{
  try {

    std::stringstream qry;
    qry << "select count(*) from " << m_schema.getName() << "." << m_name;

    return Obj_row_count(m_sess->get_cdk_session(),qry.str()).execute();
  }
  CATCH_AND_WRAP
}


/*
  Table
  =====
*/

bool Table::isView()
{
  try {

    if (UNDEFINED == m_isview)
    {
      m_isview = m_schema.getTable(m_name, true).isView() ? YES : NO;
    }

    return m_isview == YES ? true : false;
  }
  CATCH_AND_WRAP
}

bool Table::existsInDatabase() const
{
  try {

    auto table_names = List_query<TABLE>(m_sess->get_cdk_session(),
                                         m_schema.getName(),
                                         m_name).execute();
    if (!table_names.empty())
    {
      const_cast<Table*>(this)->m_isview = table_names.begin()->second ? YES : NO;
    }

    return !table_names.empty();

  }
  CATCH_AND_WRAP
}


uint64_t Table::count()
{
  try {

    std::stringstream qry;
    qry << "select count(*) from " << m_schema.getName() << "." << m_name;

    return Obj_row_count(m_sess->get_cdk_session(),qry.str()).execute();
  }
  CATCH_AND_WRAP
}


/*
  Executing SQL queries
  =====================
*/


struct Op_sql : public Op_base<internal::Bind_impl>
{
  string m_query;

  typedef std::list<Value> param_list_t;

  Op_sql(Session &sess, const string &query)
    : Op_base(sess), m_query(query)
  {}

  struct
    : public cdk::Any_list
    , cdk::Format_info
  {
    param_list_t m_values;

    void process(Processor &prc) const override
    {
      prc.list_begin();

      for (auto &val : m_values)
      {
        Processor::Element_prc::Scalar_prc
         *sprc = safe_prc(prc)->list_el()->scalar();
        if (!sprc)
          continue;
        switch (val.getType())
        {
        case Value::VNULL:
          sprc->null();
          break;
        case Value::UINT64:
          sprc->num(static_cast<uint64_t>(val));
          break;
        case Value::INT64:
          sprc->num(static_cast<int64_t>(val));
          break;
        case Value::FLOAT:
          sprc->num(static_cast<float>(val));
          break;
        case Value::DOUBLE:
          sprc->num(static_cast<double>(val));
          break;
        case Value::BOOL:
          sprc->yesno(static_cast<bool>(val));
          break;
        case Value::STRING:
          sprc->str(static_cast<mysqlx::string>(val));
          break;
        case Value::RAW:
          sprc->value(cdk::TYPE_BYTES,
            static_cast<const cdk::Format_info&>(*this),
            Value::Access::get_bytes(val));
          break;
        default:
          THROW("Unexpected value type");
        }
      }

      prc.list_end();
    }

    // Trivial Format_info for raw byte values

    bool for_type(cdk::Type_info) const override { return true; }
    void get_info(cdk::Format<cdk::TYPE_BYTES>&) const override {}
    using cdk::Format_info::get_info;
  }
  m_params;

  void add_param(Value val) override
  {
    m_params.m_values.emplace_back(std::move(val));
  }

  void add_param(const mysqlx::string&, Value&&) override
  {
    assert(false);
  }

  Executable_impl* clone() const override
  {
    return new Op_sql(*this);
  }


  cdk::Reply* send_command() override
  {
    return new cdk::Reply(
      get_cdk_session().sql(
        m_query,
        m_params.m_values.empty() ? NULL : &m_params
      )
    );
  }
};


internal::SQL_statement::SQL_statement(mysqlx::Session *sess, const string &query)
{
  try {
    reset(new Op_sql(*sess, query));
  }
  CATCH_AND_WRAP
}


// ---------------------------------------------------------------------


#if 0

string::string(const std::string &other)
  : std::wstring(cdk::string(other))
{}

string::string(const char *other)
{
  if (other)
    *this = std::string(other);
}

string::operator std::string() const
{
  return std::string(cdk::string(*this));
}

#endif


std::string string::Impl::to_utf8(const string &other)
{
  return cdk::string(other);
}

void string::Impl::from_utf8(string &s, const std::string &other)
{
  s = cdk::string(other);
}


/*
string::operator const cdk::foundation::string&() const
{
  return cdk::string(*static_cast<const std::wstring*>(this));
}
*/

ostream& operator<<(ostream &out, const Error&)
{
  out <<"MYSQLX Error!";
  return out;
}
