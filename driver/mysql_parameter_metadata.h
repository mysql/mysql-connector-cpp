/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_PARAMETER_METADATA_H_
#define _MYSQL_PARAMETER_METADATA_H_

#include <string>

#include <cppconn/parameter_metadata.h>

#include "mysql_private_iface.h"

namespace sql
{
namespace mysql
{

class MySQL_ParameterMetaData : public sql::ParameterMetaData
{
	unsigned int param_count;
public:
	MySQL_ParameterMetaData(const MYSQL_STMT * const stmt);

	virtual ~MySQL_ParameterMetaData() {}

	virtual int getParameterCount();

private:
	/* Prevent use of these */
	MySQL_ParameterMetaData(const MySQL_ParameterMetaData &);
	void operator=(MySQL_ParameterMetaData &);
};

} /* namespace mysql */
} /* namespace sql */

#endif // _MYSQL_PARAMETER_METADATA_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
