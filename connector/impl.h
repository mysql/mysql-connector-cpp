#ifndef MYSQLX_IMPL_H
#define MYSQLX_IMPL_H

/*
  Internal implementations for public DevAPI classes.
*/

#include <mysqlxx.h>
#include <map>
#include <memory>


namespace mysqlx {

/**
  DbDoc implementation which stores document data in std::map.
*/

class DbDoc::Impl
{
  class JSONDoc;

  virtual void print(std::ostream &out) const
  {
    out << "<document>";  // TODO
  }

  virtual void prepare() {}

  // Data storage

  typedef std::map<Field, Value> Map;
  Map m_map;

  bool has_field(const Field &fld)
  {
    prepare();
    return m_map.end() != m_map.find(fld);
  }

  Value& get(const Field &fld)
  {
    prepare();
    return m_map.at(fld);
  }

  // Iterating over fields of the document

  Map::iterator m_it;

  void reset() { prepare(); m_it = m_map.begin(); }

  const Field& get_current_fld() { return m_it->first; }
  void next() { ++m_it; }
  bool at_end() const { return m_it == m_map.end(); }

  struct Builder;

  friend class DocResult;
  friend class DbDoc;
  friend class RowDoc;
};


/**
  DbDoc::Impl specialization which takes document data from
  a JSON string.
*/

class DbDoc::Impl::JSONDoc
  : public DbDoc::Impl
{
  std::string m_json;
  bool m_parsed;

public:

  JSONDoc(const std::string &json)
    : m_json(json)
    , m_parsed(false)
  {}

  void prepare();

  void print(std::ostream &out) const
  {
    out << m_json;
  }
};


/**
  DocResult implementation over RowResult.

  This implementation takes rows from RowResult and builds a document
  using JSON data in the row.
*/

class DocResult::Impl
  : RowResult
{
  bool   m_at_front;
  DbDoc  m_doc;

  Impl(Result &init)
    : RowResult(std::move(init))
    , m_at_front(true)
  {
    get_next_doc();
  }

  void get_next_doc()
  {
    m_doc.m_impl.reset();
    Row *row = next();
    if (!row)
      return;
    bytes data = row->getBytes(0);
    m_doc = DbDoc(std::string(data.begin(), data.end() - 1));
  }

  bool has_doc() const
  {
    return (bool)m_doc.m_impl;
  }

  void next_doc()
  {
    if (m_at_front)
      m_at_front = false;
    else
      get_next_doc();
  }

  friend class DocResult;
};


}

#endif