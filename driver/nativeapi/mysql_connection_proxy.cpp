/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include <sqlstring.h>
#include <exception.h>

#include "../mysql_util.h"
#include "../mysql_connection_options.h"

#include "mysql_client_api.h"
#include "mysql_resultset_proxy.h"
#include "mysql_statement_proxy.h"

#include "mysql_connection_proxy.h"


namespace sql
{
namespace mysql
{
namespace NativeAPI
{
    Connection_Proxy * createConnectionProxy( const SQLString & clientFileName )
    {
        return new MySQL_Connection_Proxy( clientFileName );
    }


    MySQL_Connection_Proxy::MySQL_Connection_Proxy( const ::sql::SQLString & clientFileName )

        :   api     (::sql::mysql::NativeAPI::getCApiHandle( clientFileName ) ),
            mysql   ( api->init( NULL ) )
    {
        if ( mysql == NULL )
            throw sql::SQLException("Insufficient memory: cannot create MySQL handle using mysql_init()");
    }

    MySQL_Connection_Proxy::~MySQL_Connection_Proxy()
    {
        api->close( mysql );
    }


    uint64_t MySQL_Connection_Proxy::affected_rows()
	{
        return api->affected_rows( mysql );
	}


    bool MySQL_Connection_Proxy::autocommit( bool mode )
	{
        return (api->autocommit( mysql, mode ) != '\0');
	}


    bool MySQL_Connection_Proxy::connect( const ::sql::SQLString & host
                                        , const ::sql::SQLString & user
                                        , const ::sql::SQLString & passwd
                                        , const ::sql::SQLString & db
                                        , unsigned int             port
                                        , const ::sql::SQLString & socket_or_pipe
                                        , unsigned long            client_flag )
    {
        return ( api->real_connect( mysql, host.c_str(), user.c_str(), passwd.c_str()
            , db.length() > 0 ? db.c_str() : NULL, port
            , socket_or_pipe.length() > 0 ? socket_or_pipe.c_str() : NULL, client_flag ) != NULL );
    }


    bool MySQL_Connection_Proxy::commit()
	{
        return (api->commit( mysql ) != '\0');
	}


    void MySQL_Connection_Proxy::debug( const SQLString & debug )
	{
        api->debug( debug.c_str() );
	}


    unsigned int MySQL_Connection_Proxy::errNo()
	{
        return api->mysql_errno( mysql );
	}


    SQLString MySQL_Connection_Proxy::error()
	{
        return api->error( mysql );
	}


    unsigned int MySQL_Connection_Proxy::field_count()
	{
        return api->field_count( mysql );
	}


    unsigned long MySQL_Connection_Proxy::get_client_version()
	{
        return api->get_client_version();
	}


    const SQLString & MySQL_Connection_Proxy::get_server_info()
	{
        serverInfo= api->get_server_info( mysql ); 
        return serverInfo;
	}


    unsigned long MySQL_Connection_Proxy::get_server_version()
	{
        return api->get_server_version( mysql );
	}


    bool MySQL_Connection_Proxy::more_results()
	{
        return ( api->more_results( mysql ) != '\0' );
	}



    int MySQL_Connection_Proxy::next_result()
	{
        return api->next_result( mysql );
	}


    int MySQL_Connection_Proxy::options( ::sql::mysql::MySQL_Connection_Options option
                                        , const void * value )
	{
        return api->options( mysql, static_cast<::mysql_option>(option), value );
	}



    int MySQL_Connection_Proxy::query( const SQLString & stmt_str )
	{
        return api->real_query( mysql, stmt_str.c_str(), stmt_str.length() );
	}


    bool MySQL_Connection_Proxy::rollback()
	{
        return ( api->rollback( mysql ) != '\0' );
	}


    SQLString MySQL_Connection_Proxy::sqlstate()
	{
        return api->sqlstate( mysql );
	}


    bool MySQL_Connection_Proxy::ssl_set( const SQLString & key
                                        , const SQLString & cert
                                        , const SQLString & ca
                                        , const SQLString & capath
                                        , const SQLString & cipher )
	{
        return ( api->ssl_set( mysql, key.c_str(), cert.c_str(), ca.c_str()
                            , capath.c_str(), cipher.c_str() ) != '\0' );
	}


    Resultset_Proxy & MySQL_Connection_Proxy::store_result()
	{
        ::st_mysql_res * raw= api->store_result( mysql );

        if ( raw == 0 )
        {
            /*CPP_ERR_FMT("Error during %s_result : %d:(%s) %s", resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY? "use":"store",
            this->errNo(), this->sqlstate(), this->error());*/
            ::sql::mysql::util::throwSQLException(*this);
        }

        return *(new MySQL_Resultset_Proxy( raw, api ) );
	}


    Resultset_Proxy & MySQL_Connection_Proxy::use_result()
	{
        ::st_mysql_res * raw= api->use_result( mysql );

        if ( raw == 0 )
        {
            /*CPP_ERR_FMT("Error during %s_result : %d:(%s) %s", resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY? "use":"store",
                            this->errNo(), this->sqlstate(), this->error());*/
              ::sql::mysql::util::throwSQLException(*this);
        }

        return *(new MySQL_Resultset_Proxy( raw, api ) );
	}

 
    Statement_Proxy & MySQL_Connection_Proxy::stmt_init()
	{
        ::st_mysql_stmt * raw= api->stmt_init( mysql );

        if ( raw == NULL )
        {
            /*CPP_ERR_FMT("No statement : %d:(%s) %s", e->errNo(), proxy->sqlstate(), proxy->error());*/
            ::sql::mysql::util::throwSQLException(*this);
        }

        return *(new MySQL_Statement_Proxy( raw, api, this ) );
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
