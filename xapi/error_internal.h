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

class MYSQLX_EXCEPTION
{
public:

  enum mysqlx_exception_type {
    MYSQLX_EXCEPTION_INTERNAL, MYSQLX_EXCEPTION_EXTERNAL
  };

  MYSQLX_EXCEPTION(mysqlx_exception_type t, uint32_t code, std::string message) :
    m_type(t), m_code(code), m_message(message)
  {}

  MYSQLX_EXCEPTION(std::string message) :
    m_type(MYSQLX_EXCEPTION_INTERNAL), m_code(0), m_message(message)
  {}

  std::string message() const { return m_message; }
  mysqlx_exception_type type() const { return m_type; }
  uint32_t code() const { return m_code; }

private:
  mysqlx_exception_type m_type;
  uint32_t m_code;
  std::string m_message;
};


typedef struct MYSQLX_ERROR_T
{
  std::string m_message;
  unsigned int m_error_num;
  const cdk::Error* m_cdk_error;
  bool m_is_warning;

  MYSQLX_ERROR_T() : m_error_num(0), m_cdk_error(NULL), m_is_warning(false)
  {}

  MYSQLX_ERROR_T(const char *m, unsigned int n, bool is_warning = false) : m_cdk_error(NULL), m_is_warning(is_warning)
  {
    set(m, n);
  }
  
  MYSQLX_ERROR_T(const cdk::Error* cdk_error, bool is_warning = false) : m_cdk_error(NULL), m_is_warning(is_warning)
  {
    set(cdk_error);
  }

  MYSQLX_ERROR_T(const cdk::Error &cdk_error, bool is_warning = false) : m_cdk_error(NULL), m_is_warning(is_warning)
  {
    set(&cdk_error);
  }

  void set(const MYSQLX_EXCEPTION &ex)
  {
    m_message = ex.message();
    m_error_num = ex.code();
    if(m_cdk_error)
      delete m_cdk_error;
    m_cdk_error = NULL;
  }

  void set(const char *m, unsigned int n)
  {
    m_message = std::string(m);
    m_error_num = n;
    if(m_cdk_error)
      delete m_cdk_error;
    m_cdk_error = NULL;
  }

  void set(const cdk::Error* cdk_error)
  {
    m_message = "";
    m_error_num = 0;

    if(m_cdk_error)
      delete m_cdk_error;

    m_cdk_error = (cdk_error ? cdk_error->clone() : NULL);
  }

  void reset()
  {
    set((const cdk::Error*)NULL);
  }

  unsigned int error_num()
  {
    if (m_cdk_error)
    {
      if (!m_is_warning || m_cdk_error->code().category() == cdk::server_error_category())
        m_error_num = (unsigned int)m_cdk_error->code().value();
      else
        m_error_num = 0;
    }

    return m_error_num;
  }

  const char* message()
  {
    if (m_cdk_error)
    {
      m_message = m_cdk_error->description();
    }
    return m_message.size() ? m_message.data() : NULL;
  }

  ~MYSQLX_ERROR_T()
  {
    if(m_cdk_error)
      delete m_cdk_error;
  }

} MYSQLX_ERROR;
