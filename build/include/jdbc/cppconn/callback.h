/*
 * Copyright (c) 2021, 2024, Oracle and/or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms, as
 * designated in a particular file or component or in included license
 * documentation. The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * https://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */



#ifndef _SQL_CALLBACK_H_
#define _SQL_CALLBACK_H_

#include "sqlstring.h"
#include <functional>

namespace sql
{

namespace mysql
{
class MySQL_Connection;
class MySQL_Driver;
}


/*
  A callback to be used with `Driver::setCallback()` to define reaction
  to the user action request during WebAuthn authentication handshake.

  The client library defines default reaction which prints message on stderr.
  This callback can be used to change it.

  Example usage:

    // Use lambda

    driver->setCallback(WebAuthn_Callback{[](SQLString msg){
      cerr << "User action request: " << msg << endl;
    }});

    // Disable default behavior (and do nothing upon action request)

    driver->setCallback(WebAuthn_Callback{});

    // Return to default behavior

    driver->setCallbacak(WebAuthn_Callback{nullptr});

    // User defined callback

    struct My_Callback : WebAuthn_Callback
    {
      void ActionRequested(SQLString) override;
    }
    cb;

    driver->setCallback(cb);
*/

class WebAuthn_Callback
{
  std::function<void(SQLString)> callback_func;

public:

  /*
    Create a callback that will call given lambda upon user action request.
  */

  WebAuthn_Callback(std::function<void(SQLString)>&& cb)
    : callback_func{std::move(cb)}
  {}

  /*
    Create an empty callback that will do nothing upon user action request.
    This disables the default callback defined by the client library.
  */

  WebAuthn_Callback()
    : callback_func{[](SQLString){}}
  {}

  /*
    Create a null callback. Setting such callback has the effect of using
    the default callback defined by the client library.
  */

  WebAuthn_Callback(std::nullptr_t)
  {}

  /*
    Derived class can override this method to react to user action request.
  */

  virtual void ActionRequested(sql::SQLString msg)
  {
    if (callback_func)
      callback_func(msg);
  }

  // Returns true if this callback is not null.

  operator bool() const
  {
    return (bool)callback_func;
  }

  void operator()(sql::SQLString msg)
  {
    ActionRequested(msg);
  }

  virtual ~WebAuthn_Callback() {}

};


/*
 * TODO: This class is not needed. Remove it when the decision to
 *       break ABI is made.
 *
 * Class that provides functionality allowing user code to set the
 * callback functions through inheriting, passing the callback as
 * constructor parameters or using lambdas.
 */

class Fido_Callback
{
  std::function<void(SQLString)> callback_func = nullptr;
  bool is_null = false;

public:

  /**
  * Constructor to set the callback as function or as lambda
  */
  Fido_Callback(std::function<void(SQLString)> cb) : callback_func(cb)
  {}

  Fido_Callback()
  {}

  /**
  * Constructor to reset the callback to default
  */
  Fido_Callback(std::nullptr_t) : is_null(true)
  {}

  /**
  * Override this message to receive Fido Action Requests
  */
  virtual void FidoActionRequested(sql::SQLString msg)
  {
    if (callback_func)
      callback_func(msg);
  }

  operator bool() const
  {
    return !is_null;
  }

  void operator()(sql::SQLString msg)
  {
    if (is_null)
      return;
    FidoActionRequested(msg);
  }

  virtual ~Fido_Callback() {}

  friend class mysql::MySQL_Connection;
  friend class mysql::MySQL_Driver;
};


} /* namespace sql */

#endif // _SQL_CONNECTION_H_
