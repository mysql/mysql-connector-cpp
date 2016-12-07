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


#include <mysql/cdk/codec.h>
#include "../parser/json_parser.h"

PUSH_SYS_WARNINGS
#include <algorithm>  // std::min
POP_SYS_WARNINGS

// Include Protobuf headers needed for decoding float numbers

PUSH_PB_WARNINGS
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
POP_PB_WARNINGS

#undef min

using namespace cdk;
using namespace parser;



size_t cdk::Codec<TYPE_BYTES>::from_bytes(bytes raw, std::string &str)
{
  str.assign(raw.begin(), raw.end());
  if (m_fmt.pad_width() != 0 && str.size() < m_fmt.pad_width())
    str.append((size_t)(m_fmt.pad_width() - str.size()), 0x00);
  return raw.size();
}


size_t cdk::Codec<TYPE_BYTES>::to_bytes(const std::string &str, bytes raw)
{
  size_t len = std::min(raw.size(), str.size());
  memcpy(raw.begin(), str.data(), len);
  return len;
}

size_t Codec<TYPE_STRING>::measure(const string &str)
{
  return get_codec().measure(str);
}


size_t Codec<TYPE_STRING>::from_bytes(bytes raw, string &str)
{
  //TODO: padding

  /*
    Note: xprotocol adds 0x00 byte at the end of bytes encoding
    a string to distinguisht the empty string from the null value.
    When decoding, we strip the extra 0x00 byte at the end, if present.
  */

  return get_codec().from_bytes(bytes(raw.begin(),
                                ( raw.size() > 0 && *(raw.end()-1) == '\0') ?
                                  raw.end()-1 :
                                  raw.end()),
                           str);
}


size_t Codec<TYPE_STRING>::to_bytes(const string& str, bytes raw)
{
  return get_codec().to_bytes(str, raw);
}


foundation::api::String_codec* Format<TYPE_STRING>::codec() const
{
  /*
    TODO: This implementation uses ASCII codec for all non utf8 strings,
    which works only for simple strings. Correctly handle all MySQL
    character encodings.
  */

  static foundation::String_codec<foundation::codecvt_utf8>  utf8;
  static foundation::String_codec<foundation::codecvt_ascii> ascii;

  return Charset::utf8 == charset() ?
      (foundation::api::String_codec*)&utf8
    : (foundation::api::String_codec*)&ascii;
}


/*
  Decoding/encoding Protobuf zig-zag number encoding.
*/

template <typename T, bool> struct zigzag;

// Specialization for signed types

template <typename T>
struct zigzag<T, true>
{
  static
  uint64_t encode(T val)
  {
    // ZigZagEncode accepts signed 64-bit integer
    // Note: cast to uint64_t to avoid signed/unsigned comparison

    if ((val > 0) && ((uint64_t)val > (uint64_t)std::numeric_limits<int64_t>::max()))
      throw_error(cdkerrc::conversion_error,
                  "Codec<TYPE_INTEGER>: conversion overflow");

    return google::protobuf::internal::WireFormatLite::ZigZagEncode64(
             static_cast<int64_t>(val)
           );
  }

  static
  T decode(uint64_t val)
  {
    int64_t tmp = google::protobuf::internal::WireFormatLite::ZigZagDecode64(val);

    /*
      Note: to avoid singed/unsigned comparison we cast to uint64_t or
      int64_t. Cast to int64_t happens only when tmp is <= 0 and thus it
      is correct even for 64-bit unsigned numbers.
    */

    if (((tmp > 0) && ((uint64_t)tmp > (uint64_t)std::numeric_limits<T>::max()))
        || ((int64_t)tmp < (int64_t)std::numeric_limits<T>::min()))
      throw_error(cdkerrc::conversion_error,
                  "Codec<TYPE_INTEGER>: conversion overflow");

    return static_cast<T>(tmp);
  }
};

// Specialization for unsigned types

template <typename T>
struct zigzag<T, false>
{
  static
  uint64_t encode(T val)
  {
    return static_cast<uint64_t>(val);
  }

