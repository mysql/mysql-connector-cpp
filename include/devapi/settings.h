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

#ifndef MYSQLX_DEVAPI_SETTINGS_H
#define MYSQLX_DEVAPI_SETTINGS_H

/**
  @file
  TODO
*/

#include "common.h"
#include "detail/settings.h"


namespace mysqlx {

/*
  TODO: Cross-references to session options inside Doxygen docs do not work.
*/

#ifndef _WIN32
#define ADD_SOCKET(x) x(SOCKET) /*!< path to a Unix domain socket*/ END_LIST
#else
#define ADD_SOCKET(x)
#endif //_WIN32

#define SETTINGS_OPTIONS(x)                                                   \
  x(URI)          /*!< connection URI or string */                            \
  /*! DNS name of the host, IPv4 address or IPv6 address */                   \
  x(HOST)                                                                     \
  x(PORT)          /*!< X Plugin port to connect to */                        \
  /*! Assign a priority (a number in range 1 to 100) to the last specified
      host; these priorities are used to determine the order in which multiple
      hosts are tried by the connection fail-over logic (see description
      of `Session` class) */                                                  \
  x(PRIORITY)                                                                 \
  x(USER)          /*!< user name */                                          \
  x(PWD)           /*!< password */                                           \
  x(DB)            /*!< default database */                                   \
  x(SSL_MODE)      /*!< define `SSLMode` option to be used */                 \
  /*! path to a PEM file specifying trusted root certificates*/               \
  x(SSL_CA)                                                                   \
  x(AUTH)          /*!< authentication method, PLAIN, MYSQL41, etc.*/         \
  ADD_SOCKET(x) \
  END_LIST

#define OPTIONS_ENUM(x) x,


/**
  Session creation options

  @note `PRIORITY` should be defined after a `HOST` (`PORT`) to which
  it applies.

  @note Specifying `SSL_CA` option requires `SSL_MODE` value of `VERIFY_CA`
  or `VERIFY_IDENTITY`. If `SSL_MODE` is not explicitly given then
  setting `SSL_CA` implies `VERIFY_CA`.
*/

enum_class SessionOption
{
  SETTINGS_OPTIONS(OPTIONS_ENUM)
  LAST
};


/// @cond DISABLED
// Note: Doxygen gets confused here and renders docs incorrectly.

inline
std::string SessionOptionName(SessionOption opt)
{
#define OPTIONS_NAME(x) case SessionOption::x: return #x;

  switch(opt)
  {
    SETTINGS_OPTIONS(OPTIONS_NAME)
    default:
    {
      std::ostringstream buf;
      buf << "<UKNOWN (" << unsigned(opt) << ")>" << std::ends;
      return buf.str();
    }
  };
}

/// @endcond


#define SSL_MODE_TYPES(x)\
  x(DISABLED)        /*!< Establish an unencrypted connection.  */ \
  x(REQUIRED)        /*!< Establish a secure connection if the server supports
                          secure connections. The connection attempt fails if a
                          secure connection cannot be established. This is the
                          default if `SSL_MODE` is not specified. */ \
  x(VERIFY_CA)       /*!< Like `REQUIRED`, but additionally verify the server
                          TLS certificate against the configured Certificate
                          Authority (CA) certificates (defined by `SSL_CA`
                          Option). The connection attempt fails if no valid
                          matching CA certificates are found.*/ \
  x(VERIFY_IDENTITY) /*!< Like `VERIFY_CA`, but additionally verify that the
                          server certificate matches the host to which the
                          connection is attempted.*/\
  END_LIST

#define SSL_ENUM(x) x,

/**
  Modes to be used with `SSL_MODE` option
*/

enum_class SSLMode
{
  SSL_MODE_TYPES(SSL_ENUM)
};


/// @cond DISABLED

inline
std::string SSLModeName(SSLMode m)
{
#define MODE_NAME(x) case SSLMode::x: return #x;

  switch(m)
  {
    SSL_MODE_TYPES(MODE_NAME)
    default:
    {
      std::ostringstream buf;
      buf << "<UKNOWN (" << unsigned(m) << ")>" << std::ends;
      return buf.str();
    }
  };
}

/// @endcond


#define AUTH_METHODS(x)\
  x(PLAIN)        /*!< Plain text authentication method. The password is
                       sent as a clear text. This method is used by
                       default in encrypted connections. */ \
  x(MYSQL41)      /*!< Authentication method supported by MySQL 4.1 and newer.
                       The password is hashed before being sent to the server.
                       This method is used by default in unencrypted
                       connections */ \
  x(EXTERNAL)     /*!< External authentication when the server establishes
                       the user authenticity by other means such as SSL/x509
                       certificates. Currently not supported by X Plugin */ \
  END_LIST

#define AUTH_ENUM(x) x,

/**
  Authentication methods to be used with `AUTH` option.
*/

enum_class AuthMethod
{
  AUTH_METHODS(AUTH_ENUM)
};


/// @cond DISABLED

inline
std::string AuthMethodName(AuthMethod m)
{
#define AUTH_NAME(x) case AuthMethod::x: return #x;

  switch(m)
  {
    AUTH_METHODS(AUTH_NAME)
    default:
    {
      std::ostringstream buf;
      buf << "<UKNOWN (" << unsigned(m) << ")>" << std::ends;
      return buf.str();
    }
  };
}

/// @endcond


namespace internal {

/*
  Encapsulate public enumerations in the Settings_traits class to be used
  by Settings_detail<> template.
*/

struct Settings_traits
{
  using Options    = mysqlx::SessionOption;
  using SSLMode    = mysqlx::SSLMode;
  using AuthMethod = mysqlx::AuthMethod;

