/* Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   There are special exceptions to the terms and conditions of the GPL 
   as it is applied to this software. View the full text of the 
   exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this 
   software distribution.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _MYSQL_DEBUG_H_
#define _MYSQL_DEBUG_H_

#if CPPCONN_TRACE_ENABLED
#define CPP_ENTER(msg)			MySQL_DebugLogger * __l = this->logger? this->logger->get():NULL;(void)__l;\
								MySQL_DebugEnterEvent __this_func(__LINE__, __FILE__, msg, logger)
#define CPP_ENTER_WL(l, msg)	MySQL_DebugLogger * __l = (l)? (l)->get():NULL;(void)__l;\
								MySQL_DebugEnterEvent __this_func(__LINE__, __FILE__, msg, (l))
#define CPP_INFO(msg)		{if (__l) __l->log("INF", msg); }
#define CPP_INFO_FMT(...)	{if (__l) __l->log_va("INF", __VA_ARGS__); }
#define CPP_ERR(msg)		{if (__l) __l->log("ERR", msg); }
#define CPP_ERR_FMT(...)	{if (__l) __l->log_va("ERR", __VA_ARGS__); }
#else
#define CPP_ENTER(msg)
#define CPP_INFO(msg)
#define CPP_ERR(msg)
static inline void CPP_INFO_FMT(...) {}
static inline void CPP_ERR_FMT(...) {}
#endif

#include <string>
#include <stack>
#include "mysql_util.h"

namespace sql {
namespace mysql {

class MySQL_DebugEnterEvent; 


class MySQL_DebugLogger
{
	std::stack< const MySQL_DebugEnterEvent * > callStack;
	bool tracing;
public:
	MySQL_DebugLogger();
	virtual ~MySQL_DebugLogger();

	void disableTracing();

	void enableTracing();

	void enter(const MySQL_DebugEnterEvent * obj);

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
	sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * logger;
	
	MySQL_DebugEnterEvent(unsigned int l, const char * const f, const char * const func_name, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * logger_object);
	~MySQL_DebugEnterEvent();
};

}; /* namespace mysql */
}; /* namespace sql */

#endif /* _MYSQL_DEBUG_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