  static
  T decode(uint64_t val)
  {
    if (val > (uint64_t)std::numeric_limits<T>::max())
      throw cdk::Error(cdkerrc::conversion_error,
        "Codec<TYPE_INTEGER>: conversion overflow");

    return static_cast<T>(val);
  }
};


template <typename T>
inline
uint64_t zigzag_encode_signed(T val)
{
  return zigzag<T, true>::encode(val);
}

template <typename T>
inline
uint64_t zigzag_encode_unsigned(T val)
{
  return zigzag<T, false>::encode(val);
}

template <typename T>
inline
T zigzag_decode_signed(uint64_t val)
{
  return zigzag<T, true>::decode(val);
}

template <typename T>
inline
T zigzag_decode_unsigned(uint64_t val)
{
  return zigzag<T, false>::decode(val);
}



template <typename T>
size_t Codec<TYPE_INTEGER>::internal_from_bytes(bytes buf, T &val)
{
  uint64_t val_tmp;

  assert(buf.size() < (size_t)std::numeric_limits<int>::max());

  google::protobuf::io::CodedInputStream input_buffer(buf.begin(), (int)buf.size());

  if (!input_buffer.ReadVarint64(&val_tmp))
  {
    throw Error(cdkerrc::conversion_error,
                "Codec<TYPE_INTEGER>: integer conversion error");
  }

  if (m_fmt.is_unsigned())
    val = zigzag_decode_unsigned<T>(val_tmp);
  else
    val = zigzag_decode_signed<T>(val_tmp);

  assert(input_buffer.CurrentPosition() >= 0);
  size_t sz = static_cast<size_t>(input_buffer.CurrentPosition());

  return sz;
}


size_t Codec<TYPE_INTEGER>::from_bytes(bytes buf, int8_t &val)
{
  return internal_from_bytes(buf, val);
}


size_t Codec<TYPE_INTEGER>::from_bytes(bytes buf, int16_t &val)
{
  return internal_from_bytes(buf, val);
}


size_t Codec<TYPE_INTEGER>::from_bytes(bytes buf, int32_t &val)
{
  return internal_from_bytes(buf, val);
}


size_t Codec<TYPE_INTEGER>::from_bytes(bytes buf, int64_t &val)
{
  return internal_from_bytes(buf, val);
}


size_t Codec<TYPE_INTEGER>::from_bytes(bytes buf, uint8_t &val)
{
  return internal_from_bytes(buf, val);
}


size_t Codec<TYPE_INTEGER>::from_bytes(bytes buf, uint16_t &val)
{
  return internal_from_bytes(buf, val);
}


size_t Codec<TYPE_INTEGER>::from_bytes(bytes buf, uint32_t &val)
{
  return internal_from_bytes(buf, val);
}


size_t Codec<TYPE_INTEGER>::from_bytes(bytes buf, uint64_t &val)
{
  return internal_from_bytes(buf, val);
}


template <typename T>
size_t Codec<TYPE_INTEGER>::internal_to_bytes(T val, bytes buf)
{
  assert(buf.size() < (size_t)std::numeric_limits<int>::max());
  google::protobuf::io::ArrayOutputStream buffer(buf.begin(), (int)buf.size());
  google::protobuf::io::CodedOutputStream output_buffer(&buffer);

  uint64_t val_tmp;

  if (m_fmt.is_unsigned())
    val_tmp = zigzag_encode_unsigned(val);
  else
    val_tmp = zigzag_encode_signed(val);

  output_buffer.WriteVarint64(val_tmp);

  if (output_buffer.HadError())
    throw Error(cdkerrc::conversion_error,
                "Codec<TYPE_INTEGER>: buffer to small");

  return static_cast<size_t>(output_buffer.ByteCount());
}


size_t Codec<TYPE_INTEGER>::to_bytes(int8_t val, bytes buf)
{
  return internal_to_bytes(val, buf);
}


