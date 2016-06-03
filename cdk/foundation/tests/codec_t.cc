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

#include "test.h"
#include <mysql/cdk/foundation/codec.h>

using namespace ::std;
using namespace ::cdk::foundation;

/*
  String Codec
  ============
*/


// Samples prepared using: http://www.endmemo.com/unicode/unicodeconverter.php
// and: http://www.columbia.edu/~kermit/utf8.html
// ("I can eat glass" phrase in different languages)

#define SAMPLES(X) \
  X (english, L"I can eat glass", "I can eat glass") \
  X (polish, L"Mog\u0119 je\u015B\u0107 szk\u0142o", \
     "\x4D\x6F\x67\xC4\x99\x20\x6A\x65\xC5\x9B\xC4\x87\x20\x73\x7A\x6B\xC5\x82\x6F") \
  X (japaneese, L"\u79C1\u306F\u30AC\u30E9\u30B9\u3092\u98DF\u3079\u3089\u308C\u307E\u3059\u3002\u305D\u308C\u306F\u79C1\u3092\u50B7\u3064\u3051\u307E\u305B\u3093\u3002", \
     "\xE7\xA7\x81\xE3\x81\xAF\xE3\x82\xAC\xE3\x83\xA9\xE3\x82\xB9\xE3\x82\x92\xE9\xA3\x9F\xE3\x81\xB9\xE3\x82\x89\xE3\x82\x8C\xE3\x81\xBE\xE3\x81\x99\xE3\x80\x82\xE3\x81\x9D\xE3\x82\x8C\xE3\x81\xAF\xE7\xA7\x81\xE3\x82\x92\xE5\x82\xB7\xE3\x81\xA4\xE3\x81\x91\xE3\x81\xBE\xE3\x81\x9B\xE3\x82\x93\xE3\x80\x82") \
  X (ukrainian, L"\u042F \u043C\u043E\u0436\u0443 \u0457\u0441\u0442\u0438 \u0441\u043A\u043B\u043E, \u0456 \u0432\u043E\u043D\u043E \u043C\u0435\u043D\u0456 \u043D\u0435 \u0437\u0430\u0448\u043A\u043E\u0434\u0438\u0442\u044C", \
     "\xD0\xAF\x20\xD0\xBC\xD0\xBE\xD0\xB6\xD1\x83\x20\xD1\x97\xD1\x81\xD1\x82\xD0\xB8\x20\xD1\x81\xD0\xBA\xD0\xBB\xD0\xBE\x2C\x20\xD1\x96\x20\xD0\xB2\xD0\xBE\xD0\xBD\xD0\xBE\x20\xD0\xBC\xD0\xB5\xD0\xBD\xD1\x96\x20\xD0\xBD\xD0\xB5\x20\xD0\xB7\xD0\xB0\xD1\x88\xD0\xBA\xD0\xBE\xD0\xB4\xD0\xB8\xD1\x82\xD1\x8C") \
  X (portuguese, L"Posso comer vidro, n\u00E3o me faz mal", \
     "\x50\x6F\x73\x73\x6F\x20\x63\x6F\x6D\x65\x72\x20\x76\x69\x64\x72\x6F\x2C\x20\x6E\xC3\xA3\x6F\x20\x6D\x65\x20\x66\x61\x7A\x20\x6D\x61\x6C")

TEST(Foundation, string)
{
  using cdk::foundation::string;

  unsigned int sample_count = 0;

#define SAMPLE_COUNT(X,Y,Z) ++sample_count;

  SAMPLES(SAMPLE_COUNT)

#define SAMPLE_WIDE(X,Y,Z) Y,
#define SAMPLE_UTF8(X,Y,Z) Z,

  string sample[] = {
    SAMPLES(SAMPLE_WIDE)
  };

  std::string utf8[] = {
    SAMPLES(SAMPLE_UTF8)
  };

  Codec<Type::STRING> codec;

  byte buf[128];

  for (unsigned i=0; i < sample_count; ++i)
  {
    cout <<"checking sample " <<i <<endl;

    string  wide = sample[i];
    std::string narrow = utf8[i];

    size_t len = codec.to_bytes(wide, bytes(buf, sizeof(buf)));

    EXPECT_EQ(len, narrow.length());
    EXPECT_EQ(narrow, std::string(buf, buf+len));

    string back;
    size_t len1 = codec.from_bytes(bytes(buf, len), back);

    EXPECT_EQ(len, len1);
    EXPECT_EQ(wide, back);

    EXPECT_EQ(narrow, (std::string)wide);
    EXPECT_EQ(wide, string(narrow));
  }
}


/*
  Number Codecs
  =============
*/

/*
  Structure to hold test data consisting of byte sequence and
  numeric values when this sequence is treated as little endian
  or big endian integer.
*/

template <size_t S>
struct test_data
{
  typedef typename num_type<S,false>::type uint;
  typedef typename num_type<S,true>::type sint;

  byte buf[S];
  uint le;  // little endian value
  uint be;  // big endian value

