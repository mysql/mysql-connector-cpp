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

#ifndef SDK_FOUNDATION_TYPES_H
#define SDK_FOUNDATION_TYPES_H

#include "common.h"
#include "cdk_time.h"

PUSH_SYS_WARNINGS
#include <stdint.h>
#include <string.h>
#include <string>
#include <memory>
POP_SYS_WARNINGS

#if !defined(HAVE_SHARED_PTR)
 PUSH_BOOST_WARNINGS
 #include <boost/smart_ptr.hpp>
 POP_BOOST_WARNINGS
#endif

namespace cdk {
namespace foundation {

typedef wchar_t      char_t;

/*
  Note: we do not include error.h from here because this would create
  circular header dependency (error.h needs types.h). Still, we need
  throw_error() for the THROW() macro, so we declare it here.
*/
void throw_error(const char*);


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
};

inline
std::ostream& operator<<(std::ostream &out, const string &str)
{
  return out << (std::string)str;
}


#ifdef USE_NATIVE_BYTE
  using ::byte;
#else
  typedef unsigned char byte;
#endif

/*
  Convenience class to disable copy constructor in a derived class.
*/

class nocopy
{
  nocopy(const nocopy&);
  nocopy& operator=(const nocopy&);

protected:
  nocopy() {}
};


class Iterator
{
public:

  /*
    Move to next item in the sequence. If there are no more items in
    the sequence, returns false. This method should be called before
    accessing the first item in the sequence.
  */

  virtual bool next() = 0;
};


class bytes
{
  byte *m_begin;
  byte *m_end;

public:

  bytes() : m_begin(NULL), m_end(NULL) {}
  bytes(byte *_buf, size_t _len) : m_begin(_buf), m_end(_buf+_len) {}
  bytes(byte *_begin, byte *_end) : m_begin(_begin), m_end(_end) {}
  bytes(const std::string &str)
    : m_begin((byte*)str.data()), m_end(m_begin + str.size())
  {}
  bytes(const char *str)
    : m_begin((byte*)str), m_end(m_begin + strlen(str))
  {}

  virtual byte* begin() const { return m_begin; }
  virtual byte* end() const { return m_end; }
  size_t size() const
  {
    assert(m_end >= m_begin);
    return m_begin && m_end ? static_cast<size_t>(m_end - m_begin) : 0;
  }
};


class buffers
{
  bytes    m_first;
  const buffers *m_rest;

public:

  buffers(byte *buf, size_t len) : m_first(buf, len), m_rest(NULL) {}
  buffers(byte *begin, byte *end) : m_first(begin, end), m_rest(NULL) {}
  buffers(const bytes &buf) : m_first(buf), m_rest(NULL) {}
  buffers(const bytes &first, buffers &rest)
    : m_first(first), m_rest(&rest)
  {}

  virtual unsigned buf_count() const
  { return 1 + (m_rest ? m_rest->buf_count() : 0); }

  virtual bytes    get_buffer(unsigned pos) const;
  virtual size_t   length() const
  { return m_first.size() + (m_rest ? m_rest->length() : 0); }

};


inline
bytes buffers::get_buffer(unsigned pos) const
{
  if (0 == pos)
    return m_first;

  if (!m_rest)
    THROW("buffers: get_buffer: pos out of range");

  return m_rest->get_buffer(pos-1);
}

class option_t
{

public:

  enum option
  {
    UNKNOWN = -1,
    NO = 0,
    YES = 1,
  };

  option_t() { m_option = UNKNOWN; }
  option_t(bool x) { m_option = x ? YES : NO; }

  operator bool()
  {
    if (m_option == UNKNOWN)
      THROW("Converting UNKNOWN option to bool");
    return m_option==YES;
  }

  option state() { return m_option; }

protected:
  option m_option;
};


/*
  Smart pointer types
  ===================
  We define our own scoped_ptr<> which differs from boost::scoped_ptr<>
  by having extra release() method (see below). Otherwise see
  http://www.boost.org/doc/libs/1_39_0/libs/smart_ptr/scoped_ptr.htm.

  If std::shared_ptr<> is available we prefer it to boost::shared_ptr<>
  because the latter is implemented using deprecated std::auto_ptr<>
  and we want this code to compile under C++11 compiler without
  warnings.
*/


// Note: this implementation is *not* thread safe.

template <typename T>
class scoped_ptr : nocopy
{
  T *m_ptr;

public:

  scoped_ptr(T *ptr = NULL) : m_ptr(ptr) {}

  ~scoped_ptr()
  {
    delete m_ptr;
  }

  void reset(T *ptr = NULL)
  {
    delete m_ptr;
    m_ptr= ptr;
  }

  /*
    Return pointer to the owned object and clear
    the ownership so that this scoped_ptr instance
    no longer owns the object (so it will not be
    auto-deleted by the scoped_ptr).
  */

  T* release()
  {
    T *ptr = get();
    m_ptr = NULL;
    return ptr;
  }

  T* get() { return m_ptr; }
  const T* get() const { return m_ptr; }

  T& operator*() { return *m_ptr; }
  const T& operator*() const { return *m_ptr; }

  T* operator->() { return m_ptr; }
  const T* operator->() const { return m_ptr; }

  operator bool() const { return m_ptr != NULL; }

#ifdef HAVE_MOVE_SEMANTICS

  /*
    Disable move ctor. Avoid C++11 extensions warning
    generated by clang.
  */

#ifdef __clang__
  DIAGNOSTIC_PUSH
  DISABLE_WARNING(-Wc++11-extensions)
#endif

private:
  scoped_ptr(scoped_ptr&&);

#ifdef __clang__
  DIAGNOSTIC_POP
#endif

#endif

};


#ifdef HAVE_SHARED_PTR

using ::std::shared_ptr;

#else

using ::boost::shared_ptr;

#endif


}} // cdk::foundation

#endif
