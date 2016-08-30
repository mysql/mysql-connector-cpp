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

#ifndef __COMPAT_INTERNAL_H__
#define __COMPAT_INTERNAL_H__

/*
  Compatibility layer
*/
typedef struct MYSQL_RES_T
{
  /*
    Helper class that converts numbers and holds the char* values
    used in MYSQL_ROW returned to the user
  */
  class helper_row
  {
    std::vector<char*> m_row_values;

  public:

    template <typename T> char * add_value(T val)
    {
      std::ostringstream os;
      os << val;
      std::string str = os.str();
      return store_in_vector(str);
    }

    char * store_in_vector(std::string &s)
    {
      size_t sz = s.size();
      char * buf = new char[sz + 1]; // one more byte for \0
      memcpy(buf, s.c_str(), sz);
      buf[sz] = 0;
      return buf;
    }

    ~helper_row()
    {
      for (size_t i = 0; i < m_row_values.size(); ++i)
        delete m_row_values[i];
    }
  };


  mysqlx_result_t *m_result;
  MYSQL_FIELD *m_fields;
  std::vector<MYSQL_ROW> m_rowset;
  std::vector<helper_row> m_helper_rowset;
  bool m_store_result;
  uint32_t m_col_count;
  uint32_t m_cur_column;
  
  unsigned long *m_lengths;

  MYSQL_RES_T(mysqlx_result_t *result) : m_result(result), m_store_result(false), m_cur_column(0)
  {
    m_col_count = mysqlx_column_get_count(m_result);
    if (m_col_count)
    {
      m_lengths = new unsigned long[m_col_count];
      m_fields = new MYSQL_FIELD[m_col_count];
      for (uint32_t pos = 0; pos < m_col_count; ++pos)
      {
        m_lengths[pos] = result->column_get_info_int(pos, mysqlx_result_t::COL_INFO_LENGTH);
        fill_column_info(pos, &m_fields[pos]);
      }
    }
    else
    {
      m_lengths = NULL;
      m_fields = NULL;
    }
  }

  bool type_needs_conversion(enum_field_types t)
  {
     return (t == MYSQL_TYPE_LONGLONG || t == MYSQL_TYPE_FLOAT ||
             t == MYSQL_TYPE_DOUBLE || t == MYSQL_TYPE_DECIMAL ||
             t == MYSQL_TYPE_DATETIME);
  }

  void fill_column_info(uint32_t pos, MYSQL_FIELD *f)
  {
    f->name = const_cast<char*>(m_result->column_get_info_char(pos, mysqlx_result_t::COL_INFO_NAME));
    f->org_name = const_cast<char*>(m_result->column_get_info_char(pos, mysqlx_result_t::COL_INFO_ORIG_NAME));
    f->table = const_cast<char*>(m_result->column_get_info_char(pos, mysqlx_result_t::COL_INFO_TABLE));
    f->org_table = const_cast<char*>(m_result->column_get_info_char(pos, mysqlx_result_t::COL_INFO_ORIG_TABLE));
    f->db = const_cast<char*>(m_result->column_get_info_char(pos, mysqlx_result_t::COL_INFO_SCHEMA));

    f->length = m_result->column_get_info_int(pos, mysqlx_result_t::COL_INFO_LENGTH);
    f->flags =  m_result->column_get_info_int(pos, mysqlx_result_t::COL_INFO_FLAGS);
    f->type = (enum_field_types) m_result->column_get_info_int(pos, mysqlx_result_t::COL_INFO_TYPE);
  }

  void store_result()
  {
  }
  
  /*
    We need to clear the rowset explicitly for each row
  */
  void clear_rowset()
  {
    for (size_t i = 0; i < m_rowset.size(); ++i)
      delete m_rowset[i];
    m_cur_column = 0;
    m_rowset.clear();
  }

  MYSQL_ROW fetch_row()
  {
    MYSQL_ROW row = NULL;
    // TODO: rework for store_result()
    if (!m_store_result)
    {
      clear_rowset();
      m_helper_rowset.clear();

      if (m_col_count && m_fields && m_lengths)
      {
        mysqlx_row_t *xrow = m_result->read_row();
        if (!xrow)
          return NULL;

        row = new char*[m_col_count];
        m_rowset.push_back(row);
        m_helper_rowset.push_back(helper_row());

        for (uint32_t pos = 0; pos < m_col_count; ++pos)
        {
          switch (m_result->column_get_info_int(pos, mysqlx_result_t::COL_INFO_TYPE))
          {
            case MYSQL_TYPE_LONGLONG:
              if (m_fields[pos].flags & UNSIGNED_FLAG)
              {
                uint64_t val = 0;
                if (mysqlx_get_uint(xrow, pos, &val))
                  /*
                    In case of error there is no need to free memory here.
                    It will be taken care of in the destructor or upon fetching
                    another row.
                  */
                  return NULL;
                row[pos] = m_helper_rowset[0].add_value(val);
              }
              else
              {
                int64_t val = 0;
                if (mysqlx_get_sint(xrow, pos, &val))
                  return NULL;
                row[pos] = m_helper_rowset[0].add_value(val);
              }
              break;

            case MYSQL_TYPE_FLOAT:
            {
              float val = 0;
              if (mysqlx_get_float(xrow, pos, &val))
                return NULL;
              row[pos] = m_helper_rowset[0].add_value(val);
              break;
            }

            case MYSQL_TYPE_DOUBLE:
            {
              double val = 0;
              if (mysqlx_get_double(xrow, pos, &val))
                return NULL;
              row[pos] = m_helper_rowset[0].add_value(val);
              break;
            }

            // TODO: handle these types
            case MYSQL_TYPE_DECIMAL:
            case MYSQL_TYPE_DATETIME:
              return NULL;
            
            case MYSQL_TYPE_BLOB:
            case MYSQL_TYPE_STRING:
              row[pos] = (char *)xrow->get_col_data(pos).begin();
          }
        }
      }
    }

    return row;
  }

  MYSQL_FIELD *fetch_next_field()
  {
    if (m_cur_column < m_col_count)
    {
      ++m_cur_column;
      // m_cur_column is already incremented
      return &m_fields[m_cur_column - 1];
    }
    return NULL;
  }

  unsigned long *lenghts()
  { return m_lengths; }

  uint32_t column_get_count()
  { return m_col_count; }

  mysqlx_result_t *get() { return m_result; }

  ~MYSQL_RES_T()
  {
    if (m_lengths)
      delete m_lengths;
    if (m_fields)
      delete m_fields;
  }

}MYSQL_RES;