  // array with test data
  static test_data samples[];
};


/*
  Perform tests for count entries in test_data<S>::samples[] array.
*/

template<size_t S>
void do_test(unsigned count)
{
  typedef test_data<S> test_data;
  typedef typename test_data::uint uint_t;
  typedef typename test_data::sint sint_t;

  cout <<"== Testing " <<8*S <<"bit integers ==" <<endl;

  Number_codec<Endianess::BIG>    codec_big;
  Number_codec<Endianess::LITTLE> codec_little;

  for (unsigned pos=0; pos < count; ++pos)
  {
    test_data &sample = test_data::samples[pos];

    cout <<endl <<"= sample# " <<pos <<" =" <<endl;
    cout <<"bytes: " <<::testing::PrintToString(sample.buf) <<endl;

    // Conversion from bytes to number

    uint_t uval;
    sint_t sval;

    codec_little.from_bytes(bytes(sample.buf,S), uval);
    cout <<"little unsigned: " <<uval <<endl;
    EXPECT_EQ(sample.le, uval);

    codec_little.from_bytes(bytes(sample.buf,S), sval);
    cout <<"  little signed: " <<sval <<endl;
    uval= (uint_t)sval;
    EXPECT_EQ(sample.le, uval);

    codec_big.from_bytes(bytes(sample.buf,S), uval);
    cout <<"   big unsigned: " <<uval <<endl;
    EXPECT_EQ(sample.be, uval);

    codec_big.from_bytes(bytes(sample.buf,S), sval);
    cout <<"     big signed: " <<sval <<endl;
    uval= (uint_t)sval;
    EXPECT_EQ(sample.be, uval);

    cout <<"- conversion to 64-bit integer value" <<endl;

    uint64_t uval1;
    int64_t  sval1;

    codec_little.from_bytes(bytes(sample.buf,S), uval1);
    cout <<"little unsigned: " <<uval1 <<endl;
    EXPECT_EQ(sample.le, uval1);

    codec_little.from_bytes(bytes(sample.buf,S), sval1);
    cout <<"  little signed: " <<sval1 <<endl;
    uval1= (uint_t)sval1;
    EXPECT_EQ(sample.le, uval1);

    codec_big.from_bytes(bytes(sample.buf,S), uval1);
    cout <<"   big unsigned: " <<uval1 <<endl;
    EXPECT_EQ(sample.be, uval1);

    codec_big.from_bytes(bytes(sample.buf,S), sval1);
    cout <<"     big signed: " <<sval1 <<endl;
    uval1= (uint_t)sval1;
    EXPECT_EQ(sample.be, uval1);

    // Conversion from number to bytes

#define ARRAY_EQ(A,B) \
    for (unsigned i=0; i<S; ++i) EXPECT_EQ(A[i],B[i]);

    byte buf[S];

    memset(buf, 0, S);
    codec_little.to_bytes(sample.le, bytes(buf,S));
    ARRAY_EQ(sample.buf, buf);

    memset(buf, 0, S);
    codec_little.to_bytes((sint_t)sample.le, bytes(buf,S));
    ARRAY_EQ(sample.buf, buf);

    memset(buf, 0, S);
    codec_big.to_bytes(sample.be, bytes(buf,S));
    ARRAY_EQ(sample.buf, buf);

    memset(buf, 0, S);
    codec_big.to_bytes((sint_t)sample.be, bytes(buf,S));
    ARRAY_EQ(sample.buf, buf);
  }

  cout <<endl;
}


/*
  Standard C++ does not have literals for 64-bit numbers (LL suffix is
  introduced in C++11). For that reason we split test values into two
  32-bit numbers. Function do_test<8>() has slightly different code which
  takes this into account.
*/

template <>
struct test_data<8>
{

  byte buf[8];
  uint32_t le_lo, le_hi;  // little endian value
  uint32_t be_lo, be_hi;  // big endian value

  static test_data samples[];
};


