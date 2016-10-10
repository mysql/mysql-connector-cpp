/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C is licensed under the terms of the GPLv2
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

#include <mysql_xapi.h>
#include "mysqlx_cc_internal.h"
#include <mysql/cdk/api/expression.h>
#include <mysql/cdk/api/query.h>

class mysqlx_result_t::Row_processor : public cdk::Row_processor
{
  bool m_buffer_created;
  size_t m_full_data_len;

public:
  mysqlx_row_t *m_row;

  /*
    Passing NULL for the constructor creates the processor, which
    discards all data.
  */
  Row_processor(mysqlx_row_t *row) : m_buffer_created(false),
                                   m_full_data_len(0),
                                   m_row(row)
  {}

  /*
    Group of functions implementing Row_processor interface
  */
  bool row_begin(row_count_t)
  {
    if(!m_row)
      return false;

    m_row->clear();
    return true;
  }

  void row_end(row_count_t) {}

  size_t field_begin(col_count_t, size_t data_len)
  {
    m_full_data_len = data_len;
    m_buffer_created = false; // reset the flag used in field_data()
    return data_len;
  }

  void field_end(col_count_t) {}

  void field_null(col_count_t)
  {
    if(!m_row) return;
    m_row->add_field_null();
  }

  size_t field_data(col_count_t pos, bytes data)
  {
    if(!m_row) return 0;

    if(!m_buffer_created)
    {
      m_row->add_field_data(data, m_full_data_len);
      m_buffer_created = true;
    }
    else
      m_row->append_field_data(pos, data);

    m_full_data_len -= data.size();
    // return the remainig number of bytes
    return m_full_data_len;
  }

  void end_of_data() {}
};


mysqlx_result_t::mysqlx_result_struct(mysqlx_stmt_t &parent, cdk::Reply &reply) :
                                  m_current_row(0),
                                  m_reply(reply),/*, m_pos(0),*/
                                  m_row_proc(NULL),
                                  m_crud(parent),
                                  m_store_result(false),
                                  m_filter_mask(0),
                                  m_current_id_index(0)
{
  init_result(true);
}

bool mysqlx_result_t::init_result(bool wait)
{
  if (wait)
    m_reply.wait();

  m_current_warning_index = 0;
  m_current_error_index = 0;

  if (m_reply.has_results())
  {
    m_cursor = new cdk::Cursor(m_reply);
    m_cursor->wait();

    if (m_cursor->col_count())
    {
      m_col_info.resize(m_cursor->col_count()); // prepare to buffer column metadata
    }
  }
  else
  {
    m_cursor = NULL;
  }

  // Processing warnings and errors
  if (m_reply.entry_count(cdk::foundation::api::Severity::ERROR))
  {
    /*
      MYSQLX_EXCEPTION_EXTERNAL means that the list of errors
      should be accessed through cdk::Reply
    */
    throw Mysqlx_exception(Mysqlx_exception::MYSQLX_EXCEPTION_EXTERNAL, 0, "");
  }

  return (m_cursor != NULL);
}

/*
  Get metadata information such as column name, table, etc that could
  be represented by character strings
*/
const char *mysqlx_result_t::column_get_info_char(uint32_t pos, col_info_type info_type)
{
  if (!m_cursor || !m_cursor->col_count() || pos >= m_cursor->col_count())
    return NULL; // if no columns in the result

  /* Buffer the column metadata if needed */
  if (!m_col_info[pos].inited())
    m_col_info[pos].set_info(m_cursor->col_info(pos));

  switch (info_type)
  {
    case COL_INFO_NAME: return m_col_info[pos].name();
    case COL_INFO_ORIG_NAME: return m_col_info[pos].orig_name();
    case COL_INFO_TABLE: return m_col_info[pos].table_name();
    case COL_INFO_ORIG_TABLE: return m_col_info[pos].table_orig_name();
    case COL_INFO_SCHEMA: return m_col_info[pos].schema();
    case COL_INFO_CATALOG: return m_col_info[pos].catalog();
    default: // the rest of info types are processed in column_get_info_int
      break;
  }

  return NULL;
}

