/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <cppconn/exception.h>
#include "mysql_util.h"
#include "mysql_connection.h"
#include "mysql_statement.h"
#include "mysql_prepared_statement.h"
#include "mysql_ps_resultset.h"
#include "mysql_ps_resultset_metadata.h"
#include "mysql_parameter_metadata.h"
#include "mysql_warning.h"
#include "mysql_resultbind.h"


#define mysql_stmt_conn(s) (s)->mysql

#include "mysql_debug.h"


namespace sql
{

namespace mysql
{



class MySQL_ParamBind
{
	unsigned int param_count;
	sql::mysql::util::my_array_guard< MYSQL_BIND > bind;
	sql::mysql::util::my_array_guard< bool > value_set;
	sql::mysql::util::my_array_guard< bool > delete_blob_after_execute;

	sql::mysql::util::my_array_guard< std::istream	* > blob_bind;

public:

	MySQL_ParamBind(unsigned int paramCount)
		: param_count(paramCount), bind(NULL), value_set(NULL),
		  delete_blob_after_execute(NULL), blob_bind(NULL)
	{
		if (param_count) {
			bind.reset(new MYSQL_BIND[paramCount]);
			memset(bind.get(), 0, sizeof(MYSQL_BIND) * paramCount);

			value_set.reset(new bool[paramCount]);
			delete_blob_after_execute.reset(new bool[paramCount]);
			for (unsigned int i = 0; i < paramCount; ++i) {
				bind[i].is_null_value = 1;
				value_set[i] = false;
				delete_blob_after_execute[i] = false;
			}

			blob_bind.reset(new std::istream *[paramCount]);
			memset(blob_bind.get(), 0, sizeof(std::istream *) * paramCount);
		}
	}

	virtual ~MySQL_ParamBind()
	{
		clearParameters();

		if (blob_bind.get()) {
			for (unsigned int i = 0; i < param_count; ++i) {
				if (delete_blob_after_execute[i]) {
					delete_blob_after_execute[i] = false;
					delete blob_bind[i];
					blob_bind[i] = NULL;
				}
			}
		}
	}

	void set(unsigned int position)
	{
		value_set[position] = true;
	}

	void unset(unsigned int position)
	{
		value_set[position] = false;
		if (delete_blob_after_execute[position]) {
			delete_blob_after_execute[position] = false;
			delete blob_bind[position];
			blob_bind[position] = NULL;
		}
	}

	void setBlob(unsigned int position, std::istream * blob, bool delete_after_execute)
	{
		if (blob_bind[position] && delete_blob_after_execute[position]) {
			delete blob_bind[position];
		}
		blob_bind[position] = blob;
		delete_blob_after_execute[position] = delete_after_execute;
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
			delete (char*) bind[i].length;
			bind[i].length = NULL;
			delete[] (char*) bind[i].buffer;
			bind[i].buffer = NULL;
			if (value_set[i]) {
				if (blob_bind[i] && delete_blob_after_execute[i]) {
					delete blob_bind[i];
				}
				blob_bind[i] = NULL;
				value_set[i] = false;
			}
		}
	}

	MYSQL_BIND * get()
	{
		return bind.get();
	}

