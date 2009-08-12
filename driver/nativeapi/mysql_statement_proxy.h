/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_STATEMENT_PROXY_H_
#define _MYSQL_STATEMENT_PROXY_H_


#include <boost/shared_ptr.hpp>

#include "statement_proxy.h"

struct ::st_mysql_stmt;

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

class IMySQLCAPI;
class Connection_Proxy;
/*
 * 
 * 
 * 
 */
class MySQL_Statement_Proxy : public Statement_Proxy
{

    boost::shared_ptr<IMySQLCAPI>   api;
    ::st_mysql_stmt *               stmt;
    Connection_Proxy *              conn;

                        MySQL_Statement_Proxy(){}

public:
                        MySQL_Statement_Proxy( ::st_mysql_stmt *, boost::shared_ptr<IMySQLCAPI>, Connection_Proxy * connProxy );
                        ~MySQL_Statement_Proxy();

    uint64_t            affected_rows   ();

    bool                attr_set        ( enum ::enum_stmt_attr_type option
                                        , const void *arg );

    bool                bind_param      ( ::st_mysql_bind *  );

    bool                bind_result     ( ::st_mysql_bind *  );

    void                data_seek       ( uint64_t  );

    unsigned int        errNo           ();

    ::sql::SQLString    error           ();

    int                 execute         ();

    int                 fetch           ();

    unsigned int        field_count     ();

    bool                more_results    ();

    int                 next_result     ();

    uint64_t            num_rows        ();

    unsigned long       param_count     ();

    int                 prepare         ( const ::sql::SQLString & );

    Resultset_Proxy &   result_metadata ();

    bool                send_long_data  ( unsigned int  par_number
                                        , const char *  data
                                        , unsigned long len);

    ::sql::SQLString    sqlstate        ();

    int                 store_result    ();
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
