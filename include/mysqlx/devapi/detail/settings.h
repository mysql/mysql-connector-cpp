#ifndef MYSQLX_DETAIL_SETTINGS_H
#define MYSQLX_DETAIL_SETTINGS_H

#include "../common.h"
#include "../document.h"

#include <list>

namespace mysqlx {
namespace internal {

/*
  Note: Options and SSLMode enumerations are given by Traits template parameter to allow defining (and documenting) them in the main settings.h header.
*/


template <typename Traits>
class Settings_detail
  : public common::Settings_impl
{
  using Value      = common::Value;
  using SOption    = typename Traits::Options;
  using SSLMode    = typename Traits::SSLMode;
  using AuthMethod = typename Traits::AuthMethod;

public:

  template <typename V, typename... Ty>
  void set(SOption opt, V&& val, Ty&&... rest)
  {
    do_set(get_options(opt, std::forward<V>(val), std::forward<Ty>(rest)...));
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
  if (opt == Option::Opt) \
    throw Error(#Opt "setting requires value of type " #Type);

  /*
    Store option value in Value object (with basic run-time type checks)
    TODO: More precise type checking using per-option types.
  */

  static Value opt_val(Option opt, Value &&val)
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
  static Value opt_val(Option opt, V &&val)
  {
    OPT_VAL_TYPE(CHECK_OPT)
    return string(val);
  }

  static Value opt_val(Option opt, SSLMode m)
  {
    if (opt != Option::SSL_MODE)
      throw Error(
        "SessionSettings::SSLMode value can only be used on SSL_MODE setting."
      );
    return unsigned(m);
  }

  static Value opt_val(Option opt, AuthMethod m)
  {
    if (opt != Option::AUTH)
      throw Error(
        "SessionSettings::AuthMethod value can only be used on AUTH setting."
      );
    return unsigned(m);
  }


  using opt_val_t = std::pair<Option, Value>;
  using opt_list_t = std::list<opt_val_t>;

  /*
    Set list of options with consistency checks.

    This operation is atomic - settings are changed only if all options could
    be set without error, otherwise settings remain unchanged.
  */

  void do_set(opt_list_t&&);

  /*
    Templates that collect varargs list of options into opt_list_t list
    that can be passed to do_set().
  */

  static opt_list_t get_options()
  {
    return {};
  }

  /*
    Note: if we ever support options without values, another overload is
    needed: get_options(Option opt, Option opt1, R&... rest).
  */

  template <typename V, typename... Ty>
  static opt_list_t get_options(SOption opt, V&& val, Ty&&... rest)
  {
    Option oo = (Option)opt;
    opt_list_t opts = get_options(std::forward<Ty>(rest)...);
    opts.emplace_front(oo, opt_val(oo, std::forward<V>(val)));
    return std::move(opts);
  }

  /*
    Note: Methods below rely on the fact that DevAPI SessionOption constants
    have the same numeric values as common::Settings_impl::Option ones.
  */

  void erase(SOption opt)
  {
    Settings_impl::erase((Option)opt);
  }

  bool has_option(SOption opt)
  {
    return Settings_impl::has_option((Option)opt);
  }

  Value get(SOption opt)
  {
    return Settings_impl::get((Option)opt);
  }
};


}  // internal namespace
}  // mysqlx namespace

#endif
