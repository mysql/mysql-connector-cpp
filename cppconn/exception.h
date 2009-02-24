/*
   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _SQL_EXCEPTION_H_
#define _SQL_EXCEPTION_H_


#include <stdexcept>
#include <string>
#include <memory>

namespace sql
{

#define MEMORY_ALLOC_OPERATORS(Class) \
	void* operator new(size_t size) throw (std::bad_alloc) { return ::operator new(size); }  \
	void* operator new(size_t, void*) throw(); \
	void* operator new(size_t, const std::nothrow_t&) throw(); \
	void* operator new[](size_t) throw (std::bad_alloc); \
	void* operator new[](size_t, void*) throw(); \
	void* operator new[](size_t, const std::nothrow_t&) throw(); \
	void* operator new(size_t N, std::allocator<Class>&); \
	virtual SQLException* copy() { return new Class(*this); }

#ifdef _WIN32
#pragma warning (disable : 4290)
//warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#endif

class SQLException : public std::runtime_error
{
protected:
	const std::string sql_state;
	const int errNo;

public:
	SQLException(const SQLException& e) : std::runtime_error(e.what()), sql_state(e.sql_state), errNo(e.errNo) {}

	SQLException(const std::string& reason, const std::string& SQLState, int vendorCode) :
		std::runtime_error  (reason    ),
		sql_state           (SQLState  ),
		errNo               (vendorCode)
	{}

	SQLException(const std::string& reason, const std::string& SQLState) : std::runtime_error(reason), sql_state(SQLState), errNo(0) {}

	SQLException(const std::string& reason) : std::runtime_error(reason), sql_state("HY000"), errNo(0) {}

	SQLException() : std::runtime_error(""), sql_state("HY000"), errNo(0) {}

	const std::string& getSQLState() const
	{
		return sql_state;
	}

	int getErrorCode() const
	{
		return errNo;
	}

	virtual ~SQLException() throw () {};

protected:
	MEMORY_ALLOC_OPERATORS(SQLException)
};

struct MethodNotImplementedException : public SQLException
{
 	MethodNotImplementedException(const MethodNotImplementedException& e) : SQLException(e.what(), e.sql_state, e.errNo) { }
	MethodNotImplementedException(const std::string& reason) : SQLException(reason, "", 0) {}

private:
	virtual SQLException* copy() { return new MethodNotImplementedException(*this); }
};

struct InvalidArgumentException : public SQLException
{
 	InvalidArgumentException(const InvalidArgumentException& e) : SQLException(e.what(), e.sql_state, e.errNo) { }
 	InvalidArgumentException(const std::string& reason) : SQLException(reason, "", 0) {}

private:
  virtual SQLException* copy() { return new InvalidArgumentException(*this); }
};

struct InvalidInstanceException : public SQLException
{
 	InvalidInstanceException(const InvalidInstanceException& e) : SQLException(e.what(), e.sql_state, e.errNo) { }
 	InvalidInstanceException(const std::string& reason) : SQLException(reason, "", 0) {}

private:
  virtual SQLException* copy() { return new InvalidInstanceException(*this); }
};

}; /* namespace sql */

#endif /* _SQL_EXCEPTION_H_ */
