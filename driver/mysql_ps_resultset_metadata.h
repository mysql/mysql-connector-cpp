/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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



#ifndef _MYSQL_PS_RESULTSET_METADATA_H_
#define _MYSQL_PS_RESULTSET_METADATA_H_

#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include "nativeapi/mysql_private_iface.h"

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace sql
{
namespace mysql
{
namespace NativeAPI
{
class NativeResultsetWrapper;
class NativeStatementWrapper;
}

class MySQL_DebugLogger;


class MySQL_PreparedResultSetMetaData : public sql::ResultSetMetaData
{
  boost::shared_ptr< NativeAPI::NativeStatementWrapper > proxy;

  boost::shared_ptr< MySQL_DebugLogger > logger;

  boost::scoped_ptr< NativeAPI::NativeResultsetWrapper > result_meta;

  unsigned int num_fields;

public:
  MySQL_PreparedResultSetMetaData(boost::shared_ptr< NativeAPI::NativeStatementWrapper > & _proxy,
                    boost::shared_ptr< MySQL_DebugLogger> & l);

  virtual ~MySQL_PreparedResultSetMetaData();

  SQLString getCatalogName(unsigned int columnIndex);

  unsigned int getColumnCount();

  unsigned int getColumnDisplaySize(unsigned int columnIndex);

  SQLString getColumnLabel(unsigned int columnIndex);

  SQLString getColumnName(unsigned int columnIndex);

  int getColumnType(unsigned int columnIndex);

  SQLString getColumnTypeName(unsigned int columnIndex);

  SQLString getColumnCharset(unsigned int columnIndex);

  SQLString getColumnCollation(unsigned int columnIndex);

  unsigned int getPrecision(unsigned int columnIndex);

  unsigned int getScale(unsigned int columnIndex);

  SQLString getSchemaName(unsigned int columnIndex);

  SQLString getTableName(unsigned int columnIndex);

  bool isAutoIncrement(unsigned int columnIndex);

  bool isCaseSensitive(unsigned int columnIndex);

  bool isCurrency(unsigned int columnIndex);

  bool isDefinitelyWritable(unsigned int columnIndex);

  int isNullable(unsigned int columnIndex);

  bool isNumeric(unsigned int columnIndex);

  bool isReadOnly(unsigned int columnIndex);

  bool isSearchable(unsigned int columnIndex);

  bool isSigned(unsigned int columnIndex);

  bool isWritable(unsigned int columnIndex);

  bool isZerofill(unsigned int column);

protected:
  void checkColumnIndex(unsigned int columnIndex) const;

  MYSQL_FIELD * getFieldMeta(unsigned int columnIndex) const;

private:
  /* Prevent use of these */
  MySQL_PreparedResultSetMetaData(const MySQL_PreparedResultSetMetaData &);
  void operator=(MySQL_PreparedResultSetMetaData &);
};


} /* namespace mysql */
} /* namespace sql */

#endif /* _MYSQL_PS_RESULTSET_METADATA_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