	std::istream * getBlobObject(unsigned int position)
	{
		return blob_bind[position];
	}

};



/* {{{ MySQL_Prepared_Statement::MySQL_Prepared_Statement() -I- */
MySQL_Prepared_Statement::MySQL_Prepared_Statement(
			MYSQL_STMT *s, sql::Connection * conn, sql::ResultSet::enum_type rset_type,
			sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * log
		)
	:connection(conn), stmt(s), isClosed(false),
	 logger(log? log->getReference():NULL), resultset_type(rset_type)
{
	CPP_ENTER("MySQL_Prepared_Statement::MySQL_Prepared_Statement");
	CPP_INFO_FMT("this=%p", this);
	param_count = mysql_stmt_param_count(s);
	param_bind.reset(new MySQL_ParamBind(param_count));

	res_meta.reset(new MySQL_Prepared_ResultSetMetaData(stmt, logger));
	param_meta.reset(new MySQL_ParameterMetaData(stmt));
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
				if (mysql_stmt_send_long_data(stmt, i, buf, static_cast<unsigned long>(my_blob->gcount()))) {
					CPP_ERR_FMT("Couldn't send long data : %d:(%s) %s", mysql_stmt_errno(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_error(stmt));
					switch (mysql_stmt_errno(stmt)) {
						case CR_OUT_OF_MEMORY:
							throw std::bad_alloc();
						case CR_INVALID_BUFFER_USE:
							throw InvalidArgumentException("MySQL_Prepared_Statement::setBlob: can't set blob value on that column");
						case CR_SERVER_GONE_ERROR:
						case CR_COMMANDS_OUT_OF_SYNC:
						default:		
							sql::mysql::util::throwSQLException(stmt);
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
		CPP_ERR_FMT("Couldn't bind : %d:(%s) %s", mysql_stmt_errno(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_error(stmt));
		sql::mysql::util::throwSQLException(stmt);
	}
	if (!sendLongDataBeforeParamBind() || mysql_stmt_execute(stmt)) {
		CPP_ERR_FMT("Couldn't execute : %d:(%s) %s", mysql_stmt_errno(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_error(stmt));
		sql::mysql::util::throwSQLException(stmt);
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
sql::Connection *
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


/* {{{ MySQL_Prepared_Statement::executeQuery() -I- */
sql::ResultSet *
MySQL_Prepared_Statement::executeQuery()
{
	CPP_ENTER("MySQL_Prepared_Statement::executeQuery");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();

	do_query();

	my_bool	bool_tmp=1;
	mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &bool_tmp);
	sql::ResultSet::enum_type tmp_type;
	if (resultset_type == sql::ResultSet::TYPE_SCROLL_INSENSITIVE) {
		mysql_stmt_store_result(stmt);
		tmp_type = sql::ResultSet::TYPE_SCROLL_INSENSITIVE;
	} else if (resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY) {
		tmp_type = sql::ResultSet::TYPE_FORWARD_ONLY;
	} else {
		throw SQLException("Invalid value for result set type");
	}
	
	std::auto_ptr< MySQL_ResultBind > result_bind(new MySQL_ResultBind(stmt, logger));
	
	sql::ResultSet * tmp = new MySQL_Prepared_ResultSet(stmt, result_bind.get(), tmp_type, this, logger);
	result_bind.release();

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


/* {{{ MySQL_Prepared_Statement::setBlob_intern() -I- */
void
MySQL_Prepared_Statement::setBlob_intern(unsigned int parameterIndex, std::istream * blob, bool deleteBlobAfterExecute)
{
	CPP_ENTER("MySQL_Prepared_Statement::setBlob_intern");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();

	--parameterIndex; /* DBC counts from 1 */

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	delete [] static_cast<char *>(param->buffer);

	param->buffer_type	= MYSQL_TYPE_LONG_BLOB;
	param->buffer		= NULL;
	param->buffer_length= 0;
	param->is_null_value= 0;

	delete param->length;
	param->length = new unsigned long(0);

	param_bind->setBlob(parameterIndex, blob, deleteBlobAfterExecute);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setBlob() -I- */
void
MySQL_Prepared_Statement::setBlob(unsigned int parameterIndex, std::istream * blob)
{
	CPP_ENTER("MySQL_Prepared_Statement::setBlob");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();

	if (parameterIndex == 0 || parameterIndex > param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setBlob: invalid 'parameterIndex'");
	}

	setBlob_intern(parameterIndex, blob, false);
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

typedef std::pair<char *, size_t> BufferSizePair;
static BufferSizePair
allocate_buffer_for_type(enum_field_types t)
{
	switch (t) {
#if A1
		// We don't use these now. When we have setXXX, we can enable them
		case MYSQL_TYPE_TINY:
			return BufferSizePair(new char[1], 1);
		case MYSQL_TYPE_SHORT:
			return BufferSizePair(new char[2], 2);
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_FLOAT:
#endif
		case MYSQL_TYPE_LONG:
			return BufferSizePair(new char[4], 4);
		case MYSQL_TYPE_DOUBLE:
		case MYSQL_TYPE_LONGLONG:
			return BufferSizePair(new char[8], 8);
#if A1
		// We don't use these now. When we have setXXX, we can enable them
		case MYSQL_TYPE_NEWDATE:
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATETIME:
			return BufferSizePair(new char[sizeof(MYSQL_TIME)], sizeof(MYSQL_TIME));
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_VAR_STRING:
#endif
		case MYSQL_TYPE_STRING:
			return BufferSizePair(NULL, 0);

#if A1
		// We don't use these now. When we have setXXX, we can enable them
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
			return BufferSizePair(new char[64], 64);
		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_YEAR:
			return BufferSizePair(new char[10], 10);
#endif
#if A0
		// There two are not sent over the wire
		case MYSQL_TYPE_SET:
		case MYSQL_TYPE_ENUM:
#endif
#if A1
		// We don't use these now. When we have setXXX, we can enable them
		case MYSQL_TYPE_GEOMETRY:
		case MYSQL_TYPE_BIT:
#endif
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
	CPP_INFO_FMT("this=%p %f", this, value);
	checkClosed();

	if (parameterIndex == 0 || parameterIndex > param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setDouble: invalid 'parameterIndex'");
	}
	--parameterIndex; /* DBC counts from 1 */

	if (param_bind->getBlobObject(parameterIndex)) {
		param_bind->setBlob(parameterIndex, NULL, false);
		param_bind->unset(parameterIndex);
	}

	enum_field_types t = MYSQL_TYPE_DOUBLE;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	param->buffer_type	= t;
	delete [] static_cast<char *>(param->buffer);
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

	if (parameterIndex == 0 || parameterIndex > param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setInt: invalid 'parameterIndex'");
	}
	--parameterIndex; /* DBC counts from 1 */

	if (param_bind->getBlobObject(parameterIndex)) {
		param_bind->setBlob(parameterIndex, NULL, false);
		param_bind->unset(parameterIndex);
	}

	enum_field_types t = MYSQL_TYPE_LONG;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	param->buffer_type	= t;
	delete [] static_cast<char *>(param->buffer);
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

	if (parameterIndex == 0 || parameterIndex > param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setInt: invalid 'parameterIndex'");
	}
	--parameterIndex; /* DBC counts from 1 */

	if (param_bind->getBlobObject(parameterIndex)) {
		param_bind->setBlob(parameterIndex, NULL, false);
		param_bind->unset(parameterIndex);
	}

	enum_field_types t = MYSQL_TYPE_LONG;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	param->buffer_type	= t;
	delete [] static_cast<char *>(param->buffer);
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

	if (parameterIndex == 0 || parameterIndex > param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setInt64: invalid 'parameterIndex'");
	}
	--parameterIndex; /* DBC counts from 1 */

	if (param_bind->getBlobObject(parameterIndex)) {
		param_bind->setBlob(parameterIndex, NULL, false);
		param_bind->unset(parameterIndex);
	}

	enum_field_types t = MYSQL_TYPE_LONGLONG;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	param->buffer_type	= t;
	delete [] static_cast<char *>(param->buffer);
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

	if (parameterIndex == 0 || parameterIndex > param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setUInt64: invalid 'parameterIndex'");
	}
	--parameterIndex; /* DBC counts from 1 */

	if (param_bind->getBlobObject(parameterIndex)) {
		param_bind->setBlob(parameterIndex, NULL, false);
		param_bind->unset(parameterIndex);
	}


	enum_field_types t = MYSQL_TYPE_LONGLONG;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	param->buffer_type	= t;
	delete [] static_cast<char *>(param->buffer);
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

	if (parameterIndex == 0 || parameterIndex > param_count) {
		throw InvalidArgumentException("MySQL_Prepared_Statement::setNull: invalid 'parameterIndex'");
	}
	--parameterIndex; /* DBC counts from 1 */

	if (param_bind->getBlobObject(parameterIndex)) {
		param_bind->setBlob(parameterIndex, NULL, false);
		param_bind->unset(parameterIndex);
	}

	enum_field_types t = MYSQL_TYPE_NULL;

	BufferSizePair p = allocate_buffer_for_type(t);

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	param->buffer_type	= t;
	delete [] static_cast<char *>(param->buffer);
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

	if (parameterIndex == 0 || parameterIndex > param_count) {
		CPP_ERR("Invalid parameterIndex");
		throw InvalidArgumentException("MySQL_Prepared_Statement::setString: invalid 'parameterIndex'");
	}
	if (value.length() > 256*1024) {
		return setBlob_intern(parameterIndex, new std::istringstream(value), true);
	}

	--parameterIndex; /* DBC counts from 1 */

	if (param_bind->getBlobObject(parameterIndex)) {
		param_bind->setBlob(parameterIndex, NULL, false);
		param_bind->unset(parameterIndex);
	}

	enum_field_types t = MYSQL_TYPE_STRING;

	param_bind->set(parameterIndex);
	MYSQL_BIND * param = &param_bind->get()[parameterIndex];

	delete [] static_cast<char *>(param->buffer);

	param->buffer_type	= t;
	param->buffer		= memcpy(new char[value.length() + 1], value.c_str(), value.length() + 1);
	param->buffer_length= static_cast<unsigned long>(value.length()) + 1;
	param->is_null_value= 0;

	delete param->length;
	param->length = new unsigned long(static_cast<unsigned long>(value.length()));
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getMetaData() -I- */
sql::ResultSetMetaData *
MySQL_Prepared_Statement::getMetaData()
{
	CPP_ENTER("MySQL_Prepared_Statement::getMetaData");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	return res_meta.get();
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getParameterMetaData() -I- */
sql::ParameterMetaData *
MySQL_Prepared_Statement::getParameterMetaData()
{
	CPP_ENTER("MySQL_Prepared_Statement::getParameterMetaData");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	return param_meta.get();
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

	my_bool	bool_tmp = 1;
	mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &bool_tmp);
	sql::ResultSet::enum_type tmp_type;
	if (resultset_type == sql::ResultSet::TYPE_SCROLL_INSENSITIVE) {
		mysql_stmt_store_result(stmt);
		tmp_type = sql::ResultSet::TYPE_SCROLL_INSENSITIVE;
	} else if (resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY) {
		tmp_type = sql::ResultSet::TYPE_FORWARD_ONLY;
	} else {
		throw SQLException("Invalid valude for result set type");
	}

	std::auto_ptr< MySQL_ResultBind > result_bind(new MySQL_ResultBind(stmt, logger));

	sql::ResultSet * tmp = new MySQL_Prepared_ResultSet(stmt, result_bind.get(), tmp_type, this, logger);
	result_bind.release();
	CPP_INFO_FMT("rset=%p", tmp);

	return tmp;
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


/* {{{ MySQL_Prepared_Statement::getMoreResults() -U- */
bool
MySQL_Prepared_Statement::getMoreResults()
{
	checkClosed();
	throw MethodNotImplementedException("MySQL_Prepared_Statement::getMoreResults");
	return false; // fool compilers
}
/* }}} */


/* {{{ MySQL_Statement::getResultSetType() -I- */
sql::ResultSet::enum_type
MySQL_Prepared_Statement::getResultSetType()
{
	CPP_ENTER("MySQL_Statement::getResultSetType");
	checkClosed();
	return resultset_type;
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


/* {{{ MySQL_Prepared_Statement::setResultSetType() -U- */
sql::PreparedStatement *
MySQL_Prepared_Statement::setResultSetType(sql::ResultSet::enum_type type)
{
	checkClosed();
#if WE_SUPPORT_USE_RESULT_WITH_PS
		/* The connector is not ready for unbuffered as we need to refetch */
	resultset_type = type;
#else
	throw MethodNotImplementedException("MySQL_Prepared_Statement::setResultSetType");	
#endif
	return this;
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

	isClosed = true;
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
