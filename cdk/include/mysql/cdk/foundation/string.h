/*
 * Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef SDK_FOUNDATION_STRING_H
#define SDK_FOUNDATION_STRING_H

#include "common.h"
#include "types.h"

// TODO: Replace with std::variant<> when available.
#include "variant.h"

#include <rapidjson/encodings.h>
#include <rapidjson/memorystream.h>
#include <rapidjson/stringbuffer.h>

PUSH_SYS_WARNINGS
#include <stdint.h>
#include <string.h>
#include <string>
#include <memory>
POP_SYS_WARNINGS


namespace cdk {
namespace foundation {

typedef char32_t      char_t;
constexpr char_t   invalid_char = (char_t)-1;
typedef std::basic_string<char16_t> ustring;


struct String_encoding {

using UCS4  = rapidjson::UTF32<char32_t>;
using UTF16 = rapidjson::UTF16<char16_t>;
using UTF8  = rapidjson::UTF8<char>;
using ASCII = rapidjson::ASCII<char>;

using CHR   = UCS4;
using WIDE  = rapidjson::UTF16<wchar_t>; // TODO: different on Linux!
using STR   = WIDE;
//using STR   = UTF16;

};


// NOTE: Modified code taken from rapidjson memorystream.h

template <typename CHAR>
struct Mem_stream
{
    typedef CHAR Ch; // byte


    Mem_stream(const Ch *beg, const Ch *end)
      : src_(const_cast<Ch*>(beg)), begin_(beg), end_(end)
    {}

    Mem_stream(const Ch *src, size_t size)
      : Mem_stream(src, src + size)
    {}

    Mem_stream()
      : Mem_stream(nullptr, nullptr)
    {}

    bool hasData() const { return src_ && src_ < end_; }

    Ch Peek() const
    {
      return RAPIDJSON_UNLIKELY(src_ == end_) ? Ch(0) : *src_;
    }

    Ch Take()
    {
      return RAPIDJSON_UNLIKELY(src_ == end_) ? Ch(0) : *src_++;
    }

    size_t Tell() const
    {
      return static_cast<size_t>(src_ - begin_);
    }

    void SetPos(const Ch *pos)
    {
      assert(begin_ <= pos);
      assert(pos <= end_);
      src_ = const_cast<Ch*>(pos);
    }

    // TODO: ReserveN() and copy optimization...

     void Put(Ch c)
     {
       if (RAPIDJSON_LIKELY(src_ < end_))
       {
         *src_++ = c;
         return;
       }
       overflow_ = true;
     }

     void Flush() {}

     //Ch* PutBegin() { RAPIDJSON_ASSERT(false); return 0; }
     //size_t PutEnd(Ch*) { RAPIDJSON_ASSERT(false); return 0; }

     Ch* src_;     //!< Current read position.
     const Ch* begin_;   //!< Original head of the string.
     const Ch* end_;     //!< End of stream.
     bool  overflow_ = false;
};


template <typename CHAR>
struct Str_stream
{
    typedef CHAR Ch; // byte
    using string = std::basic_string<Ch>;

    Str_stream(string &str)
      : buf_(str), pos_(str.length())
    {}


    bool hasData() const { return pos_ < buf_.length(); }

    Ch Peek() const
    {
      return RAPIDJSON_LIKELY(!hasData()) ? Ch(0) : buf_[src_];
    }

    Ch Take()
    {
      return RAPIDJSON_LIKELY(!hasData()) ? Ch(0) : buf_[src_++];
    }

    size_t Tell() const
    {
      return pos_;
    }

    void SetPos(const Ch *pos)
    {
      assert(buf_.data() <= pos);
      assert(pos <= buf_.data() + buf_.length());
      pos_ = pos - buf_.data();
    }

    // TODO: ReserveN() and copy optimization...

     void Put(Ch c)
     {
       if (RAPIDJSON_UNLIKELY(hasData()))
       {
         buf_[pos_++] = c;
         return;
       }
       buf_.push_back(c);
       pos_++;
     }

     void Flush() {}

     //Ch* PutBegin() { RAPIDJSON_ASSERT(false); return 0; }
     //size_t PutEnd(Ch*) { RAPIDJSON_ASSERT(false); return 0; }

     string &buf_;
     size_t pos_ = 0;     //!< Current read position.
};


// code points -> string

template<class FROM, class TO = String_encoding::STR>
size_t str_encode(
  const typename FROM::Ch *beg, size_t len,
  std::basic_string<typename TO::Ch> &out
)
{
  using Transcoder = rapidjson::Transcoder<FROM, TO>;
  Mem_stream<typename FROM::Ch> input(beg, len);
  Str_stream<typename TO::Ch>   output(out);

  while (input.hasData())
  {
    if (!Transcoder::Transcode(input, output))
    {
      // TODO: add some context info from the input stream.
      throw_error("Failed string conversion");
    }
  }

  return input.Tell();
}


// string -> code points

template<class TO, class FROM = String_encoding::STR>
size_t str_decode(
  const std::basic_string<typename FROM::Ch> &in,
  typename TO::Ch *out, size_t len
)
{
  using Transcoder = rapidjson::Transcoder<FROM, TO>;
  Mem_stream<typename FROM::Ch> input(in.data(), in.length());
  Mem_stream<typename TO::Ch>   output(out, len);

  while (input.hasData())
  {
    if (!Transcoder::Transcode(input, output))
    {
      // TODO: add some context info from the input stream.
      throw_error("Failed string conversion");
    }
  }

  return output.Tell();
}



/*
  Iterate through a sequence of code units of the given encoding, one character
  at a time (single character can be encoded using one or more code units).
*/

