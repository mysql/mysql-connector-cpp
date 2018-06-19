/*
 * Copyright (c) 2012, 2018, Oracle and/or its affiliates. All rights reserved.
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
#include "mysql_uri.h"
#include "mysql_util.h"
#include "exception.h"

namespace sql
{
namespace mysql
{

static const char MYURI_SOCKET_PREFIX[]=	"unix://";
static const char MYURI_PIPE_PREFIX[]=		"pipe://";
static const char MYURI_TCP_PREFIX[]=		"tcp://";

static const char MYURI_HOST_BEGIN=		'[';
static const char MYURI_HOST_END=		']';

static const int DEFAULT_TCP_PORT=		3306;

/* {{{ MySQL_Uri::MySQL_Uri() -I- */
MySQL_Uri::MySQL_Uri()
  :	protocol(NativeAPI::PROTOCOL_TCP),
    port	(DEFAULT_TCP_PORT),
    /* Perhaps `localhost` has to be default. so w/out any parameter
       driver will still connect? */
    host	(""),
    schema	("")
{}
/* }}} */


/* {{{ MySQL_Uri::Host() -I- */
const sql::SQLString & MySQL_Uri::Host()
{
  static const sql::SQLString hostValue4Pipe(".");
  static const sql::SQLString hostValue4sock(util::LOCALHOST);

  switch (Protocol())
  {
  case NativeAPI::PROTOCOL_TCP:
    return host;
  case NativeAPI::PROTOCOL_PIPE:
    return hostValue4Pipe;
  case NativeAPI::PROTOCOL_SOCKET:
    return hostValue4sock;

  case NativeAPI::PROTOCOL_COUNT:
      throw sql::InvalidArgumentException("NativeAPI::PROTOCOL_COUNT shouldn't be used.");
    break;
  }

  // throw smoething maybe?
  return host;
}
/* }}} */


/* {{{ MySQL_Uri::SocketOrPipe() -I- */
const sql::SQLString & MySQL_Uri::SocketOrPipe()
{
  if (tcpProtocol(*this))
  {
    static const sql::SQLString emptystr(util::EMPTYSTR);
    return emptystr;
  }

  return host;
}
/* }}} */


/* {{{ MySQL_Uri::setHost() -I- */
void MySQL_Uri::setHost(const sql::SQLString &h)
{
  setProtocol(NativeAPI::PROTOCOL_TCP);
  host= h.c_str();
}
/* }}} */


/* {{{ MySQL_Uri::setSocket() -I- */
void MySQL_Uri::setSocket(const sql::SQLString &s)
{
  setProtocol(NativeAPI::PROTOCOL_SOCKET);
  host= s.c_str();
}
/* }}} */


/* {{{ MySQL_Uri::setPipe() -I- */
void MySQL_Uri::setPipe(const sql::SQLString &p)
{
  setProtocol(NativeAPI::PROTOCOL_PIPE);
  host= p.c_str();
}
/* }}} */


/* {{{ MySQL_Uri::setPort() -I- */
void MySQL_Uri::setPort(unsigned int p)
{
  setProtocol(NativeAPI::PROTOCOL_TCP);
  port= p;
}


/* {{{ tcpProtocol() -I- */
bool tcpProtocol(MySQL_Uri& uri)
{
  return uri.Protocol() == NativeAPI::PROTOCOL_TCP;
}
/* }}} */


/* {{{ Parse_Uri() -I- */
/* URI formats tcp://[host]:port/schema
               unix://socket
               pipe://named_pipe
 */
bool parseUri(const sql::SQLString & str, MySQL_Uri& uri)
{
  if (!str.compare(0, sizeof(MYURI_SOCKET_PREFIX) - 1, MYURI_SOCKET_PREFIX))
  {
    uri.setSocket(str.substr(sizeof(MYURI_SOCKET_PREFIX) - 1, sql::SQLString::npos));

    return true;
  }

  if (!str.compare(0, sizeof(MYURI_PIPE_PREFIX) - 1 , MYURI_PIPE_PREFIX))
  {
    uri.setPipe(str.substr(sizeof(MYURI_PIPE_PREFIX) - 1, sql::SQLString::npos));

    return true;
  }

  sql::SQLString host;
  size_t start_sep, end_sep;

  /* i wonder how did it work with "- 1"*/
  if (!str.compare(0, sizeof(MYURI_TCP_PREFIX) - 1, MYURI_TCP_PREFIX) )
  {
    host= str.substr(sizeof(MYURI_TCP_PREFIX) - 1, sql::SQLString::npos);
  }
  else
  {
    /* allowing to have port and schema specified even w/out protocol
       specifier("tcp://") */
    host= str.c_str();
  }

  if (host[0] == MYURI_HOST_BEGIN)
  {
    end_sep= host.find(MYURI_HOST_END);

    /* No closing ] after [*/
    if (end_sep == sql::SQLString::npos)
    {
      return false;
    }

    uri.setHost(host.substr(1, end_sep - 1));
    /* Cutting host to continue w/ port and schema reading */
    host= host.substr(end_sep + 1);
  }

  /* Looking where schema part begins */
  start_sep = host.find('/');

  if (start_sep != sql::SQLString::npos)
  {
    if ((host.length() - start_sep) > 1/*Slash*/)
    {
      uri.setSchema(host.substr(start_sep + 1, host.length() - start_sep - 1));
    }

    host= host.substr(0, start_sep);
  }
  else
  {
    uri.setSchema("");
  }

  /* Looking where port part begins*/
  start_sep = host.find_last_of(':', sql::SQLString::npos);

  if (start_sep != sql::SQLString::npos)
  {
    uri.setPort(atoi(host.substr(start_sep + 1, sql::SQLString::npos).c_str()));
    host = host.substr(0, start_sep);
  }
  else
  {
    uri.setPort(DEFAULT_TCP_PORT);
  }

  /* If host was enclosed in [], it has been already set, and "host" variable is
     empty */
  if (host.length() > 0)
  {
    uri.setHost(host);
  }

  return true;
}


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

