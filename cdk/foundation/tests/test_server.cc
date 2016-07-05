/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * This code is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FLOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

/*
  Simple echo server for testing socket connections.

  Server listens on port given by PORT macro and accepts single
  connection. Then it reads a message from the connection and
  sends it back to the client.
*/

#include <mysql/cdk/foundation/socket.h>
#include <iostream>
#include <exception>

using namespace ::std;
using namespace ::cdk::foundation;

typedef Socket::Connection   Connection;
typedef Socket::Connection::Read_some_op  Rd_op;
typedef Socket::Connection::Write_op      Wr_op;

int main()
{
  try
  {
    Socket sock(PORT);

    cout <<"Waiting for connection on port " <<PORT <<" ..." <<endl;

    Connection conn(sock);
    conn.wait();

    cout <<"Connected, waiting for data ..." <<endl;

    char input[128];

    Rd_op read(conn, buffers((byte*)input,sizeof(input)-1));
    read.wait();
    size_t howmuch= read.get_result();
    input[howmuch]= '\0';

    cout <<"Received " <<howmuch <<" bytes: " <<input <<endl;

    cout <<"Sending back ..." <<endl;

    Wr_op write(conn, buffers((byte*)input, howmuch+1));
    write.wait();

    cout <<"Done!" <<endl;
  }
  catch (std::exception& e)
  {
    cout <<"Test server exit with exception: " <<e.what() <<endl;
  }
  catch (...)
  {
    cout <<"Test server exit with unknown exception." <<endl;
  }
}