template <class ENC>
class char_iterator_base
  : public std::iterator<
      std::input_iterator_tag,
      char_t,
      long,
      const typename char_t*,
      const typename char_t&
    >
{
protected:

  using code_unit = typename ENC::Ch;

  Mem_stream<code_unit>   m_stream;

  /*
    If m_char !=0 then it contains the current character (which was already
    decoded) and m_pos points at the next code unit after the character.
    If m_char == 0 then m_pos points at the first code unit of the next
    character (which is not yet decoded).

           m_char != 0
           |   m_pos                    m_pos (m_char == 0)
           |   |                        |
           v   v                        v
    ------|===|--------            ----|===|-----

    TODO: m_pos is redundant, as it is the same as m_stream.src_ ?
  */

  const code_unit *m_pos = nullptr;
  char_t   m_char = 0;
  bool     m_at_end = false;

public:

  char_iterator_base()
    : m_at_end(true)
  {}

  char_iterator_base(const code_unit *beg, const code_unit *end)
    : m_stream(beg, end), m_pos(beg)
  {}

  char_iterator_base(const code_unit *beg, size_t len)
    : char_iterator_base(beg, beg + len)
  {}

  char_iterator_base(const char_iterator_base &other) = default;
  char_iterator_base& operator=(const char_iterator_base&) = default;

  // examine current character

  reference operator*() const
  {
    /*
      If m_char != 0 then it already contains the current character and
      the corresponding code units have been consumed from the input stream.

      Otherwise, the input stream contains code units of the current character
      and we need to decode it here. The input stream is moved to the next
      code unit after the current character.

      If decoding of the character fails, then the current character is invalid
      and input stream is positioned at the next code unit after error has been
      detected. After increasing iterator, decoding will continue from that
      position.
    */

    if (!m_char && m_stream.hasData())
    {
      auto *self = const_cast<char_iterator_base*>(this);
      if (!ENC::Decode(self->m_stream, (unsigned*)&(self->m_char)))
        self->m_char = invalid_char;
    }
    return m_char;
  }

  code_unit get_unit()
  {
    assert(!at_end());
    return *m_pos;
  }

  // examine current position

  bool at_end() const
  {
    return m_at_end;
  }

  const code_unit* cur_pos() const
  {
    assert(!m_pos || (m_pos <= m_stream.end_));
    return m_pos;
  }

  // change position

  char_iterator_base& operator++()
  {
    if (at_end())
      return *this;

    operator*();  // moves stream to next position, if not already done
    m_pos = m_stream.src_;
    m_char = 0;
    m_at_end = (m_pos == m_stream.end_);

    return *this;
  }

  char_iterator_base& operator++(int)
  {
    return operator++();
  }

  /*
    Move to the next code unit in the input.

    In general this method moves to the next code unit in the input
    sequence. The only exception is if the current character takes more than
    one code unit and was already decoded (and so, the code units were already
    consumed). In this case position is moved to the next character.
  */

  void next_unit()
  {
    assert(!at_end());

    // if m_char is set, then corresponding code unit(s) are already consumed
    // from the stream.

    if (!m_char)
    {
      m_stream.Take();
      m_pos++;
    }
    else
      m_pos = m_stream.src_;

    m_at_end = (m_pos == m_stream.end_);
    m_char = 0;
  }

  void set_pos(const code_unit *pos)
  {
    assert(m_stream.begin_ <= pos);
    assert(pos <= m_stream.end_);
    m_pos = pos;
    m_at_end = (m_pos == m_stream.end_);
    m_stream.SetPos(pos);
  }

  // Other methods

  const code_unit* get_end() const
  {
    return m_stream.end_;
  }

  const code_unit* get_beg() const
  {
    return m_stream.begin_;
  }

  bool operator==(const char_iterator_base &other) const
  {
    // Note: only at end iterators compare - do we need more?

    if (at_end() && other.at_end())
      return true;
    return false;
  }

  bool operator!=(const char_iterator_base &other) const
  {
    return !operator==(other);
  }

};  // char_iterator_base


