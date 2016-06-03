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

#ifndef MYSQL_CDK_MYSQLX_TESTS_SESSION_TEST_H
#define MYSQL_CDK_MYSQLX_TESTS_SESSION_TEST_H

#include <mysql/cdk.h>
#include <iostream>

namespace cdk {
namespace test {


template <class MD>
class Row_processor : public cdk::Row_processor
{
protected:

  MD *m_md;

  Row_processor() : m_md(NULL)
  {}


  void set_meta_data(MD &md)
  {
    m_md= &md;
  }


  virtual void process_field_val(col_count_t pos, bytes data,
                                 uint64_t val)
  {
    std::cout << val;
  }

  virtual void process_field_val(col_count_t pos, bytes data,
                                 int64_t val)
  {
    std::cout << val;
  }

  virtual void process_field_val(col_count_t pos, bytes data,
                                 double val)
  {
    std::cout << val;
  }

  virtual void process_field_val(col_count_t pos, bytes data,
                                 const cdk::string &val)
  {
    std::cout << val;
  }

  virtual void process_field_doc(col_count_t pos, bytes data)
  {
    std::cout <<std::string(data.begin(), data.end());
  }

  virtual void process_field_bytes(col_count_t pos, bytes data)
  {
    std::cout << std::string(data.begin(), data.end());
  }

  // Row_processor callbacks

  virtual bool row_begin(row_count_t row)
  {
    std::cout << "Process Row Begin: "
              << row
              << std::endl;
    return true;
  }
  virtual void row_end(row_count_t row)
  {
    std::cout << "Process Row End: "
              << row
              << std::endl;
  }

  virtual void field_null(col_count_t pos)
  {
    field_begin(pos, 0);
    std::cout << "Null" << std::endl;
  }



  virtual size_t field_begin(col_count_t pos, size_t)
  {
    if (m_md)
    {
      const cdk::mysqlx::Column_ref &column = m_md->col_info(pos);
      const cdk::mysqlx::Table_ref* table = column.table();
      if (table)
      {
        const cdk::mysqlx::Schema_ref* schema = table->schema();
        if (schema)
        {
          if (schema->catalog())
          {
            std::cout << schema->catalog()->name()
                      << ".";
          }
          std::cout << schema->name()
                    << ".";
        }
        std::cout << table->name()
                  << ".";
      }
      std::cout << column.name();
    }

    std::cout << ": ";

    return  SIZE_MAX;
  }

  size_t field_data(col_count_t pos, bytes data)
  {
    if (!m_md)
      return 0;

    cdk::Type_info type        = m_md->type(pos);
    const cdk::Format_info &fi = m_md->format(pos);

    switch (type)
    {
    case cdk::TYPE_INTEGER:
      {
        cdk::Format<cdk::TYPE_INTEGER> fmt(fi);
        cdk::Codec<cdk::TYPE_INTEGER>  codec(fi);

        if (fmt.is_unsigned())
        {
          uint64_t val;
          codec.from_bytes(data, val);
          process_field_val(pos, data, val);
        }
        else
        {
          int64_t val;
          codec.from_bytes(data, val);
          process_field_val(pos, data, val);
        }
      };
      break;

    case cdk::TYPE_FLOAT:
      {
        cdk::Format<cdk::TYPE_FLOAT> fmt(fi);
        cdk::Codec<cdk::TYPE_FLOAT> codec(fi);

        if (fmt.type() == cdk::Format<cdk::TYPE_FLOAT>::FLOAT)
        {
          float val;
          codec.from_bytes(data, val);
          process_field_val(pos, data, val);
        }
        else if (fmt.type() == cdk::Format<cdk::TYPE_FLOAT>::DOUBLE)
        {
          double val;
          codec.from_bytes(data, val);
          process_field_val(pos, data, val);
        }
        else if (fmt.type() == cdk::Format<cdk::TYPE_FLOAT>::DECIMAL)
        {
          std::cout <<"<DECIMAL value>";
          // TODO: codec for decimal values not yet functional
          //double val;
          //codec.from_bytes(data, val);
          //process_field_val(pos, data, val);
        }
      };
      break;

    case cdk::TYPE_STRING:
      {
        cdk::Codec<cdk::TYPE_STRING> codec(fi);
        cdk::string val;
        codec.from_bytes(data, val);
        process_field_val(pos, data, val);
      };
      break;

    case cdk::TYPE_BYTES:
      {
        process_field_bytes(pos, data);
      }
      break;

    case cdk::TYPE_DOCUMENT:
      {
        process_field_doc(pos, data);
      }
      break;

      //TODO: Other Types
    default:
      std::cout <<"value of type " <<type <<" (" <<data.size() <<" bytes)";
    }
    return 0;
  }

  virtual void field_end(col_count_t /*pos*/)
  {
    std::cout << std::endl;
  }

  virtual void end_of_data()
  {
    m_md= NULL;
    std::cout << "DONE" << std::endl;
  }

};

}}

#endif
