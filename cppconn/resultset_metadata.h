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

#ifndef _SQL_RESULTSET_METADATA_H_
#define _SQL_RESULTSET_METADATA_H_

#include <string>

namespace sql
{

class ResultSetMetaData 
{
public:
	enum
	{
		columnNoNulls,
		columnNullable,
		columnNullableUnknown
	};

	virtual std::string getCatalogName(unsigned int column) = 0;

	virtual unsigned int getColumnCount() = 0;

	virtual unsigned int getColumnDisplaySize(unsigned int column) = 0;

	virtual std::string getColumnLabel(unsigned int column) = 0;

	virtual std::string getColumnName(unsigned int column) = 0;

	virtual int getColumnType(unsigned int column) = 0;

	virtual std::string getColumnTypeName(unsigned int column) = 0;

	virtual unsigned int getPrecision(unsigned int column) = 0;

	virtual unsigned int getScale(unsigned int column) = 0;

	virtual std::string getSchemaName(unsigned int column) = 0;

	virtual std::string getTableName(unsigned int column) = 0;

	virtual bool isAutoIncrement(unsigned int column) = 0;

	virtual bool isCaseSensitive(unsigned int column) = 0;

	virtual bool isCurrency(unsigned int column) = 0;

	virtual bool isDefinitelyWritable(unsigned int column) = 0;

	virtual int isNullable(unsigned int column) = 0;

	virtual bool isReadOnly(unsigned int column) = 0;

	virtual bool isSearchable(unsigned int column) = 0;

	virtual bool isSigned(unsigned int column) = 0;

	virtual bool isWritable(unsigned int column) = 0;

	virtual ~ResultSetMetaData() {}
};


}; /* namespace sql */

#endif /* _SQL_RESULTSET_METADATA_H_ */
