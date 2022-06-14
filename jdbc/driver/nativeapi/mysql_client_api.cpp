/*
 * Copyright (c) 2009, 2019, Oracle and/or its affiliates. All rights reserved.
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



#include <map>
#include <boost/shared_ptr.hpp>
#include <cppconn/sqlstring.h>

#include "binding_config.h"

#ifdef MYSQLCLIENT_STATIC_BINDING
/* MySQL client library is linked */
# include "libmysql_static_proxy.h"
#else
/* MySQL client library will be dynamically loaded */
# include "libmysql_dynamic_proxy.h"
#endif

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

/* We need probably multi_index map by path and HMODULE/void * as we can load same library by different name */
static std::map< sql::SQLString, boost::shared_ptr<IMySQLCAPI> > wrapper;

boost::shared_ptr< IMySQLCAPI > getCApiHandle(const sql::SQLString & name)
{
#ifdef MYSQLCLIENT_STATIC_BINDING
  return LibmysqlStaticProxy::theInstance();
#else

  std::map< sql::SQLString, boost::shared_ptr< IMySQLCAPI > >::const_iterator cit;

  if ((cit = wrapper.find(name)) != wrapper.end()) {
    return cit->second;
  } else {
    boost::shared_ptr< IMySQLCAPI > newWrapper;

    newWrapper.reset(new LibmysqlDynamicProxy(name));
    wrapper[name] = newWrapper;

    return newWrapper;
  }
#endif
}

} /* namespace NativeAPI */
} /* namespace mysql */
} /* namespace sql */

//#include "binding_config.h"

#ifdef MYSQLCLIENT_STATIC_BINDING
# include "libmysql_static_proxy.cpp"
#else
# include "libmysql_dynamic_proxy.cpp"
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