/*
  Get metadata information such as column precision, flags, etc that could
  be represented by numbers
*/
uint32_t mysqlx_result_t::column_get_info_int(uint32_t pos, col_info_type info_type)
{
  if (!m_cursor || !m_cursor->col_count() || pos >= m_cursor->col_count())
    return 0; // if no columns in the result

  //if ((cdk::Format<cdk::TYPE_INTEGER>)m_cursor->format(pos)).is_unsigned()))
  switch (info_type)
  {
    case COL_INFO_TYPE:
      switch (m_cursor->type(pos))
      {
        case cdk::TYPE_INTEGER:
        {
          cdk::Format<cdk::TYPE_INTEGER> format(m_cursor->format(pos));
          if (format.length() == 1)
            return MYSQLX_TYPE_BOOL;

          if (format.is_unsigned())
            return MYSQLX_TYPE_UINT;

          return MYSQLX_TYPE_SINT;
        }
        case cdk::TYPE_FLOAT:
        {
          cdk::Format<cdk::TYPE_FLOAT> format(m_cursor->format(pos));

          if (format.type() == cdk::Format<cdk::TYPE_FLOAT>::FLOAT)
            return MYSQLX_TYPE_FLOAT;
          else if (format.type() == cdk::Format<cdk::TYPE_FLOAT>::DOUBLE)
            return MYSQLX_TYPE_DOUBLE;
          else
            return MYSQLX_TYPE_DECIMAL;
        }
        case cdk::TYPE_DATETIME:
        {
          cdk::Format<cdk::TYPE_DATETIME> format(m_cursor->format(pos));
          switch(format.type())
          {
            case cdk::Format<cdk::TYPE_DATETIME>::TIME:
              return MYSQLX_TYPE_TIME;
            case cdk::Format<cdk::TYPE_DATETIME>::TIMESTAMP:
              return MYSQLX_TYPE_TIMESTAMP;
            default:
              return MYSQLX_TYPE_DATETIME;
          }
          break;
        }
        // TODO: differ the blob types by their length
        case cdk::TYPE_BYTES:
        {
          // TODO: use it when GEOMETRY type is supported by CDK
          // cdk::Format<cdk::TYPE_BYTES> format(m_cursor->format(pos));
          return MYSQLX_TYPE_BYTES;
        }
        case cdk::TYPE_STRING:
        {
          cdk::Format<cdk::TYPE_STRING> format(m_cursor->format(pos));
          if (format.is_enum())
            return MYSQLX_TYPE_ENUM;
          else if (format.is_set())
            return MYSQLX_TYPE_SET;

          return MYSQLX_TYPE_STRING;
        }
        case cdk::TYPE_DOCUMENT:
          return MYSQLX_TYPE_JSON;
        case cdk::TYPE_GEOMETRY:
          return MYSQLX_TYPE_GEOMETRY;
        default:
          return m_cursor->type(pos);
      }

    case COL_INFO_LENGTH: return m_cursor->col_info(pos).length();
    case COL_INFO_PRECISION: return m_cursor->col_info(pos).decimals();

    // TODO: collation and flags should be added later
    case COL_INFO_FLAGS:
    case COL_INFO_COLLATION: return 0;
    default: // the rest of info types are processed in column_get_info_char
      break;
  }

  return 0;
}

void mysqlx_result_t::set_table_list_mask(uint32_t mask)
{ m_filter_mask = mask; }

/*
  The method which can filter out rows returned by the server on the client side.
  Returns true if the row is allowed to go through or false otherwise.
*/
bool mysqlx_result_t::row_filter(mysqlx_row_t *row)
{
  if (m_crud.op_type() != OP_ADMIN_LIST || row->row_size() < 2)
    return true;

  /*
    If the operation type is OP_ADMIN_LIST and mask is set
    the column should be a NULL-terminated string
  */
  std::string s = (char *)row->get_col_data(1).begin();
  if ((m_filter_mask & FILTER_TABLE) && s.compare("TABLE") == 0)
    return true;
  if ((m_filter_mask & FILTER_COLLECTION) && s.compare("COLLECTION") == 0)
    return true;
  if ((m_filter_mask & FILTER_VIEW) && s.compare("VIEW") ==0 )
    return true;

  return false;
}


