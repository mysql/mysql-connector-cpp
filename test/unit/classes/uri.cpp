/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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



#include <cppconn/warning.h>

#include "uri.h"
#include <stdlib.h>

#include "driver/mysql_uri.h"

namespace testsuite
{
namespace classes
{

/*Overriding setUp and tearDown */
void uri::setUp()
{
  _uri.reset(new ::sql::mysql::MySQL_Uri());
}


void uri::tearDown()
{
}


void uri::tcp()
{
  logMsg("uri::tcp()");

  conn_string= "tcp://192.168.1.102:3306/t1";
  ::sql::mysql::parseUri(conn_string, *_uri);

  ASSERT(tcpProtocol(*_uri));

  ASSERT_EQUALS("192.168.1.102", _uri->Host());
  ASSERT_EQUALS(3306, _uri->Port());
  ASSERT_EQUALS("t1", _uri->Schema());

  conn_string= "tcp://192.168.1.101:3307";
  ::sql::mysql::parseUri(conn_string, *_uri);

  ASSERT(tcpProtocol(*_uri));

  ASSERT_EQUALS("192.168.1.101", _uri->Host());
  ASSERT_EQUALS(3307, _uri->Port());
  ASSERT_EQUALS("", _uri->Schema());

  conn_string= "192.168.1.102/t2";
  ::sql::mysql::parseUri(conn_string, *_uri);

  ASSERT(tcpProtocol(*_uri));

  ASSERT_EQUALS("192.168.1.102", _uri->Host());
  ASSERT_EQUALS(3306, _uri->Port());
  ASSERT_EQUALS("t2", _uri->Schema());


}


void uri::tcpIpV6()
{
  conn_string= "[2001:db8:0:f101::1]";
  ::sql::mysql::parseUri(conn_string, *_uri);

  ASSERT_EQUALS(::sql::mysql::NativeAPI::PROTOCOL_TCP, _uri->Protocol());

  ASSERT_EQUALS("2001:db8:0:f101::1", _uri->Host());
  ASSERT_EQUALS(3306, _uri->Port());
  ASSERT_EQUALS("", _uri->Schema());

  conn_string= "tcp://[2001:db8:0:f101::2]:3307/test";
  ::sql::mysql::parseUri(conn_string, *_uri);

  ASSERT_EQUALS(::sql::mysql::NativeAPI::PROTOCOL_TCP, _uri->Protocol());

  ASSERT_EQUALS("2001:db8:0:f101::2", _uri->Host());
  ASSERT_EQUALS(3307, _uri->Port());

  ASSERT_EQUALS("test", _uri->Schema());
}


void uri::socket()
{
  conn_string= "unix:///tmp/mysql.socket";
  ::sql::mysql::parseUri(conn_string, *_uri);

  ASSERT_EQUALS(::sql::mysql::NativeAPI::PROTOCOL_SOCKET, _uri->Protocol());

  ASSERT_EQUALS("localhost", _uri->Host());
  ASSERT_EQUALS("/tmp/mysql.socket", _uri->SocketOrPipe());
  // We do not care about port in this case
  //ASSERT_EQUALS(0, _uri->Port());
}


void uri::pipe()
{
  conn_string= "pipe://MySQL";
  ::sql::mysql::parseUri(conn_string, *_uri);

  ASSERT_EQUALS(::sql::mysql::NativeAPI::PROTOCOL_PIPE, _uri->Protocol());

  ASSERT_EQUALS(".", _uri->Host());
  ASSERT_EQUALS("MySQL", _uri->SocketOrPipe());
  // We do not care about port in this case
  //ASSERT_EQUALS(0, _uri->Port());
}

} /* namespace classes   */
} /* namespace testsuite */
