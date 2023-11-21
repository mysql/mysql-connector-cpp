/*
 * Copyright (c) 2008, 2022, Oracle and/or its affiliates.
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


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <iostream>
#include <sstream>
#include <memory>
#include "mysql_util.h"
#if (defined __GNUC__) && (__GNUC__ >= 8)
DIAGNOSTIC_PUSH
DISABLE_WARNING(-Wparentheses)
#endif
#include <variant>
#if (defined __GNUC__) && (__GNUC__ >= 8)
DIAGNOSTIC_POP
#endif
#include <cppconn/exception.h>
#include "mysql_connection.h"
#include "mysql_connection_data.h"
#include "mysql_statement.h"
#include "mysql_prepared_statement.h"
#include "mysql_ps_resultset.h"
#include "mysql_ps_resultset_metadata.h"
#include "mysql_parameter_metadata.h"
#include "mysql_warning.h"
#include "mysql_resultbind.h"

#include "nativeapi/native_statement_wrapper.h"


#include "mysql_debug.h"


namespace sql
{

namespace mysql
{

static const unsigned int MAX_SEND_LONGDATA_BUFFER= 1 << 18; //1<<18=256k (for istream)
static const unsigned int MAX_SEND_LONGDATA_CHUNK=  1 << 18; //1<<19=512k (for string)

// Visitor class to send long data contained in blob_bind
class LongDataSender
{
  unsigned	position;
  std::shared_ptr<NativeAPI::NativeStatementWrapper> proxy;
  std::shared_ptr<MySQL_DebugLogger> logger;

  LongDataSender()
  {}

public:
 LongDataSender(unsigned int i,
                std::shared_ptr<NativeAPI::NativeStatementWrapper> &_proxy,
                std::shared_ptr<MySQL_DebugLogger> _logger)
     : position(i), proxy(_proxy), logger(_logger) {}

 bool operator()(std::istream *my_blob) const {
   CPP_ENTER("LongDataSender::operator()(std::istream *)");
   if (my_blob == NULL) return false;

   // char buf[MAX_SEND_LONGDATA_BUFFER];
   std::unique_ptr<char[]> buf(new char[MAX_SEND_LONGDATA_BUFFER]);

   do {
     if (my_blob->eof()) {
       break;
     }
     my_blob->read(buf.get(), MAX_SEND_LONGDATA_BUFFER);

     if (my_blob->bad()) {
       throw SQLException("Error while reading from blob (bad)");
     } else if (my_blob->fail()) {
       if (!my_blob->eof()) {
         throw SQLException("Error while reading from blob (fail)");
       }
     }
     if (proxy->send_long_data(position, buf.get(),
                               static_cast<unsigned long>(my_blob->gcount()))) {
       CPP_ERR_FMT("Couldn't send long data : %d:(%s) %s", proxy->errNo(),
                   proxy->sqlstate().c_str(), proxy->error().c_str());
       switch (proxy->errNo()) {
         case CR_OUT_OF_MEMORY:
           throw std::bad_alloc();
         case CR_INVALID_BUFFER_USE:
           throw InvalidArgumentException(
               "MySQL_Prepared_Statement::setBlob: can't set blob value on "
               "that column");
         case CR_SERVER_GONE_ERROR:
         case CR_COMMANDS_OUT_OF_SYNC:
         default:
           sql::mysql::util::throwSQLException(*proxy.get());
       }
     }
   } while (1);

   return true;
  }

  bool operator()(sql::SQLString * str) const
  {
    CPP_ENTER("LongDataSender::operator()(sql::SQLString *)");
    if ( str == NULL )
      return false;

    unsigned int sent= 0, chunkSize;

    while (sent < str->length())
    {
      chunkSize= (sent + MAX_SEND_LONGDATA_CHUNK > str->length()
            ? static_cast<unsigned int>(str->length() - sent)
            : MAX_SEND_LONGDATA_CHUNK);

      if (proxy->send_long_data(position, str->c_str() + sent, chunkSize)) {
        CPP_ERR_FMT("Couldn't send long data : %d:(%s) %s", proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());
        switch (proxy->errNo()) {
        case CR_OUT_OF_MEMORY:
          throw std::bad_alloc();
        case CR_INVALID_BUFFER_USE:
          throw InvalidArgumentException("MySQL_Prepared_Statement::setBlob: can't set blob value on that column");
        case CR_SERVER_GONE_ERROR:
        case CR_COMMANDS_OUT_OF_SYNC:
        default:
          sql::mysql::util::throwSQLException(*proxy.get());
        }
      }

      sent+= chunkSize;
    }

    return true;
  }
};


class BlobBindDeleter
{
public:

  void operator()(sql::SQLString *& str) const
  {
    if (str != NULL) {
    delete str;
    str= NULL;
    }
  }

  void operator()(std::istream *& my_blob) const
  {
    if (my_blob!= NULL) {
      delete my_blob;
      my_blob= NULL;
    }
  }
};

class BlobIsNull
{
public:

  bool operator()(sql::SQLString *& str) const
  {
    return str == NULL;
  }

  bool operator()(std::istream *& my_blob) const
  {
    return my_blob == NULL;
  }
};


void  resetBlobBind(MYSQL_BIND & param)
{
  delete [] static_cast<char *>(param.buffer);

  param.buffer_type=		MYSQL_TYPE_LONG_BLOB;
  param.buffer=			NULL;
  param.buffer_length=	0;
  param.is_null_value=	0;

  delete param.length;
  param.length=			new unsigned long(0);
}


/*
  This class stores parameter and attribute values.

  Note: The number of parameter slots is fixed and determined when
  an instance is created. Ininitially all these slots are empty.
*/

