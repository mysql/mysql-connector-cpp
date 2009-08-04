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


MySQL_ResultsetData::MySQL_ResultsetData( MYSQL_RES * res
                                , boost::shared_ptr<NativeAPI::IMySQLCAPI> & _capi
                                , boost::shared_ptr< MySQL_DebugLogger > & l )
                                : logger( l     )
                                , capi  ( _capi )
                                , rs    ( res   )
{
}


MySQL_ResultsetData::~MySQL_ResultsetData()
{
    capi->mysql_free_result( rs );
}


void MySQL_ResultsetData::data_seek( my_ulonglong offset )
{
    capi->mysql_data_seek( rs, offset );
}


MYSQL_FIELD * MySQL_ResultsetData::fetch_field()
{
    return capi->mysql_fetch_field( rs );
}


MYSQL_FIELD * MySQL_ResultsetData::fetch_field_direct( unsigned int field_nr )
{
    return capi->mysql_fetch_field_direct( rs, field_nr );
}


unsigned long * MySQL_ResultsetData::fetch_lengths()
{
    return capi->mysql_fetch_lengths( rs );
}


MYSQL_ROW MySQL_ResultsetData::fetch_row()
{
    return capi->mysql_fetch_row( rs );
}


unsigned int MySQL_ResultsetData::num_fields()
{
    return capi->mysql_num_fields( rs );
}


my_ulonglong MySQL_ResultsetData::num_rows()
{
    return capi->mysql_num_rows( rs );
}


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
