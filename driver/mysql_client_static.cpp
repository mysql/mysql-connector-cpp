/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/


#include "mysql_client_static.h"


namespace sql
{
namespace mysql
{
namespace NativeAPI
{

    MySQL_Client_Static::MySQL_Client_Static()
    {
        this->mysql_library_init(0, NULL, NULL);
    }

    MySQL_Client_Static::~MySQL_Client_Static()
    {
        this->mysql_library_end();
    }

    // MySQL C-API calls wrappers

    my_ulonglong MySQL_Client_Static::mysql_affected_rows( MYSQL * mysql )
    {
        return ::mysql_affected_rows( mysql );
    }

    my_bool MySQL_Client_Static::mysql_autocommit( MYSQL * mysql, my_bool mode )
	{
        return ::mysql_autocommit( mysql, mode );
	}

    void MySQL_Client_Static::mysql_close( MYSQL * mysql )
	{
        return ::mysql_close( mysql );
	}

    my_bool MySQL_Client_Static::mysql_commit( MYSQL * mysql )
	{
        return ::mysql_commit( mysql );
	}

    void MySQL_Client_Static::mysql_data_seek( MYSQL_RES *  result
                                             , my_ulonglong offset )
	{
        return ::mysql_data_seek( result, offset );
	}

    void MySQL_Client_Static::mysql_debug( const char * debug )
	{
        return ::mysql_debug( debug );
	}

    unsigned int MySQL_Client_Static::mysql_errno( MYSQL * mysql )
	{
		return ::mysql_errno( mysql );
	}

    const char * MySQL_Client_Static::mysql_error( MYSQL * mysql )
	{
		return ::mysql_error( mysql );
	}

    MYSQL_FIELD * MySQL_Client_Static::mysql_fetch_field( MYSQL_RES * result )
	{
		return ::mysql_fetch_field( result );
	}

    MYSQL_FIELD * MySQL_Client_Static::mysql_fetch_field_direct( MYSQL_RES * result
                                                              , unsigned int fieldnr )
	{
        return ::mysql_fetch_field_direct( result, fieldnr );
	}

    unsigned long * MySQL_Client_Static::mysql_fetch_lengths( MYSQL_RES * result )
	{
		return ::mysql_fetch_lengths( result );
	}

    MYSQL_ROW MySQL_Client_Static::mysql_fetch_row( MYSQL_RES * result )
	{
		return ::mysql_fetch_row( result );
	}

    unsigned int MySQL_Client_Static::mysql_field_count( MYSQL * mysql )
	{
		return ::mysql_field_count( mysql );
	}

    void MySQL_Client_Static::mysql_free_result( MYSQL_RES * result )
	{
		return ::mysql_free_result( result );
	}

    unsigned long MySQL_Client_Static::mysql_get_client_version()
    {
        return ::mysql_get_client_version();
    }

    const char * MySQL_Client_Static::mysql_get_server_info( MYSQL * mysql )
	{
		return ::mysql_get_server_info( mysql );
	}

    unsigned long MySQL_Client_Static::mysql_get_server_version( MYSQL * mysql )
	{
		return ::mysql_get_server_version( mysql );
	}

    MYSQL * MySQL_Client_Static::mysql_init( MYSQL * mysql )
	{
		return ::mysql_init( mysql );
	}

    int MySQL_Client_Static::mysql_library_init(int argc,char **argv,char **groups)
	{
		return ::mysql_library_init( argc, argv, groups );
	}

    void MySQL_Client_Static::mysql_library_end()
    {
        return ::mysql_library_end();
    }

    my_bool MySQL_Client_Static::mysql_more_results( MYSQL * mysql )
	{
		return ::mysql_more_results( mysql );
	}

    int MySQL_Client_Static::mysql_next_result( MYSQL * mysql )
	{
		return ::mysql_next_result( mysql );
	}

    unsigned int MySQL_Client_Static::mysql_num_fields( MYSQL_RES * result )
	{
		return ::mysql_num_fields( result );
	}

    my_ulonglong MySQL_Client_Static::mysql_num_rows( MYSQL_RES * result )
	{
		return ::mysql_num_rows( result );
	}


    int MySQL_Client_Static::mysql_options( MYSQL * mysql 
                                            , enum mysql_option option
                                            , const void *arg )
	{
        // in 5.0 mysql_options's 3rd parameter is "const char *"
        return ::mysql_options( mysql, option, static_cast<const char *>(arg) );
	}


    int MySQL_Client_Static::mysql_query( MYSQL * mysql, const char *stmt_str )
	{
        return ::mysql_query( mysql, stmt_str );
	}


    MYSQL * MySQL_Client_Static::mysql_real_connect( MYSQL * mysql, const char * host
                                                , const char * user, const char * passwd
                                                , const char * db, unsigned int port
                                                , const char * unix_socket
                                                , unsigned long client_flag )
    {
        return ::mysql_real_connect( mysql, host, user, passwd, db, port, unix_socket, client_flag );
    }


