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

#ifndef _SQL_WARNING_H_
#define _SQL_WARNING_H_


#include <stdexcept>
#include <string>
#include <memory>

namespace sql
{

#ifdef _WIN32
#pragma warning (disable : 4290)
//warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#endif

class SQLWarning
{
protected:

	const std::string                       sql_state;
	const int                               errNo;
	mutable std::auto_ptr<const SQLWarning> next;
	const std::string                       descr;

public:

	SQLWarning(const std::string& reason, const std::string& SQLState, int vendorCode) :sql_state(SQLState), errNo(vendorCode),descr(reason)
	{
	}

	SQLWarning(const std::string& reason, const std::string& SQLState) :sql_state (SQLState), errNo(0), descr(reason)
	{
	}

	SQLWarning(const std::string& reason) : sql_state ("HY000"), errNo(0), descr(reason)
	{
	}

	SQLWarning() : sql_state ("HY000"), errNo(0) {}


	const std::string & getMessage() const
	{
		return descr;
	}


	const std::string & getSQLState() const
	{
		return sql_state;
	}

	int getErrorCode() const
	{
		return errNo;
	}

	const SQLWarning & getNextWarning() const
	{
		return *next;
	}

	void setNextWarning(SQLWarning * _next)
	{
		next.reset(_next);
	}

	virtual ~SQLWarning() throw () {};

protected:

	SQLWarning(const SQLWarning& e) : sql_state(e.sql_state), errNo(e.errNo), next(e.next), descr(e.descr) {}

	virtual SQLWarning * copy()
	{
		return new SQLWarning(*this);
	}

private:
	const SQLWarning & operator = (const SQLWarning & rhs);

};


}; /* namespace sql */

#endif /* _SQL_WARNING_H_ */
