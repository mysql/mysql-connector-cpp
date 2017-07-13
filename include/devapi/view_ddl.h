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

#ifndef MYSQL_DEVAPI_VIEW_DDL_H
#define MYSQL_DEVAPI_VIEW_DDL_H

#ifndef __cplusplus
#error This header can be only used with C++ code
#endif


#include "common.h"
#include "detail/view.h"


namespace mysqlx {

/**
  Check options for an updatable view.
  @see https://dev.mysql.com/doc/refman/en/view-check-option.html
*/

enum class CheckOption
{
  CASCADED, //!< cascaded
  LOCAL     //!< local
};

/**
  Algorithms used to process views.
  @see https://dev.mysql.com/doc/refman/en/view-algorithms.html
*/

enum class Algorithm
{
  UNDEFINED,  //!< undefined
  MERGE,      //!< merge
  TEMPTABLE   //!< temptable
};

/**
  View security settings.
  @see https://dev.mysql.com/doc/refman/en/stored-programs-security.html
*/

enum class SQLSecurity
{
  DEFINER,  //!< definer
  INVOKER   //!< invoker
};


namespace internal {

  /**
    Part of a class hierarchy enforcing correct order of fluent API calls for a
    view create or update operation.
  */

  template <class Op>
  class View_check_opt
    : public View_base<Op>
  {
    using Executable = View_base<Op>;

  public:

    /**
      Specify checks that are done upon insertion of rows into an updatable
      view.

      @see https://dev.mysql.com/doc/refman/en/view-check-option.html
    */

    Executable& withCheckOption(CheckOption option)
    {
      try {
        get_impl()->with_check_option(static_cast<unsigned int>(option));
        return *this;
      }
      CATCH_AND_WRAP
    }

  protected:

    using View_base<Op>::get_impl;

  };

  /// @copydoc View_check_opt

  template <class Op>
  class View_defined_as
    : public View_check_opt<Op>
  {
    using ViewCheckOpt = View_check_opt<Op>;

  public:

    ///@{
    // TODO: How to copy documentation here?
    ViewCheckOpt& definedAs(TableSelect&& table)
    {
      try {
        get_impl()->defined_as(std::move(table));
        return *this;
      }
      CATCH_AND_WRAP
    }

    /**
       Specify table select operation for which the view is created.

       @note In situations where select statement is modified after
       passing it to definedAs() method, later changes do not affect
       view definition which uses the state of the statement at the time
       of definedAs() call.
    */

    ViewCheckOpt& definedAs(const TableSelect& table)
    {
      try {
        TableSelect table_tmp(table);
        get_impl()->defined_as(std::move(table_tmp));
        return *this;
      }
      CATCH_AND_WRAP
    }

    ///@}

  protected:

    using View_check_opt<Op>::get_impl;

  };


  /// @copydoc View_check_opt

  template <class Op>
  class View_definer
    : public View_defined_as<Op>
  {
    using ViewDefinedAs = View_defined_as<Op>;

  public:

    /**
      Specify definer of a view.

      The definer is used to determine access rights for the view. It is specified
      as a valid MySQL account name of the form "user@host".

      @see https://dev.mysql.com/doc/refman/en/stored-programs-security.html
    */
    ViewDefinedAs& definer(const string &user)
    {
      try {
        get_impl()->definer(user);
        return *this;
      }
      CATCH_AND_WRAP
    }

  protected:

    using View_defined_as<Op>::get_impl;

  };


  /// @copydoc View_check_opt

  template <class Op>
  class View_security
    : public View_definer<Op>
  {
    using ViewDefiner = View_definer<Op>;

  public:

    /**
      Specify security characteristics of a view.

      @see https://dev.mysql.com/doc/refman/en/stored-programs-security.html
    */

    ViewDefiner& security(SQLSecurity sec)
    {
      try {
        get_impl()->security(static_cast<unsigned int>(sec));
        return *this;
      }
      CATCH_AND_WRAP
    }

  protected:

    using View_definer<Op>::get_impl;
  };


  /// @copydoc View_check_opt

  template <class Op>
  class View_algorithm
    : public View_security<Op>
  {
    using ViewSecurity = View_security<Op>;

  public:

    /**
      Specify algorithm used to process the view.

      @see https://dev.mysql.com/doc/refman/en/view-algorithms.html
    */

    ViewSecurity& algorithm(Algorithm alg)
    {
      try {
        get_impl()->algorithm(static_cast<unsigned int>(alg));
        return *this;
      }
      CATCH_AND_WRAP
    }

  protected:

    using View_security<Op>::get_impl;

  };


  /// @copydoc View_check_opt

  template <class Op>
  class View_columns
    : public View_algorithm<Op>
    , View_columns_detail
  {
    using ViewAlgorithm = View_algorithm<Op>;

  public:

    /**
      Define the column names of the created/altered View.
    */

    template<typename...T>
    ViewAlgorithm& columns(const T&... names)
    {
      try {
        add_columns(get_impl(), names...);
        return *this;
      }
      CATCH_AND_WRAP
    }

  protected:

    using View_algorithm<Op>::get_impl;

  };

}   // internal namespace


/**
  Represents an operation which creates a view.

  The query for which the view is created must be specified with
  `definedAs()` method. Other methods can specify different view creation
  options. When operation is fully specified, it can be executed with
  a call to `execute()`.
*/

class PUBLIC_API ViewCreate
  : public internal::View_columns<ViewCreate>
{

  ViewCreate(Schema &sch, const string& name, bool replace);

  /// @cond IGNORED
  friend Schema;
  /// @endcond
};


/**
  Represents an operation which modifies an existing view.

  ViewAlter operation must specify new query for the view with
  `definedAs()` method (it is not possible to change other characteristics
  of a view without changing its query).
*/

class PUBLIC_API ViewAlter
  : public internal::View_columns<ViewAlter>
{

  ViewAlter(Schema &sch, const string& name);

  /// @cond IGNORED
  friend Schema;
  /// @endcond
};


/**
  Represents an operation which drops a view.
*/

class PUBLIC_API ViewDrop
  : public Executable<Result, ViewDrop>
{
  using Executable = Executable<Result, ViewDrop>;

  ViewDrop(Schema &sch, const string& name);

public:

  /**
    Modify drop view operation so that it checks existence of the view
    before dropping it.
  */

  Executable& ifExists()
  {
    try {
      get_impl()->if_exists();
      return *this;
    }
    CATCH_AND_WRAP
  }

protected:

  using Impl = internal::View_drop_impl;

  Impl* get_impl()
  {
    return static_cast<Impl*>(Executable::get_impl());
  }

  /// @cond IGNORED
  friend Schema;
  /// @endcond
};


}  // mysqlx namespace

#endif
