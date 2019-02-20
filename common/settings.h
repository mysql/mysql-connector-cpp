/*
 * Copyright (c) 2017, 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef MYSQLX_COMMON_SETTINGS_INT_H
#define MYSQLX_COMMON_SETTINGS_INT_H

#include <mysqlx/common.h>
#include <mysql/cdk.h>
#include <uri_parser.h>
#include "value.h"

PUSH_SYS_WARNINGS
#include <vector>
#include <bitset>
#include <sstream>
POP_SYS_WARNINGS

namespace mysqlx {
MYSQLX_ABI_BEGIN(2,0)
namespace common {

/*
  A class for "transactional" adding new settings to a given
  Settings_impl instance.

  The primary way of specifying new options is via CDK document containing
  key-value pairs with options or option names and their values. A Setter
  instance can act as a processor for such CDK document.
*/

class Settings_impl::Setter
  : public cdk::JSON::Processor
  , public parser::URI_processor
  , cdk::JSON::Processor::Any_prc
  , cdk::JSON::Processor::Any_prc::Scalar_prc
{
  Settings_impl &m_settings;
  Settings_impl::Data m_data;


  // SessionOption is > 0
  // ClientOption is < 0
  int m_cur_opt = 0;

  int get_option_from_name(const std::string& key)
  {

    //Client options object
    std::string upper_key = to_upper(key);
    if (!m_inside_client_opts && upper_key == "POOLING")
    {
      m_inside_client_opts = true;
      return 0;
    }
    else if (m_inside_client_opts)
    {
      if (upper_key == "ENABLED")
        return Client_option_impl::POOLING;
      else if (upper_key == "MAXSIZE")
        return Client_option_impl::POOL_MAX_SIZE;
      else if (upper_key == "QUEUETIMEOUT")
        return Client_option_impl::POOL_QUEUE_TIMEOUT;
      else if (upper_key == "MAXIDLETIME")
        return Client_option_impl::POOL_MAX_IDLE_TIME;
    }

    std::string msg = "Invalid client option: " + key;
    throw_error(msg.c_str());
    // Quiet compiler warnings
    return 0;
  }

public:

  Setter(Settings_impl &settings)
    : m_settings(settings)
    , m_data(settings.m_data)
  {}


  void set_client_opts(const Settings_impl &opts)
  {
    Setter set(*this);
    for (auto &opt_val : opts.m_data.m_options)
    {
      set.add_option(opt_val.first, opt_val.second);
    }
    set.commit();
  }

  /*
    This method should be called after setting options to actually update
    settings in the Settings_impl instance. Settings are updated only if
    all consistency checks are passed.
  */

  void commit()
  {
    /*
      If more hosts are added to the settings, error if the first host was
      defined by PORT only, without explicit HOST setting.
    */

    if (
      m_data.m_tcpip && m_settings.m_data.m_tcpip
      && 0 == m_settings.m_data.m_host_cnt
    )
      throw_error("PORT without explicit HOST in multi-host settings");

    /*
      Check if priority is missing in case some priorities were specified
      earlier.
    */

    if (m_data.m_user_priorities && (m_host && !m_prio))
      throw_error("Expected PRIORITY for a host in multi-host settings");

    /*
      If all is OK, copy settings collected here to the parent settings
      object.
    */

    m_settings.m_data = std::move(m_data);
  }

  // JSON doc processor

  void doc_end() override
  {
    if (m_inside_client_opts)
    {
      m_inside_client_opts = false;
    }
    else
      commit();
  }

  Any_prc* key_val(const string &opt) override
  {
    return key_val(get_option_from_name(opt));
  }

  Any_prc* key_val(int opt)
  {
    m_cur_opt = opt;
    return this;
  }

private:


  /*
    Add or replace option value, report error if option defined twice (except
    for options which build the list of hosts).
  */

  template <typename T> void add_option(int, const T&);

  // State used for option consistency checks.

  bool m_host = false;
  bool m_port = false;
  bool m_socket = false;
  bool m_prio = false;
  bool m_inside_client_opts = false;
  std::set<int> m_opt_set;
  int m_prev_option = 0;

  // Set option value doing all consistency checks.

  template <int OPT, typename T>
  void set_option(const T &val)
  {
    if (OPT == Session_option_impl::CONNECT_TIMEOUT)
      throw_error("The connection timeout value must be a positive integer (including 0)");

    add_option(OPT, val);
  }

  template <int OPT>
  void set_option(const int &val)
  {
    if (0 > val)
      throw_error("Option value can not be a negative number");
    set_option<OPT>((unsigned)val);
  }

  template <int OPT, typename T>
  void set_cli_option(const T &val)
  {
    add_option(OPT, val);
  }

  template <int OPT>
  void set_cli_option(const int &val)
  {
    if (0 > val)
      throw_error("Option value can not be a negative number");
    set_option<OPT>((unsigned)val);
  }


  // Any processor

  Scalar_prc* scalar() override
  {
    return this;
  }

  List_prc* arr() override
  {
    throw_error("Option ... does not accept list values");
    return nullptr;
  }

  Doc_prc* doc() override
  {
    if (!m_inside_client_opts || m_cur_opt > 0)
      throw_error("Option ... does not accept document values");
    return this;
  }

  // Scalar processor

  void str(const string &val) override;
  void num(uint64_t val) override;
  void null() override;

  void num(int64_t val) override
  {
    if (0 > val)
      throw_error("Option value can not be a negative number");
    num(uint64_t(val));
  }

  void yesno(bool) override;

  // These value types should not be used

  void num(float) override
  {
    assert(false);
  }

  void num(double) override
  {
    assert(false);
  }

public:

  // URI processor

  void user(const std::string &usr) override;
  void password(const std::string &pwd) override;
  void schema(const std::string &db) override;

  void host(unsigned short priority, const std::string &host) override;

  void host(
    unsigned short priority,
    const std::string &host,
    unsigned short port
  ) override;

  void socket(unsigned short priority, const std::string &path) override;

  void pipe(unsigned short /*priority*/, const std::string &/*pipe*/) override
  {
    // should not happen
    assert(false);
  }

  /*
    Callbacks for reporting the query component, which is a sequence
    of key-value pair. Keys without any value are allowed. Key value
    can be a list: "...&key=[v1,..,vN]&...".
  */

  void key_val(const std::string &key, const std::string &val) override;
  void key_val(const std::string &key) override;
  void key_val(const std::string &key, const std::list<std::string>&) override;

  static int get_uri_option(const std::string&);
};


