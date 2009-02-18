/* Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

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

#ifndef _SQL_STATEMENT_H_
#define _SQL_STATEMENT_H_

#include <string>
#include "resultset.h"
#include "warning.h"

namespace sql
{

class ResultSet;
class Connection;


class Statement
{
public:
	virtual ~Statement() {};

	virtual Connection * getConnection() = 0;

	virtual void cancel() = 0;

	virtual void clearWarnings() = 0;

	virtual void close() = 0;

	virtual bool execute(const std::string& sql) = 0;

	virtual ResultSet * executeQuery(const std::string& sql) = 0;

	virtual int executeUpdate(const std::string& sql) = 0;

	virtual unsigned int getFetchSize() = 0;

	virtual unsigned int getMaxFieldSize() = 0;

	virtual uint64_t getMaxRows() = 0;

	virtual bool getMoreResults() = 0;

	virtual unsigned int getQueryTimeout() = 0;

	virtual ResultSet * getResultSet() = 0;

	virtual uint64_t getUpdateCount() = 0;

	virtual const SQLWarning * getWarnings() = 0;

	virtual void setCursorName(const std::string & name) = 0;

	virtual void setEscapeProcessing(bool enable) = 0;

	virtual void setFetchSize(unsigned int rows) = 0;

	virtual void setMaxFieldSize(unsigned int max) = 0;

	virtual void setMaxRows(unsigned int max) = 0;

	virtual void setQueryTimeout(unsigned int seconds) = 0;
};

}; /* namespace sql */

#endif /* _SQL_STATEMENT_H_ */
