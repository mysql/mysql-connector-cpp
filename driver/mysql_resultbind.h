/*
 * Copyright (c) 2008, 2020, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */



#ifndef _MYSQL_RESULTBIND_H_
#define _MYSQL_RESULTBIND_H_

#include <boost/scoped_array.hpp>
#include <vector>

#include <cppconn/prepared_statement.h>
#include <cppconn/parameter_metadata.h>

#include "nativeapi/mysql_private_iface.h"
#include "mysql_util.h"

namespace sql
{
namespace mysql
{
namespace NativeAPI
{
class NativeStatementWrapper;
}

struct MySQL_Bind : public MYSQL_BIND
{
  MySQL_Bind();
  MySQL_Bind(MySQL_Bind&&);
  MySQL_Bind(const MySQL_Bind&) = delete;
  ~MySQL_Bind();


  void setBigInt(const sql::SQLString& value);
  void setBlob(std::istream * blob);
  void setBoolean(bool value);
  void setDateTime(const sql::SQLString& value);
  void setDouble(double value);
  void setInt(int32_t value);
  void setUInt(uint32_t value);
  void setInt64(int64_t value);
  void setUInt64(uint64_t value);
  void setNull();
  void setString(const sql::SQLString& value);

  void clear();
};

class MySQL_ResultBind
{
  unsigned int num_fields;
  boost::scoped_array< my_bool > is_null;
  boost::scoped_array< my_bool > err;
  boost::scoped_array< unsigned long > len;

  boost::shared_ptr< NativeAPI::NativeStatementWrapper > proxy;

  boost::shared_ptr< MySQL_DebugLogger > logger;

public:
  boost::scoped_array< MYSQL_BIND > rbind;


  MySQL_ResultBind( boost::shared_ptr< NativeAPI::NativeStatementWrapper > & _capi, boost::shared_ptr< MySQL_DebugLogger > & log);

  ~MySQL_ResultBind();

  void bindResult();

};


class MySQL_AttributesBind
{
  std::vector<MySQL_Bind> bind;
  std::vector<const char*> names;

  int getBindPos(const SQLString &name);

public:

  MySQL_AttributesBind();

  ~MySQL_AttributesBind();

  int setQueryAttrBigInt(const sql::SQLString &name, const sql::SQLString& value);
  int setQueryAttrBoolean(const sql::SQLString &name, bool value);
  int setQueryAttrDateTime(const sql::SQLString &name, const sql::SQLString& value);
  int setQueryAttrDouble(const sql::SQLString &name, double value);
  int setQueryAttrInt(const sql::SQLString &name, int32_t value);
  int setQueryAttrUInt(const sql::SQLString &name, uint32_t value);
  int setQueryAttrInt64(const sql::SQLString &name, int64_t value);
  int setQueryAttrUInt64(const sql::SQLString &name, uint64_t value);
  int setQueryAttrNull(const SQLString &name);
  int setQueryAttrString(const sql::SQLString &name, const sql::SQLString& value);

  void clearAttributes();

  int nrAttr();
  MYSQL_BIND* getBinds();
  const char **getNames();

};

} /* namespace mysql */
} /* namespace sql */

#endif /* _MYSQL_RESULTBIND_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