/*
  Logic for handling individual options.
*/


// Options which build a list of hosts.



template<>
inline void
Settings_impl::Setter::set_option<Settings_impl::Session_option_impl::HOST>(
  const std::string &val
)
{
  if (0 == m_data.m_host_cnt && m_port)
    throw_error("PORT without prior host specification in multi-host settings");

  /*
    In the case of explicit priorities, if a previous host was added, check that
    a priority was specified for the previous host.
  */

  if (m_data.m_user_priorities && m_host && !m_prio)
    throw_error("PRIORITY not set for all hosts in a multi-host settings");

  m_host = true;
  m_port = false;
  m_socket = false;
  m_prio = false;
  ++m_data.m_host_cnt;
  m_data.m_tcpip = true;
  add_option(Session_option_impl::HOST, val);
}



template<>
inline void
Settings_impl::Setter::set_option<Settings_impl::Session_option_impl::SOCKET>(
#ifdef _WIN32
  const std::string&
#else
  const std::string &val
#endif
)
{
#if _WIN32

  throw_error("SOCKET option not supported on Windows");

#else

  /*
    In the case of explicit priorities, if a previous host was added, check that
    a priority was specified for the previous host.
  */

  if (m_data.m_user_priorities && m_host && !m_prio)
    throw_error("PRIORITY not set for all hosts in a multi-host settings");

  m_host = true;
  m_socket = true;
  m_prio = false;
  m_port = false;
  ++m_data.m_host_cnt;
  m_data.m_sock = true;
  add_option(Session_option_impl::SOCKET, val);

#endif
}


template<>
inline void
Settings_impl::Setter::set_option<Settings_impl::Session_option_impl::PORT>(
  const unsigned &val
)
{
  if (m_port)
    throw_error("duplicate PORT value");  // TODO: overwrite instead?

  if (0 < m_data.m_host_cnt && (Session_option_impl::HOST != m_prev_option))
    throw_error("PORT must follow HOST setting in multi-host settings");

  if (m_socket)
    throw_error("Invalid PORT setting for socked-based connection");

  if (m_prio)
    throw_error("PORT should be specified before PRIORITY");

  if (val > 65535U)
    throw_error("Port value out of range");

  m_port = true;
  m_data.m_tcpip = true;
  add_option(Session_option_impl::PORT, val);
}