class MySQL_ParamBind
{
public:
 typedef std::variant<std::istream *, sql::SQLString *> Blob_t;

private:

  size_t param_count;

  std::vector<MYSQL_BIND> bind;
  std::vector<bool> value_set;
  std::vector<bool> delete_blob_after_execute;

  typedef std::map<unsigned int, Blob_t > Blobs;
  using Base = MySQL_Names;

  Blobs blob_bind;

  // Storage for attributes.

  MySQL_AttributesBind attrs;

public:

  MySQL_ParamBind(unsigned int paramCount)
     : param_count(paramCount),
       bind(paramCount, MYSQL_BIND {}),
       value_set(paramCount),
       delete_blob_after_execute(paramCount)
  {
    if (paramCount) {
      for (unsigned int i = 0; i < paramCount; ++i) {
        bind[i].is_null_value = 1;
        delete_blob_after_execute[i] = false;
      }
    }
  }

  virtual ~MySQL_ParamBind()
  {
    clearParameters();

    // TODO: is it needed? Similar thing is done in `clearParameters()`.

    for (Blobs::iterator it= blob_bind.begin();
      it != blob_bind.end(); ++it) {
      if (delete_blob_after_execute[it->first]) {
        delete_blob_after_execute[it->first] = false;
        std::visit(::sql::mysql::BlobBindDeleter(), it->second);
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

    if (!delete_blob_after_execute[position])
      return;
    if (!blob_bind.count(position))
      return;

    std::visit(::sql::mysql::BlobBindDeleter(), blob_bind[position]);
    delete_blob_after_execute[position] = false;
    blob_bind.erase(position);
  }


  void setBlob(unsigned int position, Blob_t & blob, bool delete_after_execute)
  {
    set(position);

    resetBlobBind(bind[position]);

    Blobs::iterator it = blob_bind.find(position);
    if (it != blob_bind.end() && delete_blob_after_execute[position]) {
      std::visit(::sql::mysql::BlobBindDeleter(), it->second);
    }

    if (std::visit(::sql::mysql::BlobIsNull(), blob)) {
      if (it != blob_bind.end()) blob_bind.erase(it);

      delete_blob_after_execute[position] = false;
    } else {
      blob_bind[position] = blob;
      delete_blob_after_execute[position] = delete_after_execute;
    }
  }

  /*
    Set or overwrite existing value of an attribute with given name.
    
    If `is_external` is false then existing external value will not
    be changed.
  */

  int setQueryAttrString(
    const sql::SQLString &name,
    const sql::SQLString &value,
    bool is_external
  )
  {
    int pos = attrs.setQueryAttrString(name, value, is_external);

    /*
      Note: Here we only resize `bind` vector accordingly but do not copy
      attribute value from `attrs` -- this is done later in `getBindObject()`.
    */

    if (param_count + attrs.size() > bind.size())
      bind.resize(param_count + attrs.size());

    return pos;
  }


  bool isAllSet()
  {
    for (unsigned int i = 0; i < value_set.size(); ++i) {
      if (!value_set[i]) {
        return false;
      }
    }
    return true;
  }


  void clearParameters()
  {
    clearAttributes();

    /*
      Note: Here we need to clear only slots corresponding to statement
      parameters. Slots that correspond to attributes were handled above
      (and discarded).
    */

    for (unsigned int i = 0; i < param_count; ++i)
    {
      if (bind[i].length)
        delete bind[i].length;

      bind[i].length = NULL;

      if (bind[i].buffer)
        delete[] (char *)bind[i].buffer;

      bind[i].buffer = NULL;

      if (value_set[i]) {
        unset(i);
        blob_bind[i] = Blob_t();
      }
    }
  }

  void clearAttributes()
  {
    /*
      Note: The `MYSQL_BIND` buffers storing attribute values are managed
      by the `attrs` object. Even if we copy these attribute `MYSQL_BIND`
      structures to `bind` vector, we can discard the copies here as
      the originals are kept in `aatrs` object and will be properly freed etc.
    */

    bind.resize(param_count);
    attrs.clearAttributes();
  }

  /*
    Return array of `MYSQL_BIND` structures holding parameter values followed
    by attribute values. The size of this array is given by `size()` method.
  */

  MYSQL_BIND * getBindObject()
  {
    /*
      Note: The `bind` vector should be resized when new attributes are added.
    */

    assert(bind.size() == param_count + attrs.size());

    /*
      Copy `MYSQL_BIND` structures holding attribute values from the `attrs`
      object. Note however that the original structures remain in and
      are owned by the `attrs` member.
    */

    MYSQL_BIND *attr_binds = attrs.getBinds();

    for (size_t i=0; i < attrs.size(); ++i)
      bind.at(param_count + i) = attr_binds[i];

    return bind.data();
  }

  /*
    Number of entries in the array returned by `getBindObject()`. This
    includes both parameters and attributes.
  */

  size_t size()
  {
    return bind.size();
  }

  /*
    Return vector of names corresponding to bind structures returned
    by `getBindObject()`. This starts with empty names for parameters
    (which are anonymous) followed by attribute names.
  */
 
  std::vector<const char*> getNames()
  {
    std::vector<const char*> names{bind.size(), nullptr};
    const char **attr_names = attrs.getNames();

    for (size_t i=0; i < attrs.size(); ++i)
      names.at(param_count+i) = attr_names[i];

    return names;
  }

  std::variant< std::istream *, SQLString *> getBlobObject(unsigned int position)
  {
    Blobs::iterator it= blob_bind.find( position );

    if (it != blob_bind.end())
      return it->second;

    return Blob_t();
  }

};


telemetry::Telemetry<MySQL_Connection>&
MySQL_Prepared_Statement::conn_telemetry()
{
  assert(connection);
  assert(connection->intern);
  return connection->intern->telemetry;
}


/* {{{ MySQL_Prepared_Statement::MySQL_Prepared_Statement() -I- */
MySQL_Prepared_Statement::MySQL_Prepared_Statement(
    const sql::SQLString &sql,
    MySQL_Connection *conn, sql::ResultSet::enum_type rset_type,
    std::shared_ptr<MySQL_DebugLogger> &log)
    : connection(conn),
      isClosed(false),
      warningsHaveBeenLoaded(true),
      logger(log),
      resultset_type(rset_type),
      warningsCount(0)

{
  CPP_ENTER("MySQL_Prepared_Statement::MySQL_Prepared_Statement");
  CPP_INFO_FMT("this=%p", this);

  std::shared_ptr<NativeAPI::NativeStatementWrapper> stmt;

  //TODO change - probably no need to catch and throw here. Logging can be done inside proxy
  auto &connProxy = connection->proxy;
  try {
     stmt.reset(&connProxy->stmt_init());
  } catch (sql::SQLException& e) {
    CPP_ERR_FMT("No statement : %d:(%s) %s", connProxy->errNo(),
      connProxy->sqlstate().c_str(), connProxy->error().c_str());
    throw e;
  }

  telemetry.span_start(this, "SQL prepare");
  try
  {
    if (stmt->prepare(sql)) {
      CPP_ERR_FMT("Cannot prepare %d:(%s) %s", stmt->errNo(), stmt->sqlstate().c_str(), stmt->error().c_str());
      sql::SQLException e(stmt->error(), stmt->sqlstate(), stmt->errNo());
      stmt.reset();
      throw e;
    }

    proxy = stmt;
    result_bind.reset(new MySQL_ResultBind(proxy, logger));

    param_count = proxy->param_count();
    param_bind.reset(new MySQL_ParamBind(param_count));

    res_meta.reset(new MySQL_PreparedResultSetMetaData(proxy, logger));
    param_meta.reset(new MySQL_ParameterMetaData(proxy));
  }
  catch(sql::SQLException &e)
  {
    telemetry.set_error(this, e.what());
    throw;
  }
  telemetry.span_end(this);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::~MySQL_Prepared_Statement() -I- */
MySQL_Prepared_Statement::~MySQL_Prepared_Statement()
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
/* }}} */


/* {{{ MySQL_Prepared_Statement::sendLongDataBeforeParamBind() -I- */
bool
MySQL_Prepared_Statement::sendLongDataBeforeParamBind()
{
  CPP_ENTER("MySQL_Prepared_Statement::sendLongDataBeforeParamBind");

  MYSQL_BIND * bind= param_bind->getBindObject();

  for (unsigned int i = 0; i < param_count; ++i) {
    if (bind[i].buffer_type == MYSQL_TYPE_LONG_BLOB) {
      ::sql::mysql::LongDataSender lv(i, proxy, logger);
      MySQL_ParamBind::Blob_t dummy(param_bind->getBlobObject(i));
      std::visit(lv, dummy);
    }

  }
  return true;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::do_query() -I- */
void
MySQL_Prepared_Statement::do_query()
{
  telemetry.span_start(this, "SQL execute");

  assert(param_bind);

  try
  {
    CPP_ENTER("MySQL_Prepared_Statement::do_query");
    if (param_count && !param_bind->isAllSet()) {
      CPP_ERR("Value not set for all parameters");
      throw sql::SQLException("Value not set for all parameters");
    }

    auto *bind_obj = param_bind->getBindObject();
    size_t bind_cnt = param_bind->size();
    std::vector<const char*> names = param_bind->getNames();
    bool bind_res;

#if MYSQL_VERSION_ID >= 80300
    // Assume the named params are most likely implemented
    try
    {
      bind_res = proxy->bind_named_param(
        bind_obj, bind_cnt, names.data()
      );
    }
    catch(const ::sql::MethodNotImplementedException& e)
    {
      // Dynamically linked libmysqclient is too old and it does not have
      // mysql_stmt_bind_named_param() function.
      // Fall back to mysql_stmt_bind_param().
      bind_res = proxy->bind_param(bind_obj);
    }
#else
    try
    {
      bind_res = proxy->bind_param(bind_obj);
    }
    catch(const ::sql::MethodNotImplementedException& e)
    {
      // If a new libmysqlclient library is used it might have
      // mysql_stmt_bind_param() function removed. In that case
      // mysql_stmt_bind_named_param() can be used.
      // NOTE: In some versions both functions might exist.
      bind_res = proxy->bind_named_param(
        bind_obj, bind_cnt, names.data()
      );
    }
#endif

    if (bind_res) {
      CPP_ERR_FMT("Couldn't bind : %d:(%s) %s", proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());
      sql::mysql::util::throwSQLException(*proxy.get());
    }

    if (!sendLongDataBeforeParamBind() || proxy->execute()) {
      CPP_ERR_FMT("Couldn't execute : %d:(%s) %s", proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());
      sql::mysql::util::throwSQLException(*proxy.get());
    }
  }
  catch(sql::SQLException &e)
  {
    telemetry.set_error(this, e.what());
    throw;
  }

  warningsCount= proxy->warning_count();

  warningsHaveBeenLoaded= false;
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

  if (proxy->num_rows() == 0 && !proxy->more_results())
  {
    // No result set, just end the span
    telemetry.span_end(this);
  }
  return (proxy->field_count() > 0);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::execute() -U- */
bool
MySQL_Prepared_Statement::execute(const sql::SQLString&)
{
  CPP_ENTER("MySQL_Prepared_Statement::execute(const sql::SQLString& sql)");
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

  return _getResultSet();
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::executeQuery() -U- */
sql::ResultSet *
MySQL_Prepared_Statement::executeQuery(const sql::SQLString&)
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
  // No result set, just end the span
  telemetry.span_end(this);
  return static_cast<int>(proxy->affected_rows());
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::executeUpdate() -U- */
int
MySQL_Prepared_Statement::executeUpdate(const sql::SQLString&)
{
  throw sql::MethodNotImplementedException("MySQL_Prepared_Statement::executeUpdate"); /* TODO - what to do? Comes from Statement */
  return 0; // fool compilers
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setBigInt() -I- */
void
MySQL_Prepared_Statement::setBigInt(unsigned int parameterIndex, const sql::SQLString& value)
{
  CPP_ENTER("MySQL_Prepared_Statement::setBigInt");
  setString(parameterIndex, value);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setBlob_intern() -I- */
/*
void
setBlob_intern(unsigned int parameterIndex
                     , / *boost::variant< std::istream *, sql::SQLString *>* /MySQL_ParamBind::Blob_t & blob
                                         , bool deleteBlobAfterExecute)
{
  CPP_ENTER("MySQL_Prepared_Statement::setBlob_intern");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();

  --parameterIndex; / * DBC counts from 1 * /

  param_bind->set(parameterIndex);
  MYSQL_BIND * param = &param_bind->getBindObject()[parameterIndex];

  delete [] static_cast<char *>(param->buffer);

  param->buffer_type	= MYSQL_TYPE_LONG_BLOB;
  param->buffer		= NULL;
  param->buffer_length= 0;
  param->is_null_value= 0;

  delete param->length;
  param->length = new unsigned long(0);

  param_bind->setBlob(parameterIndex, blob, deleteBlobAfterExecute);
}*/

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

  MySQL_ParamBind::Blob_t dummy(blob);
  param_bind->setBlob(--parameterIndex, dummy, false);
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
MySQL_Prepared_Statement::setDateTime(unsigned int parameterIndex, const sql::SQLString& value)
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
      return BufferSizePair(NULLCSTR, 0);

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
      return BufferSizePair(NULLCSTR, 0);
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

  {
    MySQL_ParamBind::Blob_t dummy;
    param_bind->setBlob(parameterIndex, dummy, false);
    param_bind->unset(parameterIndex);
  }

  enum_field_types t = MYSQL_TYPE_DOUBLE;

  BufferSizePair p = allocate_buffer_for_type(t);

  param_bind->set(parameterIndex);
  MYSQL_BIND * param = &param_bind->getBindObject()[parameterIndex];

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

  {
    MySQL_ParamBind::Blob_t dummy;
    param_bind->setBlob(parameterIndex, dummy, false);
    param_bind->unset(parameterIndex);
  }

  enum_field_types t = MYSQL_TYPE_LONG;

  BufferSizePair p = allocate_buffer_for_type(t);

  param_bind->set(parameterIndex);
  MYSQL_BIND * param = &param_bind->getBindObject()[parameterIndex];

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

  {
    MySQL_ParamBind::Blob_t dummy;
    param_bind->setBlob(parameterIndex, dummy, false);
    param_bind->unset(parameterIndex);
  }

  enum_field_types t = MYSQL_TYPE_LONG;

  BufferSizePair p = allocate_buffer_for_type(t);

  param_bind->set(parameterIndex);
  MYSQL_BIND * param = &param_bind->getBindObject()[parameterIndex];

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

  {
    MySQL_ParamBind::Blob_t dummy;
    param_bind->setBlob(parameterIndex, dummy, false);
    param_bind->unset(parameterIndex);
  }

  enum_field_types t = MYSQL_TYPE_LONGLONG;

  BufferSizePair p = allocate_buffer_for_type(t);

  param_bind->set(parameterIndex);
  MYSQL_BIND * param = &param_bind->getBindObject()[parameterIndex];

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

  {
    MySQL_ParamBind::Blob_t dummy;
    param_bind->setBlob(parameterIndex, dummy, false);
    param_bind->unset(parameterIndex);
  }


  enum_field_types t = MYSQL_TYPE_LONGLONG;

  BufferSizePair p = allocate_buffer_for_type(t);

  param_bind->set(parameterIndex);
  MYSQL_BIND * param = &param_bind->getBindObject()[parameterIndex];

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

  {
    MySQL_ParamBind::Blob_t dummy;
    param_bind->setBlob(parameterIndex, dummy, false);
    param_bind->unset(parameterIndex);
  }

  enum_field_types t = MYSQL_TYPE_NULL;

  param_bind->set(parameterIndex);
  MYSQL_BIND * param = &param_bind->getBindObject()[parameterIndex];

  param->buffer_type	= t;
  delete [] static_cast<char *>(param->buffer);
  param->buffer = NULL;
  delete param->length;
  param->length = NULL;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setString() -I- */
void
MySQL_Prepared_Statement::setString(unsigned int parameterIndex, const sql::SQLString& value)
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
    sql::SQLString* pvalue = new sql::SQLString(value);
    MySQL_ParamBind::Blob_t dummy(pvalue);
    return param_bind->setBlob(--parameterIndex, dummy, true);
  }

  --parameterIndex; /* DBC counts from 1 */

  {
    MySQL_ParamBind::Blob_t dummy;
    param_bind->setBlob(parameterIndex, dummy, false);
    param_bind->unset(parameterIndex);
  }

  enum_field_types t = MYSQL_TYPE_STRING;

  param_bind->set(parameterIndex);
  MYSQL_BIND * param = &param_bind->getBindObject()[parameterIndex];

  delete [] static_cast<char *>(param->buffer);

  param->buffer_type	= t;
  param->buffer		= memcpy(new char[value.length() + 1], value.c_str(), value.length() + 1);
  param->buffer_length= static_cast<unsigned long>(value.length()) + 1;
  param->is_null_value= 0;

  delete param->length;
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
size_t
MySQL_Prepared_Statement::getFetchSize()
{
  checkClosed();
  throw MethodNotImplementedException("MySQL_Prepared_Statement::getFetchSize");
  return 0; // fool compilers
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


sql::ResultSet *
MySQL_Prepared_Statement::_getResultSet()
{
  my_bool	bool_tmp = 1;
  try
  {
    proxy->attr_set(STMT_ATTR_UPDATE_MAX_LENGTH, &bool_tmp);
    sql::ResultSet::enum_type tmp_type;
    if (resultset_type == sql::ResultSet::TYPE_SCROLL_INSENSITIVE) {
      if (proxy->store_result()) {
        sql::mysql::util::throwSQLException(*proxy.get());
      }
      tmp_type = sql::ResultSet::TYPE_SCROLL_INSENSITIVE;
    } else if (resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY) {
      tmp_type = sql::ResultSet::TYPE_FORWARD_ONLY;
    } else {
      throw SQLException("Invalid value for result set type");
    }

    sql::ResultSet * tmp = new MySQL_Prepared_ResultSet(proxy, result_bind, tmp_type, this, logger);

    CPP_INFO_FMT("rset=%p", tmp);
    return tmp;
  }
  catch(sql::SQLException &e)
  {
    telemetry.set_error(this, e.what());
    throw;
  }

  // Normally it should never come to this line.
  return nullptr;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getResultSet() -I- */
sql::ResultSet *
MySQL_Prepared_Statement::getResultSet()
{
  CPP_ENTER("MySQL_Prepared_Statement::getResultSet");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();
  return _getResultSet();
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setFetchSize() -U- */
void
MySQL_Prepared_Statement::setFetchSize(size_t /* size */)
{
  checkClosed();
  throw MethodNotImplementedException("MySQL_Prepared_Statement::setFetchSize");
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setQueryTimeout() -U- */
void
MySQL_Prepared_Statement::setQueryTimeout(unsigned int timeout)
{
  CPP_ENTER("MySQL_Statement::setQueryTimeout");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();
  connection->setSessionVariable("max_execution_time", timeout*1000);
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::clearWarnings() -I- */
void
MySQL_Prepared_Statement::clearWarnings()
{
  CPP_ENTER("MySQL_Prepared_Statement::clearWarnings");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();
  if (warnings)
  {
    warnings.reset();
  }
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
  CPP_ENTER("MySQL_Prepared_Statement::getMoreResults");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();

  if (proxy->more_results())
  try
  {
    int next_result = proxy->stmt_next_result();

    if (next_result == 0) {
      bool ret = proxy->field_count() > 0;
      if (!ret)
      {
        telemetry.span_end(this);
      }
      return  ret;
    } else if (next_result == -1) {
      throw sql::SQLException("Impossible! more_results() said true, next_result says no more results");
    } else {
      CPP_ERR_FMT("Error during getMoreResults : %d:(%s) %s", proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());
      sql::mysql::util::throwSQLException(*proxy.get());
    }
  }
  catch(sql::SQLException &e)
  {
    telemetry.set_error(this, e.what());
    throw;
  }

  telemetry.span_end(this);
  return false;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::getQueryTimeout() -U- */
unsigned int
MySQL_Prepared_Statement::getQueryTimeout()
{
  checkClosed();
  sql::SQLString value= connection->getSessionVariable("max_execution_time");
  if (value.length() > 0) {
    unsigned int timeout;
    std::istringstream buffer(value);
    buffer >> timeout;
    timeout/=1000;
    if (buffer.rdstate() & std::istringstream::failbit) {
      return 0;
    } else {
      return timeout;
    }
  } else {
    return 0;
  }
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

  if (!warningsHaveBeenLoaded)
  {
    if (warningsCount)
    warnings.reset( loadMysqlWarnings(connection, warningsCount) );
    warningsHaveBeenLoaded= true;
  }

  return warnings.get();
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setCursorName() -U- */
void
MySQL_Prepared_Statement::setCursorName(const sql::SQLString &)
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
sql::PreparedStatement *
MySQL_Prepared_Statement::setResultSetType(sql::ResultSet::enum_type /* type */)
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


/* {{{ MySQL_Prepared_Statement::setQueryAttrBigInt() -U- */
int
MySQL_Prepared_Statement::setQueryAttrBigInt(const sql::SQLString &name, const sql::SQLString& value)
{
  CPP_ENTER("MySQL_Prepared_Statement::setQueryAttrBigInt");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%s", name.c_str(), value.c_str());
  return 0;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setQueryAttrBoolean() -U- */
int
MySQL_Prepared_Statement::setQueryAttrBoolean(const sql::SQLString &name, bool value)
{
  CPP_ENTER("MySQL_Prepared_Statement::setQueryAttrBoolean");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%s", name.c_str(), value ? "true" : "false");
  return 0;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setQueryAttrDateTime() -U- */
int
MySQL_Prepared_Statement::setQueryAttrDateTime(const sql::SQLString &name, const sql::SQLString& value)
{
  CPP_ENTER("MySQL_Prepared_Statement::setQueryAttrDateTime");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%s", name.c_str(), value.c_str());
  return 0;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setQueryAttrDouble() -U- */
int
MySQL_Prepared_Statement::setQueryAttrDouble(const sql::SQLString &name, double value)
{
  CPP_ENTER("MySQL_Prepared_Statement::setQueryAttrDouble");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%f", name.c_str(), value);
  return 0;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setQueryAttrInt() -U- */
int
MySQL_Prepared_Statement::setQueryAttrInt(const sql::SQLString &name, int32_t value)
{
  CPP_ENTER("MySQL_Prepared_Statement::setQueryAttrInt");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%f", name.c_str(), value);
  return 0;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setQueryAttrUInt() -U- */
int
MySQL_Prepared_Statement::setQueryAttrUInt(const sql::SQLString &name, uint32_t value)
{
  CPP_ENTER("MySQL_Prepared_Statement::setQueryAttrUInt");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%f", name.c_str(), value);
  return 0;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setQueryAttrInt64() -U- */
int
MySQL_Prepared_Statement::setQueryAttrInt64(const sql::SQLString &name, int64_t value)
{
  CPP_ENTER("MySQL_Prepared_Statement::setQueryAttrInt64");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%f", name.c_str(), value);
  return 0;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setQueryAttrUInt64() -U- */
int
MySQL_Prepared_Statement::setQueryAttrUInt64(const sql::SQLString &name, uint64_t value)
{
  CPP_ENTER("MySQL_Prepared_Statement::setQueryAttrUInt64");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%f", name.c_str(), value);
  return 0;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::setQueryAttrNull() -U- */
int
MySQL_Prepared_Statement::setQueryAttrNull(const sql::SQLString &name)
{
  CPP_ENTER("MySQL_Prepared_Statement::setQueryAttrNull");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s", name.c_str());
  return 0;
}
/* }}} */


/*
  A helper function to set or replace existing named parameter value.

  We need it instead of using the `setQueryAttrString()` method because
  the latter is meant for setting "external" attributes by user. Here we need
  to be able say whether we set internal or external value.

  Note that setting external attributes for prepared statements is not yet
  suported.
*/

int setStmtAttrString(MySQL_Prepared_Statement &stmt,
  const sql::SQLString &name, const sql::SQLString& value,
  bool is_external)
{
  return stmt.param_bind->setQueryAttrString(name, value, is_external);
}

/* {{{ MySQL_Prepared_Statement::setQueryAttrString() -U- */
int
MySQL_Prepared_Statement::setQueryAttrString(const sql::SQLString &name, const sql::SQLString& value)
{
  CPP_ENTER("MySQL_Prepared_Statement::setQueryAttrString");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%f", name.c_str(), value.c_str());
  return 0;
}
/* }}} */


/* {{{ MySQL_Prepared_Statement::clearAttributes() -U- */
void
MySQL_Prepared_Statement::clearAttributes()
{
  param_bind->clearAttributes();
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
  proxy.reset();
  clearParameters();
  clearWarnings();

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