template<>
void do_test<8>(unsigned count)
{
  typedef test_data<8> test_data;
  typedef uint64_t uint_t;
  typedef int64_t  sint_t;

  cout <<"== Testing " <<64 <<"bit integers ==" <<endl;

  Number_codec<Endianess::BIG>    codec_big;
  Number_codec<Endianess::LITTLE> codec_little;

  for (unsigned pos=0; pos < count; ++pos)
  {
    test_data &sample = test_data::samples[pos];

    cout <<endl <<"= sample# " <<pos <<" =" <<endl;
    cout <<"bytes: " <<::testing::PrintToString(sample.buf) <<endl;

    // Conversion from bytes to number

    uint_t uval;
    sint_t sval;

    codec_little.from_bytes(bytes(sample.buf,8), uval);
    cout <<"little unsigned: " <<uval <<endl;
    EXPECT_EQ(sample.le_lo, uval & 0xFFFFFFFFU);
    EXPECT_EQ(sample.le_hi, uval >>32);

    codec_little.from_bytes(bytes(sample.buf,8), sval);
    cout <<"  little signed: " <<sval <<endl;
    uval= (uint_t)sval;
    EXPECT_EQ(sample.le_lo, uval & 0xFFFFFFFFU);
    EXPECT_EQ(sample.le_hi, uval >>32);

    codec_big.from_bytes(bytes(sample.buf,8), uval);
    cout <<"   big unsigned: " <<uval <<endl;
    EXPECT_EQ(sample.be_lo, uval & 0xFFFFFFFFU);
    EXPECT_EQ(sample.be_hi, uval >>32);

    codec_big.from_bytes(bytes(sample.buf,8), sval);
    cout <<"     big signed: " <<sval <<endl;
    uval= (uint_t)sval;
    EXPECT_EQ(sample.be_lo, uval & 0xFFFFFFFFU);
    EXPECT_EQ(sample.be_hi, uval >>32);

    // Conversion from number to bytes

#define ARRAY_EQ8(A,B) \
    for (unsigned i=0; i<8; ++i) EXPECT_EQ(A[i],B[i]);

    byte buf[8];

    uval = sample.le_hi;
    uval = (uval<<32) + sample.le_lo;
    sval = (sint_t)uval;
    codec_little.to_bytes(uval, bytes(buf,8));
    ARRAY_EQ8(sample.buf, buf);
    codec_little.to_bytes(sval, bytes(buf,8));
    ARRAY_EQ8(sample.buf, buf);

    uval = sample.be_hi;
    uval = (uval<<32) + sample.be_lo;
    sval = (sint_t)uval;
    codec_big.to_bytes(uval, bytes(buf,8));
    ARRAY_EQ8(sample.buf, buf);
    codec_big.to_bytes(sval, bytes(buf,8));
    ARRAY_EQ8(sample.buf, buf);
  }

  cout <<endl;
}


template<>
test_data<1> test_data<1>::samples[] =
{
  { {0x9c}, 0x9c, 0x9c },
};

template<>
test_data<2> test_data<2>::samples[] =
{
  { {0x9C,0x00}, 0x009C, 0x9C00 },
  { {0x00,0x9C}, 0x9C00, 0x009C },
  { {0x9C,0xFF}, 0xFF9C, 0x9CFF },
};

template<>
test_data<4> test_data<4>::samples[] =
{
  { {0x9C,0x00,0x00,0x00}, 0x0000009C, 0x9C000000 },
  { {0x9C,0xFF,0xFF,0xFF}, 0xFFFFFF9C, 0x9CFFFFFF },
  { {0x01,0x02,0x03,0x04}, 0x04030201, 0x01020304 },
  { {0xF1,0xF2,0xF3,0xF4}, 0xF4F3F2F1, 0xF1F2F3F4 },
};

test_data<8> test_data<8>::samples[] =
{
  { {0x9C,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    0x0000009C, 0x00000000, 0x00000000, 0x9C000000 },
  { {0x9C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
    0xFFFFFF9C, 0xFFFFFFFF, 0xFFFFFFFF, 0x9CFFFFFF },
  { {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08},
    0x04030201, 0x08070605, 0x05060708, 0x01020304 },
  { {0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8},
    0xF4F3F2F1, 0xF8F7F6F5, 0xF5F6F7F8, 0xF1F2F3F4 },
};


TEST(Foundation, number)
{
  do_test<1>(1);
  do_test<2>(3);
  do_test<4>(4);
  do_test<8>(4);

  cout <<endl <<"== Negative tests ==" <<endl <<endl;

  Codec<Type::NUMBER> codec;

  int32_t val= -100;
  byte buf[8];

#define EXPECT_ERROR(Code) \
  try { Code; FAIL() <<"Should throw error"; } \
  catch (Error &err) { cout <<"Expected error: " <<err <<endl; }

  EXPECT_ERROR(codec.to_bytes(val, bytes(buf,2)));
  EXPECT_ERROR(codec.from_bytes(bytes(buf,(size_t)0),val));

  // should be OK to convert to a buffer which is too big...

  size_t howmuch= codec.to_bytes(val, bytes(buf,8));
  EXPECT_EQ(sizeof(int32_t),howmuch);
  codec.from_bytes(bytes(buf,howmuch), val);
  EXPECT_EQ(-100,val);

  // when converting to smaller variable, only initial part of buffer
  // is used

  howmuch= codec.from_bytes(bytes(buf,8), val);
  EXPECT_EQ(-100,val);
  EXPECT_EQ(sizeof(val),howmuch);

  int16_t val1;
  howmuch= codec.from_bytes(bytes(buf,8), val1);
  EXPECT_EQ(sizeof(val1),howmuch);

  // The number of bytes converted always equals valid integer type size

  howmuch= codec.from_bytes(bytes(buf,sizeof(val)+1), val);
  EXPECT_EQ(-100,val);
  EXPECT_EQ(sizeof(val),howmuch);

  howmuch= codec.from_bytes(bytes(buf,3), val);
  EXPECT_EQ(2U,howmuch);

}
