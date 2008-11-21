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

#ifndef _SQL_PARAMETER_METADATA_H_
#define _SQL_PARAMETER_METADATA_H_

#include <string>


namespace sql
{

class ParameterMetaData 
{
public:
	enum
	{
		parameterModeIn,
		parameterModeInOut,
		parameterModeOut,
		parameterModeUnknown
	};
	enum
	{
		parameterNoNulls,
		parameterNullable,
		parameterNullableUnknown
	};

	virtual std::string getParameterClassName(unsigned int param) = 0;

	virtual int getParameterCount() = 0;

	virtual int getParameterMode(unsigned int param) = 0;

	virtual int getParameterType(unsigned int param) = 0;

	virtual std::string getParameterTypeName(unsigned int param) = 0;

	virtual int getPrecision(unsigned int param) = 0;

	virtual int getScale(unsigned int param) = 0;

	virtual int isNullable(unsigned int param) = 0;

	virtual bool isSigned(unsigned int param) = 0;

	virtual ~ParameterMetaData() {}
};


}; /* namespace sql */

#endif /* _SQL_PARAMETER_METADATA_H_ */
