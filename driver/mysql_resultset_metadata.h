/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_RESULTSET_METADATA_H_
#define _MYSQL_RESULTSET_METADATA_H_

#include <cppconn/resultset_metadata.h>
#include "mysql_res_wrapper.h"

namespace sql
{
namespace mysql
{
namespace util {template<class T> class my_shared_ptr; }; // forward declaration.
class MySQL_DebugLogger;

class MySQL_ResultSetMetaData : public sql::ResultSetMetaData
{
	MYSQL_RES_Wrapper * result;
	sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * logger;
	unsigned int num_fields;

public:
	MySQL_ResultSetMetaData(MYSQL_RES_Wrapper * res, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l);
	virtual ~MySQL_ResultSetMetaData();

	std::string getCatalogName(unsigned int columnIndex);

	unsigned int getColumnCount();

	unsigned int getColumnDisplaySize(unsigned int columnIndex);

	std::string getColumnLabel(unsigned int columnIndex);

	std::string getColumnName(unsigned int columnIndex);

	int getColumnType(unsigned int columnIndex);

	std::string getColumnTypeName(unsigned int columnIndex);

	unsigned int getPrecision(unsigned int columnIndex);

	unsigned int getScale(unsigned int columnIndex);

	std::string getSchemaName(unsigned int columnIndex);

	std::string getTableName(unsigned int columnIndex);

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

	MYSQL_FIELD * getFieldMeta(unsigned int columnIndex) const { return mysql_fetch_field_direct(result->get(), columnIndex - 1); }

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
