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

#ifndef MYSQLX_RESULT_IMPL_H
#define MYSQLX_RESULT_IMPL_H


#include <mysql_devapi.h>
#include <mysql/cdk.h>

#include "../global.h"


namespace mysqlx {

// TODO: Use std::variant when available
using cdk::foundation::variant;

/*
  Convenience wrapper around std container that is used
  to store incoming raw bytes sequence.
*/

class Buffer
{
  std::vector<byte> m_impl;

public:

  void append(bytes data)
  {
    m_impl.insert(m_impl.end(), data.begin(), data.end());
  }

  size_t size() const { return m_impl.size(); }

  cdk::bytes data() const
  {
    return cdk::bytes((byte*)m_impl.data(), m_impl.size());
  }
};


/*
  Handling column meta-data information
  =====================================

  Meta-data for result columns is provided by CDK cursor object which implements
  cdk::Meta_data interface. This information is read from cursor
  in Result_base::Impl::init() method and is stored in m_mdata member of type
  Meta_data.

  Meta_data class contains a map from column positions to instances of Column
  class. Each Column instance can store meta-data information for a single
  column. The Column instances are created in the Meta_data constructor which
  reads meta-data information from cdk::Meta_data interface and adds Column
  objects using add() methods.

  The CDK meta-data for a single column consists of:

  - CDK type constant (cdk::Type_info)
  - encoding format information (cdk::Format_info)
  - additional column information (cdk::Column_info)

  The first two items describe the type and encoding of the values that appear
  in the result in the corresponding column. Additional column information
  is mainly the column name etc.

  Classes Format_descr<TTT> and Format_info are used to store type and encoding
  format information for each column. For CDK type TTT, class Format_descr<TTT>
  stores encoding format descriptor (instance of cdk::Format<TTT> class) and
  encoder/decoder for the values (instance of cdk::Codec<TTT> class)
  (Note: for some types TTT there is no codec or no format descriptor). Class
  Format_info is a variant class that can store Format_descr<TTT> values
  for different TTT.

  Class Column::Impl extends Format_info with additional storage for
  the cdk::Column_info data (column name etc). Class Column uses information
  stored in Column::Impl to implement the DevAPI Column interface.

  Using meta-data to decode result values
  ---------------------------------------

  Meta-data information stored in m_mdata member of RowResult::Impl class
  is used to interpret raw bytes returned by CDK in the reply to a query.
  This interpretation is done by Row::Impl class whose instance is created,
  for example, in RowResult::fetchOne() passing the metad-data information
  to Row::Impl constructor. This meta-data is used in Row::get() method, which
  first checks the type of the value and then uses get<TTT>() method which
  calls appropriate convert() method after extracting encoding format
  information from the meta-data.

  Presenting meta-data via DevAPI Column interface
  ------------------------------------------------

  This is done by the Column class, using column meta-data stored in
  the Column::Impl instance. The type and encoding format information must
  be translated to types defined by DevAPI. This translation happens
  in Column::getType() method. For example, a CDK column of type FLOAT can
  be reported as DevAPI type FLOAT, DOUBLE or DECIMAL, depending on the
  encoding format that was reported by CDK. Additional encoding information
  is exposed via other DevAPI Column methods such as isNumberSigned().
  The information from cdk::Column_info interface is extracted and stored
  in Column::Impl class by store_info() method. Then it is exposed by relevant
  DevAPI Column methods.
*/


template <cdk::Type_info T>
struct Format_descr
{
  cdk::Format<T> m_format;
  cdk::Codec<T>  m_codec;

  Format_descr(const cdk::Format_info &fi)
    : m_format(fi), m_codec(fi)
  {}
};

template <>
struct Format_descr<cdk::TYPE_DOCUMENT>
{
  cdk::Format<cdk::TYPE_DOCUMENT> m_format;
  cdk::Codec<cdk::TYPE_DOCUMENT>  m_codec;

  Format_descr(const cdk::Format_info &fi)
    : m_format(fi)
  {}
};


/*
  Phony Format_descr<> structure used for raw bytes and values
  of types which we do not process in any way (but present as
  raw bytes).
*/

template <>
struct Format_descr<cdk::TYPE_BYTES>
{};

/*
  Note: we do not decode temporal values yet, thus there is
  no codec in Format_descr class.
*/

template<>
struct Format_descr<cdk::TYPE_DATETIME>
{
  cdk::Format<cdk::TYPE_DATETIME> m_format;

