/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
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


#include <string>
#include <stdexcept>
#include <ostream>
#include <memory>
#include <string.h>  // for memcpy
#include <utility>   // std::move etc

namespace cdk {
namespace foundation {

class bytes;
class string;

}}  // cdk::foundation


namespace mysqlx {

class nocopy
{
public:
  nocopy(const nocopy&) = delete;
  nocopy& operator=(const nocopy&) = delete;
protected:
  nocopy() {}
};

typedef unsigned char byte;

class string : public std::wstring
{
public:

  string() {}

  string(const wchar_t *other) : std::wstring(other) {}
  string(const std::wstring &other) : std::wstring(other) {}
  string(std::wstring &&other) : std::wstring(std::move(other)) {}

  string& operator=(const std::wstring &other)
  {
    assign(other);
    return *this;
  }

  // TODO: make utf8 conversions explicit

  string(const char*);
  string(const std::string&);

  //  operator cdk::foundation::string&();
  operator std::string() const;  // conversion to utf-8
//  operator const cdk::foundation::string&() const;
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

class bytes
{

  byte *m_begin;
  byte *m_end;

  bytes(byte *beg, byte *end)
    : m_begin(beg), m_end(end)
  {}

public:

//  operator const cdk::foundation::bytes&() const;
  virtual byte* begin() const { return m_begin; }
  virtual byte* end() const { return m_end; }
  size_t size() const { return m_begin && m_end ? m_end - m_begin : 0; }

  class Access;
  friend class Access;
};


class Printable
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


/**
Global unique identifiers for documents.

TODO: Windows GUID type
*/

class GUID : public Printable
{
  char m_data[32];

  void set(const char *data)
  {
    // Note: Windows gives compile warnings when using strncpy

    for(unsigned i=0; data[i] && i < sizeof(m_data); ++i)
      m_data[i]= data[i];
  }

  void set(const std::string &data) { set(data.c_str()); }

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


/*
  TODO: Derive from std::system_error and introduce proper
  error codes.
*/

class Error : public std::runtime_error
{
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

}  // mysqlx


#endif
