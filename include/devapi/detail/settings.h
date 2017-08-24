#ifndef MYSQLX_DETAIL_SETTINGS_H
#define MYSQLX_DETAIL_SETTINGS_H

#include "../document.h"

#include <vector>
#include <bitset>
#include <sstream>

namespace mysqlx {
namespace internal {



/*
  Note: Options and SSLMode enumerations are given by Traits template parameter to allow defining (and documenting) them in the main settings.h header.
*/

template <typename Traits>
class Settings_detail
{
  using Options    = typename Traits::Options;
  using SSLMode    = typename Traits::SSLMode;
  using AuthMethod = typename Traits::AuthMethod;

protected:

  using option_list_t = std::vector<std::pair<Options, Value>>;
  using iterator = typename option_list_t::iterator;

  option_list_t           m_options;
  std::bitset<size_t(Options::LAST)>  m_option_used;
  std::bitset<size_t(Options::LAST)>  m_call_used;

  void do_add(Options opt, Value &&v);
  Value& find(Options opt);

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
  if (opt == Options::Opt) \
    throw Error(#Opt "setting requires value of type " #Type);

  /*
    Store option value in Value object (with basic run-time type checks)
    TODO: More precise type checking using per-option types.
  */

  static Value opt_val(Options opt, Value &&val)
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
  static Value opt_val(Options opt, V &&val)
  {
    OPT_VAL_TYPE(CHECK_OPT)
    return string(val);
  }

  static Value opt_val(Options opt, SSLMode m)
  {
    if (opt != Options::SSL_MODE)
      throw Error("SessionSettings::SSLMode value can only be used on SSL_MODE setting.");
    return unsigned(m);
  }

  static Value opt_val(Options opt, AuthMethod m)
  {
    if (opt != Options::AUTH)
      throw Error("SessionSettings::AuthMethod value can only be used on AUTH setting.");
    return unsigned(m);
  }



  /*
    TODO: Document it
  */

  void do_set(bool) {}

  template <typename V, typename...R>
  void do_set(bool host_optional, Options opt, V v, R...rest)
  {
    switch (opt)
    {
#ifndef WIN32
    case Options::SOCKET:
      return do_add_socket(opt_val(Options::SOCKET, v), rest...);
#endif

    case Options::HOST:
      return do_add_host(opt_val(Options::HOST, v), rest...);

    case Options::PORT:
      if (host_optional)
        return do_add_host("localhost", opt_val(Options::PORT, v), rest...);
      else
        throw Error("Defining PORT without first defining HOST.");

    case Options::PRIORITY:
      if (host_optional)
      {
        do_add_host(
          "localhost",
          opt_val(Options::PORT, DEFAULT_MYSQLX_PORT),
          opt_val(Options::PRIORITY, v)
        );
        do_set(false, rest...);
        return;
      }
      else
        throw Error("Defining PRIORITY without first defining HOST.");

    default:

      if (m_call_used.test(size_t(opt)))
      {
        std::stringstream error;
        error << "SessionSettings option "
              << Traits::get_option_name(opt) << " defined twice";

        throw Error(error.str().c_str());
      }

      m_call_used.set(size_t(opt));

      do_add(opt, opt_val(opt,v));
      do_set(host_optional, rest...);
    }
  }


  /*
    Add HOST setting checking valid order of options after it
    (PORT/PRIORITY).
  */

  void do_add_host(Value &&host)
  {
    do_add(Options::HOST, std::move(host));
  }

  void do_add_host(Value &&host, Value &&port)
  {
    do_add(Options::HOST, std::move(host));
    do_add(Options::PORT, std::move(port));
  }

  void do_add_host(Value &&host, Value &&port, Value &&priority)
  {
    do_add(Options::HOST, std::move(host));
    do_add(Options::PORT, std::move(port));
    do_add(Options::PRIORITY, std::move(priority));
  }

  template <typename V, typename...R>
  void do_add_host(Value &&host, Options opt, V v, R...rest)
  {
    if (opt == Options::PORT)
    {
      //we could still have priority
      do_add_host(std::move(host), opt_val(Options::PORT,v), rest...);
      return;
    }
    else if (opt == Options::PRIORITY)
    {
      do_add_host(std::move(host), DEFAULT_MYSQLX_PORT,
        opt_val(Options::PRIORITY,v));
      do_set(false, rest...);
      return;
    }

    do_add_host(std::move(host));
    do_set(false, opt, v, rest...);
  }

  template <typename V, typename...R>
  void do_add_host(Value &&host, Value &&port, Options opt, V v, R...rest)
  {
    if (opt == Options::PRIORITY)
    {
      do_add_host(std::move(host), std::move(port),
        opt_val(Options::PRIORITY, v));
      do_set(false, rest...);
      return;
    }
    do_add_host(std::move(host), std::move(port));
    do_set(false, opt, v, rest...);
  }

#ifndef WIN32

  /*
    Add SOCKET setting checking if PRIORITY is available.
  */

  void do_add_socket(Value &&socket)
  {
    do_add(Options::SOCKET, std::move(socket));
  }

  void do_add_socket(Value &&socket, Value &&priority)
  {
    do_add(Options::SOCKET, std::move(socket));
    do_add(Options::PRIORITY, std::move(priority));
  }

  template <typename V, typename...R>
  void do_add_socket(Value &&socket, Options opt, V v, R...rest)
  {
    if (opt == Options::PRIORITY)
    {
      do_add_socket(std::move(socket), opt_val(opt,v));
      do_set(false, rest...);
    }
    else
    {
      do_add_socket(std::move(socket));
      do_set(false, opt, v, rest...);
    }
  }

#endif

};

}  // internal namespace
}  // mysqlx namespace

#endif
