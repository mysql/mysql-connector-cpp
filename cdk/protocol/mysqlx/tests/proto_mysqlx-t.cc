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

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>


#include <mysql/cdk.h>
#include <mysql/cdk/protocol/mysqlx.h>

#include <cdk_test.h>
#include <gtest/gtest.h>


using namespace cdk;
using namespace cdk::protocol::mysqlx;
//using namespace google::protobuf;

using std::cout;
using std::endl;
//using cdk::string;



TEST(Protocol_mysqlx, io_buffers)
{
  typedef foundation::test::Mem_stream<16*1024*1024> Stream;

  try {

    /*
      Note: objects of large size can not be allocated on stack.
      Hence we use heap allocated objects.
    */

    scoped_ptr<Stream> conn(new Stream());

    Protocol proto(*conn);
    Protocol_server srv(*conn);

    // Send large packet

    std::string buf;
    buf.reserve(12*1024*1024);

    bytes data((byte*)buf.data(), buf.capacity());

    cout <<"Sending AuthStart message with " <<buf.capacity()
         <<" bytes of auth data" <<endl;

    proto.snd_AuthenticateStart("test", data, bytes("")).wait();

    // read it from the other end

    struct : public Init_processor
    {
      size_t auth_size;

      void auth_start(const char *mech, bytes data, bytes resp)
      {
        cout <<"Got AuthStart message for " <<mech;
        cout <<" with " <<data.size() <<" bytes of auth data" <<endl;
        EXPECT_EQ(auth_size, data.size());
      }

      void auth_continue(bytes data)
      {}

    } m_iproc;

    m_iproc.auth_size= data.size();
    srv.rcv_InitMessage(m_iproc).wait();

    cout <<"Done!" <<endl;
  }
  CATCH_TEST_GENERIC;
}
