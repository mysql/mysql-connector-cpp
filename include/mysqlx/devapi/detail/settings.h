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

#ifndef MYSQLX_DETAIL_SETTINGS_H
#define MYSQLX_DETAIL_SETTINGS_H

#include "../common.h"
#include "../document.h"
#include <chrono>

#include <list>
#include <chrono>

namespace mysqlx {
namespace internal {

/*
  Note: Options and SSLMode enumerations are given by Traits template parameter
  to allow defining (and documenting) them in the main settings.h header.
*/


template <typename Traits>
class Settings_detail
  : public common::Settings_impl
{
  using Value      = common::Value;
  using SOption    = typename Traits::Options;
  using COption    = typename Traits::CliOptions;
  using SSLMode    = typename Traits::SSLMode;
  using AuthMethod = typename Traits::AuthMethod;

public:



  template <bool session_only,typename OPT,typename... Ty>
  void set(OPT opt, Ty&&... rest)
  {
    do_set(get_options<session_only>(opt, std::forward<Ty>(rest)...));
  }


protected:

  /*
    Declare options that require specific type of value (mostly enumerations).
    For such options we do not accept setting them to arbitrary values. Instead
    an overload of opt_val() with appropriate type will be used to set value
    of the option.
  */

#define OPT_VAL_TYPE(X) \
  X(SSL_MODE,SSLMode) \
  X(AUTH,AuthMethod)

#define CHECK_OPT(Opt,Type) \
  if (opt == Option_impl::Opt) \
    throw Error(#Opt "setting requires value of type " #Type);

  /*
    Store option value in Value object (with basic run-time type checks)
    TODO: More precise type checking using per-option types.
  */

  static Value opt_val(Option_impl opt, Value &&val)
  {
    OPT_VAL_TYPE(CHECK_OPT)
    return val;
  }

  /*
    For types which are not convertible to Value, but can be converted to string
    go through string conversion.
  */

  template <
    typename V,
    typename std::enable_if<std::is_convertible<V,string>::value>::type*
    = nullptr
  >
  static Value opt_val(Option_impl opt, V &&val)
  {
    OPT_VAL_TYPE(CHECK_OPT)
    return string(val);
  }

  template<typename _Rep, typename _Period>
  static Value opt_val(Option_impl opt,
                       const std::chrono::duration<_Rep, _Period> &val)
  {
    if (opt != Option_impl::CONNECT_TIMEOUT)
    {
      std::stringstream err_msg;
      err_msg << "Option " << option_name(opt) << " does not accept time value";
      throw_error(err_msg.str().c_str());
    }
    return Value(std::chrono::duration_cast<std::chrono::milliseconds>(val)
                 .count());
  }

  static Value opt_val(Option_impl opt, SSLMode m)
  {
    if (opt != Option_impl::SSL_MODE)
      throw Error(
        "SessionSettings::SSLMode value can only be used on SSL_MODE setting."
      );
    return unsigned(m);
  }

  static Value opt_val(Option_impl opt, AuthMethod m)
  {
    if (opt != Option_impl::AUTH)
      throw Error(
        "SessionSettings::AuthMethod value can only be used on AUTH setting."
      );
    return unsigned(m);
  }

  template<typename _Rep, typename _Period>
  static Value opt_val(Client_option_impl opt,
                       const std::chrono::duration<_Rep,_Period> &duration)
  {
    if (opt != Client_option_impl::POOL_QUEUE_TIMEOUT &&
        opt != Client_option_impl::POOL_MAX_IDLE_TIME)
    {
      std::stringstream err_msg;
      err_msg << "Option " << option_name(opt) << " does not accept time value";
      throw_error(err_msg.str().c_str());
    }

    return Value(std::chrono::duration_cast<std::chrono::milliseconds>(duration)
                 .count());
  }


  template <
    typename V,
    typename std::enable_if<std::is_convertible<V,int>::value>::type*
    = nullptr
  >
  static Value opt_val(Client_option_impl, V &&val)
  {
    //ClientOptions are all bool or int, so convertible to int
    return val;
  }




  using session_opt_val_t = std::pair<int, Value>;
  using session_opt_list_t = std::list<session_opt_val_t>;

  /*
    Set list of options with consistency checks.

    This operation is atomic - settings are changed only if all options could
    be set without error, otherwise settings remain unchanged.
  */

  void do_set(session_opt_list_t&&);

  /*
    ClientOptions and Options are converted to int, one positive and other
    negative
  */

  static
  int option_to_int(Option_impl opt)
  {
    return static_cast<int>(opt);
  }

  static
  int option_to_int(Client_option_impl opt)
  {
    return -static_cast<int>(opt);
  }

  static
  Option_impl int_to_option(int opt)
  {
    return static_cast<Option_impl>(opt);
  }

  static
  Client_option_impl int_to_client_option(int opt)
  {
    return static_cast<Client_option_impl>(-opt);
  }

  /*
    Templates that collect varargs list of options into opt_list_t list
    that can be passed to do_set().
  */

  template<bool session_only>
  static session_opt_list_t get_options()
  {
    return {};
  }

  /*
    Note: if we ever support options without values, another overload is
    needed: get_options(Option opt, Option opt1, R&... rest).
  */

  template <bool session_only,typename V, typename... Ty>
  static session_opt_list_t get_options(SOption opt, V&& val, Ty&&... rest)
  {
    Option_impl oo = (Option_impl)(unsigned)opt;
    session_opt_list_t opts = get_options<session_only>(std::forward<Ty>(rest)...);
    opts.emplace_front(
      option_to_int(oo),
      Settings_detail::opt_val(oo, std::forward<V>(val))
    );
    return opts;
  }

  template <bool session_only,typename V, typename... Ty,
            typename std::enable_if<!session_only,int>::type*
            = nullptr>
  static session_opt_list_t get_options(COption opt, V&& val, Ty&&... rest)
  {
    Client_option_impl oo = (Client_option_impl)(unsigned)opt;
    session_opt_list_t opts = get_options<session_only>(std::forward<Ty>(rest)...);
    opts.emplace_front(
      option_to_int(oo),
      Settings_detail::opt_val(oo, std::forward<V>(val))
    );
    return opts;
  }


  /*
    Note: Methods below rely on the fact that DevAPI SessionOption constants
    have the same numeric values as common::Settings_impl::Option ones.
  */

  void erase(SOption opt)
  {
    Settings_impl::erase((Option_impl)(unsigned)opt);
  }

  void erase(COption opt)
  {
    Settings_impl::erase((Client_option_impl)(unsigned)opt);
  }

  bool has_option(SOption opt)
  {
    return Settings_impl::has_option((Option_impl)(unsigned)opt);
  }

  bool has_option(COption opt)
  {
    return Settings_impl::has_option((Client_option_impl)(unsigned)opt);
  }

  Value get(SOption opt)
  {
    return Settings_impl::get((Option_impl)(unsigned)opt);
  }

  Value get(COption opt)
  {
    return Settings_impl::get((Client_option_impl)(unsigned)opt);
  }
};


}  // internal namespace
}  // mysqlx namespace

#endif

