/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/


#include "mysql_client_loader.h"

#include <cppconn/exception.h>

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

#ifdef _WIN32
    const char * MySQL_Client_Loader::baseName= "libmysql.dll";
#else
    const char * MySQL_Client_Loader::baseName= "libmysqlclient.so";
#endif

    MySQL_Client_Loader::MySQL_Client_Loader()
        : LibraryLoader( baseName )
    {
        init();
    }


    MySQL_Client_Loader::MySQL_Client_Loader( const SQLString & path2libFile )
        : LibraryLoader( path2libFile.length() > 0 ? path2libFile : baseName )
    {
        init();
    }


    MySQL_Client_Loader::MySQL_Client_Loader( const SQLString & dir2look
                                    , const SQLString & libFileName )
        : LibraryLoader( dir2look, libFileName.length() > 0 ? libFileName : baseName )
    {
        init();
    }


    MySQL_Client_Loader::~MySQL_Client_Loader()
    {
        ptr2mysql_library_end endProc= reinterpret_cast<ptr2mysql_library_end>(GetProcAddr("mysql_server_end"));
        
        if ( endProc != NULL )
            (*endProc)();
    }


    void MySQL_Client_Loader::init()
    {
        ptr2mysql_library_init initProc= reinterpret_cast<ptr2mysql_library_init>(GetProcAddr("mysql_server_init"));
        
        if ( initProc != NULL )
            (*initProc)(0, NULL, NULL);
        else
        {
            throw ::sql::InvalidArgumentException( "Loaded library doesn't contain mysql_library_init" );
        }
    }
    /************************************************************************/
    /* MySQL C-API calls wrappers                                           */
    /************************************************************************/

    my_ulonglong MySQL_Client_Loader::mysql_affected_rows( MYSQL * mysql )
    {
        ptr2mysql_affected_rows ptr2_affected_rows= reinterpret_cast<ptr2mysql_affected_rows>(GetProcAddr("mysql_affected_rows"));

	    return (*ptr2_affected_rows)( mysql );
    }

    my_bool MySQL_Client_Loader::mysql_autocommit( MYSQL * mysql, my_bool mode )
    {
        ptr2mysql_autocommit ptr2_autocommit= reinterpret_cast<ptr2mysql_autocommit>(GetProcAddr("mysql_autocommit"));

		return (*ptr2_autocommit)( mysql, mode );
    }

    void MySQL_Client_Loader::mysql_close( MYSQL * mysql )
    {
        ptr2mysql_close ptr2_close= reinterpret_cast<ptr2mysql_close>(GetProcAddr("mysql_close"));

		return (*ptr2_close)( mysql );
    }

    my_bool MySQL_Client_Loader::mysql_commit( MYSQL * mysql )
    {
        ptr2mysql_commit ptr2_commit= reinterpret_cast<ptr2mysql_commit>(GetProcAddr("mysql_commit"));

		return (*ptr2_commit)( mysql );
    }

    void MySQL_Client_Loader::mysql_data_seek( MYSQL_RES *  result
        , my_ulonglong offset )
    {
        ptr2mysql_data_seek ptr2_data_seek= reinterpret_cast<ptr2mysql_data_seek>(GetProcAddr("mysql_data_seek"));

		return (*ptr2_data_seek)( result, offset );
    }

    void MySQL_Client_Loader::mysql_debug( const char * debug )
    {
        ptr2mysql_debug ptr2_debug= reinterpret_cast<ptr2mysql_debug>(GetProcAddr("mysql_debug"));

		return (*ptr2_debug)( debug );
    }

    unsigned int MySQL_Client_Loader::mysql_errno( MYSQL * mysql )
    {
        ptr2mysql_errno ptr2_errno= reinterpret_cast<ptr2mysql_errno>(GetProcAddr("mysql_errno"));

		return (*ptr2_errno)( mysql );
    }

    const char * MySQL_Client_Loader::mysql_error( MYSQL * mysql )
    {
        ptr2mysql_error ptr2_error= reinterpret_cast<ptr2mysql_error>(GetProcAddr("mysql_error"));

		return (*ptr2_error)( mysql );
    }

    MYSQL_FIELD * MySQL_Client_Loader::mysql_fetch_field( MYSQL_RES * result )
    {
        ptr2mysql_fetch_field ptr2_fetch_field= reinterpret_cast<ptr2mysql_fetch_field>(GetProcAddr("mysql_fetch_field"));

		return (*ptr2_fetch_field)( result );
    }

    MYSQL_FIELD * MySQL_Client_Loader::mysql_fetch_field_direct( MYSQL_RES * result
        , unsigned int fieldnr )
    {
        ptr2mysql_fetch_field_direct ptr2fetchFieldDirect=
            reinterpret_cast<ptr2mysql_fetch_field_direct>(GetProcAddr("mysql_fetch_field_direct"));

        return (*ptr2fetchFieldDirect)(result, fieldnr);
    }

    unsigned long * MySQL_Client_Loader::mysql_fetch_lengths( MYSQL_RES * result )
    {
        ptr2mysql_fetch_lengths ptr2_fetch_lengths= reinterpret_cast<ptr2mysql_fetch_lengths>(GetProcAddr("mysql_fetch_lengths"));

		return (*ptr2_fetch_lengths)( result );
    }

    MYSQL_ROW MySQL_Client_Loader::mysql_fetch_row( MYSQL_RES * result )
    {
        ptr2mysql_fetch_row ptr2_fetch_row= reinterpret_cast<ptr2mysql_fetch_row>(GetProcAddr("mysql_fetch_row"));

		return (*ptr2_fetch_row)( result );
    }

    unsigned int MySQL_Client_Loader::mysql_field_count( MYSQL * mysql )
    {
        ptr2mysql_field_count ptr2_field_count= reinterpret_cast<ptr2mysql_field_count>(GetProcAddr("mysql_field_count"));

		return (*ptr2_field_count)( mysql );
    }

    void MySQL_Client_Loader::mysql_free_result( MYSQL_RES * result )
    {
        ptr2mysql_free_result ptr2_free_result= reinterpret_cast<ptr2mysql_free_result>(GetProcAddr("mysql_free_result"));

		return (*ptr2_free_result)( result );
    }

    unsigned long MySQL_Client_Loader::mysql_get_client_version()
    {
        ptr2mysql_get_client_version ptr2_get_client_version=
            (ptr2mysql_get_client_version)( GetProcAddr( "mysql_get_client_version" ) );

        return (*ptr2_get_client_version)();
    }

    const char * MySQL_Client_Loader::mysql_get_server_info( MYSQL * mysql )
    {
        ptr2mysql_get_server_info ptr2_get_server_info= reinterpret_cast<ptr2mysql_get_server_info>(GetProcAddr("mysql_get_server_info"));

		return (*ptr2_get_server_info)( mysql );
    }

    unsigned long MySQL_Client_Loader::mysql_get_server_version( MYSQL * mysql )
    {
        ptr2mysql_get_server_version ptr2_get_server_version= reinterpret_cast<ptr2mysql_get_server_version>(GetProcAddr("mysql_get_server_version"));

		return (*ptr2_get_server_version)( mysql );
    }

    MYSQL * MySQL_Client_Loader::mysql_init( MYSQL * mysql )
    {
        ptr2mysql_init ptr2init= reinterpret_cast<ptr2mysql_init>( GetProcAddr( "mysql_init" ) );

        return (*ptr2init)( mysql );
    }

    int MySQL_Client_Loader::mysql_library_init(int argc,char **argv,char **groups)
    {
        ptr2mysql_library_init ptr2_library_init= reinterpret_cast<ptr2mysql_library_init>(GetProcAddr("mysql_library_init"));

		return (*ptr2_library_init)( argc, argv, groups );
    }

    void MySQL_Client_Loader::mysql_library_end()
    {
        ptr2mysql_library_end ptr2_library_end= reinterpret_cast<ptr2mysql_library_end>(GetProcAddr("mysql_library_end"));

		return (*ptr2_library_end)();
    }

    my_bool MySQL_Client_Loader::mysql_more_results( MYSQL * mysql )
    {
        ptr2mysql_more_results ptr2_more_results= reinterpret_cast<ptr2mysql_more_results>(GetProcAddr("mysql_more_results"));

		return (*ptr2_more_results)( mysql );
    }

    int MySQL_Client_Loader::mysql_next_result( MYSQL * mysql )
    {
        ptr2mysql_next_result ptr2_next_result= reinterpret_cast<ptr2mysql_next_result>(GetProcAddr("mysql_next_result"));

		return (*ptr2_next_result)( mysql );
    }

    unsigned int MySQL_Client_Loader::mysql_num_fields( MYSQL_RES * result )
    {
        ptr2mysql_num_fields ptr2_num_fields= reinterpret_cast<ptr2mysql_num_fields>(GetProcAddr("mysql_num_fields"));

		return (*ptr2_num_fields)( result );
    }

    my_ulonglong MySQL_Client_Loader::mysql_num_rows( MYSQL_RES * result )
    {
        ptr2mysql_num_rows ptr2_num_rows= reinterpret_cast<ptr2mysql_num_rows>(GetProcAddr("mysql_num_rows"));

		return (*ptr2_num_rows)( result );
    }

    int MySQL_Client_Loader::mysql_options( MYSQL * mysql 
        , enum mysql_option option
        , const void *arg )
    {
        ptr2mysql_options ptr2_options= reinterpret_cast<ptr2mysql_options>(GetProcAddr("mysql_options"));

		return (*ptr2_options)( mysql, option, arg );
    }


    int MySQL_Client_Loader::mysql_query( MYSQL * mysql, const char *stmt_str )
    {
        ptr2mysql_query ptr2_query= reinterpret_cast<ptr2mysql_query>(GetProcAddr("mysql_query"));

		return (*ptr2_query)( mysql, stmt_str );
    }


    MYSQL * MySQL_Client_Loader::mysql_real_connect( MYSQL * mysql, const char * host
                                            , const char * user, const char * passwd
                                            , const char * db, unsigned int port
                                            , const char * unix_socket
                                            , unsigned long client_flag )
    {
        ptr2mysql_real_connect ptr2_real_connect=
            reinterpret_cast<ptr2mysql_real_connect>(GetProcAddr("mysql_real_connect"));

        return (*ptr2_real_connect)( mysql, host, user, passwd, db, port, unix_socket, client_flag );
    }


    int MySQL_Client_Loader::mysql_real_query(MYSQL *mysql,const char *stmt_str
        , unsigned long len)
    {
        ptr2mysql_real_query ptr2_real_query= reinterpret_cast<ptr2mysql_real_query>(GetProcAddr("mysql_real_query"));

		return (*ptr2_real_query)( mysql, stmt_str, len );
    }


    my_bool MySQL_Client_Loader::mysql_rollback( MYSQL * mysql )
    {
        ptr2mysql_rollback ptr2_rollback= reinterpret_cast<ptr2mysql_rollback>(GetProcAddr("mysql_rollback"));

		return (*ptr2_rollback)( mysql );
    }


    const char * MySQL_Client_Loader::mysql_sqlstate( MYSQL * mysql )
    {
        ptr2mysql_sqlstate ptr2_sqlstate= reinterpret_cast<ptr2mysql_sqlstate>(GetProcAddr("mysql_sqlstate"));

		return (*ptr2_sqlstate)( mysql );
    }


    my_bool MySQL_Client_Loader::mysql_ssl_set( MYSQL    * mysql
        , const char * key
        , const char * cert
        , const char * ca
        , const char * capath
        , const char * cipher )
    {
        ptr2mysql_ssl_set ptr2_ssl_set= reinterpret_cast<ptr2mysql_ssl_set>(GetProcAddr("mysql_ssl_set"));

		return (*ptr2_ssl_set)( mysql, key, cert, ca, capath, cipher );
    }


    MYSQL_RES * MySQL_Client_Loader::mysql_store_result( MYSQL * mysql )
    {
        ptr2mysql_store_result ptr2_store_result= reinterpret_cast<ptr2mysql_store_result>(GetProcAddr("mysql_store_result"));

		return (*ptr2_store_result)( mysql );
    }

    MYSQL_RES * MySQL_Client_Loader::mysql_use_result( MYSQL * mysql )
    {
        ptr2mysql_use_result ptr2_use_result= reinterpret_cast<ptr2mysql_use_result>(GetProcAddr("mysql_use_result"));

		return (*ptr2_use_result)( mysql );
    }


    /* Prepared Statement mysql_stmt_* functions */
    my_ulonglong MySQL_Client_Loader::mysql_stmt_affected_rows(MYSQL_STMT *stmt)
    {
        ptr2mysql_stmt_affected_rows ptr2_stmt_affected_rows= reinterpret_cast<ptr2mysql_stmt_affected_rows>(GetProcAddr("mysql_stmt_affected_rows"));

		return (*ptr2_stmt_affected_rows)( stmt );
    }

    my_bool MySQL_Client_Loader::mysql_stmt_attr_set( MYSQL_STMT *      stmt
        , enum enum_stmt_attr_type  option
        , const void *      arg )
    {
        ptr2mysql_stmt_attr_set ptr2_stmt_attr_set= reinterpret_cast<ptr2mysql_stmt_attr_set>(GetProcAddr("mysql_stmt_attr_set"));

		return (*ptr2_stmt_attr_set)( stmt, option, arg );
    }


    my_bool MySQL_Client_Loader::mysql_stmt_bind_param( MYSQL_STMT * stmt
        , MYSQL_BIND * bind )
    {
        ptr2mysql_stmt_bind_param ptr2_stmt_bind_param= reinterpret_cast<ptr2mysql_stmt_bind_param>(GetProcAddr("mysql_stmt_bind_param"));

		return (*ptr2_stmt_bind_param)( stmt, bind );
    }


    my_bool MySQL_Client_Loader::mysql_stmt_bind_result( MYSQL_STMT * stmt
        , MYSQL_BIND * bind )
    {
        ptr2mysql_stmt_bind_result ptr2_stmt_bind_result= reinterpret_cast<ptr2mysql_stmt_bind_result>(GetProcAddr("mysql_stmt_bind_result"));

		return (*ptr2_stmt_bind_result)( stmt, bind );
    }


    my_bool MySQL_Client_Loader::mysql_stmt_close( MYSQL_STMT * stmt )
    {
        ptr2mysql_stmt_close ptr2_stmt_close= reinterpret_cast<ptr2mysql_stmt_close>(GetProcAddr("mysql_stmt_close"));

		return (*ptr2_stmt_close)( stmt );
    }


    void MySQL_Client_Loader::mysql_stmt_data_seek( MYSQL_STMT * stmt
        , my_ulonglong row_nr )
    {
        ptr2mysql_stmt_data_seek ptr2_stmt_data_seek= reinterpret_cast<ptr2mysql_stmt_data_seek>(GetProcAddr("mysql_stmt_data_seek"));

		return (*ptr2_stmt_data_seek)( stmt, row_nr );
    }


    unsigned int MySQL_Client_Loader::mysql_stmt_errno( MYSQL_STMT * stmt )
    {
        ptr2mysql_stmt_errno ptr2_stmt_errno= reinterpret_cast<ptr2mysql_stmt_errno>(GetProcAddr("mysql_stmt_errno"));

		return (*ptr2_stmt_errno)( stmt );
    }


    const char * MySQL_Client_Loader::mysql_stmt_error( MYSQL_STMT * stmt )
    {
        ptr2mysql_stmt_error ptr2_stmt_error= reinterpret_cast<ptr2mysql_stmt_error>(GetProcAddr("mysql_stmt_error"));

		return (*ptr2_stmt_error)( stmt );
    }


    int MySQL_Client_Loader::mysql_stmt_execute( MYSQL_STMT * stmt )
    {
        ptr2mysql_stmt_execute ptr2_stmt_execute= reinterpret_cast<ptr2mysql_stmt_execute>(GetProcAddr("mysql_stmt_execute"));

		return (*ptr2_stmt_execute)( stmt );
    }


    int MySQL_Client_Loader::mysql_stmt_fetch( MYSQL_STMT * stmt )
    {
        ptr2mysql_stmt_fetch ptr2_stmt_fetch= reinterpret_cast<ptr2mysql_stmt_fetch>(GetProcAddr("mysql_stmt_fetch"));

		return (*ptr2_stmt_fetch)( stmt );
    }


    unsigned int MySQL_Client_Loader::mysql_stmt_field_count( MYSQL_STMT * stmt )
    {
        ptr2mysql_stmt_field_count ptr2_stmt_field_count= reinterpret_cast<ptr2mysql_stmt_field_count>(GetProcAddr("mysql_stmt_field_count"));

		return (*ptr2_stmt_field_count)( stmt );
    }


    MYSQL_STMT * MySQL_Client_Loader::mysql_stmt_init( MYSQL * mysql )
    {
        ptr2mysql_stmt_init ptr2_stmt_init= reinterpret_cast<ptr2mysql_stmt_init>(GetProcAddr("mysql_stmt_init"));

		return (*ptr2_stmt_init)( mysql );
    }


    my_ulonglong MySQL_Client_Loader::mysql_stmt_num_rows( MYSQL_STMT * stmt )
    {
        ptr2mysql_stmt_num_rows ptr2_stmt_num_rows= reinterpret_cast<ptr2mysql_stmt_num_rows>(GetProcAddr("mysql_stmt_num_rows"));

		return (*ptr2_stmt_num_rows)( stmt );
    }


    unsigned long MySQL_Client_Loader::mysql_stmt_param_count( MYSQL_STMT * stmt )
    {
        ptr2mysql_stmt_param_count ptr2_stmt_param_count= reinterpret_cast<ptr2mysql_stmt_param_count>(GetProcAddr("mysql_stmt_param_count"));

		return (*ptr2_stmt_param_count)( stmt );
    }


    int MySQL_Client_Loader::mysql_stmt_prepare( MYSQL_STMT * stmt
        , const char * stmt_str
        , unsigned long len )
    {
        ptr2mysql_stmt_prepare ptr2_stmt_prepare= reinterpret_cast<ptr2mysql_stmt_prepare>(GetProcAddr("mysql_stmt_prepare"));

		return (*ptr2_stmt_prepare)( stmt, stmt_str, len );
    }


    MYSQL_RES * MySQL_Client_Loader::mysql_stmt_result_metadata( MYSQL_STMT * stmt )
    {
        ptr2mysql_stmt_result_metadata ptr2_stmt_result_metadata= reinterpret_cast<ptr2mysql_stmt_result_metadata>(GetProcAddr("mysql_stmt_result_metadata"));

		return (*ptr2_stmt_result_metadata)( stmt );
    }


    my_bool MySQL_Client_Loader::mysql_stmt_send_long_data( MYSQL_STMT * stmt 
        , unsigned int  par_number
        , const char *  data
        , unsigned long len )
    {
        ptr2mysql_stmt_send_long_data ptr2_stmt_send_long_data= reinterpret_cast<ptr2mysql_stmt_send_long_data>(GetProcAddr("mysql_stmt_send_long_data"));

		return (*ptr2_stmt_send_long_data)( stmt, par_number, data, len );
    }


    const char * MySQL_Client_Loader::mysql_stmt_sqlstate( MYSQL_STMT * stmt )
    {
        ptr2mysql_stmt_sqlstate ptr2_stmt_sqlstate= reinterpret_cast<ptr2mysql_stmt_sqlstate>(GetProcAddr("mysql_stmt_sqlstate"));

		return (*ptr2_stmt_sqlstate)( stmt );
    }


    int MySQL_Client_Loader::mysql_stmt_store_result( MYSQL_STMT * stmt )
    {
        ptr2mysql_stmt_store_result ptr2_stmt_store_result= reinterpret_cast<ptr2mysql_stmt_store_result>(GetProcAddr("mysql_stmt_store_result"));

		return (*ptr2_stmt_store_result)( stmt );
    } 
} /* namespace util */
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
