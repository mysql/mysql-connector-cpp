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
  Task implementation
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
  cdk::Reply *m_reply = NULL;

  Impl(XSession &sess)
    : m_sess(sess)
  {}
  Impl(Collection &coll)
    : m_sess(coll.m_schema.m_sess)
  {}
  Impl(Table &tbl)
    : m_sess(tbl.m_schema.m_sess)
  {}

  virtual ~Impl() {}

  cdk::Session& get_cdk_session() { return m_sess.get_cdk_session(); }


  virtual cdk::Reply* send_command() = 0;

  void init()
  {
    if (m_reply)
      return;
    m_reply = send_command();
  }

  bool is_completed()
  {
    init();
    return m_reply->is_completed();
  }

  void cont()
  {
    init();
    m_reply->cont();
  }

  BaseResult wait()
  {
    init();
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



struct Value::Access
{
  static Value mk_raw(const cdk::bytes data)
  {
    Value ret;
    ret.m_type = Value::RAW;
    ret.m_str.assign(data.begin(), data.end());
    return std::move(ret);
  }

  static Value mk_doc(const string &json)
  {
    Value ret;
    ret.m_type = Value::DOCUMENT;
    ret.m_doc = DbDoc(json);
    return std::move(ret);
  }

  static cdk::bytes get_bytes(const Value &val)
  {
    return cdk::bytes(val.m_str);
  }
};

}

#endif
