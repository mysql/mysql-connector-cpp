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



#ifndef _MYSQL_DEBUG_H_
#define _MYSQL_DEBUG_H_

#include <boost/shared_ptr.hpp>

/* _MSC_VER VC6.0=1200, VC7.0=1300, VC7.1=1310, VC8.0=1400 */
#if defined(_MSC_VER)
	#if _MSC_VER >=1400
		#define WE_HAVE_VARARGS_MACRO_SUPPORT
	#endif
#else
	#define WE_HAVE_VARARGS_MACRO_SUPPORT
#endif



#if defined(WE_HAVE_VARARGS_MACRO_SUPPORT) && (CPPCONN_TRACE_ENABLED || defined(SAL_DLLPRIVATE))
	#define CPP_ENTER(msg)			const boost::shared_ptr< MySQL_DebugLogger > __l = this->logger;(void)__l;\
									MySQL_DebugEnterEvent __this_func(__LINE__, __FILE__, msg, this->logger)
	#define CPP_ENTER_WL(l, msg)	const boost::shared_ptr< MySQL_DebugLogger > __l = (l);(void)__l;\
									MySQL_DebugEnterEvent __this_func(__LINE__, __FILE__, msg, (l))
	#define CPP_INFO(msg)		{if (__l) __l->log("INF", msg); }
	#define CPP_INFO_FMT(...)	{if (__l) __l->log_va("INF", __VA_ARGS__); }
	#define CPP_ERR(msg)		{if (__l) __l->log("ERR", msg); }
	#define CPP_ERR_FMT(...)	{if (__l) __l->log_va("ERR", __VA_ARGS__); }
#else
	#define CPP_ENTER(msg)
	#define CPP_ENTER_WL(l, msg)
	#define CPP_INFO(msg)
	#define CPP_ERR(msg)
	#define CPP_ENTER_WL(l, msg)
	static inline void CPP_INFO_FMT(...) {}
	static inline void CPP_ERR_FMT(...) {}
#endif

#include <stack>
#include "mysql_util.h"

namespace sql {
namespace mysql {

class MySQL_DebugEnterEvent;


class MySQL_DebugLogger
{
	std::stack< const MySQL_DebugEnterEvent * > callStack;
	int tracing;
	enum
	{
		NO_TRACE,
		NORMAL_TRACE
	};
public:
	MySQL_DebugLogger();
	virtual ~MySQL_DebugLogger();

	void disableTracing();

	void enableTracing();

	void enter(const MySQL_DebugEnterEvent * obj);

	bool isTracing();

	void leave(const MySQL_DebugEnterEvent * obj);

	void log(const char * const type, const char * const message);

	void log_va(const char * const type, const char * const format, ...);

private:
	/* Prevent use of these */
	MySQL_DebugLogger(const MySQL_DebugLogger &);
	void operator=(MySQL_DebugLogger &);
};


class MySQL_DebugEnterEvent
{
public:
	unsigned int line;
	const char * const file;
	const char * const func;
	const boost::shared_ptr< MySQL_DebugLogger > logger;

	MySQL_DebugEnterEvent(unsigned int l, const char * const f, const char * const func_name, const boost::shared_ptr< MySQL_DebugLogger > & logger_object);
	~MySQL_DebugEnterEvent();
};

} /* namespace mysql */
} /* namespace sql */

#endif /* _MYSQL_DEBUG_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
