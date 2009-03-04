/*
   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/
#include <stdlib.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <cppconn/exception.h>
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


class MySQL_ParamBind
{
	MYSQL_BIND * bind;
	bool 		* value_set;
	unsigned int param_count;

	std::istream	** blob_bind;

public:

	MySQL_ParamBind(unsigned int paramCount)
	{
		if (!paramCount) {
			bind = NULL;
			value_set = NULL;
			blob_bind = NULL;
		} else {
			bind = (MYSQL_BIND *) calloc(paramCount, sizeof(MYSQL_BIND));
			value_set = new bool[paramCount];
			for (unsigned int i = 0; i < paramCount; ++i) {
				bind[i].is_null_value = 1;
				value_set[i] = false;
			}
			blob_bind = (std::istream **) calloc(paramCount, sizeof(std::istream *));
		}
		param_count = paramCount;
	}

	virtual ~MySQL_ParamBind()
	{
		clearParameters();

		free(bind);
		delete [] value_set;
		free(blob_bind);
	}

	void set(unsigned int position)
	{
		value_set[position] = true;
	}

	void setBlob(unsigned int position, std::istream * blob)
	{
		blob_bind[position] = blob;
	}

	bool isAllSet()
	{
		for (unsigned int i = 0; i < param_count; ++i) {
			if (!value_set[i]) {
				return false;
			}
		}
		return true;
	}

	void clearParameters()
	{
		for (unsigned int i = 0; i < param_count; ++i) {
			if ( value_set[i] ){
			  delete (char*) bind[i].length;
			  bind[i].length = NULL;
			  delete[] (char*) bind[i].buffer;
			  bind[i].buffer = NULL;
			  blob_bind[i] = NULL;
			  value_set[i] = false;
			}
		}
	}

	MYSQL_BIND * get()
	{
		return bind;
	}

	std::istream * getBlobObject(unsigned int position)
	{
		return blob_bind[position];
	}

};


/* {{{ MySQL_Prepared_Statement::MySQL_Prepared_Statement() -I- */
MySQL_Prepared_Statement::MySQL_Prepared_Statement(MYSQL_STMT *s, sql::Connection * conn, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * log)
	:connection(conn), stmt(s), isClosed(false), logger(log? log->getReference():NULL)
{
	CPP_ENTER("MySQL_Prepared_Statement::MySQL_Prepared_Statement");
	CPP_INFO_FMT("this=%p", this);
	param_count = mysql_stmt_param_count(s);
	param_bind = new MySQL_ParamBind(param_count);

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


/* {{{ MySQL_Prepared_Statement::sendLongDataBeforeParamBind() -I- */
bool
MySQL_Prepared_Statement::sendLongDataBeforeParamBind()
{
	CPP_ENTER("MySQL_Prepared_Statement::sendLongDataBeforeParamBind");
	MYSQL_BIND * bind = param_bind->get();
	char buf[1024];
	for (unsigned int i = 0; i < param_count; ++i) {
		if (bind[i].buffer_type == MYSQL_TYPE_LONG_BLOB) {
			std::istream * my_blob = param_bind->getBlobObject(i);
			do {
				if ((my_blob->rdstate() & std::istream::eofbit) != 0 ) {
					break;
				}
				my_blob->read(buf, sizeof(buf));

				if ((my_blob->rdstate() & std::istream::badbit) != 0) {
					throw SQLException("Error while reading from blob (bad)");
				} else if ((my_blob->rdstate() & std::istream::failbit) != 0) {
					if ((my_blob->rdstate() & std::istream::eofbit) == 0) {
						throw SQLException("Error while reading from blob (fail)");
					}
				}
				if (mysql_stmt_send_long_data(stmt, i, buf, my_blob->gcount())) {
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
	}
	return true;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::do_query() -I- */
void
MySQL_Prepared_Statement::do_query()
{
	CPP_ENTER("MySQL_Prepared_Statement::do_query");
	if (param_count && !param_bind->isAllSet()) {
		CPP_ERR("Value not set for all parameters");
		throw sql::SQLException("Value not set for all parameters");
	}
	if (mysql_stmt_bind_param(stmt, param_bind->get())) {
		CPP_ERR("Couldn't bind");
		throw sql::SQLException(mysql_stmt_error(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_errno(stmt));
	}
	if (!sendLongDataBeforeParamBind() || mysql_stmt_execute(stmt)) {
		CPP_ERR("Couldn't execute");
		throw sql::SQLException(mysql_stmt_error(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_errno(stmt));
	}
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::clearParameters() -I- */
void
MySQL_Prepared_Statement::clearParameters()
{
	CPP_ENTER("MySQL_Prepared_Statement::clearParameters");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	param_bind->clearParameters();
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
bool
MySQL_Prepared_Statement::execute()
{
	CPP_ENTER("MySQL_Prepared_Statement::execute");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	do_query();
	return (mysql_stmt_field_count(stmt) > 0);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::execute() -U- */
bool
MySQL_Prepared_Statement::execute(const std::string&)
{
	CPP_ENTER("MySQL_Prepared_Statement::execute(const std::string& sql)");
	throw sql::MethodNotImplementedException("MySQL_Prepared_Statement::execute");
	return false; // fool compilers
}
/* }}} */


typedef std::pair<char *, int> BufferSizePair;
static BufferSizePair
allocate_buffer_for_type(MYSQL_FIELD *field)
{
	switch (field->type) {
		case MYSQL_TYPE_NULL:
			return BufferSizePair(NULL, 0);
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
#if A0
		// There two are not sent over the wire
		case MYSQL_TYPE_ENUM:
		case MYSQL_TYPE_SET:
#endif
		case MYSQL_TYPE_BIT:
		case MYSQL_TYPE_GEOMETRY:
		default:
			throw sql::InvalidArgumentException("allocate_buffer_for_type: invalid result_bind data type");
	}
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::bindResult() -I- */
void
MySQL_Prepared_Statement::bindResult()
{
	CPP_ENTER("MySQL_Prepared_Statement::bindResult");
	for (unsigned int i = 0; i < num_fields; ++i) {
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

	for (unsigned int i = 0; i < num_fields; ++i) {
		MYSQL_FIELD * field = mysql_fetch_field(result_meta);

		BufferSizePair p = allocate_buffer_for_type(field);
		result_bind[i].buffer_type	= field->type;
		result_bind[i].buffer		= p.first;
		result_bind[i].buffer_length= p.second;
		result_bind[i].length		= &len[i];
		result_bind[i].is_null		= &is_null[i];
		result_bind[i].error		= &err[i];
		result_bind[i].is_unsigned = field->flags & UNSIGNED_FLAG;

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
/* }}} */


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
	return NULL; // fool compilers
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
	return 0; // fool compilers
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setBigInt() -I- */
void
MySQL_Prepared_Statement::setBigInt(unsigned int parameterIndex, const std::string& value)
{
	CPP_ENTER("MySQL_Prepared_Statement::setBigInt");
	setString(parameterIndex, value);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setBlob() -I- */
void
MySQL_Prepared_Statement::setBlob(unsigned int parameterIndex, std::istream * blob)
{
	CPP_ENTER("MySQL_Prepared_Statement::setBlob");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	--parameterIndex; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setBlob: invalid 'parameterIndex'");
	}

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	delete[] (char*) param->buffer;

	param->buffer_type	= MYSQL_TYPE_LONG_BLOB;
	param->buffer		= NULL;
	param->buffer_length= 0;
	param->is_null_value= 0;

	delete (unsigned long *) param->length;
	param->length = new unsigned long(0);

	param_bind->setBlob(parameterIndex, blob);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setBoolean() -I- */
void
MySQL_Prepared_Statement::setBoolean(unsigned int parameterIndex, bool value)
{
	CPP_ENTER("MySQL_Prepared_Statement::setBoolean");
	setInt(parameterIndex, value);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setDateTime() -I- */
void
MySQL_Prepared_Statement::setDateTime(unsigned int parameterIndex, const std::string& value)
{
	CPP_ENTER("MySQL_Prepared_Statement::setDateTime");
	setString(parameterIndex, value);
}
/* }}} */


typedef std::pair<char *, int> BufferSizePair;
static BufferSizePair
allocate_buffer_for_type(enum_field_types t)
{
	switch (t) {
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
		case MYSQL_TYPE_NEWDATE:
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
#if A0
		// There two are not sent over the wire
		case MYSQL_TYPE_SET:
		case MYSQL_TYPE_ENUM:
#endif
		case MYSQL_TYPE_GEOMETRY:
		case MYSQL_TYPE_BIT:
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

	--parameterIndex; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setDouble: invalid 'parameterIndex'");
	}

	enum_field_types t = MYSQL_TYPE_DOUBLE;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	param->buffer_type	= t;
	delete[] (char *) param->buffer;
	param->buffer = p.first;
	param->buffer_length = 0;
	param->is_null_value = 0;
	delete param->length;
	param->length	= NULL;

	memcpy(param->buffer, &value, p.second);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setInt() -I- */
void
MySQL_Prepared_Statement::setInt(unsigned int parameterIndex, int32_t value)
{
	CPP_ENTER("MySQL_Prepared_Statement::setInt");
	CPP_INFO_FMT("this=%p", this);
	CPP_INFO_FMT("column=%u value=%d", parameterIndex, value);
	checkClosed();

	--parameterIndex; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setInt: invalid 'parameterIndex'");
	}

	enum_field_types t = MYSQL_TYPE_LONG;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	param->buffer_type	= t;
	delete[] (char *) param->buffer;
	param->buffer		= p.first;
	param->buffer_length = 0;
	param->is_null_value = 0;
	delete param->length;
	param->length		= NULL;

	memcpy(param->buffer, &value, p.second);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setUInt() -I- */
void
MySQL_Prepared_Statement::setUInt(unsigned int parameterIndex, uint32_t value)
{
	CPP_ENTER("MySQL_Prepared_Statement::setUInt");
	CPP_INFO_FMT("this=%p", this);
	CPP_INFO_FMT("column=%u value=%u", parameterIndex, value);
	checkClosed();

	--parameterIndex; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setInt: invalid 'parameterIndex'");
	}

	enum_field_types t = MYSQL_TYPE_LONG;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	param->buffer_type	= t;
	delete[] (char *) param->buffer;
	param->buffer		= p.first;
	param->buffer_length = 0;
	param->is_null_value = 0;
	param->is_unsigned	= 1;
	delete param->length;
	param->length		= NULL;

	memcpy(param->buffer, &value, p.second);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setInt64() -I- */
void
MySQL_Prepared_Statement::setInt64(unsigned int parameterIndex, int64_t value)
{
	CPP_ENTER("MySQL_Prepared_Statement::setInt64");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();

	--parameterIndex; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setInt64: invalid 'parameterIndex'");
	}

	enum_field_types t = MYSQL_TYPE_LONGLONG;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	param->buffer_type	= t;
	delete[] (char *) param->buffer;
	param->buffer		= p.first;
	param->buffer_length = 0;
	param->is_null_value = 0;
	delete param->length;
	param->length = NULL;

	memcpy(param->buffer, &value, p.second);
}
/* }}} */

/* {{{ MySQL_Prepared_Statement::setUInt64() -I- */
void
MySQL_Prepared_Statement::setUInt64(unsigned int parameterIndex, uint64_t value)
{
	CPP_ENTER("MySQL_Prepared_Statement::setUInt64");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();

	--parameterIndex; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setUInt64: invalid 'parameterIndex'");
	}

	enum_field_types t = MYSQL_TYPE_LONGLONG;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	param->buffer_type	= t;
	delete[] (char *) param->buffer;
	param->buffer		= p.first;
	param->buffer_length = 0;
	param->is_null_value = 0;
	param->is_unsigned = 1;
	delete param->length;
	param->length = NULL;

	memcpy(param->buffer, &value, p.second);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setNull() -I- */
void
MySQL_Prepared_Statement::setNull(unsigned int parameterIndex, int /* sqlType */)
{
	CPP_ENTER("MySQL_Prepared_Statement::setNull");
	CPP_INFO_FMT("this=%p", this);
	CPP_INFO_FMT("column=%u", parameterIndex);
	checkClosed();

	--parameterIndex; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setInt: invalid 'parameterIndex'");
	}

	enum_field_types t = MYSQL_TYPE_NULL;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	param->buffer_type	= t;
	delete[] (char *) param->buffer;
	param->buffer = NULL;
	delete param->length;
	param->length = NULL;
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

	--parameterIndex; /* DBC counts from 1 */
	if (parameterIndex >= param_count) {
		CPP_ERR("Invalid parameterIndex");
		throw InvalidArgumentException("MySQL_Prepared_Statement::setString: invalid 'parameterIndex'");
	}
	enum_field_types t = MYSQL_TYPE_STRING;

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	delete[] (char*) param->buffer;

	param->buffer_type	= t;
	param->buffer		= memcpy(new char[value.length() + 1], value.c_str(), value.length() + 1);
	param->buffer_length= static_cast<unsigned long>(value.length()) + 1;
	param->is_null_value= 0;

	delete (unsigned long *) param->length;
	param->length = new unsigned long(static_cast<unsigned long>(value.length()));
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
	return 0; // fool compilers
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
	return 0; // fool compilers
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getMaxRows() -U- */
uint64_t
MySQL_Prepared_Statement::getMaxRows()
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::getMaxRows");
	return 0; // fool compilers
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getMoreResults() -U- */
bool
MySQL_Prepared_Statement::getMoreResults()
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::getMoreResults");
	return false; // fool compilers
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getQueryTimeout() -U- */
unsigned int
MySQL_Prepared_Statement::getQueryTimeout()
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::getQueryTimeout");
	return 0; // fool compilers
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getUpdateCount() -U- */
uint64_t
MySQL_Prepared_Statement::getUpdateCount()
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::getUpdateCount");
	return 0; // fool compilers
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

	delete param_bind;

	for (unsigned int i = 0; i < num_fields; ++i) {
		delete[] (char *) result_bind[i].buffer;
	}
	delete[] result_bind;
	delete[] is_null;
	delete[] err;
	delete[] len;

	isClosed = true;
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
