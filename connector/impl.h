/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#ifndef MYSQLX_IMPL_H
#define MYSQLX_IMPL_H

/*
  Internal implementations for public DevAPI classes.
*/

#include <mysqlx.h>
#include <mysql/cdk.h>
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
  Row  m_row;

  Impl(BaseResult &init)
    : RowResult(std::move(init))
  {
    next_doc();
  }

  DbDoc get_current_doc()
  {
    if (!m_row)
      return DbDoc();

    // @todo Avoid copying of document string.

    bytes data = m_row.getBytes(0);
    return DbDoc(std::string(data.begin(), data.end() - 1));
  }

  bool has_doc() const
  {
    return (bool)m_row;
  }

  void next_doc()
  {
    m_row = fetchOne();
  }

  friend class DocResult;
};


/*
  Taks implementation
  ===================
*/

struct Task::Access
{
  typedef Task::Impl Impl;

  static void reset(Task &task, Impl *impl)
  {
    task.reset(impl);
  }

  static Impl* get_impl(Task &task)
  {
    return task.m_impl;
  }
};


class Task::Impl : nocopy
{
protected:

  XSession &m_sess;
  cdk::Reply *m_reply;

  Impl(XSession &sess)
    : m_sess(sess), m_reply(NULL)
  {}
  Impl(Collection &coll)
    : m_sess(coll.m_schema.m_sess), m_reply(NULL)
  {}

  ~Impl() {}

  cdk::Session& get_cdk_session() { return m_sess.get_cdk_session(); }

  bool is_completed() { return m_reply->is_completed(); }
  void cont() { m_reply->cont(); }

  BaseResult wait()
  {
    m_reply->wait();
    if (0 < m_reply->entry_count())
      m_reply->get_error().rethrow();
    return get_result();
  }

  virtual BaseResult get_result()
  {
    return BaseResult(m_reply);
  }

  friend class Task;
  friend class Executable;
};


// Implementation of Task API using internal implementation object

inline
Task::~Task() try { delete m_impl; } CATCH_AND_WRAP

inline
bool Task::is_completed()
try { return m_impl ? m_impl->is_completed() : true; } CATCH_AND_WRAP

inline
BaseResult Task::wait()
try {
  if (!m_impl)
    throw Error("Attempt to wait on empty task");
  return m_impl->wait();
} CATCH_AND_WRAP

inline
void Task::cont()
try {
  if (!m_impl)
    throw Error("Attempt to continue an empty task");
  m_impl->cont();
} CATCH_AND_WRAP

inline
void Task::reset(Impl *impl)
{
  delete m_impl;
  m_impl = impl;
}

}

#endif
