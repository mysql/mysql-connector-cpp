#ifndef MYSQLX_DETAIL_VIEW_H
#define MYSQLX_DETAIL_VIEW_H

#include "../executable.h"

namespace mysqlx {

class TableSelect;

namespace internal {

  struct View_drop_impl
    : public Executable_impl
  {
    virtual void if_exists() = 0;
  };

  struct View_impl
    : public Executable_impl
  {
    virtual void add_columns(const string&) = 0;
    virtual void algorithm(unsigned) = 0;
    virtual void security(unsigned) = 0;
    virtual void definer(const string&) = 0;
    virtual void defined_as(TableSelect&&) = 0;
    virtual void with_check_option(unsigned) = 0;
  };

}  // internal namespace


namespace internal {


  template <class Op>
  class View_base
    : public Executable<Result, Op>
  {
  protected:

    using Executable = mysqlx::Executable<Result, Op>;
    using Impl = View_impl;

    Impl* get_impl()
    {
      return static_cast<Impl*>(Executable::get_impl());
    }
  };


  struct PUBLIC_API View_columns_detail
  {
  protected:

    using Impl = View_impl;
    using Args_prc = Args_processor<View_columns_detail,Impl*>;

    static void process_one(Impl *impl, const string &name)
    {
      impl->add_columns(name);
    }

    template <typename C>
    static
      void add_columns(View_impl *impl, const C& col)
    {
      for (auto el : col)
      {
        impl->add_columns(el);
      }
    }

    template <typename... T>
    static void add_columns(Impl *impl, const T&... columns)
    {
      Args_prc::process_args(impl, columns...);
    }

    friend Args_prc;
  };

}  // internal namespace

} // mysqlx namespace

#endif
