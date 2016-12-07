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

#include <mysql/cdk/foundation/types.h>
#include <mysql/cdk/foundation/codec.h>


namespace cdk {
namespace foundation {


string& string::set_utf8(const std::string &str)
{
  Codec<Type::STRING> codec;
  codec.from_bytes(bytes((byte*)str.data(), str.length()), *this);
  return *this;
}

string::operator std::string() const
{
  Codec<Type::STRING> codec;
  size_t len = 4*length() + 1;
  char *buf = new char[4*length() + 1];
  len= codec.to_bytes(*this, bytes((byte*)buf, len));
  buf[len] = '\0';
  const std::string out(buf, buf+len);
  delete[] buf;
  return out;
}


}}  // cdk::foundation



#ifndef HAVE_CODECVT_UTF8

/*
  Implementation of UTF-8 codecvt facet using boost::locale

  TODO: This implementation of std::codecvt interface is incomplete
  and will probably not work as a locale facet when used with C++
  streams. Methods such as do_length() and do_unshift are not
  implemented. See: http://en.cppreference.com/w/cpp/locale/codecvt
*/

PUSH_BOOST_WARNINGS
#include <boost/locale/utf.hpp>
POP_BOOST_WARNINGS

namespace cdk {
namespace foundation {

using namespace boost::locale;

typedef utf::utf_traits<char> utf8;


// see: http://en.cppreference.com/w/cpp/locale/codecvt/out

codecvt_utf8::result
codecvt_utf8::do_out(state_type& state,
                     const intern_type* from,
                     const intern_type* from_end,
                     const intern_type*& from_next,
                     extern_type* to,
                     extern_type* to_end,
                     extern_type*& to_next ) const
{
  from_next = from;
  to_next = to;

  while (from_next < from_end)
  {
    utf::code_point c = *from_next;
    if (!utf::is_valid_codepoint(c))
      return error;
    if (to_next + utf8::width(c) > to_end)
      return partial;
    to_next = utf8::encode(c, to_next);
    from_next++;
  }

  return ok;
}


// see: http://en.cppreference.com/w/cpp/locale/codecvt/in

codecvt_utf8::result
codecvt_utf8::do_in(state_type& state,
                    const extern_type* from,
                    const extern_type* from_end,
                    const extern_type*& from_next,
                    intern_type* to,
                    intern_type* to_end,
                    intern_type*& to_next ) const
{
  from_next = from;
  to_next = to;

  while (from_next < from_end)
  {
    utf::code_point c = utf8::decode(from_next, from_end);
    if (c == utf::illegal)
      return error;
    if (c == utf::incomplete || to_next >= to_end)
      return partial;
    // TODO: compiler warns about possible loss of data when
    // converting output of utf8::decode (code_point) to intern_type.
    *to_next = (intern_type)c;
    to_next++;
  }

  return ok;
}


}} // cdk::foundation

#endif


namespace cdk {
namespace foundation {

codecvt_ascii::result
codecvt_ascii::do_out(state_type& state,
                     const intern_type* from,
                     const intern_type* from_end,
                     const intern_type*& from_next,
                     extern_type* to,
                     extern_type* to_end,
                     extern_type*& to_next ) const
{
  from_next = from;
  to_next = to;

  while (from_next < from_end)
  {
    int c = m_ctype.narrow(*from_next, -1);

    if (-1 == c)
      return error;

    *to_next = (extern_type)c;
    to_next++;
    from_next++;
  }

  return ok;
}


codecvt_ascii::result
codecvt_ascii::do_in(state_type& state,
                    const extern_type* from,
                    const extern_type* from_end,
                    const extern_type*& from_next,
                    intern_type* to,
                    intern_type* to_end,
                    intern_type*& to_next ) const
{
  from_next = from;
  to_next = to;

  while (from_next < from_end)
  {
    unsigned char c = (unsigned char)*from_next;

    /*
      Note: In absence of character encoding information, only ASCII
      characters can be reliably converted to wide chars by
      ctype<wchar_t>::widen() method.
    */
    if (c > 128)
      return error;

    intern_type wc = m_ctype.widen(*from_next);

    if (-1 == wc)
      return error;

    *to_next = wc;
    to_next++;
    from_next++;
  }

  return ok;
}

}} // cdk::foundation
