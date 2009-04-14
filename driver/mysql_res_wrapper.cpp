/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include <cppconn/exception.h>
#include "mysql_res_wrapper.h"


namespace sql
{

namespace mysql
{

MYSQL_RES_Wrapper::MYSQL_RES_Wrapper(MYSQL_RES * res)
  : result(res), refcount(1), is_valid(true)
{

}

MYSQL_RES_Wrapper::~MYSQL_RES_Wrapper()
{
}

MYSQL_RES_Wrapper * MYSQL_RES_Wrapper::getReference()
{
	if (is_valid) {
		++refcount;
		return this;
	} else {
		throw sql::InvalidInstanceException("Object is invalid");
	}
}

void MYSQL_RES_Wrapper::deleteReference()
{
	if (!--refcount) {
		delete this;
	}
}


bool MYSQL_RES_Wrapper::isValid() const throw()
{
	return is_valid;
}


void MYSQL_RES_Wrapper::dispose() throw ()
{
	mysql_free_result(result);
	is_valid = 0;
}


MYSQL_RES * MYSQL_RES_Wrapper::get() const
{
	if (!is_valid) {
		throw sql::InvalidInstanceException("Object is invalid");
	}
	return result;
}

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
