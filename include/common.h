#ifndef COMMON_H
#define COMMON_H

#include <string>

namespace cdk {
namespace foundation {

class bytes;
class string;

}}  // cdk::foundation


namespace mysqlx {

class nocopy
{
  nocopy(const nocopy&); // = delete;
  nocopy& operator=(const nocopy&); // = delete;
protected:
  nocopy() {}
};

typedef unsigned char byte;

class string : public std::wstring
{
public:

  string(const wchar_t *other) : std::wstring(other) {}
  string(const std::wstring &other) : std::wstring(other) {}

  string(const char*);
  string(const std::string&);

  //  operator cdk::foundation::string&();
  operator const std::string() const;  // conversion to utf-8
//  operator const cdk::foundation::string&() const;
};


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


}  // mysqlx


#endif
