/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include "mysql_resultset_data.h"
#include "mysql_client_api.h"

struct st_mysql;

namespace sql
{
namespace mysql
{

class MySQL_DebugLogger;
class MySQL_ConnectionMetaData;

namespace NativeAPI
{
	class IMySQLCAPI;
}


/* {{{ MySQL_ResultsetData::MySQL_ResultsetData */
MySQL_ResultsetData::MySQL_ResultsetData(MYSQL_RES * res,
											boost::shared_ptr< NativeAPI::IMySQLCAPI > & _capi,
											boost::shared_ptr< MySQL_DebugLogger > & l)
	: logger(l), capi(_capi), rs(res)
{
}
/* }}} */


/* {{{ MySQL_ResultsetData::~MySQL_ResultsetData */
MySQL_ResultsetData::~MySQL_ResultsetData()
{
	capi->free_result(rs);
}
/* }}} */


/* {{{ MySQL_ResultsetData::data_seek */
void
MySQL_ResultsetData::data_seek(my_ulonglong offset)
{
	capi->data_seek(rs, offset);
}
/* }}} */


/* {{{ MySQL_ResultsetData::fetch_field */
MYSQL_FIELD *
MySQL_ResultsetData::fetch_field()
{
	return capi->fetch_field(rs);
}
/* }}} */


/* {{{ MySQL_ResultsetData::fetch_field_direct */
MYSQL_FIELD *
MySQL_ResultsetData::fetch_field_direct(unsigned int field_nr)
{
	return capi->fetch_field_direct(rs, field_nr);
}
/* }}} */


/* {{{ MySQL_ResultsetData::fetch_lengths */
unsigned long *
MySQL_ResultsetData::fetch_lengths()
{
	return capi->fetch_lengths(rs);
}
/* }}} */


/* {{{ MySQL_ResultsetData::fetch_row */
MYSQL_ROW
MySQL_ResultsetData::fetch_row()
{
	return capi->fetch_row(rs);
}
/* }}} */


/* {{{ MySQL_ResultsetData::num_fields */
unsigned int
MySQL_ResultsetData::num_fields()
{
	return capi->num_fields(rs);
}
/* }}} */


/* {{{ MySQL_ResultsetData::num_rows */
my_ulonglong
MySQL_ResultsetData::num_rows()
{
	return capi->num_rows(rs);
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
