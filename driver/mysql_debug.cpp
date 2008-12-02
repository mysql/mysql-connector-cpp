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

#include <stdlib.h>
#include <stdio.h>
#include <memory>
#include <stdarg.h>
#include "mysql_debug.h"

#define NON_WANTED_FUNCTIONS 	!strstr(func, "Closed") \
								&& !strstr(func, "Valid") \
								&& !strstr(func, "getMySQLHandle") \
								&& !strstr(func, "isBeforeFirstOrAfterLast")


namespace sql
{
namespace mysql
{


/* {{{ MySQL_DebugEnterEvent::MySQL_DebugEnterEvent() -I- */
MySQL_DebugEnterEvent::MySQL_DebugEnterEvent(unsigned int l, const char * const f,
											 const char * const func_name,
											 sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * logger_object)
  : line(l), file(f), func(func_name), logger(logger_object? logger_object->getReference():NULL)
{
	if (logger) {
		if (NON_WANTED_FUNCTIONS) {
			logger->get()->enter(this);
		}
	}
}
/* }}} */ 


/* {{{ MySQL_DebugEnterEvent::MySQL_DebugEnterEvent() -I- */
MySQL_DebugEnterEvent::~MySQL_DebugEnterEvent()
{
	if (logger) {
		if (NON_WANTED_FUNCTIONS) {
			logger->get()->leave(this);
		}
		logger->freeReference();
	}
} 
/* }}} */ 


/* {{{ MySQL_DebugLogger::MySQL_DebugLogger() -I- */
MySQL_DebugLogger::MySQL_DebugLogger()
  : tracing(false)
{
}
/* }}} */ 


/* {{{ MySQL_DebugLogger::~MySQL_DebugLogger() -I- */
MySQL_DebugLogger::~MySQL_DebugLogger()
{
	callStack.empty();
}
/* }}} */ 


/* {{{ MySQL_DebugLogger::disableTracing() -I- */
void
MySQL_DebugLogger::disableTracing()
{
	tracing = false;
}
/* }}} */ 


/* {{{ MySQL_DebugLogger::enableTracing() -I- */
void
MySQL_DebugLogger::enableTracing()
{
	tracing = true;
}
/* }}} */ 


/* {{{ MySQL_DebugLogger::enter() -I- */
void
MySQL_DebugLogger::enter(const MySQL_DebugEnterEvent * event)
{
	if (tracing) {
		printf("\t");
		for (unsigned int i = 0; i < callStack.size(); i++) {
			printf("|  ");
		}
		printf(">%s\n", event->func);
	}
	callStack.push(event);
}
/* }}} */ 


/* {{{ MySQL_DebugLogger::leave() -I- */
void
MySQL_DebugLogger::leave(const MySQL_DebugEnterEvent * event)
{
	callStack.pop();
	if (tracing) {
		printf("\t");
		for (unsigned int i = 0; i < callStack.size(); i++) {
			printf("|  ");
		}
		printf("<%s\n", event->func);
	}
}
/* }}} */ 


/* {{{ MySQL_DebugLogger::log() -I- */
void
MySQL_DebugLogger::log(const char * const type, const char * const message)
{
	if (!tracing) {
		return;
	}
	printf("\t");
	for (unsigned int i = 0; i < callStack.size(); i++) {
		printf("|  ");
	}
	printf("%s: ", type);
	printf("%s\n", message);
}
/* }}} */ 


/* {{{ MySQL_DebugLogger::log_va() -I- */
void
MySQL_DebugLogger::log_va(const char * const type, const char * const format, ...)
{
	if (!tracing) {
		return;
	}
	va_list args;
	printf("\t");
	for (unsigned int i = 0; i < callStack.size(); i++) {
		printf("|  ");
	}
	printf("%s: ", type);
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\n");
}
/* }}} */ 

}; /* namespace mysql */
}; /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
