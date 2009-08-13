/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _STATEMENT_PROXY_H_
#define _STATEMENT_PROXY_H_

#include <config.h>
#include <boost/noncopyable.hpp>

#include "../mysql_statement_options.h"


struct st_mysql_bind;
struct st_mysql_res;

namespace sql
{
class SQLString;

namespace mysql
{
namespace NativeAPI
{
class Resultset_Proxy;
/*
 * 
 * 
 * 
 */
class Statement_Proxy : public boost::noncopyable
{
public:

    virtual                     ~Statement_Proxy(){}

    virtual uint64_t            affected_rows   ()  =0;

    virtual bool                attr_set        ( MySQL_Statement_Options attr
                                                , const void *arg ) =0;

    virtual bool                bind_param      ( ::st_mysql_bind * ) =0;

    virtual bool                bind_result     ( ::st_mysql_bind * ) =0;

    virtual void                data_seek       ( uint64_t ) =0;

    virtual unsigned int        errNo           ()  =0;

    virtual ::sql::SQLString    error           ()  =0;

    virtual int                 execute         ()  =0;

    virtual int                 fetch           ()  =0;

    virtual unsigned int        field_count     ()  =0;

    virtual bool                more_results    ()  =0;

    virtual int                 next_result     ()  =0;

    virtual uint64_t            num_rows        ()  =0;

    virtual unsigned long       param_count     ()  =0;

    virtual int                 prepare         ( const ::sql::SQLString & ) =0;

    virtual Resultset_Proxy *   result_metadata ()  =0;

    virtual bool                send_long_data  ( unsigned int  par_number
                                                , const char *  data
                                                , unsigned long len ) =0;

    virtual ::sql::SQLString    sqlstate        ()  =0;

    virtual int                 store_result    ()  =0;
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
