/*
  Copyright (c) 2009, 2010, Oracle and/or its affiliates. All rights reserved.

  The MySQL Connector/C++ is licensed under the terms of the GPLv2
  <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
  MySQL Connectors. There are special exceptions to the terms and
  conditions of the GPLv2 as it is applied to this software, see the
  FLOSS License Exception
  <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef _MYSQL_ART_RSET_METADATA_H_
#define _MYSQL_ART_RSET_METADATA_H_

#include <boost/shared_ptr.hpp>

#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>

namespace sql
{
namespace mysql
{
class MySQL_DebugLogger;

class MySQL_ArtResultSetMetaData : public sql::ResultSetMetaData
{
	const MySQL_ArtResultSet * parent;
	boost::shared_ptr< MySQL_DebugLogger > logger;
	unsigned int num_fields;

public:
	MySQL_ArtResultSetMetaData(const MySQL_ArtResultSet * p, boost::shared_ptr< MySQL_DebugLogger > & l);
	virtual ~MySQL_ArtResultSetMetaData();

	SQLString getCatalogName(unsigned int columnIndex);

	unsigned int getColumnCount();

	unsigned int getColumnDisplaySize(unsigned int columnIndex);

	SQLString getColumnLabel(unsigned int columnIndex);

	SQLString getColumnName(unsigned int columnIndex);

	int getColumnType(unsigned int columnIndex);

	SQLString getColumnTypeName(unsigned int columnIndex);

	unsigned int getPrecision(unsigned int columnIndex);

	unsigned int getScale(unsigned int columnIndex);

	SQLString getSchemaName(unsigned int columnIndex);

	SQLString getTableName(unsigned int columnIndex);

	bool isAutoIncrement(unsigned int columnIndex);

	bool isCaseSensitive(unsigned int columnIndex);

	bool isCurrency(unsigned int columnIndex);

	bool isDefinitelyWritable(unsigned int columnIndex);

	int isNullable(unsigned int columnIndex);

	bool isReadOnly(unsigned int columnIndex);

	bool isSearchable(unsigned int columnIndex);

	bool isSigned(unsigned int columnIndex);

	bool isWritable(unsigned int columnIndex);

	bool isZerofill(unsigned int columnIndex);

protected:
	void checkColumnIndex(unsigned int columnIndex) const;

private:
	/* Prevent use of these */
	MySQL_ArtResultSetMetaData(const MySQL_ArtResultSetMetaData &);
	void operator=(MySQL_ArtResultSetMetaData &);
};


} /* namespace mysql */
} /* namespace sql */

#endif /* _MYSQL_ART_RSET_METADATA_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
