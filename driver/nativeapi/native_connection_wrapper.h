/*
Copyright (c) 2009, 2013, Oracle and/or its affiliates. All rights reserved.

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



#ifndef _NATIVE_CONNECTION_WRAPPER_H_
#define _NATIVE_CONNECTION_WRAPPER_H_

#include <boost/noncopyable.hpp>
#include <config.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include "../mysql_connection_options.h"


namespace sql
{
class SQLString;

namespace mysql
{

namespace NativeAPI
{
class NativeResultsetWrapper;
class NativeStatementWrapper;

enum Protocol_Type
{
	PROTOCOL_TCP,
	PROTOCOL_SOCKET,
	PROTOCOL_PIPE,
	/* Total number of supported protocol types */
	PROTOCOL_COUNT
};

class NativeConnectionWrapper : public boost::noncopyable
{
public:
	virtual ~NativeConnectionWrapper() {}


	virtual uint64_t affected_rows() = 0;

	virtual bool autocommit(bool) = 0;

	virtual bool connect(const ::sql::SQLString & host,
						const ::sql::SQLString & user,
						const ::sql::SQLString & passwd,
						const ::sql::SQLString & db,
						unsigned int			 port,
						const ::sql::SQLString & socket_or_pipe,
						unsigned long			client_flag) = 0;

	virtual bool commit() = 0;

	virtual void debug(const ::sql::SQLString &) = 0;

	virtual unsigned int errNo() = 0;

	virtual ::sql::SQLString error() = 0;

	virtual ::sql::SQLString escapeString(const ::sql::SQLString &) = 0;

	virtual unsigned int field_count() = 0;

	virtual unsigned long get_client_version() = 0;

	virtual const ::sql::SQLString & get_server_info() = 0;

	virtual unsigned long get_server_version() = 0;

	virtual bool more_results() = 0;

	virtual int next_result() = 0;

	virtual int options(::sql::mysql::MySQL_Connection_Options, const void *) = 0;
	virtual int options(::sql::mysql::MySQL_Connection_Options,
						const ::sql::SQLString &) = 0;
	virtual int options(::sql::mysql::MySQL_Connection_Options,
						const bool &) = 0;
	virtual int options(::sql::mysql::MySQL_Connection_Options,
						const int &) = 0;

	virtual int query(const SQLString &) = 0;

	virtual int ping() = 0;

	/* virtual int real_query(const SQLString &, uint64_t) = 0;*/

	virtual bool rollback() = 0;

	virtual ::sql::SQLString sqlstate() = 0;

	virtual ::sql::SQLString info() = 0;

	virtual bool ssl_set(const ::sql::SQLString & key,
						const ::sql::SQLString & cert,
						const ::sql::SQLString & ca,
						const ::sql::SQLString & capath,
						const ::sql::SQLString & cipher) = 0;

	virtual NativeResultsetWrapper * store_result() = 0;

	virtual int use_protocol(Protocol_Type) = 0;

	virtual NativeResultsetWrapper * use_result() = 0;

	virtual NativeStatementWrapper & stmt_init() = 0;

	virtual unsigned int warning_count() = 0;
};

} /* namespace NativeAPI */
} /* namespace mysql */
} /* namespace sql */
#endif /* _NATIVE_CONNECTION_WRAPPER_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
