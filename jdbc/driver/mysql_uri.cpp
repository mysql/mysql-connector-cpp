/*
 * Copyright (c) 2012, 2020, Oracle and/or its affiliates.
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
#include <cppconn/exception.h>

namespace sql
{
namespace mysql
{

static const char MYURI_SOCKET_PREFIX[]=	"unix://";
static const char MYURI_PIPE_PREFIX[]=		"pipe://";
static const char MYURI_TCP_PREFIX[]=		"tcp://";
static const char MYURI_MYSQL_PREFIX[]=		"mysql://";

static const char MYURI_HOST_BEGIN=		'[';
static const char MYURI_HOST_END=		']';

static const int DEFAULT_TCP_PORT=		3306;

/* {{{ MySQL_Uri::MySQL_Uri() -I- */
MySQL_Uri::MySQL_Uri()
  : schema	("")
  , default_port(DEFAULT_TCP_PORT)
{}
/* }}} */


MySQL_Uri::Host_data::Host_data()
  : name(util::LOCALHOST)
  , port(DEFAULT_TCP_PORT)
  , protocol(NativeAPI::PROTOCOL_SOCKET)
  , has_port(false)
{}

/* {{{ MySQL_Uri::Host() -I- */
const sql::SQLString & MySQL_Uri::Host_data::Host()
{
  static const sql::SQLString hostValue4Pipe(".");
  static const sql::SQLString hostValue4sock(util::LOCALHOST);

  switch (Protocol())
  {
  case NativeAPI::PROTOCOL_TCP:
    return name;
  case NativeAPI::PROTOCOL_PIPE:
    return hostValue4Pipe;
  case NativeAPI::PROTOCOL_SOCKET:
    return hostValue4sock;

  case NativeAPI::PROTOCOL_COUNT:
      throw sql::InvalidArgumentException("NativeAPI::PROTOCOL_COUNT shouldn't be used.");
  }

  throw sql::InvalidArgumentException("Unexpected protocol.");
}
/* }}} */


/* {{{ MySQL_Uri::SocketOrPipe() -I- */
const sql::SQLString & MySQL_Uri::Host_data::SocketOrPipe()
{
  if (tcpProtocol(*this))
  {
    static const sql::SQLString emptystr(util::EMPTYSTR);
    return emptystr;
  }

  return name;
}

/* {{{ MySQL_Uri::Port() -I- */
unsigned int MySQL_Uri::Host_data::Port()
{
  return port;
}
/* }}} */



/* {{{ tcpProtocol() -I- */
bool tcpProtocol(MySQL_Uri::Host_data& uri)
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
  MySQL_Uri::Host_data host;
  if (!str.compare(0, sizeof(MYURI_SOCKET_PREFIX) - 1, MYURI_SOCKET_PREFIX))
  {
    host.setSocket(str.substr(sizeof(MYURI_SOCKET_PREFIX) - 1, sql::SQLString::npos));
    uri.setHost(host);
    return true;
  }

  if (!str.compare(0, sizeof(MYURI_PIPE_PREFIX) - 1 , MYURI_PIPE_PREFIX))
  {
    host.setPipe(str.substr(sizeof(MYURI_PIPE_PREFIX) - 1, sql::SQLString::npos));
    uri.setHost(host);
    return true;
  }

  std::string hostname;

  size_t end_sep;

  /* i wonder how did it work with "- 1"*/
  if (!str.compare(0, sizeof(MYURI_TCP_PREFIX) - 1, MYURI_TCP_PREFIX))
  {
    hostname= str.substr(sizeof(MYURI_TCP_PREFIX) - 1, sql::SQLString::npos);
  }
  else if ( !str.compare(0, sizeof(MYURI_MYSQL_PREFIX) - 1, MYURI_MYSQL_PREFIX))
  {
    hostname= str.substr(sizeof(MYURI_MYSQL_PREFIX) - 1, sql::SQLString::npos);
  }
  else
  {
    /* allowing to have port and schema specified even w/out protocol
       specifier("tcp://") */
    hostname= str.c_str();
  }

  uri.clear();

  auto parse_host = [&uri, &host] (std::string hostname) -> bool
  {
    // host countains [::1]:123 or hostname:123
    size_t sep = 0;

    std::string name;
    unsigned int port = uri.DefaultPort();

    //reset host data
    host = MySQL_Uri::Host_data();

    if (hostname[0] == MYURI_HOST_BEGIN)
    {
      sep= hostname.find(MYURI_HOST_END);
      /* No closing ] after [*/
      if (sep == std::string::npos)
      {
        return false;
      }

      name = hostname.substr(1, sep-1);
      //sep points to next char after ]
      sep++;
    }
    else
    {
      sep = hostname.find(':',0);
      if (sep == std::string::npos)
      {
        name = hostname;
      }
      else
      {
        name = hostname.substr(0, sep);
      }
    }

    bool has_port = false;

    if(sep < hostname.size() && hostname[sep] == ':')
    {
      //port
      hostname = hostname.substr(sep+1);
      long int val = std::atol(hostname.c_str());

      /*
        Note: strtol() returns 0 either if the number is 0
        or conversion was not possible. We distinguish two cases
        by cheking if end pointer was updated.
      */

      if (val == 0 && hostname.length()==0)
        return false;

      if (val > 65535 || val < 0)
        return false;

      port = static_cast<unsigned int>(val);
      has_port = true;
    }
    if(has_port)
      host.setHost(name, port);
    else
      host.setHost(name);
    uri.addHost(host);
    return true;
  };

  do
  {
    end_sep = hostname.find_first_of(",/");

    if (!parse_host(hostname.substr(0, end_sep)))
    {
      return false;
    }
    if(end_sep != std::string::npos)
    {
      hostname = hostname.substr(hostname[end_sep] == '/' ? end_sep : end_sep+1);
    }
  }while(end_sep != std::string::npos && hostname[0] != '/');


  /* Looking where schema part begins */
  if (hostname[0] == '/')
  {
    if (hostname.length() > 1/*Slash*/)
    {
      uri.setSchema(hostname.substr(1));
    }
  }
  else
  {
    uri.setSchema("");
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