  Format_descr(const cdk::Format_info &fi)
    : m_format(fi)
  {}
};


/*
  Note: For GEOMETRY and XML types we do not decode the values.
  Also, CDK does not provide any encoding format information -
  GEOMETRY uses some unspecified MySQL intenral representation
  format and XML format is well known.
*/

template<>
struct Format_descr<cdk::TYPE_GEOMETRY>
{
  Format_descr(const cdk::Format_info &)
  {}
};

template<>
struct Format_descr<cdk::TYPE_XML>
{
  Format_descr(const cdk::Format_info &)
  {}
};


/*
  Structure Format_info holds information about the type
  of a column (m_type) and about its encoding format in
  Format_descr<T> structure. Since C++ type of Format_descr<T>
  is different for each T, a boost::variant is used to store
  the appropriate Format_descr<T> value.
*/


typedef variant <
  Format_descr<cdk::TYPE_STRING>,
  Format_descr<cdk::TYPE_INTEGER>,
  Format_descr<cdk::TYPE_FLOAT>,
  Format_descr<cdk::TYPE_DOCUMENT>,
  Format_descr<cdk::TYPE_BYTES>,
  Format_descr<cdk::TYPE_DATETIME>,
  Format_descr<cdk::TYPE_GEOMETRY>,
  Format_descr<cdk::TYPE_XML>
> Format_info_base;


struct Format_info
  : public Format_info_base
{
  cdk::Type_info m_type;

  template <cdk::Type_info T>
  Format_info(const Format_descr<T> &fd)
    : Format_info_base(fd), m_type(T)
  {}

  Format_info(cdk::Type_info type)
    : Format_info_base(Format_descr<cdk::TYPE_BYTES>())
    , m_type(type)
  {}

  template <cdk::Type_info T>
  Format_descr<T>& get() const
  {
    /*
      Note: we cast away constness here, because using a codec can
      modify it, and thus the Format_descr<T> must be mutable.
    */
    return const_cast<Format_descr<T>&>(
            Format_info_base::get<Format_descr<T>>()
           );
  }

};


/*
  Helper classes to access private members of some classes from the
  implementation.
*/

struct internal::Result_detail::Access
{
  using Impl = Result_detail::Impl;
};


struct internal::Column_detail::Access
{
  using Impl = Column_detail::Impl;

  //Column_detail mk(const std::shared_ptr<Impl> &impl)
  //{
  //  return Column_detail(impl);
  //}
};


/*
  Internal implementation for Column objects.
*/

class internal::Column_detail::Impl : public Format_info
{
public:

  string m_name;
  string m_label;
  string m_table_name;
  string m_table_label;
  string m_schema_name;

  unsigned long m_length;
  unsigned short m_decimals;
  cdk::collation_id_t m_collation;

  template <typename T>
  Impl(const T &init) : Format_info(init)
  {}