  static std::string get_mode_name(SSLMode mode)
  {
    return SSLModeName(mode);
  }

  static std::string get_option_name(Options opt)
  {
    return SessionOptionName(opt);
  }

  static std::string get_aouth_name(AuthMethod m)
  {
    return AuthMethodName(m);
  }
};


/*
  We need to declare these methods of specialized
  Settings_deatil<Settings_traits> template as PUBLIC_API because they are used
  by public API SessionSettings class and they are defined in session.cc.
  Without these declarations the methods won't be exported by the connector
  library.
*/

template<>
PUBLIC_API
Value& Settings_detail<Settings_traits>::find(SessionOption opt);

template<>
PUBLIC_API
void Settings_detail<Settings_traits>::do_add(SessionOption, Value&&);

} // internal namespace


class Session;

/**
  Represents session options to be passed at session creation time.

  SessionSettings can be constructed using a connection string, common
  connect options (host, port, user, password, database) or with a list
  of `SessionOption` constants, each followed by the option value.

  Examples:
  ~~~~~~

    SessionSettings from_url("mysqlx://user:pwd@host:port/db?ssl-mode=required");

    SessionSettings from_options("host", port, "user", "pwd", "db");

    SessionSettings from_option_list(
      SessionOption::USER, "user",
      SessionOption::PWD,  "pwd",
      SessionOption::HOST, "host",
      SessionOption::PORT, port,
      SessionOption::DB,   "db",
      SessionOption::SSL_MODE, SSLMode::REQUIRED
    );
  ~~~~~~

  The HOST, PORT and SOCKET settings can be repeated to build a list of hosts
  to be used by the connection fail-over logic when creating a session (see
  description of `Session` class). In that case each host can be assigned
  a priority by setting the `PRIORITY` option. If priorities are not explicitly
  assigned, hosts are tried in the order in which they are specified in session
  settings. If priorities are used, they must be assigned to all hosts
  specified in the settings.

  @ingroup devapi
*/

DLL_WARNINGS_PUSH

class PUBLIC_API SessionSettings
  : internal::Settings_detail<internal::Settings_traits>
{

DLL_WARNINGS_POP

  using Base = internal::Settings_detail<internal::Settings_traits>;

public:

  /**
    Create session settings from a connection string.

    Connection sting has the form

          "user:pass@connection-data/db?option&option"

    with optional `mysqlx://` prefix.

    The `connetction-data` part is either a single host address or a coma
    separated list of hosts in square brackets: `[host1, host2, ..., hostN]`.
    In the latter case the connection fail-over logic will be used when
    creating the session.

    A single host address is either a DNS host name, an IPv4 address of
    the form "nn.nn.nn.nn" or an IPv6 address of the form "[nn:nn:nn:...]".
    On Unix systems a host can be specified as a path to a Unix domain
    socket - this path must start with `/` or `.`.

    Characters like `/` in the connection data, which otherwise have a special
    meaning inside a connection string, must be represented using percent
    encoding (e.g., `%2F` for `/`). Another option is to enclose a host name or
    a socket path in round braces. For example, one can write

        "mysqlx://(./path/to/socket)/db"

    instead of

        "mysqlx://.%2Fpath%2Fto%2Fsocket/db"

    To specify priorities for hosts in a multi-host settings, use list of pairs
    of the form `(address=host,priority=N)`. If priorities are specified, they
    must be given to all hosts in the list.

    The optional `db` part of the connection string defines the default schema
    of the session.

    Possible connection options are:

    - `ssl-mode` : define `SSLMode` option to be used
    - `ssl-ca=`path : path to a PEM file specifying trusted root certificates
  */

  SessionSettings(const string &uri)
  {
    try {
      do_set(true, SessionOption::URI, uri);
    }
    CATCH_AND_WRAP
  }


  /**
    Explicitly specify basic connection settings.

    @note Session settings constructed this way request an SSL connection
    by default.
  */

  SessionSettings(const std::string &host, unsigned port,
                  const string  &user,
                  const char *pwd = NULL,
                  const string &db = string())
  {
    try {

      do_set(true, SessionOption::HOST, host,
          SessionOption::PORT, port,
          SessionOption::USER, user,
          SessionOption::DB, db);

      if (pwd)
        do_set(true, SessionOption::PWD, pwd);
    }
    CATCH_AND_WRAP
  }

  SessionSettings(const std::string &host, unsigned port,
                  const string  &user,
                  const std::string &pwd,
                  const string &db = string())
    : SessionSettings(host, port, user, pwd.c_str(), db)
  {}

  /**
    Basic settings with the default port

    @note Session settings constructed this way request an SSL connection
    by default.
  */

  SessionSettings(const std::string &host,
                  const string  &user,
                  const char    *pwd = NULL,
                  const string  &db = string())
    : SessionSettings(host, DEFAULT_MYSQLX_PORT, user, pwd, db)
  {}

  SessionSettings(const std::string &host,
                  const string  &user,
                  const std::string &pwd,
                  const string  &db = string())
    : SessionSettings(host, DEFAULT_MYSQLX_PORT, user, pwd, db)
  {}

  /**
    Basic settings for a session on the localhost.

    @note Session settings constructed this way request an SSL connection
    by default.
  */

  SessionSettings(unsigned port,
                  const string  &user,
                  const char    *pwd = NULL,
                  const string  &db = string())
    : SessionSettings("localhost", port, user, pwd, db)
  {}

  SessionSettings(unsigned port,
                  const string  &user,
                  const std::string &pwd,
                  const string  &db = string())
    : SessionSettings("localhost", port, user, pwd.c_str(), db)
  {}

  /*
    Templates below are here to take care of the optional password
    parameter of type const char* (which can be either 3-rd or 4-th in
    the parameter list). Without these templates passing
    NULL as password is ambiguous because NULL is defined as 0,
    which has type int, and then it could be treated as port value.
  */

  template <
    typename    HOST,
    typename    PORT,
    typename    USER,
    typename... T,
    typename std::enable_if<
      std::is_constructible<SessionSettings, HOST, PORT, USER, const char*, T...>::value
    >::type* = nullptr
  >
  SessionSettings(HOST h, PORT p, USER u ,long , T... args)
    : SessionSettings(h, p, u, nullptr, args...)
  {}


  template <
    typename    PORT,
    typename    USER,
    typename... T,
    typename std::enable_if<
      std::is_constructible<SessionSettings, PORT, USER, const char*, T...>::value
    >::type* = nullptr
  >
  SessionSettings(PORT p, USER u ,long , T... args)
    : SessionSettings(p, u, nullptr, args...)
  {}


  /**
    Specify settings as a list of session options.

    The list of options consist of a `SessionOption` constant,
    identifying the option to set, followed by the value of the option.

    Example:
    ~~~~~~
      SessionSettings from_option_list(
        SessionOption::USER, "user",
        SessionOption::PWD,  "pwd",
        SessionOption::HOST, "host",
        SessionOption::PORT, port,
        SessionOption::DB,   "db",
        SessionOption::SSL_MODE, SessionSettings::SSLMode::REQUIRED
      );
    ~~~~~~
  */

  template <typename V,typename...R>
  SessionSettings(SessionOption opt, V val, R...rest)
  {
    try {
      do_set(true, opt, val, rest...);
    }
    CATCH_AND_WRAP
  }


  /*
    SessionSetting operator and methods
  */

  /*
    Return an iterator pointing to the first element of the SessionSettings.
  */

  iterator begin()
  {
    try {
      return m_options.begin();
    }
    CATCH_AND_WRAP
  }

  /*
    Return an iterator pointing to the last element of the SessionSettings.
  */

  iterator end()
  {
    try {
      return m_options.end();
    }
    CATCH_AND_WRAP
  }


  /**
    Find the specified option @p opt and returns its Value.

    Throws an error if the given option was not found in the settings.

    @note For option such as `HOST`, which can repeat several times in
    the settings, only the last value is reported.
  */

  Value& find(SessionOption opt)
  {
    try {
      return Base::find(opt);
    }
    CATCH_AND_WRAP
  }

  /**
    Set session options.

    Accepts a list of one or more `SessionOption` constants, each followed by
    the option value. Options specified here are added to the current settings.

    Repeated `HOST`, `PORT`, `SOCKET` and `PRIORITY` options build a list of
    hosts to be used by the fail-over logic. For other options, if they are set
    again, the new value overrides the previous setting.

    @note
    When using `HOST`, `PORT` and `PRIORITY` options to specify a single
    host, all have to be specified in the same `set()` call.
   */

  template<typename V,typename...R>
  void set(SessionOption opt, V v, R...rest)
  {
    try {
      m_call_used.reset();
      do_set(false, opt, v,rest...);
    }
    CATCH_AND_WRAP
  }

  /**
    Clear all settings specified so far.
  */

  void clear()
  {
    try {
      m_options.clear();
      m_option_used.reset();
    }
    CATCH_AND_WRAP
  }

  /**
    Remove all settings for the given option @p opt.

    @note For option such as `HOST`, which can repeat several times in
    the settings, all occurrences are erased.
  */

  void erase(SessionOption opt)
  {
    try {

      auto it = m_options.begin();

      while(it != m_options.end())
      {
        if(it->first == opt)
        {
          it = m_options.erase(it);
        }
        else
        {
          ++it;
        }
      }
      m_option_used.reset(size_t(opt));
    }
    CATCH_AND_WRAP
  }


  /**
    Check if option @p opt was defined.
  */

  bool has_option(SessionOption opt)
  {
    return m_option_used.test(size_t(opt));
  }

private:

  friend Session;
};


}  // mysqlx

#endif
