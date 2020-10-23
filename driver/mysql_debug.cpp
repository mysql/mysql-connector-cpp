/*
 * Copyright (c) 2008, 2020, Oracle and/or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */



#include <stdlib.h>
#include <stdio.h>
#include <memory>
#include <stdarg.h>
#include <string.h>
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
                       const boost::shared_ptr< MySQL_DebugLogger > & logger_object)
  : line(l), file(f), func(func_name), logger(logger_object)
{
  if (logger) {
    if (NON_WANTED_FUNCTIONS) {
      logger->enter(this);
    }
  }
}
/* }}} */


/* {{{ MySQL_DebugEnterEvent::MySQL_DebugEnterEvent() -I- */
MySQL_DebugEnterEvent::~MySQL_DebugEnterEvent()
{
  if (logger) {
    if (NON_WANTED_FUNCTIONS) {
      logger->leave(this);
    }
  }
}
/* }}} */


/* {{{ MySQL_DebugLogger::MySQL_DebugLogger() -I- */
MySQL_DebugLogger::MySQL_DebugLogger()
  : tracing(NO_TRACE)
{
  // ToDo: On Win getenv() is not thread-safe !
  if (getenv("MYSQLCPPCONN_TRACE_ENABLED")) {
    tracing = NORMAL_TRACE;
  }
}
/* }}} */


/* {{{ MySQL_DebugLogger::~MySQL_DebugLogger() -I- */
MySQL_DebugLogger::~MySQL_DebugLogger()
{
  while(!callStack.empty())
    callStack.pop();
}
/* }}} */


/* {{{ MySQL_DebugLogger::disableTracing() -I- */
void
MySQL_DebugLogger::disableTracing()
{
  tracing = NO_TRACE;
}
/* }}} */


/* {{{ MySQL_DebugLogger::enableTracing() -I- */
void
MySQL_DebugLogger::enableTracing()
{
  tracing = NORMAL_TRACE;
}
/* }}} */


/* {{{ MySQL_DebugLogger::enter() -I- */
void
MySQL_DebugLogger::enter(const MySQL_DebugEnterEvent * event)
{
  if (tracing != NO_TRACE) {
    printf("#\t");
    for (unsigned int i = 0; i < callStack.size(); ++i) {
      printf("|  ");
    }
    printf(">%s\n", event->func);
  }
  callStack.push(event);
}
/* }}} */


/* {{{ MySQL_DebugLogger::isTracing() -I- */
bool
MySQL_DebugLogger::isTracing()
{
  return (tracing != NO_TRACE);
}
/* }}} */


/* {{{ MySQL_DebugLogger::leave() -I- */
void
MySQL_DebugLogger::leave(const MySQL_DebugEnterEvent * event)
{
  callStack.pop();
  if (tracing != NO_TRACE) {
    printf("#\t");
    for (unsigned int i = 0; i < callStack.size(); ++i) {
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
  if (tracing == NO_TRACE) {
    return;
  }
  printf("#\t");
  for (unsigned int i = 0; i < callStack.size(); ++i) {
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
  if (tracing == NO_TRACE) {
    return;
  }
  va_list args;
  printf("#\t");
  for (unsigned int i = 0; i < callStack.size(); ++i) {
    printf("|  ");
  }
  printf("%s: ", type);
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
}
/* }}} */

} /* namespace mysql */
} /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
