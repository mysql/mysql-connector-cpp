/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _CONNECTION_PROXY_H_
#define _CONNECTION_PROXY_H_

#include <boost/noncopyable.hpp>
#include <config.h>

#include "../mysql_connection_options.h"

//TODO - move following definitions elsewhere and have them in one place only
#ifndef _ABSTRACT
#define _ABSTRACT
#endif

#ifndef _PURE
#define _PURE =0
#endif


namespace sql
{

class SQLString;

namespace mysql
{
enum MySQL_Connection_Options;

namespace NativeAPI
{
class Resultset_Proxy;
class Statement_Proxy;

_ABSTRACT class  Connection_Proxy : public boost::noncopyable
{

public:

    virtual                     ~Connection_Proxy   (){}


    virtual uint64_t            affected_rows       ()          _PURE;

    virtual bool                autocommit          ( bool )    _PURE;

    virtual bool                connect             ( const ::sql::SQLString   & host
                                                      , const ::sql::SQLString & user
                                                      , const ::sql::SQLString & passwd
                                                      , const ::sql::SQLString & db
                                                      , unsigned int             port
                                                      , const ::sql::SQLString & socket_or_pipe
                                                      , unsigned long            client_flag ) _PURE;

    virtual bool                commit              ()          _PURE;

    virtual void                debug               ( const ::sql::SQLString & ) _PURE;

    virtual unsigned int        errNo               ()          _PURE;

    virtual ::sql::SQLString    error               ()          _PURE;

    virtual unsigned int        field_count         ()			_PURE;

    virtual unsigned long       get_client_version  ()			_PURE;

    virtual const ::sql::SQLString & get_server_info()			_PURE;

    virtual unsigned long       get_server_version  ()			_PURE;

    virtual bool                more_results        ()			_PURE;

    virtual int                 next_result         ()			_PURE;

    virtual int                 options             ( ::sql::mysql::MySQL_Connect_Options
                                                    , const void *      )   _PURE;

    virtual int		            query               ( const SQLString & )   _PURE;

    /*
    virtual int                 real_query          ( const SQLString &
                                                        , uint64_t          )   _PURE;*/
    

    virtual bool                rollback            ()			_PURE;

    virtual ::sql::SQLString    sqlstate            ()			_PURE;

    virtual bool                ssl_set             ( const ::sql::SQLString & key
                                                    , const ::sql::SQLString & cert
                                                    , const ::sql::SQLString & ca
                                                    , const ::sql::SQLString & capath
                                                    , const ::sql::SQLString & cipher ) _PURE;

    virtual Resultset_Proxy &   store_result        ()			_PURE;

    virtual Resultset_Proxy &   use_result          ()			_PURE;
 
    virtual Statement_Proxy &   stmt_init           ()			_PURE;
};

Connection_Proxy * createConnectionProxy( const SQLString & clientFileName );

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