/*
  Read the next row from the result set and advance the cursor position
*/
mysqlx_row_t *mysqlx_result_t::read_row()
{
  if(!m_store_result)
  {
    clear_rows();

    if(!m_cursor)
      return NULL;

    /*
      We use m_row_set as the main storage for the data.
      In case of use_result only one entry in m_row_set is used.
      If any error/exception happens the allocated mysqlx_row_t will be
      freed anyway because the pointer is stored in m_row_set.
    */
    m_row_set.push_back(new mysqlx_row_t(*this));
    Row_processor row_proc(m_row_set[0]);

    bool row_is_read;

READING_NEXT_ROW:

    row_is_read = m_cursor->get_row(row_proc);

    // row_filter() will be called only if filter mask is set
    if (row_is_read && (!m_filter_mask || row_filter(m_row_set[0])))
      return m_row_set[0];
    else
    {
      // If row was not allowed through the filter the next row should be read
      if (row_is_read)
        goto READING_NEXT_ROW;

      delete m_row_set[0];
      m_row_set.erase(m_row_set.begin());

      if(m_reply.entry_count())
      {
        const cdk::Error &cdkerr = m_reply.get_error();
        set_diagnostic(cdkerr.what(), (unsigned int)cdkerr.code().value());
      }
    }
  }
  else
  {
    m_current_row++;
    if (m_current_row - 1 < m_row_set.size())
      return m_row_set[(unsigned int)m_current_row - 1];
  }

  return NULL;
}


/*
  Read the next document from the result and advance the cursor position
*/
mysqlx_doc_t *mysqlx_result_t::read_doc()
{
  if(!m_store_result)
  {
    clear_docs();

    if(!m_cursor)
      return NULL;

    /*
      We use m_doc_set as the main storage for the data.
      In case of use_result only one entry in m_row_set is used.
      If any error/exception happens the allocated mysqlx_doc_t will be
      freed anyway because the pointer is stored in m_doc_set.
    */
    mysqlx_row_t row(*this);
    Row_processor row_proc(&row);
    if (m_cursor->get_row(row_proc))
    {
      m_doc_set.push_back(new mysqlx_doc_t(row.get_col_data(0)));
      return m_doc_set[0];
    }
    else if(m_reply.entry_count())
    {
      const cdk::Error &cdkerr = m_reply.get_error();
      set_diagnostic(cdkerr.what(), (unsigned int)cdkerr.code().value());
    }
  }
  else
  {
/*    m_current_row++;
    if (m_current_row - 1 < m_doc_set.size())
      return m_row_set[(unsigned int)m_current_row - 1];
*/
  }

  return NULL;
}


/*
  Read the next JSON string from the result and advance the cursor position
*/
const char * mysqlx_result_t::read_json(size_t *json_byte_size)
{
  if(!m_store_result)
  {
    clear_rows();

    if(!m_cursor)
      return NULL;

    /*
      We use m_row_set as the main storage for the data.
      In case of use_result only one entry in m_row_set is used.
      If any error/exception happens the allocated mysqlx_row_t will be
      freed anyway because the pointer is stored in m_row_set.
    */
    m_row_set.push_back(new mysqlx_row_t(*this));
    Row_processor row_proc(m_row_set[0]);
    if (m_cursor->get_row(row_proc))
    {
      cdk::bytes b = m_row_set[0]->get_col_data(0);
      if (json_byte_size)
        *json_byte_size = b.size();

      return (const char *)b.begin();
    }
    else if(m_reply.entry_count())
    {
      const cdk::Error &cdkerr = m_reply.get_error();
      set_diagnostic(cdkerr.what(), (unsigned int)cdkerr.code().value());
    }
  }
  else
  {
    m_current_row++;
    if (m_current_row - 1 < m_row_set.size())
    {
      cdk::bytes b = m_row_set[(unsigned int)m_current_row - 1]->get_col_data(0);
      if (json_byte_size)
       *json_byte_size = b.size();

      return (const char *)b.begin();
    }
  }

  json_byte_size = 0;
  return NULL;
}


