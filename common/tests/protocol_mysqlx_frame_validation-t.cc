/*
 * Copyright (c) 2026, Oracle and/or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms, as
 * designated in a particular file or component or in included license
 * documentation. The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * https://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <gtest/gtest.h>

#include <mysql/cdk/foundation.h>
#include <mysql/cdk/protocol/mysqlx.h>

#include "protocol.h"

#include <sstream>
#include <vector>

using namespace cdk;
using namespace cdk::protocol::mysqlx;

namespace {

void append_frame_header(std::vector<byte> &frame, msg_size_t size,
                         msg_type_t type)
{
  HTONSIZE(size);
  const byte *size_bytes = reinterpret_cast<const byte*>(&size);
  frame.insert(frame.end(), size_bytes, size_bytes + sizeof(size));
  frame.push_back(static_cast<byte>(type));
}


std::string error_text(const cdk::Error &err)
{
  std::ostringstream out;
  out << err;
  return out.str();
}


class Rejecting_reply_processor : public Reply_processor
{
public:

  void ok(cdk::foundation::string) override
  {
    cdk::throw_error("Malformed frame was processed");
  }
};


template <size_t SIZE>
class Test_stream_adapter : public Protocol::Stream
{
  typedef foundation::test::Mem_stream<SIZE> Stream;
  typedef typename Stream::Read_op Read_op;
  typedef typename Stream::Write_op Write_op;

  Stream &m_conn;

public:

  Test_stream_adapter(Stream &conn)
    : m_conn(conn)
  {}

  Protocol::Stream::Op* read(const buffers &buf) override
  {
    return new Read_op(m_conn, buf);
  }

  Protocol::Stream::Op* write(const buffers &buf) override
  {
    return new Write_op(m_conn, buf);
  }
};


class Resize_test_protocol : public Protocol_impl
{
  typedef foundation::test::Mem_stream<1024> Stream;

  Stream m_conn;

public:

  Resize_test_protocol()
    : Protocol_impl(new Test_stream_adapter<1024>(m_conn), CLIENT)
  {}

  ~Resize_test_protocol() NOEXCEPT override
  {}

  bool resize_client_buffer(size_t requested_size)
  {
    return resize_buf(CLIENT, requested_size);
  }

  void set_pipelined_bytes(size_t size)
  {
    m_pipeline_size = size;
  }

  size_t write_buffer_size() const
  {
    return m_wr_size;
  }

  size_t remaining_write_size()
  {
    return wr_size();
  }
};


std::string receive_server_frame(std::vector<byte> &frame,
                                 Compression_type::value compression =
                                   Compression_type::NONE)
{
  typedef foundation::test::Mem_stream<1024> Stream;

  Stream conn;
  Protocol proto(conn);
  Rejecting_reply_processor processor;

  if (compression != Compression_type::NONE)
    proto.set_compression(compression, 0);

  Stream::Write_op write(conn, buffers(frame.data(), frame.size()));
  write.wait();
  conn.flush();

  try
  {
    proto.rcv_Reply(processor).wait();
  }
  catch (const cdk::Error &err)
  {
    return error_text(err);
  }

  return "";
}


std::vector<byte> make_compressed_server_frame(std::vector<byte> &inner,
                                               uint64_t uncompressed_size)
{
  Protocol_compression compressor;
  compressor.set_compression_type(Compression_type::DEFLATE);

  const size_t compressed_size =
    compressor.do_compress(inner.data(), inner.size());

  Mysqlx::Connection::Compression msg;
  msg.set_server_messages(
    static_cast<::Mysqlx::ServerMessages_Type>(msg_type::Ok));
  msg.set_uncompressed_size(uncompressed_size);
  msg.set_payload(compressor.get_out_buf(), compressed_size);

  std::string serialized;
  msg.SerializeToString(&serialized);

  std::vector<byte> frame;
  append_frame_header(frame, static_cast<msg_size_t>(serialized.size() + 1),
                      msg_type::Compression);
  frame.insert(frame.end(), serialized.begin(), serialized.end());
  return frame;
}

}  // namespace


TEST(Protocol_mysqlx_frame_validation, reject_zero_length_server_frame)
{
  std::vector<byte> frame;
  append_frame_header(frame, 0, msg_type::Ok);

  const std::string error = receive_server_frame(frame);

  EXPECT_NE(std::string::npos,
            error.find("Invalid X Protocol message size")) << error;
}


TEST(Protocol_mysqlx_frame_validation,
     resize_client_buffer_accounts_for_pipelined_bytes)
{
  Resize_test_protocol proto;
  const size_t queued_size = 800;
  const size_t requested_size = 400;

  proto.set_pipelined_bytes(queued_size);

  ASSERT_TRUE(proto.resize_client_buffer(requested_size));
  EXPECT_GE(proto.write_buffer_size(), queued_size + requested_size);
  EXPECT_GE(proto.remaining_write_size(), requested_size);
}


TEST(Protocol_mysqlx_frame_validation,
     reject_oversized_compressed_uncompressed_size)
{
  std::vector<byte> inner_frame;
  append_frame_header(inner_frame, 1, msg_type::Ok);

  std::vector<byte> frame = make_compressed_server_frame(
    inner_frame, static_cast<uint64_t>(max_msg_size) +
                 sizeof(msg_size_t) + 1);

  const std::string error =
    receive_server_frame(frame, Compression_type::DEFLATE);

  EXPECT_NE(std::string::npos,
            error.find("Invalid X Protocol message size")) << error;
}


TEST(Protocol_mysqlx_frame_validation,
     accept_max_compressed_uncompressed_size_metadata)
{
  std::vector<byte> inner_frame;
  append_frame_header(inner_frame, 1, msg_type::Ok);

  std::vector<byte> frame = make_compressed_server_frame(
    inner_frame, static_cast<uint64_t>(max_msg_size) + sizeof(msg_size_t));

  const std::string error =
    receive_server_frame(frame, Compression_type::DEFLATE);

  EXPECT_EQ(std::string::npos,
            error.find("Invalid X Protocol message size")) << error;
}


TEST(Protocol_mysqlx_frame_validation,
     reject_zero_length_compressed_inner_frame)
{
  std::vector<byte> inner_frame;
  append_frame_header(inner_frame, 0, msg_type::Ok);

  std::vector<byte> frame =
    make_compressed_server_frame(inner_frame, inner_frame.size());

  const std::string error =
    receive_server_frame(frame, Compression_type::DEFLATE);

  EXPECT_NE(std::string::npos,
            error.find("Invalid X Protocol message size")) << error;
}


TEST(Protocol_mysqlx_frame_validation,
     reject_oversized_compressed_inner_frame)
{
  std::vector<byte> inner_frame;
  append_frame_header(inner_frame, static_cast<msg_size_t>(max_msg_size + 1),
                      msg_type::Ok);

  std::vector<byte> frame =
    make_compressed_server_frame(inner_frame, inner_frame.size());

  const std::string error =
    receive_server_frame(frame, Compression_type::DEFLATE);

  EXPECT_NE(std::string::npos,
            error.find("Invalid X Protocol message size")) << error;
}
