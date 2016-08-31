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

class Mysqlx_exception
{
public:

  enum Mysqlx_exception_type {
    MYSQLX_EXCEPTION_INTERNAL, MYSQLX_EXCEPTION_EXTERNAL
  };

  Mysqlx_exception(Mysqlx_exception_type t, uint32_t code, std::string message) :
    m_type(t), m_code(code), m_message(message)
  {}

  Mysqlx_exception(std::string message) :
    m_type(MYSQLX_EXCEPTION_INTERNAL), m_code(0), m_message(message)
  {}

  std::string message() const { return m_message; }
  Mysqlx_exception_type type() const { return m_type; }
  uint32_t code() const { return m_code; }

private:
  Mysqlx_exception_type m_type;
  uint32_t m_code;
  std::string m_message;
};


typedef struct mysqlx_error_struct mysqlx_error_t;

class Mysqlx_diag_base
{
public:
  virtual mysqlx_error_t * get_error() = 0;
};

typedef struct mysqlx_error_struct : public Mysqlx_diag_base
{
  std::string m_message;
  unsigned int m_error_num;
  bool m_is_warning;

  mysqlx_error_struct() : m_error_num(0), m_is_warning(false)
  {}

  mysqlx_error_struct(const char *m, unsigned int n, bool is_warning = false) : m_is_warning(is_warning)
  {
    set(m, n);
  }
  
  mysqlx_error_struct(const cdk::Error* cdk_error, bool is_warning = false) : m_is_warning(is_warning)
  {
    set(cdk_error);
  }

  mysqlx_error_struct(const cdk::Error &cdk_error, bool is_warning = false) : m_is_warning(is_warning)
  {
    set(&cdk_error);
  }

  void set(const Mysqlx_exception &ex)
  {
    m_message = ex.message();
    m_error_num = ex.code();
  }

  void set(const char *m, unsigned int n)
  {
    m_message = std::string(m);
    m_error_num = n;
  }

  void set(const cdk::Error* cdk_error)
  {
    if (!cdk_error)
    {
      m_message = "";
      m_error_num = 0;
      return;
    }

    m_message = cdk_error->description();

    if (!m_is_warning || cdk_error->code().category() == cdk::server_error_category())
      m_error_num = (unsigned int)cdk_error->code().value();
    else
      m_error_num = 0;
  }

  void reset()
  {
    set((const cdk::Error*)NULL);
  }

  unsigned int error_num()
  {
    return m_error_num;
  }

  const char* message()
  {
    return m_message.size() ? m_message.data() : NULL;
  }

  mysqlx_error_t * get_error()
  {
    if (message() || error_num())
      return this;
    return NULL;
  }

  virtual ~mysqlx_error_struct()
  {}
} mysqlx_error_t;

class Mysqlx_diag : public Mysqlx_diag_base
{
  protected:
  mysqlx_error_t m_error;

  public:
  virtual void set_diagnostic(const Mysqlx_exception &ex)
  { m_error.set(ex); }

  virtual void set_diagnostic(const char *msg, unsigned int num)
  { m_error.set(msg, num); }

  mysqlx_error_t * get_error()
  {
    if (m_error.message() || m_error.error_num())
      return &m_error;
    return NULL;
  }

  virtual ~Mysqlx_diag() {}
};