typedef struct MYSQL_T
{
  cdk::scoped_ptr<mysqlx_session_t> m_session;
  cdk::scoped_ptr<mysqlx_stmt_t> m_crud;
  char m_connect_error[MYSQLX_MAX_ERROR_LEN];
  int m_connect_error_code;

  mysqlx_error_t m_error;
  bool m_static_alloc;

  // Default constructor will be called if static variable is declared
  MYSQL_T() : m_static_alloc(true)
  {
    init();
  }

  /*
    Parameter specifiee if the class instance was statically allocated (true)
    or was created using the new operator (false)
  */
  MYSQL_T(bool static_alloc) : m_static_alloc(static_alloc)
  { 
    init();
  }

  void init()
  {
    m_connect_error[0] = 0;
    m_connect_error_code = 0;
  }

  mysqlx_stmt_t* get_crud() { return m_crud.get(); }
  bool set_crud(mysqlx_stmt_t *crud)
  {
    m_crud.reset(crud);
    return m_crud.get() != NULL;
  }

  mysqlx_result_t* get_result() { return m_crud->get_result(); }
  bool set_result(mysqlx_result_t *result) { return m_crud->set_result(result); }

  ~MYSQL_T()
  {}
  
} MYSQL;

#endif /* __COMPAT_INTERNAL_H__ */