class string : public std::wstring
{
public:

  string() {}
  string(const wchar_t *str) : std::wstring(str) {}
  string(const std::wstring &str) : std::wstring(str) {}

  string(const char *str) { set_utf8(str); }
  string(const std::string &str) { set_utf8(str); }

  // internal -> UTF8 conversion
  operator std::string() const;

  // UTF8 -> internal conversion
  string& set_utf8(const std::string&);
  string& set_ascii(const char*, size_t);

  void push_back(char_t);
};


inline
string& string::set_ascii(const char *str, size_t len)
{
  clear();
  str_encode<String_encoding::ASCII>(str, len, *this);
  return *this;
}

inline
string& string::set_utf8(const std::string &str)
{
  clear();
  str_encode<String_encoding::UTF8>(str.data(), str.length(), *this);
  return *this;
}

inline
void string::push_back(char_t c)
{
  str_encode<String_encoding::UCS4>(&c, 1, *this);
}

inline
string::operator std::string() const
{
  std::string out;
  str_encode<String_encoding::STR, String_encoding::UTF8>(
    data(), length(), out
  );
  return out;
}


inline
std::ostream& operator<<(std::ostream &out, const string &str)
{
  return out << (std::string)str;
}



#if 0

/*
  String class which stores character data using the STR encoding defined
  above (utf16).
*/

class string
{
  using code_unit = String_encoding::STR::Ch;
  using string_base = std::basic_string<code_unit>;

  /*
    Rapidjson buffer used to store and convert code units.
  */

  struct Buffer
    : rapidjson::GenericStringBuffer<String_encoding::STR>
  {
    using Base = rapidjson::GenericStringBuffer<String_encoding::STR>;
    using Base::GenericStringBuffer;

    Buffer(const string_base &str)
    {
      Append(str);
    }

    Buffer() = default;

    Buffer(const Buffer &other)
    {
      operator=(other);
    }

    Buffer& operator=(Buffer&&) = default;
    Buffer& operator=(const Buffer&);

    void Resize(size_t new_size)
    {
      stack_.Clear();
      stack_.Reserve<char>(new_size);
      stack_.ShrinkToFit();
    }

    // Append utf16 code units
    void Append(const string_base&);

    // Append ASCII characters
    void Append(const char*, size_t);

    size_t GetLength() const { return stack_.GetSize() / sizeof(Ch); }
  };

  Buffer m_buf;

  template <class ENC>
  std::basic_string<typename ENC::Ch> conv_to_string() const;

  template <class ENC>
  size_t conv_to_bytes(bytes) const;

  template <class ENC>
  size_t conv_from_codes(
    typename ENC::Ch const *beg, typename ENC::Ch const *end
  );

  template <class ENC>
  size_t conv_from_string(const std::basic_string<typename ENC::Ch> &str)
  {
    return conv_from_codes<ENC>(str.data(), str.data() + str.length());
  }

public:

  /*
    Define the same types as are defined by std::basic_string<>.
  */

  using size_type = size_t;
  using value_type = code_unit;
  using reference = value_type&;
  using const_reference = value_type const&;

