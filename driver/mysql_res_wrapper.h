/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

*/

#ifndef _MYSQL_RES_WRAPPER_H_
#define _MYSQL_RES_WRAPPER_H_

#include "mysql_private_iface.h"

namespace sql
{
namespace mysql
{

class MYSQL_RES_Wrapper
{
	MYSQL_RES *		result; /* should be first */
	unsigned int	refcount;
	bool					is_valid;

public:
	MYSQL_RES_Wrapper(MYSQL_RES * res);
	~MYSQL_RES_Wrapper();

	MYSQL_RES_Wrapper * getReference();

	void deleteReference();

	bool isValid() const throw();

	void dispose() throw();

	MYSQL_RES * get() const;
};

} /* namespace mysql */
} /* namespace sql */

#endif
