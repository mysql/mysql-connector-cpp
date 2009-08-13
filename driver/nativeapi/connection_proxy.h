/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _CONNECTION_PROXY_H_
#define _CONNECTION_PROXY_H_

#include <boost/noncopyable.hpp>
#include <config.h>

#include "../mysql_connection_options.h"


namespace sql
{
class SQLString;

namespace mysql
{

namespace NativeAPI
{
class Resultset_Proxy;
class Statement_Proxy;

class Connection_Proxy : public boost::noncopyable
{

public:

	virtual ~Connection_Proxy(){}


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

	virtual unsigned int field_count() = 0;

	virtual unsigned long get_client_version() = 0;

	virtual const ::sql::SQLString & get_server_info() = 0;

	virtual unsigned long get_server_version() = 0;

	virtual bool more_results() = 0;

	virtual int next_result() = 0;

	virtual int options(::sql::mysql::MySQL_Connection_Options, const void *) = 0;

	virtual int query(const SQLString &) = 0;

	/* virtual int real_query(const SQLString &, uint64_t) = 0;*/

	virtual bool rollback() = 0;

	virtual ::sql::SQLString sqlstate() = 0;

	virtual bool ssl_set(const ::sql::SQLString & key,
						const ::sql::SQLString & cert,
						const ::sql::SQLString & ca,
						const ::sql::SQLString & capath,
						const ::sql::SQLString & cipher) = 0;

	virtual Resultset_Proxy * store_result() = 0;

	virtual Resultset_Proxy * use_result() = 0;

	virtual Statement_Proxy & stmt_init() = 0;
};

Connection_Proxy * createConnectionProxy(const SQLString & clientFileName);

} /* namespace NativeAPI */
} /* namespace mysql */
} /* namespace sql */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