size_t Codec<TYPE_INTEGER>::to_bytes(int16_t val, bytes buf)
{
  return internal_to_bytes(val, buf);
}


size_t Codec<TYPE_INTEGER>::to_bytes(int32_t val, bytes buf)
{
  return internal_to_bytes(val, buf);
}


size_t Codec<TYPE_INTEGER>::to_bytes(int64_t val, bytes buf)
{
  return internal_to_bytes(val, buf);
}


size_t Codec<TYPE_INTEGER>::to_bytes(uint8_t val, bytes buf)
{
  return internal_to_bytes(val, buf);
}


size_t Codec<TYPE_INTEGER>::to_bytes(uint16_t val, bytes buf)
{
  return internal_to_bytes(val, buf);
}


size_t Codec<TYPE_INTEGER>::to_bytes(uint32_t val, bytes buf)
{
  return internal_to_bytes(val, buf);
}


size_t Codec<TYPE_INTEGER>::to_bytes(uint64_t val, bytes buf)
{
  return internal_to_bytes(val, buf);
}


size_t Codec<TYPE_FLOAT>::from_bytes(bytes buf, float &val)
{
  if (m_fmt.type() == cdk::Format<cdk::TYPE_FLOAT>::DECIMAL)
    THROW("Codec<TYPE_FOAT>: DECIMAL format not supported yet");

  if (m_fmt.type() == cdk::Format<cdk::TYPE_FLOAT>::DOUBLE)
    throw Error(cdkerrc::conversion_error,
                "Codec<TYPE_FLOAT>: can not store DOUBLE value"
                " into float variable");

  uint32_t val_tmp;
  size_t sz = m_cvt.from_bytes(buf, val_tmp);

  if (sz < buf.size())
    throw Error(cdkerrc::conversion_error,
                "Codec<TYPE_FLOAT>: convertion overflow");

  val = google::protobuf::internal::WireFormatLite::DecodeFloat(val_tmp);
  return sz;
}


size_t Codec<TYPE_FLOAT>::from_bytes(bytes buf, double &val)
{
  if (m_fmt.type() == cdk::Format<cdk::TYPE_FLOAT>::DECIMAL)
    THROW("Codec<TYPE_FOAT>: DECIMAL format not supported yet");

  size_t sz;

  if (m_fmt.type() == cdk::Format<cdk::TYPE_FLOAT>::FLOAT)
  {
    float val_tmp;
    sz = from_bytes(buf, val_tmp);
    val = val_tmp;
    return sz;
  }

  uint64_t val_tmp;
  sz = m_cvt.from_bytes(buf, val_tmp);

  if (sz < buf.size())
    throw Error(cdkerrc::conversion_error,
                "Codec<TYPE_FLOAT>: convertion overflow");

  val = google::protobuf::internal::WireFormatLite::DecodeDouble(val_tmp);
  return sz;
}


size_t Codec<TYPE_FLOAT>::to_bytes(float val, bytes buf)
{
  if (buf.size() < sizeof(float))
    throw Error(cdkerrc::conversion_error,
                "Codec<TYPE_FLOAT>: buffer too small");
  return m_cvt.to_bytes(google::protobuf::internal::WireFormatLite::EncodeFloat(val), buf);
}


size_t Codec<TYPE_FLOAT>::to_bytes(double val, bytes buf)
{
  if (buf.size() < sizeof(double))
    throw Error(cdkerrc::conversion_error,
                "Codec<TYPE_FLOAT>: buffer too small");
  return m_cvt.to_bytes(google::protobuf::internal::WireFormatLite::EncodeDouble(val), buf);
}


size_t Codec<TYPE_DOCUMENT>::from_bytes(bytes data, JSON::Processor &jp)
{
  std::string json_string(data.begin(), data.end());
  JSON_parser parser(json_string);
  parser.process(jp);
  return 0; // FIXME
}

Codec<TYPE_DOCUMENT>::Doc_format Codec<TYPE_DOCUMENT>::m_format;

