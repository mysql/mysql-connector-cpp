/*
Copyright (c) 2011, 2012, Oracle and/or its affiliates. All rights reserved.

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


#ifndef _MYSQL_URI_H_
#define _MYSQL_URI_H_

#include "nativeapi/native_connection_wrapper.h"
#include <cppconn/sqlstring.h>

namespace sql
{
namespace mysql
{

class MySQL_Uri
{
private:
	NativeAPI::Protocol_Type	protocol;
	unsigned int				port;
	sql::SQLString				host;
	sql::SQLString				schema;
	
public:
	MySQL_Uri();

			const sql::SQLString &		Host();
			const sql::SQLString &		SocketOrPipe();
	inline	int							Port()		{return port;}
	inline	const sql::SQLString &		Schema()	{return schema;}
	inline	NativeAPI::Protocol_Type	Protocol()	{return protocol;}
	

			void setHost	(const sql::SQLString &h);
			void setSocket	(const sql::SQLString &s);
			void setPipe	(const sql::SQLString &p);
			void setPort	(unsigned int p);
	inline	void setSchema	(const sql::SQLString &s)	{schema= s.c_str();}

	inline	void setProtocol(NativeAPI::Protocol_Type p){protocol= p;}
};


bool tcpProtocol(MySQL_Uri& uri);


bool parseUri(const sql::SQLString & str, MySQL_Uri& uri);


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

