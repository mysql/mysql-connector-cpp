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
	MySQL_ParameterMetaData(MYSQL_STMT * stmt);

	virtual ~MySQL_ParameterMetaData() {}

	virtual std::string getParameterClassName(unsigned int param) const;

	virtual int getParameterCount() const;

	virtual int getParameterMode(unsigned int param) const;

	virtual int getParameterType(unsigned int param) const;

	virtual std::string getParameterTypeName(unsigned int param)  const;

	virtual int getPrecision(unsigned int param) const;

	virtual int getScale(unsigned int param) const;

	virtual int isNullable(unsigned int param) const;

	virtual bool isSigned(unsigned int param) const;


private:
	/* Prevent use of these */
	MySQL_ParameterMetaData(const MySQL_ParameterMetaData &);
	void operator=(MySQL_ParameterMetaData &);
};

}; /* namespace mysql */

}; /* namespace sql */

#endif // _MYSQL_PARAMETER_METADATA_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
