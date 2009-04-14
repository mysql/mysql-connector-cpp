/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>


#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/warning.h>
#include "mysql_util.h"
#include "mysql_ps_resultset.h"
#include "mysql_resultset.h"
#include "mysql_prepared_statement.h"
#include "mysql_ps_resultset_metadata.h"
#include "mysql_resultbind.h"


#include "mysql_debug.h"

namespace sql
{
namespace mysql
{


/* {{{ my_l_to_a() -I- */
static inline char * my_l_to_a(char * buf, size_t buf_size, int64_t a)
{
	snprintf(buf, buf_size, "%lld", (long long) a);
	return buf;
}
/* }}} */


/* {{{ my_ul_to_a() -I- */
static inline char * my_ul_to_a(char * buf, size_t buf_size, uint64_t a)
{
	snprintf(buf, buf_size, "%llu", (unsigned long long) a);
	return buf;
}
/* }}} */


/* {{{ my_f_to_a() -I- */
static inline char * my_f_to_a(char * buf, size_t buf_size, double a)
{
	snprintf(buf, buf_size, "%f", a);
	return buf;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::MySQL_Prepared_ResultSet() -I- */
MySQL_Prepared_ResultSet::MySQL_Prepared_ResultSet(
			MYSQL_STMT * s,
			MySQL_ResultBind * r_bind,
			sql::ResultSet::enum_type rset_type,
			MySQL_Prepared_Statement * par,
			sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l
		)
	: row(NULL), stmt(s), row_position(0), parent(par), is_valid(true),
	  logger(l? l->getReference():NULL), result_bind(r_bind),
	  resultset_type(rset_type)
{
	CPP_ENTER("MySQL_Prepared_ResultSet::MySQL_Prepared_ResultSet");
	result_bind->bindResult();

	MYSQL_RES * result_meta = mysql_stmt_result_metadata(stmt);
	num_fields = mysql_stmt_field_count(stmt);
	num_rows = mysql_stmt_num_rows(stmt);

	CPP_INFO_FMT("num_fields=%u num_rows=%u", num_fields, num_rows);
	for (unsigned int i = 0; i < num_fields; ++i) {
		sql::mysql::util::my_array_guard< char > upstring(sql::mysql::util::utf8_strup(mysql_fetch_field(result_meta)->name, 0));
		field_name_to_index_map[std::string(upstring.get())] = i;
	}
	mysql_free_result(result_meta);
	result_meta = NULL;
	rs_meta.reset(new MySQL_Prepared_ResultSetMetaData(stmt, logger));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::~MySQL_Prepared_ResultSet() -I- */
MySQL_Prepared_ResultSet::~MySQL_Prepared_ResultSet()
{
	/* Don't remove the block or we can get into problems with logger */
	{
		CPP_ENTER("MySQL_Prepared_ResultSet::~MySQL_Prepared_ResultSet");
		closeIntern();
	}
	logger->freeReference();
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::absolute() -I- */
bool
MySQL_Prepared_ResultSet::absolute(const int new_pos)
{
	CPP_ENTER("MySQL_Prepared_ResultSet::absolute");
	checkValid();
	checkScrollable();
	if (new_pos > 0) {
		if (new_pos > (int) num_rows) {
			row_position = num_rows + 1; /* after last row */
		} else {
			row_position = new_pos;
			seek();
			return true;
		}
	} else if (new_pos < 0) {
		if ((-new_pos) > (int) num_rows) {
			row_position = 0; /* before first new_pos */
		} else {
			row_position = num_rows - (-new_pos) + 1;
			seek();
			return true;
		}
	} else {
		/* According to the JDBC book, absolute(0) means before the result set */
		row_position = 0;
		/* no seek() here, as we are not on data*/
		beforeFirst();
	}
	return (row_position > 0 && row_position < (num_rows + 1));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::afterLast() -I- */
void
MySQL_Prepared_ResultSet::afterLast()
{
	CPP_ENTER("MySQL_Prepared_ResultSet::afterLast");
	checkValid();
	checkScrollable();
	row_position = num_rows + 1;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::beforeFirst() -I- */
void
MySQL_Prepared_ResultSet::beforeFirst()
{
	CPP_ENTER("MySQL_Prepared_ResultSet::beforeFirst");
	checkValid();
	checkScrollable();
	mysql_stmt_data_seek(stmt, 0);
	row_position = 0;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::checkValid() -I- */
void
MySQL_Prepared_ResultSet::checkValid() const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::checkValid");
	CPP_INFO_FMT("this=%p", this);
	if (isClosed()) {
		throw sql::InvalidInstanceException("Statement has been closed");
	}
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::checkScrollable() -I- */
void
MySQL_Prepared_ResultSet::checkScrollable() const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::checkScrollable");
	CPP_INFO_FMT("this=%p", this);
	if (resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY) {
		throw sql::NonScrollableException("Nonscrollable result set");
	}
}
/* }}} */


/* {{{ MySQL_ResultSet::isScrollable() -I- */
bool
MySQL_Prepared_ResultSet::isScrollable() const
{
	CPP_ENTER("MySQL_ResultSet::isScrollable");
	CPP_INFO_FMT("this=%p", this);
	return (resultset_type != sql::ResultSet::TYPE_FORWARD_ONLY);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::close() -I- */
void
MySQL_Prepared_ResultSet::close()
{
	CPP_ENTER("MySQL_Prepared_ResultSet::close");
	checkValid();
	closeIntern();
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::closeIntern() -I- */
void
MySQL_Prepared_ResultSet::closeIntern()
{
	CPP_ENTER("MySQL_Prepared_ResultSet::closeIntern");
	is_valid = false;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::findColumn() -I- */
uint32_t
MySQL_Prepared_ResultSet::findColumn(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::findColumn");
	checkValid();
	sql::mysql::util::my_array_guard< char > upstring(sql::mysql::util::utf8_strup(columnLabel.c_str(), 0));
	FieldNameIndexMap::const_iterator iter= field_name_to_index_map.find(upstring.get());

	if (iter == field_name_to_index_map.end()) {
		return 0;
	}
	/* findColumn returns 1-based indexes */
	return iter->second + 1;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::first() -I- */
bool
MySQL_Prepared_ResultSet::first()
{
	CPP_ENTER("MySQL_Prepared_ResultSet::first");
	checkValid();
	checkScrollable();
	if (num_rows) {
		row_position = 1;
		seek();
	}
	return num_rows? true:false;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getBlob() -I- */
std::istream *
MySQL_Prepared_ResultSet::getBlob(const uint32_t columnIndex) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getBlob(int)");
	CPP_INFO_FMT("column=%u", columnIndex);
	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getBlob: can't fetch because not on result set");
	}
	return new std::istringstream(getString(columnIndex));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getBlob() -I- */
std::istream *
MySQL_Prepared_ResultSet::getBlob(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getBlob(string)");
	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getBlob: can't fetch because not on result set");
	}
	return new std::istringstream(getString(columnLabel));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getBoolean() -I- */
bool
MySQL_Prepared_ResultSet::getBoolean(const uint32_t columnIndex) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getBoolean(int)");
	CPP_INFO_FMT("column=%u", columnIndex);
	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getBoolean: can't fetch because not on result set");
	}
	return getInt(columnIndex)? true:false;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getBoolean() -I- */
bool
MySQL_Prepared_ResultSet::getBoolean(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getBoolean(string)");
	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getBoolean: can't fetch because not on result set");
	}
	return getInt(columnLabel)? true:false;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getDouble() -I- */
long double
MySQL_Prepared_ResultSet::getDouble(const uint32_t columnIndex) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getDouble(int)");
	CPP_INFO_FMT("column=%u", columnIndex);

	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getDouble: can't fetch because not on result set");
	}

	if (columnIndex == 0 || columnIndex > num_fields) {
		throw sql::InvalidArgumentException("MySQLPreparedResultSet::getDouble: invalid 'columnIndex'");
	}

	last_queried_column = columnIndex;

	if (*result_bind->rbind[columnIndex - 1].is_null) {
		return 0.0;
	}

	switch (rs_meta->getColumnType(columnIndex)) {
		case sql::DataType::BIT:
		case sql::DataType::YEAR:	// fetched as a SMALLINT
		case sql::DataType::TINYINT:
		case sql::DataType::SMALLINT:
		case sql::DataType::MEDIUMINT:
		case sql::DataType::INTEGER:
		case sql::DataType::BIGINT:
		{
			long double ret;
			bool is_it_unsigned = result_bind->rbind[columnIndex - 1].is_unsigned != 0;
			CPP_INFO_FMT("It's an int : %ssigned", is_it_unsigned? "un":"");
			if (is_it_unsigned) {
				uint64_t ival = getUInt64_intern(columnIndex, false);
				CPP_INFO_FMT("value=%llu", ival);
				ret = static_cast<long double>(ival);
			} else {
				int64_t ival = getInt64_intern(columnIndex, false);
				CPP_INFO_FMT("value=%lld", ival);
				ret = static_cast<long double>(ival);
			}
			CPP_INFO_FMT("value=%10.10f", (double) ret);
			return ret;
		}
		case sql::DataType::NUMERIC:
		case sql::DataType::DECIMAL:
		case sql::DataType::TIMESTAMP:
		case sql::DataType::DATE:
		case sql::DataType::TIME:
		case sql::DataType::CHAR:
		case sql::DataType::BINARY:
		case sql::DataType::VARCHAR:
		case sql::DataType::VARBINARY:
		case sql::DataType::LONGVARCHAR:
		case sql::DataType::LONGVARBINARY:
		case sql::DataType::SET:
		case sql::DataType::ENUM:
		{
			CPP_INFO("It's a string");
			long double ret = sql::mysql::util::strtold(getString(columnIndex).c_str(), NULL);
			CPP_INFO_FMT("value=%10.10f", ret);
			return ret;
		}
		case sql::DataType::REAL:
		{
			long double ret = !*result_bind->rbind[columnIndex - 1].is_null? *reinterpret_cast<float *>(result_bind->rbind[columnIndex - 1].buffer):0.;
			CPP_INFO_FMT("value=%10.10f", ret);
			return ret;
		}
		case sql::DataType::DOUBLE:
		{
			long double ret = !*result_bind->rbind[columnIndex - 1].is_null? *reinterpret_cast<double *>(result_bind->rbind[columnIndex - 1].buffer):0.;
			CPP_INFO_FMT("value=%10.10f", ret);
			return ret;
		}

		// ToDo : Geometry? default ?
	}
	CPP_ERR("MySQL_Prepared_ResultSet::getDouble: unhandled type. Please, report");
	throw sql::MethodNotImplementedException("MySQL_Prepared_ResultSet::getDouble: unhandled type. Please, report");
	return .0; // fool compilers
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getDouble() -I- */
long double
MySQL_Prepared_ResultSet::getDouble(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getDouble(string)");
	return getDouble(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getInt() -I- */
int32_t
MySQL_Prepared_ResultSet::getInt(const uint32_t columnIndex) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getInt(int)");
	CPP_INFO_FMT("column=%u", columnIndex);
	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getInt: can't fetch because not on result set");
	}
	if (columnIndex == 0 || columnIndex > num_fields) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getInt: invalid value of 'columnIndex'");
	}

	last_queried_column = columnIndex;

	if (*result_bind->rbind[columnIndex - 1].is_null) {
		return 0;
	}
	
	return static_cast<int32_t>(getInt64_intern(columnIndex, true));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getInt() -I- */
int32_t
MySQL_Prepared_ResultSet::getInt(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getInt(string)");
	return getInt(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getUInt() -I- */
uint32_t
MySQL_Prepared_ResultSet::getUInt(const uint32_t columnIndex) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getUInt(int)");
	CPP_INFO_FMT("column=%u", columnIndex);
	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getUInt: can't fetch because not on result set");
	}
	if (columnIndex == 0 || columnIndex > num_fields) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getUInt: invalid value of 'columnIndex'");
	}

	last_queried_column = columnIndex;

	if (*result_bind->rbind[columnIndex - 1].is_null) {
		return 0;
	}
	return static_cast<uint32_t>(getUInt64_intern(columnIndex, true));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getUInt() -I- */
uint32_t
MySQL_Prepared_ResultSet::getUInt(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getUInt(string)");
	return getUInt(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getInt64_intern() -I- */
int64_t
MySQL_Prepared_ResultSet::getInt64_intern(const uint32_t columnIndex, bool /* cutTooBig */) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getInt64_intern");
	CPP_INFO_FMT("column=%u", columnIndex);

	switch (rs_meta->getColumnType(columnIndex)) {
		case sql::DataType::REAL:
		case sql::DataType::DOUBLE:
			CPP_INFO("It's a double");
			return static_cast<int64_t>(getDouble(columnIndex));
		case sql::DataType::NUMERIC:
		case sql::DataType::DECIMAL:
		case sql::DataType::TIMESTAMP:
		case sql::DataType::DATE:
		case sql::DataType::TIME:
		case sql::DataType::CHAR:
		case sql::DataType::BINARY:
		case sql::DataType::VARCHAR:
		case sql::DataType::VARBINARY:
		case sql::DataType::LONGVARCHAR:
		case sql::DataType::LONGVARBINARY:
		case sql::DataType::SET:
		case sql::DataType::ENUM:
			CPP_INFO("It's a string");
			return strtoll(getString(columnIndex).c_str(), NULL, 10);
		case sql::DataType::BIT:
		case sql::DataType::YEAR:	// fetched as a SMALLINT
		case sql::DataType::TINYINT:
		case sql::DataType::SMALLINT:
		case sql::DataType::MEDIUMINT:
		case sql::DataType::INTEGER:
		case sql::DataType::BIGINT: {
			// sql::DataType::YEAR is fetched as a SMALLINT, thus should not be in the switch
			int64_t ret;
			bool is_it_null = *result_bind->rbind[columnIndex - 1].is_null != 0;
			bool is_it_unsigned = result_bind->rbind[columnIndex - 1].is_unsigned != 0;

			CPP_INFO_FMT("%d byte, %ssigned, null=%d", result_bind->rbind[columnIndex - 1].buffer_length, is_it_unsigned? "un":"", is_it_null);
			switch (result_bind->rbind[columnIndex - 1].buffer_length) {
				case 1:
					if (is_it_unsigned) {
						// ToDo: Really reinterpret_case or static_cast
						ret = !is_it_null? *reinterpret_cast<uint8_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					} else {
						// ToDo: Really reinterpret_case or static_cast
						ret = !is_it_null? *reinterpret_cast<int8_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					}
					break;
				case 2:
					if (is_it_unsigned) {
						ret = !is_it_null? *reinterpret_cast<uint16_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					} else {
						ret = !is_it_null? *reinterpret_cast<int16_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					}
					break;
				case 4:
					if (is_it_unsigned) {
						ret =  !is_it_null? *reinterpret_cast<uint32_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					} else {
						ret =  !is_it_null? *reinterpret_cast<int32_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					}
					break;
				case 8:
					if (is_it_unsigned) {
						ret =  !is_it_null? *reinterpret_cast<uint64_t *>(result_bind->rbind[columnIndex - 1].buffer):0;

#if WE_WANT_TO_SEE_MORE_FAILURES_IN_UNIT_RESULTSET
						if (cutTooBig &&
							ret &&
							*reinterpret_cast<uint64_t *>(result_bind->rbind[columnIndex - 1].buffer) > UL64(9223372036854775807))
						{
							ret = UL64(9223372036854775807);
						}
#endif
					} else {
						ret =  !is_it_null? *reinterpret_cast<int64_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					}
					break;
				default:
					throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getInt64_intern: invalid type");
			}
			CPP_INFO_FMT("value=%lld", ret);
			return ret;
		}
		default:
			break;
		// ToDo : Geometry? default ?
	}
	CPP_ERR("MySQL_Prepared_ResultSet::getInt64_intern: unhandled type. Please, report");
	throw sql::MethodNotImplementedException("MySQL_Prepared_ResultSet::getInt64_intern: unhandled type. Please, report");
	return 0; // fool compilers
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getInt64() -I- */
int64_t
MySQL_Prepared_ResultSet::getInt64(const uint32_t columnIndex) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getInt64(int)");
	CPP_INFO_FMT("column=%u", columnIndex);

	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getInt64: can't fetch because not on result set");
	}
	if (columnIndex == 0 || columnIndex > num_fields) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getInt64: invalid value of 'columnIndex'");
	}

	last_queried_column = columnIndex;

	if (*result_bind->rbind[columnIndex - 1].is_null) {
		return 0;
	}
	return getInt64_intern(columnIndex, true);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getInt64() -I- */
int64_t
MySQL_Prepared_ResultSet::getInt64(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getInt64(string)");
	return getInt64(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getUInt64_intern() -I- */
uint64_t
MySQL_Prepared_ResultSet::getUInt64_intern(const uint32_t columnIndex, bool /* cutTooBig */) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getUInt64_intern");
	CPP_INFO_FMT("column=%u", columnIndex);

	switch (rs_meta->getColumnType(columnIndex)) {
		case sql::DataType::REAL:
		case sql::DataType::DOUBLE:
			CPP_INFO("It's a double");
			return static_cast<uint64_t>(getDouble(columnIndex));
		case sql::DataType::NUMERIC:
		case sql::DataType::DECIMAL:
		case sql::DataType::TIMESTAMP:
		case sql::DataType::DATE:
		case sql::DataType::TIME:
		case sql::DataType::CHAR:
		case sql::DataType::BINARY:
		case sql::DataType::VARCHAR:
		case sql::DataType::VARBINARY:
		case sql::DataType::LONGVARCHAR:
		case sql::DataType::LONGVARBINARY:
		case sql::DataType::SET:
		case sql::DataType::ENUM:
			CPP_INFO("It's a string");
			return strtoull(getString(columnIndex).c_str(), NULL, 10);
		case sql::DataType::BIT:
		case sql::DataType::YEAR:	// fetched as a SMALLINT
		case sql::DataType::TINYINT:
		case sql::DataType::SMALLINT:
		case sql::DataType::MEDIUMINT:
		case sql::DataType::INTEGER:
		case sql::DataType::BIGINT:
		{
			// sql::DataType::YEAR is fetched as a SMALLINT, thus should not be in the switch

			uint64_t ret;
			bool is_it_null = *result_bind->rbind[columnIndex - 1].is_null != 0;
			bool is_it_unsigned = result_bind->rbind[columnIndex - 1].is_unsigned != 0;

			CPP_INFO_FMT("%d byte, %ssigned, null=%d", result_bind->rbind[columnIndex - 1].buffer_length, is_it_unsigned? "un":"", is_it_null);
			switch (result_bind->rbind[columnIndex - 1].buffer_length) {
				case 1:
					if (is_it_unsigned) {
						// ToDo: Really reinterpret_case or static_cast
						ret = !is_it_null? *reinterpret_cast<uint8_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					} else {
						// ToDo: Really reinterpret_case or static_cast
						ret = !is_it_null? *reinterpret_cast<int8_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					}
					break;
				case 2:
					if (is_it_unsigned) {
						ret = !is_it_null? *reinterpret_cast<uint16_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					} else {
						ret = !is_it_null? *reinterpret_cast<int16_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					}
					break;
				case 4:
					if (is_it_unsigned) {
						ret =  !is_it_null? *reinterpret_cast<uint32_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					} else {
						ret =  !is_it_null? *reinterpret_cast<int32_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					}
					break;
				case 8:
					if (is_it_unsigned) {
						ret =  !is_it_null? *reinterpret_cast<uint64_t *>(result_bind->rbind[columnIndex - 1].buffer):0;
					} else {
						ret =  !is_it_null? *reinterpret_cast<int64_t *>(result_bind->rbind[columnIndex - 1].buffer):0;				
#if WE_WANT_TO_SEE_MORE_FAILURES_IN_UNIT_RESULTSET
						if (is_it_null) {
							if (cutTooBig && *reinterpret_cast<int64_t *>(result_bind->rbind[columnIndex - 1].buffer) < 0) {
								ret =  *reinterpret_cast<uint64_t *>(result_bind->rbind[columnIndex - 1].buffer);				
							} else {
								ret =  *reinterpret_cast<int64_t *>(result_bind->rbind[columnIndex - 1].buffer);				
							}
						} else {
							ret = 0;
						}
#endif
					}
					break;
				default:
					throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getInt64_intern: invalid type");
			}
			CPP_INFO_FMT("value=%lld", ret);
			return ret;
		}
		default:
			break;
		// ToDo : Geometry? default ?
	}
	CPP_ERR("MySQL_Prepared_ResultSet::getUInt64_intern: unhandled type. Please, report");
	throw sql::MethodNotImplementedException("MySQL_Prepared_ResultSet::getUInt64_intern: unhandled type. Please, report");
	return 0; // fool compilers
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getUInt64() -I- */
uint64_t
MySQL_Prepared_ResultSet::getUInt64(const uint32_t columnIndex) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getUInt64(int)");
	CPP_INFO_FMT("column=%u", columnIndex);

	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getUInt64: can't fetch because not on result set");
	}
	if (columnIndex == 0 || columnIndex > num_fields) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getUInt64: invalid value of 'columnIndex'");
	}

	last_queried_column = columnIndex;

	if (*result_bind->rbind[columnIndex - 1].is_null) {
		return 0;
	}
	return getUInt64_intern(columnIndex, true);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getUInt64() -I- */
uint64_t
MySQL_Prepared_ResultSet::getUInt64(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getUInt64(string)");
	return getUInt64(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getMetaData() -I- */
sql::ResultSetMetaData *
MySQL_Prepared_ResultSet::getMetaData() const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getMetaData");
	checkValid();
	return rs_meta.get();
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getRow() -I- */
size_t
MySQL_Prepared_ResultSet::getRow() const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getRow");
	checkValid();
	/* row_position is 0 based */
	return static_cast<size_t> (row_position);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getStatement() -I- */
const sql::Statement *
MySQL_Prepared_ResultSet::getStatement() const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getStatement");
	return parent;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getString() -I- */
std::string
MySQL_Prepared_ResultSet::getString(const uint32_t columnIndex) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getString(int)");
	CPP_INFO_FMT("column=%u", columnIndex);

	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::getString: can't fetch because not on result set");
	}

	if (columnIndex == 0 || columnIndex > num_fields) {
		throw sql::InvalidArgumentException("MySQLPreparedResultSet::getString: invalid 'columnIndex'");
	}

	last_queried_column = columnIndex;
	if (*result_bind->rbind[columnIndex - 1].is_null) {
		return std::string("");
	}

	switch (rs_meta->getColumnType(columnIndex)) {
		case sql::DataType::TIMESTAMP:
		{
			char buf[22];
			MYSQL_TIME * t = static_cast<MYSQL_TIME *>(result_bind->rbind[columnIndex - 1].buffer);
			snprintf(buf, sizeof(buf) - 1, "%04d-%02d-%02d %02d:%02d:%02d", t->year, t->month, t->day, t->hour, t->minute, t->second);
			CPP_INFO_FMT("It's a datetime/timestamp %s", buf);
			return std::string(buf);
		}
		case sql::DataType::DATE:
		{
			char buf[12];
			MYSQL_TIME * t = static_cast<MYSQL_TIME *>(result_bind->rbind[columnIndex - 1].buffer);
			snprintf(buf, sizeof(buf) - 1, "%02d-%02d-%02d", t->year, t->month, t->day);
			CPP_INFO_FMT("It's a date %s", buf);
			return std::string(buf);
		}
		case sql::DataType::TIME:
		{
			char buf[12];
			MYSQL_TIME * t = static_cast<MYSQL_TIME *>(result_bind->rbind[columnIndex - 1].buffer);
			snprintf(buf, sizeof(buf) - 1, "%s%02d:%02d:%02d", t->neg? "-":"", t->hour, t->minute, t->second);
			CPP_INFO_FMT("It's a time %s", buf);
			return std::string(buf);
		}
		case sql::DataType::BIT:
		case sql::DataType::YEAR:	// fetched as a SMALLINT
		case sql::DataType::TINYINT:
		case sql::DataType::SMALLINT:
		case sql::DataType::MEDIUMINT:
		case sql::DataType::INTEGER:
		case sql::DataType::BIGINT:
		{
			char buf[30];
			CPP_INFO("It's an int");
			if (result_bind->rbind[columnIndex - 1].is_unsigned) {
				my_ul_to_a(buf, sizeof(buf) - 1, getUInt64_intern(columnIndex, false));
			} else {
				my_l_to_a(buf, sizeof(buf) - 1, getInt64_intern(columnIndex, false));			
			}
			return std::string(buf);
		}
		case sql::DataType::REAL:
		case sql::DataType::DOUBLE:
		{
			char buf[50];
			CPP_INFO("It's a double");
			my_f_to_a(buf, sizeof(buf) - 1, getDouble(columnIndex));
			return std::string(buf);
		}
		case sql::DataType::NUMERIC:
		case sql::DataType::DECIMAL:
		case sql::DataType::CHAR:
		case sql::DataType::BINARY:
		case sql::DataType::VARCHAR:
		case sql::DataType::VARBINARY:
		case sql::DataType::LONGVARCHAR:
		case sql::DataType::LONGVARBINARY:
		case sql::DataType::SET:
		case sql::DataType::ENUM:
			CPP_INFO("It's a string");
			return  std::string(static_cast<char *>(result_bind->rbind[columnIndex - 1].buffer), *result_bind->rbind[columnIndex - 1].length);
		default:
			break;		
		// ToDo : Geometry? default ?
	}

	CPP_ERR("MySQL_Prepared_ResultSet::getString: unhandled type. Please, report");
	throw sql::MethodNotImplementedException("MySQL_Prepared_ResultSet::getString: unhandled type. Please, report");
	return 0; // fool compilers
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getString() -I- */
std::string
MySQL_Prepared_ResultSet::getString(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::getString(string)");
	return getString(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::getType() -I- */
sql::ResultSet::enum_type
MySQL_Prepared_ResultSet::getType() const
{
	CPP_ENTER("MySQL_ResultSet::getType");
	checkValid();
	return resultset_type;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::isAfterLast() -I- */
bool
MySQL_Prepared_ResultSet::isAfterLast() const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::beforeFirst");
	checkValid();
	checkScrollable();
	return (row_position == num_rows + 1);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::isBeforeFirst() -I- */
bool
MySQL_Prepared_ResultSet::isBeforeFirst() const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::beforeFirst");
	checkValid();
	return (row_position == 0);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::isClosed() -I- */
bool
MySQL_Prepared_ResultSet::isClosed() const
{
	return !is_valid;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::isFirst() -I- */
bool
MySQL_Prepared_ResultSet::isFirst() const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::beforeFirst");
	checkValid();
	return (row_position == 1);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::isLast() -I- */
bool
MySQL_Prepared_ResultSet::isLast() const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::isLast");
	checkValid();
	checkScrollable();
	return (row_position == num_rows);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::isNull() -I- */
bool
MySQL_Prepared_ResultSet::isNull(const uint32_t columnIndex) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::isNull(int)");
	checkValid();
	if (columnIndex == 0 || columnIndex > num_fields) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::isNull: invalid value of 'columnIndex'");
	}
	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::isNull: can't fetch because not on result set");
	}
	return *result_bind->rbind[columnIndex - 1].is_null != 0;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::isNull() -I- */
bool
MySQL_Prepared_ResultSet::isNull(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::isNull(string)");
	uint32_t col_idx = findColumn(columnLabel);
	if (col_idx == 0) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::isNull: invalid value of 'columnLabel'");
	}
	return isNull(col_idx);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::last() -I- */
bool
MySQL_Prepared_ResultSet::last()
{
	CPP_ENTER("MySQL_Prepared_ResultSet::last");
	checkValid();
	checkScrollable();
	if (num_rows) {
		row_position = num_rows;
		seek();
	}
	return num_rows? true:false;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::next() -I- */
bool
MySQL_Prepared_ResultSet::next()
{
	CPP_ENTER("MySQL_Prepared_ResultSet::next");
	CPP_INFO_FMT("row_position=%llu num_rows=%llu", row_position, num_rows);
	checkValid();
	bool ret = false;
	if (isScrollable()) {
		/* isBeforeFirst checks for validity */
		if (isLast()) {
			++row_position;
			ret = false;
		} else if (row_position < num_rows + 1) {
			mysql_stmt_data_seek(stmt, row_position);
			int result = mysql_stmt_fetch(stmt);
			if (!result || result == MYSQL_DATA_TRUNCATED) {
				ret = true;
			}
			if (result == MYSQL_NO_DATA) {
				ret = false;
			}
			++row_position;
		}
		CPP_INFO_FMT("new_row_position=%llu ret=%d", row_position, ret);
	} else {
		int result = mysql_stmt_fetch(stmt);
		if (!result || result == MYSQL_DATA_TRUNCATED) {
			ret = true;
		}
		if (result == MYSQL_NO_DATA) {
			ret = false;
		}
		++row_position;
	}
	return ret;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::previous() -I- */
bool
MySQL_Prepared_ResultSet::previous()
{
	CPP_ENTER("MySQL_Prepared_ResultSet::previous");
	/* isBeforeFirst checks for validity */
	if (isBeforeFirst()) {
		return false;
	} else if (isFirst()) {
		beforeFirst();
		return false;
	} else if (row_position > 1) {
		--row_position;
		mysql_stmt_data_seek(stmt, row_position - 1);
		int result = mysql_stmt_fetch(stmt);
		if (!result || result == MYSQL_DATA_TRUNCATED) {
			return true;
		}
		if (result == MYSQL_NO_DATA) {
			return false;
		}
		throw sql::SQLException("Error during mysql_stmt_fetch");
	}
	throw sql::SQLException("Impossible");
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::relative() -I- */
bool
MySQL_Prepared_ResultSet::relative(const int rows)
{
	CPP_ENTER("MySQL_Prepared_ResultSet::relative");
	checkValid();
	checkScrollable();
	if (rows != 0) {
		if (row_position + rows > num_rows || row_position + rows < 1) {
			row_position = rows > 0? num_rows + 1 : 0; /* after last or before first */
		} else {
			row_position += rows;
			mysql_stmt_data_seek(stmt, row_position - 1);
		}
	}

	return (row_position > 0 && row_position <= num_rows);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::rowsCount() -I- */
size_t
MySQL_Prepared_ResultSet::rowsCount() const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::rowsCount");
	checkValid();
	checkScrollable();
	return (size_t) mysql_stmt_num_rows(stmt);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::wasNull() -I- */
bool
MySQL_Prepared_ResultSet::wasNull() const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::wasNull");
	checkValid();
	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_Prepared_ResultSet::wasNull: can't fetch because not on result set");
	}
	return *result_bind->rbind[last_queried_column - 1].is_null != 0;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::isBeforeFirstOrAfterLast() -I- */
bool
MySQL_Prepared_ResultSet::isBeforeFirstOrAfterLast() const
{
	CPP_ENTER("MySQL_Prepared_ResultSet::isBeforeFirstOrAfterLast");
	checkValid();
	return (row_position == 0) || (isScrollable() && (row_position == num_rows + 1));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSet::seek() -I- */
void
MySQL_Prepared_ResultSet::seek()
{
	CPP_ENTER("MySQL_Prepared_ResultSet::seek");
	mysql_stmt_data_seek(stmt, row_position - 1);
	mysql_stmt_fetch(stmt);
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
