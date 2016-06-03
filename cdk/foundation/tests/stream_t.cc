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

/**
  Unit tests for sdk::foundation::Socket class.
*/

#include "test.h"
#include <iostream>
#include <mysql/cdk/foundation/stream.h>
#include <mysql/cdk/foundation/error.h>

using namespace ::std;
using namespace cdk::foundation;

typedef test::Mem_stream<128> Stream;

/*
  Basic I/O test that uses in-memory stream Mem_stream.
*/

TEST(Foundation_stream, basic)
{
  Stream str;

  char text1[] = "Hello";
  char text2[] = "World!";
  char space   = ' ';

  // Construct buffer sequence consisting of text1, space, text2.

  buffers buf0((byte*)text2, sizeof(text2)-1);
  buffers buf1(bytes((byte*)&space,1), buf0);
  buffers buf2(bytes((byte*)text1, sizeof(text1)-1), buf1);

  // Write data to output stream

  cout <<"Writing..." <<endl;
  Stream::Write_op wr(str, buf2);
  wr.wait();
  str.flush();
  EXPECT_EQ(wr.get_result(), buf2.length());
  cout <<"Wrote " <<wr.get_result() <<" bytes" <<endl;

  // clear buffers and read data from input stream

  memset(text2, 0, sizeof(text2));
  space ='\0';

  cout <<"Reading..." <<endl;
  Stream::Read_op rd(str, buf1);
  rd.wait();

  EXPECT_EQ(rd.get_result(), buf1.length());
  cout <<"Read " <<rd.get_result() <<" bytes" <<endl;
  cout <<"space: " <<space <<endl;
  cout <<"text2: " <<text2 <<endl;

  str.close();

  // Read the reminder

  while (!str.eos())
  {
    Stream::Read_op rd2(str, buffers((byte*)&space,1));
    rd2.wait();
    cout <<"Got: " <<space <<endl;
  }

  cout <<"Done!" <<endl;
}


TEST(Foundation_stream, close)
{
  Stream str;

  byte buf[]= "testing";

  cout <<"Writing to stream..." <<endl;
  Stream::Write_op  write(str, buffers(buf, sizeof(buf)));
  write.wait();

  cout <<"Closing the stream..." <<endl;
  str.close();

  // output stream should be full after close ...

  EXPECT_TRUE(str.is_ended());

  // ... and it should not be possible to write to it ...

  try {
    cout <<"Writing to closed stream..." <<endl;
    Stream::Write_op write2(str, buffers(buf, sizeof(buf)));
    FAIL() <<"should not be possible to write to a closed stream";
  }
  catch (Error &e)
  {
    cout <<"Expected error: ";
    e.describe(cout);
    cout <<endl;
  }

  // Reading remainder of data from closed stream

  cout <<"Reminder: ";
  while (!str.eos())
  {
    byte c;
    Stream::Read_op read(str, buffers(&c,1));
    read.wait();
    cout <<(char)c;
  }

  cout <<endl;

  cout <<"Done!" <<endl;
}
