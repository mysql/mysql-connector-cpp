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


#ifndef _SQL_PREPARED_STATEMENT_H_
#define _SQL_PREPARED_STATEMENT_H_

#include "statement.h"
#include "blob.h"


namespace sql
{

class ResultSet;
class Connection;
class ParameterMetaData;

class PreparedStatement : public Statement
{
public:
	virtual ~PreparedStatement() {}

	virtual void cancel() = 0;

	virtual void clearParameters() = 0;

	virtual bool execute(const std::string& sql) = 0;
	virtual bool execute() = 0;

	virtual ResultSet *executeQuery(const std::string& sql) = 0;
	virtual ResultSet *executeQuery() = 0;

	virtual int executeUpdate(const std::string& sql) = 0;
	virtual int executeUpdate() = 0;

	virtual ParameterMetaData * getParameterMetaData() = 0;

	virtual void setBigInt(unsigned int parameterIndex, const std::string& value) = 0;

	virtual void setBlob(unsigned int parameterIndex, sql::Blob * blob) = 0;

	virtual void setBoolean(unsigned int parameterIndex, bool value) = 0;

	virtual void setDateTime(unsigned int parameterIndex, const std::string& value) = 0;

	virtual void setDouble(unsigned int parameterIndex, double value) = 0;

	virtual void setInt(unsigned int parameterIndex, int value) = 0;

	virtual void setLong(unsigned int parameterIndex, long long value) = 0;

	virtual void setNull(unsigned int parameterIndex, int sqlType) = 0;

	virtual void setString(unsigned int parameterIndex, const std::string& value) = 0;
};


}; /* namespace sql */

#endif /* _SQL_PREPARED_STATEMENT_H_ */
