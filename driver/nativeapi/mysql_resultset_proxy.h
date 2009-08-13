/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_RESULTSET_PROXY_H_
#define _MYSQL_RESULTSET_PROXY_H_

#include <boost/shared_ptr.hpp>

#include "resultset_proxy.h"

struct st_mysql_res;

namespace sql
{
namespace mysql
{

class MySQL_DebugLogger;

namespace NativeAPI
{
class IMySQLCAPI;


class MySQL_Resultset_Proxy : public Resultset_Proxy
{
public:
	MySQL_Resultset_Proxy(::st_mysql_res *, boost::shared_ptr<NativeAPI::IMySQLCAPI> &/*, boost::shared_ptr< MySQL_DebugLogger > & l*/);

	~MySQL_Resultset_Proxy();

	void data_seek(uint64_t);

	::st_mysql_field * fetch_field();

	::st_mysql_field * fetch_field_direct(unsigned int);

	unsigned long * fetch_lengths();

	char** fetch_row();

	unsigned int num_fields();

	uint64_t num_rows();

	//boost::shared_ptr<IMySQLCAPI> getApiHandle();

private:

	MySQL_Resultset_Proxy(){}
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
