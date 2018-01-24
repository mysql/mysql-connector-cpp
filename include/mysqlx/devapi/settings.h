/*
 * Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
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
#define SESS_OPT_ENUM_any(X,N) X = N,
#define SESS_OPT_ENUM_num(X,N) X = N,
#define SESS_OPT_ENUM_str(X,N) X = N,

  SESSION_OPTION_LIST(SESS_OPT_ENUM)
  LAST
};


/// @cond DISABLED
// Note: Doxygen gets confused here and renders docs incorrectly.

inline
std::string SessionOptionName(SessionOption opt)
{
#define SESS_OPT_NAME_any(X,N) case SessionOption::X: return #X;
#define SESS_OPT_NAME_num(X,N) SESS_OPT_NAME_any(X,N)
#define SESS_OPT_NAME_str(X,N) SESS_OPT_NAME_any(X,N)

  switch(opt)
  {
    SESSION_OPTION_LIST(SESS_OPT_NAME)
    default:
    {
      std::ostringstream buf;
      buf << "<UKNOWN (" << unsigned(opt) << ")>" << std::ends;
      return buf.str();
    }
  };
}

/// @endcond


/**
  Modes to be used with `SSL_MODE` option
*/

enum_class SSLMode
{
#define SSL_ENUM(X,N) X = N,

  SSL_MODE_LIST(SSL_ENUM)
};


/// @cond DISABLED

inline
std::string SSLModeName(SSLMode m)
{
#define MODE_NAME(X,N) case SSLMode::X: return #X;

  switch(m)
  {
    SSL_MODE_LIST(MODE_NAME)
    default:
    {
      std::ostringstream buf;
      buf << "<UKNOWN (" << unsigned(m) << ")>" << std::ends;
      return buf.str();
    }
  };
}

/// @endcond


/**
  Authentication methods to be used with `AUTH` option.
*/

enum_class AuthMethod
{
#define AUTH_ENUM(X,N) X=N,

  AUTH_METHOD_LIST(AUTH_ENUM)
};


/// @cond DISABLED

inline
std::string AuthMethodName(AuthMethod m)
{
#define AUTH_NAME(X,N) case AuthMethod::X: return #X;

  switch(m)
  {
    AUTH_METHOD_LIST(AUTH_NAME)
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

  static std::string get_auth_name(AuthMethod m)
  {
    return AuthMethodName(m);
  }
};


template<>
PUBLIC_API
void
internal::Settings_detail<internal::Settings_traits>::do_set(opt_list_t &&opts);


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

class SessionSettings
  : private internal::Settings_detail<internal::Settings_traits>
{
  using Value = mysqlx::Value;

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
      Settings_detail::set_from_uri(uri);
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
    set(
      SessionOption::HOST, host,
      SessionOption::PORT, port,
      SessionOption::USER, user
    );

    if (pwd)
      set(SessionOption::PWD, std::string(pwd));

    if (!db.empty())
      set(SessionOption::DB, db);
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

  template <typename... R>
  SessionSettings(SessionOption opt, R&&...rest)
  {
    try {
      Settings_detail::set(opt, std::forward<R>(rest)...);
    }
    CATCH_AND_WRAP
  }

  /*
    Return an iterator pointing to the first element of the SessionSettings.
  */

  using Settings_detail::iterator;

  iterator begin()
  {
    try {
      return Settings_detail::begin();
    }
    CATCH_AND_WRAP
  }

  /*
    Return an iterator pointing to the last element of the SessionSettings.
  */

  iterator end()
  {
    try {
      return Settings_detail::end();
    }
    CATCH_AND_WRAP
  }


  /**
    Find the specified option @p opt and returns its Value.

    Throws an error if the given option was not found in the settings.

    @note For option such as `HOST`, which can repeat several times in
    the settings, only the last value is reported.
  */

  Value find(SessionOption opt)
  {
    try {
      return Settings_detail::get(opt);
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

  template<typename... R>
  void set(SessionOption opt, R&&... rest)
  {
    try {
      Settings_detail::set(opt, std::forward<R>(rest)...);
    }
    CATCH_AND_WRAP
  }

  /**
    Clear all settings specified so far.
  */

  void clear()
  {
    try {
      Settings_detail::clear();
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
      Settings_detail::erase(opt);
    }
    CATCH_AND_WRAP
  }


  /**
    Check if option @p opt was defined.
  */

  bool has_option(SessionOption opt)
  {
    try {
      return Settings_detail::has_option(opt);
    }
    CATCH_AND_WRAP
  }

private:

  friend Session;
};


}  // mysqlx

#endif