  void store_info(const cdk::Column_info &ci)
  {
    m_name = ci.orig_name();
    m_label = ci.name();

    if (ci.table())
    {
      m_table_name = ci.table()->orig_name();
      m_table_label = ci.table()->name();

      if (ci.table()->schema())
        m_schema_name = ci.table()->schema()->name();
    }

    m_collation = ci.collation();
    m_length = ci.length();
    assert(ci.decimals() < std::numeric_limits<short unsigned>::max());
    m_decimals = static_cast<short unsigned>(ci.decimals());
  }

};


using Column_impl = internal::Column_detail::Access::Impl;
using Col_impl_ptr = std::shared_ptr<Column_impl>;


/*
  Meta_data holds type and format information for all columns in
  a result. This information is stored in Column_impl objects that
  can be used to construct Column instances.

  A Meta_data instance is filled given information provided by
  cdk::Meta_data interface.
*/

struct Meta_data
  : private std::map<cdk::col_count_t, Col_impl_ptr>
{
  Meta_data(cdk::Meta_data&);

  col_count_t col_count() const { return m_col_count;  }

  const Format_info& get_format(cdk::col_count_t pos) const
  {
    return  *get_column(pos);
  }

  cdk::Type_info get_type(cdk::col_count_t pos) const
  {
    return get_format(pos).m_type;
  }

  const Col_impl_ptr get_column(cdk::col_count_t pos) const
  {
    return at(pos);
  }

private:

  cdk::col_count_t  m_col_count = 0;

  /*
    Add to this Meta_data instance information about column
    at position `pos`. The type and format information is given
    by cdk::Format_info object, aditional column meta-data by
    cdk::Column_info object.
  */

  template<cdk::Type_info T>
  void add(cdk::col_count_t pos,
           const cdk::Column_info &ci, const cdk::Format_info &fi)
  {
    Column_impl *col = new Column_impl(Format_descr<T>(fi));
    col->store_info(ci);

    emplace(pos, Col_impl_ptr(col));
  }

  /*
    Add raw column information (whose values are presented as
    raw bytes).
  */

  void add_raw(cdk::col_count_t pos,
               const cdk::Column_info &ci, cdk::Type_info type)
  {
    Column_impl *col = new Column_impl(type);
    col->store_info(ci);

    emplace(pos, Col_impl_ptr(col));
  }

  friend internal::Result_detail::Access::Impl;
};


/*
  Data structure used to hold raw row data. It holds a Buffer with
  raw bytes for each non-null field of a row.
*/

typedef std::map<col_count_t, Buffer> Row_data;


/*
  Internal implementation for Result objects.
*/

struct internal::Result_detail::Impl
  : public cdk::Row_processor
{
  using Session_impl = internal::Session_detail::Impl;
  using Session_impl_ptr = std::shared_ptr<Session_impl>;

  /*
    Note: Session implementation must exists as long as this result
    implementation exists. Thus we keep shared pointer to it to make sure
    that session implementation is not destroyed when session object itself
    is deleted.
  */

  Session_impl_ptr  m_sess;
  cdk::Reply  *m_reply = NULL;
  cdk::Cursor *m_cursor = NULL;
  Row_data     m_row;
  // Note: meta-data is shared with Row instances
  std::shared_ptr<Meta_data>  m_mdata;
  std::vector<GUID>           m_guid;
  bool                        m_cursor_closed = false;

  std::forward_list<Row_data> m_row_cache;
  row_count_t m_row_cache_size = 0;
  bool m_cache = false;


  Impl(const Session_impl_ptr &sess, cdk::Reply *r)
    :  m_sess(sess), m_reply(r)
  {
    init();
  }

  Impl(const Session_impl_ptr &sess, cdk::Reply *r, const std::vector<GUID> &guids)
    : m_sess(sess), m_reply(r), m_guid(guids)
  {
    init();
  }

  virtual ~Impl();

  void init();


  void clear_cache()
  {
    m_row_cache.clear();
    m_row_cache_size = 0;
    m_cache = false;
  }

  bool has_data() const
  {
    return NULL != m_cursor;
  }

  bool next_result()
  {
    /*
      Note: closing cursor discards previous rset. Only then
      we can move to the next rset (if any).
    */

    if (m_cursor)
      m_cursor->close();

    if (!m_reply || !m_reply->has_results())
      return false;

    init();
    return true;
  }


  /*
    This method is called when a session with which this result is registered
    wants to de-registers it, either because another result is being registered
    or the session does clean-ups before shutting down.

    This gives the result a chance to handle pending data in the reply before
    session is gone or moves on to next result.
  */

  void deregister();

  /*
    Discard the CDK reply object owned by the implementation. This
    is called when the corresponding session is about to be closed
    and the reply object will be no longer valid.
  */

  void discard_reply()
  {
    if (!m_reply)
      return;

    delete m_reply;
    m_reply = NULL;
  }

  /*
    Read next row from the cursor. Returns NULL if there are no
    more rows. Throws exeption if this result has no data.
  */

  const Row_data *get_row();
  row_count_t count();

  col_count_t get_col_count() const
  {
    if (!m_cursor)
      THROW("No result set");
    return m_cursor->col_count();
  }

  std::shared_ptr<Column_impl> get_column(col_count_t pos) const
  {
    if (!m_cursor)
      THROW("No result set");
    return m_mdata->get_column(pos);
  }

  cdk::row_count_t get_affected_rows() const
  {
    if (!m_reply)
      THROW("Attempt to get affected rows count on empty result");
    return m_reply->affected_rows();
  }

  cdk::row_count_t get_auto_increment() const
  {
    if (!m_reply)
      THROW("Attempt to get auto increment value on empty result");
    return m_reply->last_insert_id();
  }

  unsigned get_warning_count() const
  {
    if (!m_reply)
      THROW("Attempt to get warning count for empty result");
    const_cast<Impl*>(this)->load_warnings();
    return m_reply->entry_count(cdk::api::Severity::WARNING);
  }

  std::vector<Warning> m_warnings;
  bool m_all_warnings = false;

  Warning get_warning(unsigned pos) const
  {
    return m_warnings.at(pos);
  }

  void load_warnings();

  // Row_processor

  bool row_begin(row_count_t)
  {
    m_row.clear();
    return true;
  }
  void row_end(row_count_t) {}

  size_t field_begin(col_count_t pos, size_t);
  void   field_end(col_count_t) {}
  void   field_null(col_count_t) {}
  size_t field_data(col_count_t pos, bytes);
  void   end_of_data() {}

  friend internal::Row_result_detail;
  friend internal::Result_detail;
  friend internal::Result_base;
  friend mysqlx::Result;
  friend mysqlx::RowResult;
  friend mysqlx::SqlResult;
};


/*
  Specialized implementation for DocResult.

  This implementation takes rows from RowResult and builds a document
  using JSON data in the row.
*/


//struct internal::Doc_result_detail::Impl
//  : internal::Result_detail::Impl
//{
//  using Result_impl = internal::Result_detail::Impl;
//
//  Impl(Result_impl &&init)
//    : Result_impl(std::move(init))
//  {}
//
//  /*
//    Return JSON representation of the next document in the result or
//    empty string if no more documents.
//  */
//
//  std::string get_doc()
//  {
//    const Row_data *row = get_row();
//
//    if (!row)
//      return std::string();
//
//    // @todo Avoid copying of document string.
//    bytes data = row->at(0).data();
//    return std::string(data.begin(),data.end()-1);
//  }
//
//  uint64_t count_docs()
//  {
//    return count();
//  }
//
//  friend DocResult;
//  friend Doc_result_detail;
//};


}  // mysqlx namespace

#endif