    int MySQL_Client_Static::mysql_real_query(MYSQL *mysql,const char *stmt_str
                                            , unsigned long len)
	{
		return ::mysql_real_query( mysql, stmt_str, len );
	}

    my_bool MySQL_Client_Static::mysql_rollback( MYSQL * mysql )
	{
		return ::mysql_rollback( mysql );
	}

    const char * MySQL_Client_Static::mysql_sqlstate( MYSQL * mysql )
	{
		return ::mysql_sqlstate( mysql );
	}

    my_bool MySQL_Client_Static::mysql_ssl_set( MYSQL    * mysql
                                            , const char * key
                                            , const char * cert
                                            , const char * ca
                                            , const char * capath
                                            , const char * cipher )
	{
        return ::mysql_ssl_set( mysql, key, cert, ca, capath, cipher );
	}

    MYSQL_RES * MySQL_Client_Static::mysql_store_result( MYSQL * mysql )
	{
		return ::mysql_store_result( mysql );
	}

    MYSQL_RES * MySQL_Client_Static::mysql_use_result( MYSQL * mysql )
	{
		return ::mysql_use_result( mysql );
	}

    /* Prepared Statement mysql_stmt_* functions */
    my_ulonglong MySQL_Client_Static::mysql_stmt_affected_rows(MYSQL_STMT *stmt)
	{
		return ::mysql_stmt_affected_rows( stmt );
	}

    my_bool MySQL_Client_Static::mysql_stmt_attr_set( MYSQL_STMT *      stmt
                                            , enum enum_stmt_attr_type  option
                                                    , const void *      arg )
	{
        return ::mysql_stmt_attr_set( stmt, option, arg );
	}


    my_bool MySQL_Client_Static::mysql_stmt_bind_param( MYSQL_STMT * stmt
                                                      , MYSQL_BIND * bind )
	{
        return ::mysql_stmt_bind_param( stmt, bind );
	}


    my_bool MySQL_Client_Static::mysql_stmt_bind_result( MYSQL_STMT * stmt
                                                       , MYSQL_BIND * bind )
	{
        return ::mysql_stmt_bind_result( stmt, bind );
	}


    my_bool MySQL_Client_Static::mysql_stmt_close( MYSQL_STMT * stmt )
	{
		return ::mysql_stmt_close( stmt );
	}


    void MySQL_Client_Static::mysql_stmt_data_seek( MYSQL_STMT * stmt
                                                  , my_ulonglong row_nr )
	{
        return ::mysql_stmt_data_seek( stmt, row_nr );
	}


    unsigned int MySQL_Client_Static::mysql_stmt_errno( MYSQL_STMT * stmt )
	{
		return ::mysql_stmt_errno( stmt );
	}


    const char * MySQL_Client_Static::mysql_stmt_error( MYSQL_STMT * stmt )
	{
		return ::mysql_stmt_error( stmt );
	}


    int MySQL_Client_Static::mysql_stmt_execute( MYSQL_STMT * stmt )
	{
		return ::mysql_stmt_execute( stmt );
	}


    int MySQL_Client_Static::mysql_stmt_fetch( MYSQL_STMT * stmt )
	{
		return ::mysql_stmt_fetch( stmt );
	}


    unsigned int MySQL_Client_Static::mysql_stmt_field_count( MYSQL_STMT * stmt )
	{
		return ::mysql_stmt_field_count( stmt );
	}


    MYSQL_STMT * MySQL_Client_Static::mysql_stmt_init( MYSQL * mysql )
	{
		return ::mysql_stmt_init( mysql );
	}


    my_ulonglong MySQL_Client_Static::mysql_stmt_num_rows( MYSQL_STMT * stmt )
	{
		return ::mysql_stmt_num_rows( stmt );
	}


    unsigned long MySQL_Client_Static::mysql_stmt_param_count( MYSQL_STMT * stmt )
	{
		return ::mysql_stmt_param_count( stmt );
	}


    int MySQL_Client_Static::mysql_stmt_prepare( MYSQL_STMT * stmt
                                               , const char * stmt_str
                                               , unsigned long len )
	{
        return ::mysql_stmt_prepare( stmt, stmt_str, len );
	}


    MYSQL_RES * MySQL_Client_Static::mysql_stmt_result_metadata( MYSQL_STMT * stmt )
	{
		return ::mysql_stmt_result_metadata( stmt );
	}


    my_bool MySQL_Client_Static::mysql_stmt_send_long_data( MYSQL_STMT * stmt 
                                                    , unsigned int  par_number
                                                    , const char *  data
                                                    , unsigned long len )
	{
        return ::mysql_stmt_send_long_data( stmt, par_number, data, len );
	}


    const char * MySQL_Client_Static::mysql_stmt_sqlstate( MYSQL_STMT * stmt )
	{
		return ::mysql_stmt_sqlstate( stmt );
	}


    int MySQL_Client_Static::mysql_stmt_store_result( MYSQL_STMT * stmt )
	{
		return ::mysql_stmt_store_result( stmt );
	} 
}
}
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
