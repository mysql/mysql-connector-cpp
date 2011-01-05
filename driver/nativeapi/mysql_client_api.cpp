/*
Copyright (c) 2009, 2011, Oracle and/or its affiliates. All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPLv2
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPLv2 as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
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

#include "binding_config.h"

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
