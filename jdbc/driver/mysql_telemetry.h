/*
 * Copyright (c) 2012, 2023, Oracle and/or its affiliates.
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


#ifndef _MYSQL_TELEMETRY_H_
#define _MYSQL_TELEMETRY_H_

#include <iostream>
#include <opentelemetry/trace/provider.h>
#include <string>

#include <cppconn/sqlstring.h>
#include <cppconn/version_info.h>
#include <vector>

namespace nostd      = opentelemetry::nostd;
namespace trace      = opentelemetry::trace;

typedef opentelemetry::nostd::shared_ptr<trace::TracerProvider> otel_provider;
typedef opentelemetry::nostd::shared_ptr<trace::Tracer> otel_tracer;
typedef opentelemetry::nostd::shared_ptr<trace::Span> otel_span;
typedef opentelemetry::nostd::shared_ptr<trace::Scope> otel_scope;

namespace sql
{
namespace mysql
{

bool check_process_otel_libs();

class MySQL_Telemetry
{
private:
  otel_provider provider;
  otel_tracer tracer;
  otel_span span;
  std::unique_ptr<trace::Scope> scope;
  sql::SQLString m_trace_id, m_span_id;

public:

enum class Status
{
  UNSET, OK, ERROR
};


MySQL_Telemetry(const sql::SQLString span_name, bool set_active = false) :
    provider(trace::Provider::GetTracerProvider()),
    tracer(provider.get()->GetTracer("MySQL Connector/C++", MYCPPCONN_DM_VERSION)),
    // TODO: StartSpan() can accept the spanKind option and the links to
    //       other spans.
    span(tracer.get()->StartSpan(span_name.asStdString())),
    scope((set_active ? new trace::Scope(span) : nullptr))
{
  char buf_trace[trace::TraceId::kSize * 2];
  char buf_span[trace::SpanId::kSize * 2];
  trace::SpanContext ctx = span->GetContext();
  ctx.trace_id().ToLowerBase16(buf_trace);
  ctx.span_id().ToLowerBase16(buf_span);
  span->SetAttribute("db.system", "mysql");
  m_trace_id = std::string(buf_trace, sizeof(buf_trace));
  m_span_id = std::string(buf_span, sizeof(buf_span));
}

static bool otel_libs_loaded() { return check_process_otel_libs(); }

void  set_attribute(const sql::SQLString& name, const sql::SQLString& val)
{
  span->SetAttribute(name.asStdString(), val.asStdString());
}

void  set_status(Status status, const sql::SQLString& descr)
{
  trace::StatusCode otel_code = trace::StatusCode::kUnset;
  switch (status)
  {
    case Status::ERROR:
      otel_code = trace::StatusCode::kError;
      break;
    case Status::OK:
      otel_code = trace::StatusCode::kOk;
      break;
    default:
      otel_code = trace::StatusCode::kUnset;
      break;
  }

  span->SetStatus(otel_code, descr.asStdString());
}

const sql::SQLString& get_trace_id() { return m_trace_id; }
const sql::SQLString& get_span_id() { return m_span_id; }

};


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

