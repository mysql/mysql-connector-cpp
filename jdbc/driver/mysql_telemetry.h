/*
 * Copyright (c) 2012, 2023, Oracle and/or its affiliates.
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


#ifndef _MYSQL_TELEMETRY_H_
#define _MYSQL_TELEMETRY_H_

#include <iostream>
#include <opentelemetry/trace/provider.h>
#include <string>

#include <cppconn/sqlstring.h>
#include <cppconn/version_info.h>
#include <vector>

template <typename T>
void logm(T&& msg)
{
  std::cerr << msg << std::endl;
}

template <typename T, typename... TT>
void logm(T&& first, TT&&... rest)
{
  std::cerr << first << " ";
  logm(std::forward<TT>(rest)...);
}



namespace sql
{
namespace mysql
{

  class MySQL_Connection;
  class MySQL_Statement;
  
  namespace telemetry
  {
    namespace nostd      = opentelemetry::nostd;
    namespace trace      = opentelemetry::trace;

    using Span_ptr = nostd::shared_ptr<trace::Span>;
    
    Span_ptr mk_span(MySQL_Statement*);
    Span_ptr mk_span(MySQL_Connection*);

    // Set error status for the given span and clear the pointer.

    void set_error(Span_ptr&, std::string);

  } /* namespace telemetry */

} /* namespace mysql */
} /* namespace sql */

#endif /*_MYSQL_URI_H_*/
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

