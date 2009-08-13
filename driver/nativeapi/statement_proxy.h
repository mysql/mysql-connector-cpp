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

#ifndef _ABSTRACT
#define _ABSTRACT
#endif

#ifndef _PURE
#define _PURE =0
#endif

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
_ABSTRACT class Statement_Proxy : public boost::noncopyable
{
public:

    virtual                     ~Statement_Proxy(){}

    virtual uint64_t            affected_rows   ()  _PURE;

    virtual bool                attr_set        ( MySQL_Statement_Options attr
                                                , const void *arg ) _PURE;

    virtual bool                bind_param      ( ::st_mysql_bind * ) _PURE;

    virtual bool                bind_result     ( ::st_mysql_bind * ) _PURE;

    virtual void                data_seek       ( uint64_t ) _PURE;

    virtual unsigned int        errNo           ()  _PURE;

    virtual ::sql::SQLString    error           ()  _PURE;

    virtual int                 execute         ()  _PURE;

    virtual int                 fetch           ()  _PURE;

    virtual unsigned int        field_count     ()  _PURE;

    virtual bool                more_results    ()  _PURE;

    virtual int                 next_result     ()  _PURE;

    virtual uint64_t            num_rows        ()  _PURE;

    virtual unsigned long       param_count     ()  _PURE;

    virtual int                 prepare         ( const ::sql::SQLString & ) _PURE;

    virtual Resultset_Proxy *   result_metadata ()  _PURE;

    virtual bool                send_long_data  ( unsigned int  par_number
                                                , const char *  data
                                                , unsigned long len ) _PURE;

    virtual ::sql::SQLString    sqlstate        ()  _PURE;

    virtual int                 store_result    ()  _PURE;
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
