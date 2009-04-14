/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include <stdlib.h>

#include "mysql_util.h"
#include "mysql_art_resultset.h"
#include "mysql_art_rset_metadata.h"

#include "mysql_debug.h"

#include <cppconn/datatype.h>
#include <cppconn/exception.h>

namespace sql
{
namespace mysql
{

/* {{{ MySQL_ArtResultSetMetaData::MySQL_ArtResultSetMetaData() -I- */
MySQL_ArtResultSetMetaData::MySQL_ArtResultSetMetaData(const MySQL_ArtResultSet * p,
													   sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l)
  : parent(p), logger(l? l->getReference():NULL), num_fields(parent->num_fields)
{
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::~MySQL_ArtResultSetMetaData() -I- */
MySQL_ArtResultSetMetaData::~MySQL_ArtResultSetMetaData()
{
	/* Don't remove the block or we can get into problems with logger */
	{
		CPP_ENTER("MySQL_ArtResultSetMetaData::~MySQL_ArtResultSetMetaData");
		CPP_INFO_FMT("this=%p", this);
	}
	logger->freeReference();
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::checkColumnIndex -I- */
void
MySQL_ArtResultSetMetaData::checkColumnIndex(unsigned int columnIndex) const
{
	if (columnIndex == 0 || columnIndex > num_fields) {
		throw sql::InvalidArgumentException("Invalid value for columnIndex");
	}
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::getCatalogName() -I- */
std::string
MySQL_ArtResultSetMetaData::getCatalogName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::getCatalogName");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	return "";
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::getColumnCount() -I- */
unsigned int
MySQL_ArtResultSetMetaData::getColumnCount()
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::getColumnCount");
	CPP_INFO_FMT("this=%p", this);
	CPP_INFO_FMT("column_count=%d", num_fields);
	return num_fields;
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::getColumnDisplaySize() -U- */
unsigned int
MySQL_ArtResultSetMetaData::getColumnDisplaySize(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::getColumnDisplaySize");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	throw sql::MethodNotImplementedException("MySQL_ArtResultSetMetaData::getColumnDisplaySize()");
	return 0; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::getColumnLabel() -I- */
std::string
MySQL_ArtResultSetMetaData::getColumnLabel(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::getColumnLabel");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return parent->field_index_to_name_map[columnIndex - 1];
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::getColumnName() -I- */
std::string
MySQL_ArtResultSetMetaData::getColumnName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::getColumnName");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return parent->field_index_to_name_map[columnIndex - 1];
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::getColumnType() -I- */
int
MySQL_ArtResultSetMetaData::getColumnType(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::getColumnType");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return sql::DataType::VARCHAR;
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::getColumnTypeName() -I- */
std::string
MySQL_ArtResultSetMetaData::getColumnTypeName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::getColumnTypeName");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return "VARCHAR";
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::getPrecision() -U- */
unsigned int
MySQL_ArtResultSetMetaData::getPrecision(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::getPrecision");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	throw sql::MethodNotImplementedException("MySQL_ArtResultSetMetaData::getPrecision()");
	return 0; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::getScale() -U- */
unsigned int
MySQL_ArtResultSetMetaData::getScale(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::getScale");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	throw sql::MethodNotImplementedException("MySQL_ArtResultSetMetaData::getScale()");
	return 0; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::getSchemaName() -I- */
std::string
MySQL_ArtResultSetMetaData::getSchemaName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::getSchemaName");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return "";
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::getTableName() -I- */
std::string
MySQL_ArtResultSetMetaData::getTableName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::getTableName");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return "";
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::isAutoIncrement() -I- */
bool
MySQL_ArtResultSetMetaData::isAutoIncrement(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::isAutoIncrement");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return false;
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::isCaseSensitive() -I- */
bool
MySQL_ArtResultSetMetaData::isCaseSensitive(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::isCaseSensitive");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return "true";
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::isCurrency() -I- */
bool
MySQL_ArtResultSetMetaData::isCurrency(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::isCurrency");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return false;
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::isDefinitelyWritable() -I- */
bool
MySQL_ArtResultSetMetaData::isDefinitelyWritable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::isDefinitelyWritable");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return isWritable(columnIndex);
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::isNullable() -I- */
int
MySQL_ArtResultSetMetaData::isNullable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::isNullable");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return false;
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::isReadOnly() -I- */
bool
MySQL_ArtResultSetMetaData::isReadOnly(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::isReadOnly");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	/* We consider we connect to >= 40100 - else, we can't say */
	return true;
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::isSearchable() -I- */
bool
MySQL_ArtResultSetMetaData::isSearchable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::isSearchable");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return true;
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::isSigned() -I- */
bool
MySQL_ArtResultSetMetaData::isSigned(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::isSigned");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return false;
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::isWritable() -I- */
bool
MySQL_ArtResultSetMetaData::isWritable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::isWritable");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return !isReadOnly(columnIndex);
}
/* }}} */


/* {{{ MySQL_ArtResultSetMetaData::isZerofill() -I- */
bool
MySQL_ArtResultSetMetaData::isZerofill(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ArtResultSetMetaData::isZerofill");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);

	return false;
}
/* }}} */


} /* namespace mysql */
} /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