template<>
inline void
Settings_impl::Setter::set_option<Settings_impl::Session_option_impl::PRIORITY>(
  const unsigned &val
)
{
  switch (m_prev_option)
  {
  case Session_option_impl::HOST:
  case Session_option_impl::PORT:
  case Session_option_impl::SOCKET:
    break;
  default:
    throw_error("PRIORITY must directly follow host specification");
  };

  if (m_prio)
    throw_error("duplicate PRIORITY value");  // TODO: overwrite instead?

  /*
    Using PRIORITY implies multi-host settings and then each host must be
    defined explicitly.
  */

  if (!m_host)
    throw_error("PRIORITY without prior host specification");

  if (1 < m_data.m_host_cnt && !m_data.m_user_priorities)
    throw_error("PRIORITY not set for all hosts in a multi-host settings");

  if (val > 100)
    throw_error("PRIORITY should be a number between 0 and 100");

  m_data.m_user_priorities = true;
  m_prio = true;
  add_option(Session_option_impl::PRIORITY, val);
}


// SSL options.


template<>
inline void
Settings_impl::Setter::set_option<Settings_impl::Session_option_impl::SSL_MODE>(
  const unsigned &val
)
{
  if (val >= size_t(SSL_mode::LAST))
    throw_error("Invalid SSL_MODE value");
  m_data.m_ssl_mode = SSL_mode(val);

#ifndef WITH_SSL
  if (SSL_mode::DISABLED != m_ssl_mode)
    throw_error("secure connection requested but SSL is not supported")
#endif

  switch (m_data.m_ssl_mode)
  {
  case SSL_mode::VERIFY_CA:
  case SSL_mode::VERIFY_IDENTITY:
    break;

  default:
    if (m_data.m_ssl_ca)
      throw_error("SSL_MODE ... not valid when SSL_CA is set");
  }

  add_option(Session_option_impl::SSL_MODE, val);
}


template<>
inline void
Settings_impl::Setter::set_option<Settings_impl::Session_option_impl::SSL_CA>(
  const std::string &val
)
{
#ifndef WITH_SSL
  throw_error("SSL_CA option specified but SSL is not supported")
#endif

  switch (m_data.m_ssl_mode)
  {
  case SSL_mode::VERIFY_CA:
  case SSL_mode::VERIFY_IDENTITY:
  case SSL_mode::LAST:
    break;

  default:
    throw_error("SSL_CA option is not compatible with SSL_MODE ...");
  }

  m_data.m_ssl_ca = true;
  add_option(Session_option_impl::SSL_CA, val);
}

template <>
inline void
Settings_impl::Setter::set_option<Settings_impl::Session_option_impl::CONNECT_TIMEOUT>(
  const uint64_t &timeout
)
{
  add_option(Settings_impl::Session_option_impl::CONNECT_TIMEOUT, timeout);
}

template<>
inline void
Settings_impl::Setter::set_option<Settings_impl::Session_option_impl::SSL_MODE>(
  const std::string &val
)
{
  using std::map;

#define SSL_MODE_MAP(X,N) { #X, SSL_mode::X },

  static map< std::string, SSL_mode > option_map{
    SSL_MODE_LIST(SSL_MODE_MAP)
  };

  try {

    SSL_mode opt = option_map.at(to_upper(val));

    if (SSL_mode::LAST == opt)
      throw std::out_of_range("");

    set_option<Session_option_impl::SSL_MODE>(unsigned(opt));
    return;
  }
  catch (const std::out_of_range&)
  {
    std::string msg = "Invalid ssl mode value: " + val;
    throw_error(msg.c_str());
    // Quiet compiler warnings
    return;
  }
}


// Authentication options.

template<>
inline void
Settings_impl::Setter::set_option<Settings_impl::Session_option_impl::AUTH>(
  const unsigned &val
)
{
  if (val >= size_t(Auth_method::LAST))
    throw_error("Invalid auth method");
  add_option(Session_option_impl::AUTH, val);
}


