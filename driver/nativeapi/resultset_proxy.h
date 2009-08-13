/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _RESULTSET_PROXY_H_
#define _RESULTSET_PROXY_H_

#include <boost/noncopyable.hpp>
#include <config.h>


struct st_mysql_field;

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

class Resultset_Proxy : public boost::noncopyable
{
public:

    virtual			            ~Resultset_Proxy    (){}

    virtual void                data_seek           ( uint64_t ) =0;

    virtual ::st_mysql_field *  fetch_field         () =0;

    virtual ::st_mysql_field *  fetch_field_direct  ( unsigned int  ) =0;

    virtual unsigned long *     fetch_lengths       () =0;

    virtual char**              fetch_row           () =0;

    virtual unsigned int        num_fields          () =0;

    virtual uint64_t            num_rows            () =0;
};

} /* namespace NativeAPI*/
} /* namespace mysql    */
} /* namespace sql      */

#endif // _RESULTSET_PROXY_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
