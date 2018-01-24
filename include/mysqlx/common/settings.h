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

#ifndef MYSQLX_COMMON_SETTINGS_H
#define MYSQLX_COMMON_SETTINGS_H

/*
  Classes and code handling session settings. They are used to process session
  creation options, check their consistency and present the settings in the
  form expected by CDK.

  Known session options and their values are defined
  in mysql_common_constants.h header as SESSION_OPTION_LIST() and accompanying
  macros.
*/

#include "../common_constants.h"
#include "value.h"

#include <vector>
#include <bitset>
#include <sstream>

namespace cdk {
namespace ds {

class Multi_source;

}}


namespace mysqlx {
namespace common {


/*
  Class for storing session configuration settings.
*/

class PUBLIC_API Settings_impl
{
public:

  /*
    Enumerations with available session options and their values.
  */

#define SETTINGS_OPT_ENUM_str(X,N)  X = N,
#define SETTINGS_OPT_ENUM_num(X,N)  X = N,
#define SETTINGS_OPT_ENUM_any(X,N)  X = N,

  enum class Option {
    SESSION_OPTION_LIST(SETTINGS_OPT_ENUM)
    LAST
  };

  static  const char* option_name(Option opt);


#define SETTINGS_VAL_ENUM(X,N)  X = N,

  enum class SSL_mode {
    SSL_MODE_LIST(SETTINGS_VAL_ENUM)
    LAST
  };

  static  const char* ssl_mode_name(SSL_mode mode);


  enum class Auth_method {
    AUTH_METHOD_LIST(SETTINGS_VAL_ENUM)
    LAST
  };

  static  const char* auth_method_name(Auth_method method);

protected:

  using opt_val_t = std::pair<Option, Value>;
  // TODO: use multimap instead?
  using option_list_t = std::vector<opt_val_t>;
  using iterator = option_list_t::const_iterator;

public:

  /*
    Examine settings stored in this object.
  */

  bool has_option(Option) const;
  const Value& get(Option) const;

  // Iterating over options stored in this object.

  iterator begin() const
  {
    return m_data.m_options.cbegin();
  }

  iterator end() const
  {
    return m_data.m_options.cend();
  }

  /*
    Clear individual or all settings.
  */

  void clear();
  void erase(Option);

  /*
    Session options include connection options that specify data source
    (one or many) for which given session is created. This method initializes
    CDK Multi_source object to describe the data source(s) based on the
    connection options.
  */

  void get_data_source(cdk::ds::Multi_source&);


  // Set options based on URI

  void set_from_uri(const std::string &);

  /*
    Public API has no methods to directly set individual options. Instead,
    to change session settings implementation should create a Setter object
    and use its methods to do the changes. A Settings_impl::Setter object
    provides "transactional" semantics for changing session options -- only
    consistent option changes modify the original Settings_impl object.

    Note: This Setter class is defined in "implementation" header
    common/settings.h. The public API leaves it undefined.
  */

  class Setter;

protected:

  struct PUBLIC_API Data
  {
    DLL_WARNINGS_PUSH
    option_list_t           m_options;
    DLL_WARNINGS_POP
    unsigned m_host_cnt = 0;
    bool m_user_priorities = false;
    bool m_ssl_ca = false;
    SSL_mode m_ssl_mode = SSL_mode::LAST;
    bool m_tcpip = false; // set to true if TCPIP connection was specified
    bool m_sock = false;  // set to true if socket connection was specified

    void erase(Option);
  };

  Data m_data;

};


#define SETTINGS_OPT_NAME_str(X,N)  case N: return #X;
#define SETTINGS_OPT_NAME_num(X,N)  case N: return #X;
#define SETTINGS_OPT_NAME_any(X,N)  case N: return #X;

inline
const char* Settings_impl::option_name(Option opt)
{
  switch (unsigned(opt))
  {
    SESSION_OPTION_LIST(SETTINGS_OPT_NAME)
  default:
    return nullptr;
  }
}


#define SETTINGS_VAL_NAME(X,N) case N: return #X;

inline
const char* Settings_impl::ssl_mode_name(SSL_mode mode)
{
  switch (unsigned(mode))
  {
    SSL_MODE_LIST(SETTINGS_VAL_NAME)
  default:
    return nullptr;
  }
}

inline
const char* Settings_impl::auth_method_name(Auth_method method)
{
  switch (unsigned(method))
  {
    SSL_MODE_LIST(SETTINGS_VAL_NAME)
  default:
    return nullptr;
  }
}


/*
  Note: For options that can repeat, returns the last value.
*/

inline
const Value& Settings_impl::get(Option opt) const
{
  using std::find_if;

  auto it = find_if(m_data.m_options.crbegin(), m_data.m_options.crend(),
    [opt](opt_val_t el) -> bool { return el.first == opt; }
  );

  static Value null_value;

  if (it == m_data.m_options.crend())
    return null_value;

  return it->second;
}


inline
bool Settings_impl::has_option(Option opt) const
{
  return m_data.m_options.cend() !=
    find_if(m_data.m_options.cbegin(), m_data.m_options.cend(),
      [opt](opt_val_t el) -> bool { return el.first == opt; }
    );
}



inline
void Settings_impl::erase(Option opt)
{
  m_data.erase(opt);
}


/*
  Note: Removes all occurrences of the given option. Also updates the context
  used for checking option consistency.
*/

inline
void Settings_impl::Data::erase(Option opt)
{
  remove_from(m_options,
    [opt](opt_val_t el) -> bool
    {
      return el.first == opt;
    }
  );

  /*
    TODO: removing HOST from multi-host settings can leave "orphaned"
    PORT/PRIORITY settings. Do we correctly detect that?
  */

  switch (opt)
  {
  case Option::HOST:
    m_host_cnt = 0;
    FALLTHROUGH;
  case Option::PORT:
    if (0 == m_host_cnt)
      m_tcpip = false;
    break;
  case Option::SOCKET:
    m_sock = false;
    break;
  case Option::PRIORITY:
    m_user_priorities = false;
    break;
  case Option::SSL_CA:
    m_ssl_ca = false;
    break;
  case Option::SSL_MODE:
    m_ssl_mode = SSL_mode::LAST;
    break;
  default:
    break;
  }
}

}  // common namespace
}  // mysqlx namespace

#endif