size_t mysqlx_result_t::store_result()
{
  if (m_store_result)
    return 0; // the function was already called for this result

  mysqlx_op_t op = m_crud.op_type();

  if (op != OP_FIND && op != OP_SELECT && op != OP_SQL && op != OP_ADMIN_LIST)
    throw Mysqlx_exception(Mysqlx_exception::MYSQLX_EXCEPTION_INTERNAL, 0,
                           "Wrong operation type. Operation result cannot be stored.");

  clear_rows();
  m_store_result = true;

  if(!m_cursor)
    return 0;

  bool row_exists = false;
  size_t row_num = 0;
  do
  {
    m_row_set.push_back(new mysqlx_row_t(*this));

READING_NEXT_ROW:
    Row_processor row_proc(m_row_set[row_num]);
    row_exists = m_cursor->get_row(row_proc);
    if (!row_exists)
    {
      delete m_row_set[row_num];
      m_row_set.erase(m_row_set.begin() + row_num);

      if (m_reply.entry_count())
      {
        const cdk::Error &cdkerr = m_reply.get_error();
        set_diagnostic(cdkerr.what(), (unsigned int)cdkerr.code().value());
      }
    }
    else
    {
      if (m_filter_mask && !row_filter(m_row_set[row_num]))
        goto READING_NEXT_ROW;
      ++row_num;
    }
  } while(row_exists);

  return m_row_set.size();
}

bool mysqlx_result_t::next_result()
{
  /*
    Reading the next result starts with opening of the new cursor.
    So, the old one must be closed and destroyed
  */
  close_cursor();

  /*
    New resultset needs a new buffer for the results
  */
  m_store_result = false;
  return init_result(true);
}

// Return number of rows affected by the last operation
uint64_t mysqlx_result_t::get_affected_count()
{
  return m_reply.affected_rows();
}

uint64_t mysqlx_result_t::get_auto_increment_value()
{
  return m_reply.last_insert_id();
}

uint32_t mysqlx_result_t::get_warning_count()
{
  /*
    Warnings are at the end of the resultset.
    Getting them requires finishing all pending reads.
  */
  m_reply.wait();
  return m_reply.entry_count(cdk::foundation::api::Severity::WARNING);
}

void mysqlx_result_t::copy_doc_ids(Doc_source &doc_src)
{
  m_current_id_index = 0;
  doc_src.copy_doc_ids(m_doc_id_list);
}

const char * mysqlx_result_t::get_next_doc_id()
{
  if (m_current_id_index >= m_doc_id_list.size())
    return NULL;

  ++m_current_id_index;
  return m_doc_id_list[m_current_id_index - 1].data();
}

mysqlx_error_t * mysqlx_result_t::get_error()
{
  mysqlx_error_t *err = Mysqlx_diag::get_error();
  
  if (err)
    return err; // return the error if there is any

  // Otherwise the error might be pending
  m_reply.wait();
  if (m_reply.entry_count() > m_current_error_index)
  {
    cdk::foundation::Diagnostic_iterator *m_iter;
    uint32_t num = 0;
    m_iter = &(m_reply.get_entries(cdk::foundation::api::Severity::ERROR));

    while (m_iter->next())
    {
      if (++num > m_current_error_index)
      {
        m_current_error_index = num;
        m_current_error.reset(new mysqlx_error_t(m_iter->entry().get_error(), true));
        return m_current_error.get();
      }
    }
  }

  return NULL;
}


mysqlx_error_t * mysqlx_result_t::get_next_warning()
{
  if (get_warning_count() > m_current_warning_index)
  {
    cdk::foundation::Diagnostic_iterator *m_iter;
    uint32_t num = 0;
    m_iter = &(m_reply.get_entries(cdk::foundation::api::Severity::WARNING));

    while (m_iter->next())
    {
      if (++num > m_current_warning_index)
      {
        m_current_warning_index = num;
        m_current_warning.reset(new mysqlx_error_t(m_iter->entry().get_error(), true));
        return m_current_warning.get();
      }
    }
  }

  return NULL;
}


void mysqlx_result_t::clear_rows()
{
  for(std::vector<mysqlx_row_t*>::iterator it = m_row_set.begin();
      it != m_row_set.end(); ++it)
  {
    delete *it;
  }
  m_current_row = 0;
  m_row_set.clear();
}

void mysqlx_result_t::clear_docs()
{
  for(std::vector<mysqlx_doc_t*>::iterator it = m_doc_set.begin();
      it != m_doc_set.end(); ++it)
  {
    delete *it;
  }
  m_current_row = 0;
  m_doc_set.clear();
}

void mysqlx_result_t::close_cursor()
{
  if (m_cursor)
  {
    m_cursor->close();
    cdk::Cursor *ptr= m_cursor;
    delete ptr;
    m_cursor = NULL;
  }
}