template<>
inline void
Settings_impl::Setter::set_option<Settings_impl::Session_option_impl::AUTH>(
  const std::string &val
)
{
  using std::map;

#define AUTH_MAP(X,N) { #X, Auth_method::X },

  static map< std::string, Auth_method > auth_map{
    AUTH_METHOD_LIST(AUTH_MAP)
  };

  try {

    Auth_method m = auth_map.at(to_upper(val));

    if (Auth_method::LAST == m)
      throw std::out_of_range("");

    set_option<Session_option_impl::AUTH>(unsigned(m));
    return;
  }
  catch (const std::out_of_range&)
  {
    std::string msg = "Invalid auth method: " + val;
    throw_error(msg.c_str());
    // Quiet compiler warnings
    return;
  }
}


// Other options that need special handling.
// TODO: support std::string for PWD and other options that are ascii only?

template<>
inline void
Settings_impl::Setter::set_option<Settings_impl::Session_option_impl::URI>(
  const std::string &val
)
{
  parser::URI_parser  parser(val);
  parser.process(*this);
}


template<>
inline void
Settings_impl::Setter::set_cli_option<
  Settings_impl::Client_option_impl::POOL_MAX_SIZE
>(const uint64_t &val)
{
  if (val == 0)
    throw_error("Max pool size has to be greater than 0");
  add_option(Settings_impl::Client_option_impl::POOL_MAX_SIZE, val);
}


// Generic add_option() method.


template <typename T>
inline
void Settings_impl::Setter::add_option(int opt, const T &val)
{
  auto &options = m_data.m_options;
  m_prev_option = opt;

  switch (opt)
  {
  case Session_option_impl::HOST:
  case Session_option_impl::SOCKET:
  case Session_option_impl::PORT:
  case Session_option_impl::PRIORITY:
    options.emplace_back(opt, val);
    return;

  default:
    // Check for doubled option
    if (0 < m_opt_set.count(opt))
    {
      std::string msg = "Option ";
      msg += option_name(opt);
      msg += " defined twice";
      throw_error(msg.c_str());
      return;
    }
    m_opt_set.insert(opt);
  }

  auto it = options.begin();
  for (; it != options.end(); ++it)
  {
    if (it->first == opt)
    {
      it->second = val;
      break;
    }
  }

  if (it == options.end())
  {
    options.emplace_back(opt, val);
  }
}


// Value processor

inline
void Settings_impl::Setter::str(const string &val)
{
  // TODO: avoid utf8 conversions
  std::string utf8_val(val);

  auto to_number = [&]() -> uint64_t
  {
    std::size_t pos;
    long long x = std::stoll(utf8_val, &pos);

    if (x < 0)
      throw_error("Option ... accepts only non-negative values");

    if (pos != val.length())
      throw_error("Option ... accepts only integer values");

    return x;
  };

#define SET_OPTION_STR_str(X,N) \
  case Session_option_impl::X: return set_option<Session_option_impl::X,std::string>(utf8_val);
#define SET_OPTION_STR_any(X,N) SET_OPTION_STR_str(X,N)
#define SET_OPTION_STR_num(X,N) \
  case Session_option_impl::X: \
  try \
  { \
    return set_option<Session_option_impl::X,uint64_t>(to_number()); \
  } \
  catch (const std::invalid_argument&) \
  { \
    throw_error("Can not convert to integer value"); \
  }

  switch (m_cur_opt)
  {
    SESSION_OPTION_LIST(SET_OPTION_STR)

  default:
    throw_error("Option ... could not be processed.");
  }

}