  static const size_type npos = (size_type)(-1);


  string() {}

  string(const string &other) = default;
  string(string&&) = default;

  string& operator=(const string&) = default;
  string& operator=(string&&) = default;

  // conversion from native wide encoding

  string(const wchar_t *str)
    : string(std::wstring(str))
  {}

  string(const std::wstring &str)
  {
    m_buf.Clear();
    conv_from_string<String_encoding::WIDE>(str);
  }

  // conversion from UTF8

  string(const char *str)
    : string(std::string(str))
  {}

  string(const std::string &str)
  {
    m_buf.Clear();
    conv_from_string<String_encoding::UTF8>(str);
  }

  // conversion from UTF16

  string(const char16_t *str)
    : string(string_base(str))
  {}

  string(const string_base &str)
    : m_buf(str)
  {}

  // conversion from Unicode character string

  string(const char_t *str)
    : string(std::basic_string<char_t>(str))
  {}

  string(const std::basic_string<char_t> &str)
  {
    m_buf.Clear();
    conv_from_string<String_encoding::CHR>(str);
  }


  // conversions to standard string types


  operator std::string() const
  {
    return conv_to_string<String_encoding::UTF8>();
  }

  operator std::wstring() const
  {
    return conv_to_string<String_encoding::WIDE>();
  }

  operator std::basic_string<char_t>() const
  {
    return conv_to_string<String_encoding::CHR>();
  }

  operator string_base() const
  {
    return { m_buf.GetString(), m_buf.GetLength() };
  }


  // -- Examining the string -----------------------------------


  // NOTE: returns size in code units

  size_t size()   const
  {
    return m_buf.GetLength();
  }

  size_t length() const
  {
    return size();
  }

  bool   empty()  const
  {
    return 0 == size();
  }

  /*
    Note: These functions return utf16 code units at given position,
    not characters. Use char_iterator to access characters.
  */

  reference at(size_type pos)
  {
    const_reference ref = const_cast<const string*>(this)->at(pos);
    return const_cast<reference>(ref);
  }

  const_reference at(size_type pos) const
  {
    if (pos > m_buf.GetLength())
      throw std::out_of_range("when referencing character inside cdk string");
    return m_buf.GetString()[pos];
  }


  reference operator[]( size_type pos )
  {
    const_reference ref = const_cast<const string*>(this)->operator[](pos);
    return const_cast<reference>(ref);
  }

  const_reference operator[]( size_type pos ) const
  {
    return m_buf.GetString()[pos];
  }

  class char_iterator;

  char_iterator chars() const;
  char_iterator chars_end() const;


  size_type copy(code_unit* dest,  size_type count, size_type pos = 0) const
  {
    if (pos + count > m_buf.GetLength())
      count = m_buf.GetLength() - pos;
    std::copy_n(m_buf.GetString() + pos, count, dest);
    return count;
  }


  // -- String modifications ------------------------------


  void clear()
  {
    m_buf.Clear();
  }

  void resize(size_type count)
  {
    m_buf.Resize(count);
  }

  string& append(const string &other)
  {
    m_buf.Append(other);
    return *this;
  }

  void push_back(char_t ch)
  {
    using Transcoder
    = rapidjson::Transcoder< String_encoding::CHR, String_encoding::STR >;

    Mem_stream<char_t> input(&ch, 1);
    if (!Transcoder::Transcode(input, m_buf))
      throw_error("invalid character");
  }

  //char* utf8_str() const; // TODO (optimized version, if possible)

  string& set_utf8(bytes utf8)
  {
    m_buf.Clear();
    conv_from_codes<String_encoding::UTF8>(
      (const char*)utf8.begin(), (const char*) utf8.end()
    );
    return *this;
  }

  string& set_ascii(const char *str, size_t len)
  {
    m_buf.Clear();
    m_buf.Append(str, len);
    return *this;
  }


  string& operator+=(const string &other)
  {
    return append(other);
  }

  friend
  string operator+(const string &lhs, const string &rhs)
  {
    string ret(lhs);
    ret += rhs;
    return ret;
  }


  // -- Operators -----------------------------------------


