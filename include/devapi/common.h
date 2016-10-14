/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C++ is licensed under the terms of the GPLv2
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

#ifndef MYSQLX_COMMON_H
#define MYSQLX_COMMON_H


#include "../mysql_common.h"
#include <string>
#include <stdexcept>
#include <ostream>
#include <memory>
#include <forward_list>
#include <string.h>  // for memcpy
#include <utility>   // std::move etc

namespace cdk {
namespace foundation {

class bytes;
class string;

}}  // cdk::foundation


namespace mysqlx {

typedef unsigned char byte;


namespace internal {

/*
  List_initializer class is used to initialize user std::vector, std::list or
  own list implementations, as long as initialized by iterators of defined type
*/

template <typename T>
struct List_init
{
   std::forward_list<T> m_data;

   template <typename L>
   List_init(L&& list)
     : m_data(std::move(list.begin()), std::move(list.end()))
   {}

   //Special case for forward_list using move constructor
   List_init(std::forward_list<T>&& list)
     : m_data(std::move(list))
   {}

   template<typename U>
   operator U()
   {
     return U(m_data.begin(), m_data.end());
   }

   operator std::forward_list<T>()
   {
     return std::move(m_data);
   }
};

}  // internal


/**
  A wrapper around std::wstring that can perform
  conversions from/to different character encodings
  used by MySQL.

  Currently only utf-8 encoding is supported.

  @ingroup devapi_aux
*/

class string : public std::wstring
{

  struct Impl
  {
    PUBLIC_API static std::string to_utf8(const string&);
    PUBLIC_API static void from_utf8(string&, const std::string&);
  };

public:

  string() {}

  string(const wchar_t *other)
  {
    if (other)
      assign(other);
  }
  string(const std::wstring &other) : std::wstring(other) {}
  string(std::wstring &&other) : std::wstring(std::move(other)) {}

  string& operator=(const std::wstring &other)
  {
    assign(other);
    return *this;
  }

  // TODO: make utf8 conversions explicit

  string(const char *other)
  {
    if (!other)
      return;
    std::string utf8(other);
    Impl::from_utf8(*this, utf8);
  }

  string(const std::string &other)
  {
    Impl::from_utf8(*this, other);
  }

  // conversion to utf-8

  operator std::string() const
  {
    return Impl::to_utf8(*this);
  }

};


inline
std::ostream& operator<<(std::ostream &out, const string &str)
{
  const std::string utf8(str);
  out << utf8;
  return out;
}


typedef unsigned long col_count_t;
typedef unsigned long row_count_t;


/**
  Class representing a region of memory holding raw bytes.

  Method `begin()` returns pointer to the first byte in the
  region, `end()` to one past the last byte in the region.

  @note Instance of `bytes` type does not store the bytes -
  it merely describes a region of memory and is equivalent
  to a pair of pointers. It is very cheap to copy `bytes` and
  pass them by value.

  @note This class extends std::pair<byte *, size_t> to make
  it consistent with how memory regions are described by
  std::get_temporary_buffer(). It is also possible to initialize
  bytes instance by buffer returned from
  std::get_temporary_buffer(), as follows:

    bytes buf = std::get_temporary_buffer<byte>(size);

  @ingroup devapi_aux
*/

class bytes : public std::pair<byte*, size_t>
{

public:

  bytes(byte *beg_, byte *end_)
    : pair(beg_, end_ - beg_)
  {}

  bytes(byte *beg, size_t len) : pair(beg, len)
  {}

  bytes(const char *str) : pair((byte*)str,0)
  {
    if (NULL != str)
      second = strlen(str);
  }

  bytes(std::pair<byte*, size_t> buf) : pair(buf)
  {}

  bytes() : pair(NULL, 0)
  {}

  bytes(const bytes &) = default;

  virtual byte* begin() const { return first; }
  virtual byte* end() const { return first + second; }

  size_t length() const { return second; }
  size_t size() const { return length(); }

  class Access;
  friend Access;
};


namespace internal {

  class nocopy
  {
  public:
    nocopy(const nocopy&) = delete;
    nocopy& operator=(const nocopy&) = delete;
  protected:
    nocopy() {}
  };


  class PUBLIC_API Printable
  {
    virtual void print(std::ostream&) const = 0;
    friend std::ostream& operator<<(std::ostream&, const Printable&);
  };

  inline
  std::ostream& operator<<(std::ostream &out, const Printable &obj)
  {
    obj.print(out);
    return out;
  }


  /*
    Defined here because std::enable_if_t is not defined on all platforms on
    which we build (clang is missing one).
  */

  template<bool Cond, typename T = void>
  using enable_if_t = typename std::enable_if<Cond, T>::type;

}  // internal


/*
  Global unique identifiers for documents.

  TODO: Windows GUID type
*/

class PUBLIC_API GUID : public internal::Printable
{
  char m_data[32];

  void set(const char *data)
  {
    // Note: Windows gives compile warnings when using strncpy
    for(unsigned i=0; data[i] && i < sizeof(m_data); ++i)
      m_data[i]= data[i];
  }

  INTERNAL void set(const std::string &data) { set(data.c_str()); }

public:

  GUID()
  {
    memset(m_data, 0, sizeof(m_data));
  }

  template <typename T> GUID(T data) { set(data); }
  template<typename T>  GUID& operator=(T data) { set(data); return *this; }

  operator std::string() const
  {
    return std::string(m_data, m_data + sizeof(m_data));
  }

  void generate();

  void print(std::ostream &out) const
  {
    out << std::string(*this);
  }
};


using std::out_of_range;

/**
  Base class for connector errors.

  @internal
  TODO: Derive from std::system_error and introduce proper
  error codes.
  @endinternal

  @ingroup devapi
*/

DLL_WARNINGS_PUSH

class PUBLIC_API Error : public std::runtime_error
{

DLL_WARNINGS_POP

public:

  Error(const char *msg)
    : std::runtime_error(msg)
  {}
};


inline
std::ostream& operator<<(std::ostream &out, const Error &e)
{
  out << e.what();
  return out;
}

#define CATCH_AND_WRAP \
  catch (const ::mysqlx::Error&) { throw; }       \
  catch (const std::exception &e)                 \
  { throw ::mysqlx::Error(e.what()); }            \
  catch (const char *e)                           \
  { throw ::mysqlx::Error(e); }                   \
  catch (...)                                     \
  { throw ::mysqlx::Error("Unknown exception"); } \


inline
void throw_error(const char *msg)
{
  throw ::mysqlx::Error(msg);
}


}  // mysqlx


#endif
