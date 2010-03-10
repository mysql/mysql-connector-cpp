/*
   Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_RESULTSET_METADATA_H_
#define _MYSQL_RESULTSET_METADATA_H_

#include <boost/weak_ptr.hpp>

#include <cppconn/resultset_metadata.h>

struct st_mysql_field;

namespace sql
{
namespace mysql
{
namespace
{
class NativeResultsetWrapper;
}

class MySQL_DebugLogger;


class MySQL_ResultSetMetaData : public sql::ResultSetMetaData
{
	boost::weak_ptr< NativeAPI::NativeResultsetWrapper > result;
	boost::shared_ptr< MySQL_DebugLogger > logger;
	unsigned int num_fields;

public:
	MySQL_ResultSetMetaData(boost::shared_ptr< NativeAPI::NativeResultsetWrapper > res, boost::shared_ptr< MySQL_DebugLogger > & l);
	virtual ~MySQL_ResultSetMetaData();

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
	void checkValid() const;

	void checkColumnIndex(unsigned int columnIndex) const;

	::st_mysql_field * getFieldMeta(unsigned int columnIndex) const;

private:

	/* Prevent use of these */
	MySQL_ResultSetMetaData(const MySQL_ResultSetMetaData &);
	void operator=(MySQL_ResultSetMetaData &);
};

} /* namespace mysql */
} /* namespace sql */

#endif // _MYSQL_RESULTSET_METADATA_H_
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
