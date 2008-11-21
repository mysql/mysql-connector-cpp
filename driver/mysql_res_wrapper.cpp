/* Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   There are special exceptions to the terms and conditions of the GPL
   as it is applied to this software. View the full text of the
   exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
   software distribution.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
		refcount++;
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

}; /* namespace mysql */
}; /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
