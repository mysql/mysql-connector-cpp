/* Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   There are special exceptions to the terms and conditions of the GPL
   as it is applied to this software. View the full text of the
   exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
   software distribution.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <stdlib.h>
#include <memory>
#include "mysql_connection.h"
#include "mysql_statement.h"
#include "mysql_prepared_statement.h"
#include "mysql_ps_resultset.h"
#include "mysql_parameter_metadata.h"
#include "mysql_warning.h"

#ifndef _WIN32
#include <string.h>
#endif

#define mysql_stmt_conn(s) (s)->mysql

#include "mysql_debug.h"
#include "mysql_util.h"


namespace sql
{

namespace mysql
{

/*
  Plug only relevant parts of Zend and main/streams and statically build them.
  Remove the ZEND MM, and put own implementation, which is just malloc.
*/

/* {{{ get_new_param_bind() -I- */
static MYSQL_BIND *
get_new_param_bind(int param_count)
{
	if (!param_count) {
		return NULL;
	}
	MYSQL_BIND * bind = (MYSQL_BIND *) calloc(param_count, sizeof(MYSQL_BIND));

	return bind;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::MySQL_Prepared_Statement() -I- */
MySQL_Prepared_Statement::MySQL_Prepared_Statement(MYSQL_STMT *s, sql::Connection * conn, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * log)
	:connection(conn), stmt(s), param_bind(NULL), isClosed(false), logger(log? log->getReference():NULL)
{
	CPP_ENTER("MySQL_Prepared_Statement::MySQL_Prepared_Statement");
	CPP_INFO_FMT("this=%p", this);
	param_count = mysql_stmt_param_count(s);
	if (param_count) {
		param_bind = get_new_param_bind(param_count);
		for (unsigned int i = 0; i < param_count; i++) {
			param_bind[i].is_null_value = 1;
		}
	}
	result_bind = NULL;
	is_null = NULL;
	err = NULL;
	len = NULL;
	num_fields = 0;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::~MySQL_Prepared_Statement() -I- */
MySQL_Prepared_Statement::~MySQL_Prepared_Statement()
{
	/* Don't remove the block or we can get into problems with logger */
	{
		CPP_ENTER("MySQL_Prepared_Statement::~MySQL_Prepared_Statement");
		/*
		  This will free param_bind.
		  We should not do it or there will be double free.
		*/
		if (!isClosed) {
			closeIntern();
		}
	}
	logger->freeReference();
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::do_query() -I- */
void
MySQL_Prepared_Statement::do_query()
{
	CPP_ENTER("MySQL_Prepared_Statement::do_query");
	if (param_count && mysql_stmt_bind_param(stmt, param_bind)) {
		CPP_ERR("Couldn't bind");
		throw sql::SQLException(mysql_stmt_error(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_errno(stmt));
	}

	if (mysql_stmt_execute(stmt)) {
		CPP_ERR("Couldn't execute");
		throw sql::SQLException(mysql_stmt_error(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_errno(stmt));
	}
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::clearParameters() -U- */
void
MySQL_Prepared_Statement::clearParameters()
{
	CPP_ENTER("MySQL_Prepared_Statement::clearParameters");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	if (param_bind) {
		for (unsigned int i = 0; i < param_count; i++) {
			delete (char*) param_bind[i].length;
			param_bind[i].length = NULL;
			delete[] (char*) param_bind[i].buffer;
			param_bind[i].buffer = NULL;
		}
	}
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getConnection() -I- */
Connection *
MySQL_Prepared_Statement::getConnection()
{
	CPP_ENTER("MySQL_Prepared_Statement::getConnection");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	return connection;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::execute() -I- */
bool MySQL_Prepared_Statement::execute()
{
	CPP_ENTER("MySQL_Prepared_Statement::execute");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	do_query();
	return (mysql_stmt_field_count(stmt) > 0);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::execute() -U- */
bool MySQL_Prepared_Statement::execute(const std::string&)
{
	CPP_ENTER("MySQL_Prepared_Statement::execute(const std::string& sql)");
	throw sql::MethodNotImplementedException("MySQL_Prepared_Statement::execute");
	return(false);
}
/* }}} */


typedef std::pair<char *, int> BufferSizePair;
static BufferSizePair
allocate_buffer_for_type(MYSQL_FIELD *field)
{
	switch (field->type) {
		case MYSQL_TYPE_TINY:
			return BufferSizePair(new char[1], 1);
		case MYSQL_TYPE_SHORT:
			return BufferSizePair(new char[2], 2);
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_FLOAT:
			return BufferSizePair(new char[4], 4);
		case MYSQL_TYPE_DOUBLE:
		case MYSQL_TYPE_LONGLONG:
			return BufferSizePair(new char[8], 8);
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATETIME:
			return BufferSizePair(new char[sizeof(MYSQL_TIME)], sizeof(MYSQL_TIME));
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_VAR_STRING:
			if (!(field->max_length))
				return BufferSizePair(new char[1], 1);
			return BufferSizePair(new char[field->max_length], field->max_length);

		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
			return BufferSizePair(new char[64], 64);
		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_YEAR:
			return BufferSizePair(new char[10], 10);
		case MYSQL_TYPE_SET:
		case MYSQL_TYPE_BIT:
		case MYSQL_TYPE_ENUM:
		case MYSQL_TYPE_GEOMETRY:
		case MYSQL_TYPE_NULL:
			if (!(field->max_length))
				return BufferSizePair(new char[1], 1);
			return BufferSizePair(new char[field->max_length], field->max_length);
		default:
			throw sql::InvalidArgumentException("allocate_buffer_for_type: invalid result_bind data type");
	}
}


/* {{{ MySQL_Prepared_Statement::bindResult() -I- */
void
MySQL_Prepared_Statement::bindResult()
{
	CPP_ENTER("MySQL_Prepared_Statement::bindResult");
	for (unsigned int i = 0; i < num_fields; i++) {
		delete[] (char *) result_bind[i].buffer;
	}
	delete[] result_bind;
	delete[] is_null;
	delete[] err;
	delete[] len;

	result_bind = NULL;
	is_null = NULL;
	err = NULL;
	len = NULL;

	MYSQL_RES * result_meta = mysql_stmt_result_metadata(stmt);
	num_fields = mysql_stmt_field_count(stmt);
	result_bind = new MYSQL_BIND[num_fields];
	memset(result_bind, 0, sizeof(MYSQL_BIND) * num_fields);

	is_null = new my_bool[num_fields];
	memset(is_null, 0, sizeof(my_bool) * num_fields);

	err = new my_bool[num_fields];
	memset(err, 0, sizeof(my_bool) * num_fields);

	len = new unsigned long[num_fields];
	memset(len, 0, sizeof(unsigned long) * num_fields);

	my_bool	tmp=1;
	mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &tmp);
	mysql_stmt_store_result(stmt);

	for (unsigned int i = 0; i < num_fields; i++) {
		MYSQL_FIELD *field = mysql_fetch_field(result_meta);

		BufferSizePair p = allocate_buffer_for_type(field);
		result_bind[i].buffer_type	= field->type;
		result_bind[i].buffer		= p.first;
		result_bind[i].buffer_length= p.second;
		result_bind[i].length		= &len[i];
		result_bind[i].is_null		= &is_null[i];
		result_bind[i].error		= &err[i];
		if (field->type == MYSQL_TYPE_BLOB || field->type == MYSQL_TYPE_MEDIUM_BLOB ||
			field->type == MYSQL_TYPE_LONG_BLOB) {

			if (!(result_bind[i].buffer_length = field->max_length))
				++result_bind[i].buffer_length;
			result_bind[i].buffer = new char[result_bind[i].buffer_length];
		}
	}
	mysql_free_result(result_meta);
	result_meta = NULL;
	if (mysql_stmt_bind_result(stmt, result_bind)) {
		throw sql::SQLException(mysql_stmt_error(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_errno(stmt));
	}
}

/* {{{ MySQL_Prepared_Statement::executeQuery() -I- */
sql::ResultSet *
MySQL_Prepared_Statement::executeQuery()
{
	CPP_ENTER("MySQL_Prepared_Statement::executeQuery");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();

	do_query();

	bindResult();

	sql::ResultSet * tmp = new MySQL_Prepared_ResultSet(stmt, this, logger);
	CPP_INFO_FMT("rset=%p", tmp);
	return tmp;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::executeQuery() -U- */
sql::ResultSet *
MySQL_Prepared_Statement::executeQuery(const std::string&)
{
	throw sql::MethodNotImplementedException("MySQL_Prepared_Statement::executeQuery"); /* TODO - what to do? Comes from Statement */
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::executeUpdate() -I- */
int
MySQL_Prepared_Statement::executeUpdate()
{
	CPP_ENTER("MySQL_Prepared_Statement::executeUpdate");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	do_query();
	return static_cast<int>(mysql_stmt_affected_rows(stmt));
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::executeUpdate() -U- */
int
MySQL_Prepared_Statement::executeUpdate(const std::string&)
{
	throw sql::MethodNotImplementedException("MySQL_Prepared_Statement::executeUpdate"); /* TODO - what to do? Comes from Statement */
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setBlob() -U- */
void
MySQL_Prepared_Statement::setBlob(unsigned int parameterIndex, sql::Blob & blob)
{
	CPP_ENTER("MySQL_Prepared_Statement::setBlob");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	parameterIndex--; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setBlob: invalid 'parameterIndex'");
	}
	do {
		std::string chunk = blob.readChunk(1024);
		if (!chunk.length()) {
			break;
		}
		if (mysql_stmt_send_long_data(stmt, parameterIndex, chunk.c_str(), chunk.length())) {
			switch (mysql_stmt_errno(stmt)) {
				case CR_OUT_OF_MEMORY:
					throw std::bad_alloc();
				case CR_INVALID_BUFFER_USE:
					throw InvalidArgumentException("MySQL_Prepared_Statement::setBlob: can't set blob value on that column");
				case CR_SERVER_GONE_ERROR:
				case CR_COMMANDS_OUT_OF_SYNC:
				default:
					throw SQLException(mysql_stmt_error(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_errno(stmt));
			}
		}
	} while (1);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setBoolean() -I- */
void
MySQL_Prepared_Statement::setBoolean(unsigned int parameterIndex, bool value)
{
	setInt(parameterIndex, value);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setDateTime() -I- */
void
MySQL_Prepared_Statement::setDateTime(unsigned int parameterIndex, const std::string& value)
{
	CPP_ENTER("MySQL_Prepared_Statement::setDateTime");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	parameterIndex--; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setDateTime: invalid 'parameterIndex'");
	}
	setString(parameterIndex, value);
}
/* }}} */


typedef std::pair<char *, int> BufferSizePair;
static BufferSizePair
allocate_buffer_for_type(enum_field_types t)
{
	switch(t) {
		case MYSQL_TYPE_TINY:
			return BufferSizePair(new char[1], 1);
		case MYSQL_TYPE_SHORT:
			return BufferSizePair(new char[2], 2);
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_FLOAT:
			return BufferSizePair(new char[4], 4);
		case MYSQL_TYPE_DOUBLE:
		case MYSQL_TYPE_LONGLONG:
			return BufferSizePair(new char[8], 8);
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATETIME:
			return BufferSizePair(new char[sizeof(MYSQL_TIME)], sizeof(MYSQL_TIME));
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_VAR_STRING:
			return BufferSizePair(NULL, 0);

		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
			return BufferSizePair(new char[64], 64);
		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_YEAR:
			return BufferSizePair(new char[10], 10);
		case MYSQL_TYPE_SET:
		case MYSQL_TYPE_BIT:
		case MYSQL_TYPE_ENUM:
		case MYSQL_TYPE_GEOMETRY:
		case MYSQL_TYPE_NULL:
			return BufferSizePair(NULL, 0);
		default:
			throw sql::InvalidArgumentException("allocate_buffer_for_type: invalid result_bind data type");
	}
}


/* {{{ MySQL_Prepared_Statement::setDouble() -I- */
void
MySQL_Prepared_Statement::setDouble(unsigned int parameterIndex, double value)
{
	CPP_ENTER("MySQL_Prepared_Statement::setDouble");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();

	parameterIndex--; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setDouble: invalid 'parameterIndex'");
	}

	enum_field_types t = MYSQL_TYPE_DOUBLE;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind[parameterIndex].buffer_type	= t;
	delete[] (char *) param_bind[parameterIndex].buffer;
	param_bind[parameterIndex].buffer		= p.first;
	param_bind[parameterIndex].buffer_length= 0;
	param_bind[parameterIndex].is_null_value= 0;
	delete param_bind[parameterIndex].length;
	param_bind[parameterIndex].length		= NULL;

	memcpy(param_bind[parameterIndex].buffer, &value, p.second);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setInt() -I- */
void
MySQL_Prepared_Statement::setInt(unsigned int parameterIndex, int value)
{
	CPP_ENTER("MySQL_Prepared_Statement::setInt");
	CPP_INFO_FMT("this=%p", this);
	CPP_INFO_FMT("column=%u value=%d", parameterIndex, value);
	checkClosed();

	parameterIndex--; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setInt: invalid 'parameterIndex'");
	}

	enum_field_types t = MYSQL_TYPE_LONG;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind[parameterIndex].buffer_type	= t;
	delete[] (char *) param_bind[parameterIndex].buffer;
	param_bind[parameterIndex].buffer		= p.first;
	param_bind[parameterIndex].buffer_length= 0;
	param_bind[parameterIndex].is_null_value= 0;
	delete param_bind[parameterIndex].length;
	param_bind[parameterIndex].length		= NULL;

	memcpy(param_bind[parameterIndex].buffer, &value, p.second);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setLong() -I- */
void
MySQL_Prepared_Statement::setLong(unsigned int parameterIndex, long long value)
{
	CPP_ENTER("MySQL_Prepared_Statement::setLong");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();

	parameterIndex--; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setLong: invalid 'parameterIndex'");
	}

	enum_field_types t = MYSQL_TYPE_LONGLONG;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind[parameterIndex].buffer_type	= t;
	delete[] (char *) param_bind[parameterIndex].buffer;
	param_bind[parameterIndex].buffer		= p.first;
	param_bind[parameterIndex].buffer_length= 0;
	param_bind[parameterIndex].is_null_value= 0;
	delete param_bind[parameterIndex].length;
	param_bind[parameterIndex].length		= NULL;

	memcpy(param_bind[parameterIndex].buffer, &value, p.second);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setBigInt() -I- */
void
MySQL_Prepared_Statement::setBigInt(unsigned int parameterIndex, const std::string& value)
{
	CPP_ENTER("MySQL_Prepared_Statement::setBigInt");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	parameterIndex--; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setBigInt: invalid 'parameterIndex'");
	}
	setString(parameterIndex, value);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::cancel() -U- */
void
MySQL_Prepared_Statement::cancel()
{
	CPP_ENTER("MySQL_Prepared_Statement::cancel");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::cancel");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getFetchSize() -U- */
unsigned int
MySQL_Prepared_Statement::getFetchSize()
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::getFetchSize");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getParameterMetaData() -I- */
sql::ParameterMetaData *
MySQL_Prepared_Statement::getParameterMetaData()
{
	CPP_ENTER("MySQL_Prepared_Statement::getParameterMetaData");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	return new MySQL_ParameterMetaData(stmt);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getResultSet() -I- */
sql::ResultSet *
MySQL_Prepared_Statement::getResultSet()
{
	CPP_ENTER("MySQL_Prepared_Statement::getResultSet");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	if (mysql_more_results(mysql_stmt_conn(stmt))) {
		mysql_next_result(mysql_stmt_conn(stmt));
	}
	bindResult();

	sql::ResultSet * tmp = new MySQL_Prepared_ResultSet(stmt, this, logger);
	CPP_INFO_FMT("rset=%p", tmp);
	return tmp;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setFetchSize() -U- */
void
MySQL_Prepared_Statement::setFetchSize(unsigned int)
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::setFetchSize");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setQueryTimeout() -U- */
void
MySQL_Prepared_Statement::setQueryTimeout(unsigned int)
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::setQueryTimeout");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::clearWarnings() -I- */
void
MySQL_Prepared_Statement::clearWarnings()
{
	CPP_ENTER("MySQL_Prepared_Statement::clearWarnings");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	warnings.reset();
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::close() -I- */
void
MySQL_Prepared_Statement::close()
{
	CPP_ENTER("MySQL_Prepared_Statement::close");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	closeIntern();
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getMaxFieldSize() -U- */
unsigned int
MySQL_Prepared_Statement::getMaxFieldSize()
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::getMaxFieldSize");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getMaxRows() -U- */
unsigned long long
MySQL_Prepared_Statement::getMaxRows()
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::getMaxRows");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getMoreResults() -U- */
bool
MySQL_Prepared_Statement::getMoreResults()
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::getMoreResults");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getQueryTimeout() -U- */
unsigned int
MySQL_Prepared_Statement::getQueryTimeout()
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::getQueryTimeout");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getUpdateCount() -U- */
long long
MySQL_Prepared_Statement::getUpdateCount()
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::getUpdateCount");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getWarnings() -I- */
const SQLWarning *
MySQL_Prepared_Statement::getWarnings()
{
	CPP_ENTER("MySQL_Prepared_Statement::getWarnings");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();

  warnings.reset( loadMysqlWarnings( dynamic_cast<MySQL_Connection*>(connection) ) );

  return warnings.get();
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setCursorName() -U- */
void
MySQL_Prepared_Statement::setCursorName(const std::string &)
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::setCursorName");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setEscapeProcessing() -U- */
void
MySQL_Prepared_Statement::setEscapeProcessing(bool)
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::setEscapeProcessing");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setMaxFieldSize() -U- */
void
MySQL_Prepared_Statement::setMaxFieldSize(unsigned int)
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::setMaxFieldSize");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setMaxRows() -U- */
void
MySQL_Prepared_Statement::setMaxRows(unsigned int)
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::setMaxRows");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setResultSetConcurrency() -U- */
void
MySQL_Prepared_Statement::setResultSetConcurrency(int)
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::setResultSetConcurrency");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setResultSetType() -U- */
void
MySQL_Prepared_Statement::setResultSetType(int)
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::setResultSetType");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setString() -I- */
void
MySQL_Prepared_Statement::setString(unsigned int parameterIndex, const std::string& value)
{
	CPP_ENTER("MySQL_Prepared_Statement::setString");
	CPP_INFO_FMT("this=%p", this);
	CPP_INFO_FMT("column=%u value_len=%d value=%s ", parameterIndex, value.length(), value.c_str());
	checkClosed();

	parameterIndex--; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		CPP_ERR("Invalid parameterIndex");
		throw InvalidArgumentException("MySQL_Prepared_Statement::setString: invalid 'parameterIndex'");
	}
	enum_field_types t = MYSQL_TYPE_STRING;
	delete[] (char*) param_bind[parameterIndex].buffer;

	param_bind[parameterIndex].buffer_type	= t;
	param_bind[parameterIndex].buffer		= memcpy(new char[value.length() + 1], value.c_str(), value.length() + 1);
	param_bind[parameterIndex].buffer_length= static_cast<unsigned long>(value.length()) + 1;
	param_bind[parameterIndex].is_null_value= 0;

	delete (unsigned long *) param_bind[parameterIndex].length;
	param_bind[parameterIndex].length = new unsigned long(static_cast<unsigned long>(value.length()));
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::checkClosed() -I- */
void
MySQL_Prepared_Statement::checkClosed()
{
	CPP_ENTER("MySQL_Prepared_Statement::checkClosed");
	if (isClosed) {
		CPP_ERR("Statement has already been closed");
		throw sql::InvalidInstanceException("Statement has been closed");
	}
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::closeIntern() -I- */
void
MySQL_Prepared_Statement::closeIntern()
{
	CPP_ENTER("MySQL_Prepared_Statement::closeIntern");
	mysql_stmt_close(stmt);
	clearParameters();
	/* allocated with calloc */
	free(param_bind);

	for (unsigned int i = 0; i < num_fields; i++) {
		delete[] (char *) result_bind[i].buffer;
	}
	delete[] result_bind;
	delete[] is_null;
	delete[] err;
	delete[] len;
}
/* }}} */

};/* namespace mysql */
};/* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
