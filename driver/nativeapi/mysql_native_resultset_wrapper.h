/*
  Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

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

#ifndef _MYSQL_NativeResultsetWrapper_H_
#define _MYSQL_NativeResultsetWrapper_H_

#include <boost/shared_ptr.hpp>

#include "native_resultset_wrapper.h"

struct st_mysql_res;

namespace sql
{
namespace mysql
{

class MySQL_DebugLogger;

namespace NativeAPI
{
class IMySQLCAPI;


class MySQL_NativeResultsetWrapper : public NativeResultsetWrapper
{
public:
	MySQL_NativeResultsetWrapper(::st_mysql_res *, boost::shared_ptr<NativeAPI::IMySQLCAPI> &/*, boost::shared_ptr< MySQL_DebugLogger > & l*/);

	~MySQL_NativeResultsetWrapper();

	void data_seek(uint64_t);

	::st_mysql_field * fetch_field();

	::st_mysql_field * fetch_field_direct(unsigned int);

	unsigned long * fetch_lengths();

	char** fetch_row();

	unsigned int num_fields();

	uint64_t num_rows();

	//boost::shared_ptr<IMySQLCAPI> getApiHandle();

private:

	MySQL_NativeResultsetWrapper(){}
	//Also need to decide should it be copyable

	boost::shared_ptr< MySQL_DebugLogger > logger;

	boost::shared_ptr< NativeAPI::IMySQLCAPI > capi;

	::st_mysql_res * rs;
};

} /* namespace NativeAPI */
} /* namespace mysql	 */
} /* namespace sql	   */

#endif // _MYSQL_RESULTSET_DATA_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