  friend
  bool operator==(const string &lhs, const string &rhs)
  {
    size_t len = lhs.length();
    if (len != rhs.length())
      return false;

    const char16_t *a = lhs.m_buf.GetString();
    const char16_t *b = rhs.m_buf.GetString();

    for (; len > 0; len--)
    {
      if (*a++ != *b++)
        return false;
    }

    return true;
  }

  friend
  bool operator!=(const string &lhs, const string &rhs)
  {
    return !(lhs == rhs);
  }

  /*
    Note: This compares code units, so it is not exact string comparison.
    But it should be good enough for situations such as when using string as
    a std::map<> key.
  */

  friend
  bool operator<(const string &lhs, const string &rhs)
  {
    const code_unit *a = lhs.m_buf.GetString();
    const code_unit *b = rhs.m_buf.GetString();

    return std::lexicographical_compare(
      a, a + lhs.length(), b, b + rhs.length()
    );
  }

  friend
  bool operator>(const string &lhs, const string &rhs)
  {
    return rhs < lhs;
  }

  template <class ENC>
  friend class String_codec;

};  // string


/*
  Character iterator.
*/


class string::char_iterator
  : public char_iterator_base<String_encoding::STR>
{
  char_iterator(const string &str)
    : char_iterator_base(str.m_buf.GetString(), str.length())
  {}

  char_iterator() = default;

  friend string;
};


inline
auto string::chars() const -> char_iterator
{
  return *this;
}

inline
auto string::chars_end() const -> char_iterator
{
  return {};
}


/*
  Conversions to different encodings.
*/


template <typename ENC>
inline
size_t string::conv_from_codes(
  typename ENC::Ch const *beg, typename ENC::Ch const *end
)
{
  using Transcoder = rapidjson::Transcoder<ENC, String_encoding::STR>;
  Mem_stream<typename ENC::Ch> input(beg, end);

  while (input.hasData())
  {
    if (!Transcoder::Transcode(input, m_buf))
    {
      // TODO: add some context info from the input stream.
      throw_error("Failed to convert string into internal representation");
    }
  }

  return input.Tell();
}


template <class ENC>
inline
size_t string::conv_to_bytes(bytes out) const
{
  using rapidjson::Transcoder;
  using char_type = typename ENC::Ch;
  Mem_stream<char16_t> in_str(m_buf.GetString(), m_buf.GetLength());
  Mem_stream<char_type> out_str((const char_type*)out.begin(), (const char_type*)out.end());
  while (in_str.hasData())
  {
    Transcoder<String_encoding::STR, ENC>::Transcode(in_str, out_str);
  }

  return sizeof(char_type)*out_str.Tell();
}


template <class ENC>
inline
auto string::conv_to_string() const
->std::basic_string<typename ENC::Ch>
{
  using namespace rapidjson;
  using Transcoder = Transcoder<String_encoding::STR, ENC>;
  using char_type = typename ENC::Ch;

  GenericStringBuffer<ENC> out;

  Mem_stream<char16_t> in(m_buf.GetString(), m_buf.GetLength());

  while (in.hasData())
  {
    // TODO: better error description
    if (!Transcoder::Transcode(in, out))
      throw_error("Could not convert string to external representation");
  }

  return { out.GetString(), out.GetSize() / sizeof(char_type) };
}


/*
  Internal string buffer.
*/


inline
void string::Buffer::Append(const ustring &str)
{
  Ch *pos = Push(str.length());
  PUSH_SCL_SECURE_WARNINGS
  str.copy(pos, str.length());
  POP_SCL_SECURE_WARNINGS
}

inline
void string::Buffer::Append(const char *str, size_t len)
{
  Ch *pos = Push(len);
  PUSH_SCL_SECURE_WARNINGS
  std::copy_n(str, len, pos);
  POP_SCL_SECURE_WARNINGS
}


inline
auto string::Buffer::operator=(const Buffer &other) -> Buffer&
{
  if (this == &other)
    return *this;

  stack_.Clear();

  size_t howmuch = other.GetSize();

  if (0 == howmuch)
    return *this;

  stack_.Push<char>(howmuch);
  memcpy(stack_.Bottom<char>(), other.stack_.Bottom<char>(), howmuch);

  return *this;
}


/*
  Other
*/

inline
std::ostream& operator<<(std::ostream &out, const string &str)
{
  return out << (std::string)str;
}

#endif

}} // cdk::foundation

#endif