inline
void Settings_impl::Setter::num(uint64_t val)
{
#define SET_OPTION_NUM_num(X,N) \
  case Session_option_impl::X: return set_option<Session_option_impl::X,unsigned>((unsigned)val);
#define SET_OPTION_NUM_any(X,N) SET_OPTION_NUM_num(X,N)
#define SET_OPTION_NUM_str(X,N)

#define SET_CLI_OPTION_NUM_num(X,N) \
  case Client_option_impl::X: return set_cli_option<Client_option_impl::X,uint64_t>(val);
#define SET_CLI_OPTION_NUM_bool(X,N) SET_CLI_OPTION_NUM_num(X,N)
#define SET_CLI_OPTION_NUM_any(X,N) SET_CLI_OPTION_NUM_num(X,N)
#define SET_CLI_OPTION_NUM_str(X,N)

  /*
    This cannot be processed inside switch because the numeric
    values are converted to unsigned int. For timeout uint64_t is
    required
  */
  if (m_cur_opt == Session_option_impl::CONNECT_TIMEOUT)
    return set_option<Session_option_impl::CONNECT_TIMEOUT>(val);

  if (m_cur_opt == Session_option_impl::CONNECT_TIMEOUT)
    return set_option<Session_option_impl::CONNECT_TIMEOUT>(val);

  if (m_cur_opt < 0 && !check_num_limits<int64_t>(val))
    throw_error("Option ... value too big");

  switch (m_cur_opt)
  {
    SESSION_OPTION_LIST(SET_OPTION_NUM)
    CLIENT_OPTION_LIST(SET_CLI_OPTION_NUM)
    default:break;
  }

  throw_error("Option ... does not accept numeric values.");
}


inline
void Settings_impl::Setter::null()
{
  switch (m_cur_opt)
  {
  case Session_option_impl::HOST:
  case Session_option_impl::PORT:
  case Session_option_impl::PRIORITY:
  case Session_option_impl::USER:
    throw_error("Option ... can not be unset");
    break;
  case Session_option_impl::LAST:
    break;
  default:
    m_data.erase(m_cur_opt);
  }

}


inline
void Settings_impl::Setter::yesno(bool b)
{
  switch (m_cur_opt)
  {
  case Client_option_impl::POOLING:
    add_option(m_cur_opt, b);
    return;
  default: break;
  }
  throw_error("Option ... can not be bool");
}

// URI processor

inline
void Settings_impl::Setter::user(const std::string &usr)
{
  set_option<Session_option_impl::USER>(usr);
}

inline
void Settings_impl::Setter::password(const std::string &pwd)
{
  set_option<Session_option_impl::PWD>(pwd);
}

inline
void Settings_impl::Setter::schema(const std::string &db)
{
  set_option<Session_option_impl::DB>(db);
}

inline
void Settings_impl::Setter::host(
  unsigned short priority, const std::string &host
)
{
  set_option<Session_option_impl::HOST>(host);
  if (0 < priority)
    set_option<Session_option_impl::PRIORITY>(priority - 1);
}

inline
void Settings_impl::Setter::host(
  unsigned short priority,
  const std::string &host,
  unsigned short port
)
{
  set_option<Session_option_impl::HOST>(host);
  set_option<Session_option_impl::PORT>(port);
  if (0 < priority)
    set_option<Session_option_impl::PRIORITY>(priority - 1);
}

inline
void Settings_impl::Setter::socket(unsigned short priority, const std::string &path)
{
  set_option<Session_option_impl::SOCKET>(path);
  if (0 < priority)
    set_option<Session_option_impl::PRIORITY>(priority - 1);
}

inline
void Settings_impl::Setter::key_val(const std::string &key, const std::string &val)
{
  try {
    key_val(get_uri_option(key))->scalar()->str(val);
  }
  catch (const std::out_of_range&)
  {
    throw_error("Invalid URI option ...");
  }
}

inline
void Settings_impl::Setter::key_val(const std::string &key)
{
  try {
    get_uri_option(key);
    throw_error("Option ... requires a value");
  }
  catch (const std::out_of_range&)
  {
    throw_error("invalid URI option ...");
  }
}

inline
void Settings_impl::Setter::key_val(const std::string &key, const std::list<std::string>&)
{
  try {
    get_uri_option(key);
    throw_error("Option ... does not accept a list value");
  }
  catch (const std::out_of_range&)
  {
    throw_error("Invalid URI option ...");
  }
}


inline
int
Settings_impl::Setter::get_uri_option(const std::string &name)
{
  using std::map;

#define URI_OPT_MAP(X,Y) { X, Y },

  static map< std::string, int > uri_map{
    URI_OPTION_LIST(URI_OPT_MAP)
  };

  int opt = uri_map.at(to_lower(name));
  assert(Session_option_impl::LAST != opt);
  return opt;
}


}  // internal namespace
MYSQLX_ABI_END(2,0)
}  // mysqlx namespace

#endif
