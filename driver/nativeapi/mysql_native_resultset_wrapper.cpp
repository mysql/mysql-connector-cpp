/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2010 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include "mysql_native_resultset_wrapper.h"
#include "mysql_client_api.h"


namespace sql
{
namespace mysql
{

class MySQL_DebugLogger;
class MySQL_ConnectionMetaData;

namespace NativeAPI
{

/* {{{ MySQL_NativeResultsetWrapper::MySQL_NativeResultsetWrapper */
MySQL_NativeResultsetWrapper::MySQL_NativeResultsetWrapper(::st_mysql_res * res, boost::shared_ptr< NativeAPI::IMySQLCAPI > & _capi
											/*, boost::shared_ptr< MySQL_DebugLogger > & l*/)
	: /*logger(l),*/ capi(_capi), rs(res)
{
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::~MySQL_NativeResultsetWrapper */
MySQL_NativeResultsetWrapper::~MySQL_NativeResultsetWrapper()
{
	capi->free_result(rs);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::data_seek */
void
MySQL_NativeResultsetWrapper::data_seek(uint64_t offset)
{
	capi->data_seek(rs, offset);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::fetch_field */
::st_mysql_field *
MySQL_NativeResultsetWrapper::fetch_field()
{
	return capi->fetch_field(rs);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::fetch_field_direct */
::st_mysql_field *
MySQL_NativeResultsetWrapper::fetch_field_direct(unsigned int field_nr)
{
	return capi->fetch_field_direct(rs, field_nr);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::fetch_lengths */
unsigned long *
MySQL_NativeResultsetWrapper::fetch_lengths()
{
	return capi->fetch_lengths(rs);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::fetch_row */
char**
MySQL_NativeResultsetWrapper::fetch_row()
{
	return capi->fetch_row(rs);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::num_fields */
unsigned int
MySQL_NativeResultsetWrapper::num_fields()
{
	return capi->num_fields(rs);
}
/* }}} */


/* {{{ MySQL_NativeResultsetWrapper::num_rows */
uint64_t
MySQL_NativeResultsetWrapper::num_rows()
{
	return capi->num_rows(rs);
}
/* }}} */

} /* namespace NativeAPI */
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
