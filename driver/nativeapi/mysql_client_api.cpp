/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include <map>
#include <boost/shared_ptr.hpp>
#include <cppconn/sqlstring.h>

#include "binding_config.h"

#ifdef MYSQLCLIENT_STATIC_BINDING
/* MySQL client library is linked */
# include "mysql_client_static.h"
#else
/* MySQL client library will be dynamically loaded */
# include "mysql_client_loader.h"
#endif

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

static std::map< sql::SQLString, boost::shared_ptr<IMySQLCAPI> > wrapper;

boost::shared_ptr<IMySQLCAPI> getCApiHandle( const sql::SQLString & name )
{
#ifdef MYSQLCLIENT_STATIC_BINDING
    return MySQL_Client_Static::theInstance();
#else

    std::map< sql::SQLString, boost::shared_ptr< IMySQLCAPI > >::const_iterator cit;

    if ((cit= wrapper.find(name)) != wrapper.end()) {
        return cit->second;
    } else {
        boost::shared_ptr<IMySQLCAPI> newWrapper;

        newWrapper.reset(new Libmysql_Dynamic_Proxy(name));
        wrapper[name] = newWrapper;

        return newWrapper;
    }
#endif
}

} /* namespace NativeAPI */
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
