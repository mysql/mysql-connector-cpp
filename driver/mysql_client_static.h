/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_LIBMYSQL_STATIC_H_
#define _MYSQL_LIBMYSQL_STATIC_H_

#include "mysql_client_api.h"
#include <cppconn/config.h>
#include <boost/noncopyable.hpp>

namespace sql
{
namespace mysql
{
namespace util
{
template <class T>
class Singleton: public boost::noncopyable
{
protected:

    Singleton(){}

public:

    static boost::shared_ptr<T> & theInstance()
    {
        //shared::ptr is used only to be able to use in getCApiHandle
        static boost::shared_ptr<T> instance(new T());

        return instance;
    }
};


// macros to use in private/protected part of singleton-ed class
#define CCPP_SINGLETON(classname) classname();\
    friend class sql::mysql::util::Singleton<classname>
}

namespace NativeAPI
{


class MySQL_Client_Static : public IMySQLCAPI, public ::sql::mysql::util::Singleton<MySQL_Client_Static>
{
private:

    CCPP_SINGLETON(MySQL_Client_Static);

public:

    virtual ~MySQL_Client_Static();

    // MySQL C-API calls wrappers

    my_ulonglong  mysql_affected_rows       ( MYSQL *       );

    my_bool       mysql_autocommit          ( MYSQL *
                                            , my_bool       );

    void          mysql_close               ( MYSQL *mysql  );

    my_bool       mysql_commit              ( MYSQL *mysql  );

    void          mysql_data_seek           ( MYSQL_RES *
                                            , my_ulonglong  );

    void          mysql_debug               ( const char *  );

    unsigned int  mysql_errno               ( MYSQL *       );

    const char *  mysql_error               ( MYSQL *       );

    MYSQL_FIELD * mysql_fetch_field         ( MYSQL_RES *   );

    MYSQL_FIELD * mysql_fetch_field_direct  ( MYSQL_RES *
                                            , unsigned int  );

    unsigned long * mysql_fetch_lengths     ( MYSQL_RES *   );

    MYSQL_ROW     mysql_fetch_row           ( MYSQL_RES *   );

    unsigned int  mysql_field_count         ( MYSQL *       );

    void          mysql_free_result         ( MYSQL_RES *   );

    unsigned long mysql_get_client_version  ();

    const char *  mysql_get_server_info     ( MYSQL *       );

    unsigned long mysql_get_server_version  ( MYSQL *       );

    MYSQL *       mysql_init                ( MYSQL *mysql  );

    int           mysql_library_init        ( int, char **
                                            , char **       );

    void          mysql_library_end         ();

    my_bool       mysql_more_results        ( MYSQL *       );

    int           mysql_next_result         ( MYSQL *       );

    unsigned int  mysql_num_fields          ( MYSQL_RES *   );

    my_ulonglong  mysql_num_rows            ( MYSQL_RES *   );

    int           mysql_options             ( MYSQL *
                                            , enum mysql_option
                                            , const void *  );

    int		      mysql_query               ( MYSQL *
                                            , const char *  );

    MYSQL *       mysql_real_connect        ( MYSQL *       mysql
                                            , const char *  host
                                            , const char *  user
                                            , const char *  passwd
                                            , const char *  db
                                            , unsigned int  port
                                            , const char *  unix_socket
                                            , unsigned long client_flag );

    int           mysql_real_query          ( MYSQL *
                                            , const char *
                                            , unsigned long );

    my_bool       mysql_rollback            ( MYSQL *       );

    const char *  mysql_sqlstate            ( MYSQL *       );

    my_bool       mysql_ssl_set             ( MYSQL * mysql
                                            , const char * key
                                            , const char * cert
                                            , const char * ca
                                            , const char * capath
                                            , const char * cipher );

    MYSQL_RES *   mysql_store_result        ( MYSQL *       );

    MYSQL_RES *   mysql_use_result          ( MYSQL *       );

    /* Prepared Statement mysql_stmt_* functions */
    my_ulonglong  mysql_stmt_affected_rows  ( MYSQL_STMT *  );

    my_bool       mysql_stmt_attr_set       ( MYSQL_STMT *
                                            , enum enum_stmt_attr_type
                                            , const void *  );

    my_bool       mysql_stmt_bind_param     ( MYSQL_STMT *
                                            , MYSQL_BIND *  );

    my_bool       mysql_stmt_bind_result    ( MYSQL_STMT *
                                            , MYSQL_BIND *  );

    my_bool       mysql_stmt_close          ( MYSQL_STMT *  );

    void          mysql_stmt_data_seek      ( MYSQL_STMT *
                                            , my_ulonglong  );

    unsigned int  mysql_stmt_errno          ( MYSQL_STMT *  );

    const char *  mysql_stmt_error          ( MYSQL_STMT *  );

    int           mysql_stmt_execute        ( MYSQL_STMT *  );

    int           mysql_stmt_fetch          ( MYSQL_STMT *  );

    unsigned int  mysql_stmt_field_count    ( MYSQL_STMT *  );

    MYSQL_STMT *  mysql_stmt_init           ( MYSQL *       );

    my_ulonglong  mysql_stmt_num_rows       ( MYSQL_STMT *  );

    unsigned long mysql_stmt_param_count    ( MYSQL_STMT *  );

    int           mysql_stmt_prepare        ( MYSQL_STMT *
                                            , const char *
                                            , unsigned long );

    MYSQL_RES *   mysql_stmt_result_metadata( MYSQL_STMT *  );

    my_bool       mysql_stmt_send_long_data ( MYSQL_STMT * 
                                            , unsigned int
                                            , const char * 
                                            , unsigned long );

    const char *  mysql_stmt_sqlstate       ( MYSQL_STMT *  );

    int           mysql_stmt_store_result   ( MYSQL_STMT *  ); 


};

}
}
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
