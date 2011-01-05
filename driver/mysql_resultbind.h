/*
Copyright (c) 2008, 2011, Oracle and/or its affiliates. All rights reserved.

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



#ifndef _MYSQL_RESULTBIND_H_
#define _MYSQL_RESULTBIND_H_

#include <boost/scoped_array.hpp>

#include <cppconn/prepared_statement.h>
#include <cppconn/parameter_metadata.h>

#include "nativeapi/mysql_private_iface.h"
#include "mysql_util.h"

namespace sql
{
namespace mysql
{
namespace NativeAPI
{
class NativeStatementWrapper;
}


class MySQL_ResultBind
{
	unsigned int num_fields;
	boost::scoped_array< char > is_null;
	boost::scoped_array< char > err;
	boost::scoped_array< unsigned long > len;

	boost::shared_ptr< NativeAPI::NativeStatementWrapper > proxy;

	boost::shared_ptr< MySQL_DebugLogger > logger;

public:
	boost::scoped_array< MYSQL_BIND > rbind;


	MySQL_ResultBind( boost::shared_ptr< NativeAPI::NativeStatementWrapper > & _capi, boost::shared_ptr< MySQL_DebugLogger > & log);

	~MySQL_ResultBind();

	void bindResult();

};

} /* namespace mysql */
} /* namespace sql */

#endif /* _MYSQL_RESULTBIND_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

