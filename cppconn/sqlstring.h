/*
Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPL
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPL as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _SQL_STRING_H_
#define _SQL_STRING_H_

#include <string>
#include "build_config.h"


namespace sql
{
	class CPPCONN_PUBLIC_FUNC SQLString
	{
		std::string realStr;

	public:

		~SQLString()
		{
		}


		SQLString()
		{
		}


		SQLString(const SQLString & other) : realStr(other.realStr)
		{
		}


		SQLString(const std::string & other) : realStr(other)
		{
		}


		SQLString(const char other[]) : realStr(other)
		{
		}

		// Needed for stuff like SQLString str= "char * string constant"
		// ctor SQLString( const char *) would do the job too, but i like this one better
		SQLString & operator= ( const std::string & rhs )
		{
			realStr = rhs;

			return *this;
		}


		// Conversion to st::string. Comes in play for stuff like std::string str= SQLString_var;
		operator std::string const&() const
		{
			return realStr;
		}


		/** For access std::string methods. Not sure we need it. Makes it look like some smart ptr.
			possibly operator* - will look even more like smart ptr */
		std::string * operator ->()
		{
			return & realStr;
		}


		const std::string & get() const
		{
			return realStr;
		}


		const char * c_str() const
		{
			return realStr.c_str();
		}


		const SQLString & operator+=( const SQLString & op2 )
		{
			realStr+= op2.get();
			return *this;
		}
};

  /*
  * Operators that can and have to be not a member.
  */
inline const SQLString operator+( const SQLString & op1, const SQLString & op2 )
{
	return sql::SQLString(op1.get() + op2.get());
}


inline bool operator ==( const SQLString & op1, const SQLString & op2 )
{
	return (op1.get() == op2.get());
}


inline bool operator !=(const SQLString & op1, const SQLString & op2)
{
	return (op1.get() != op2.get());
}

}// namespace sql


namespace std
{
	// operator << for SQLString output
	inline ostream & operator << (ostream & os, const sql::SQLString & str )
	{
		return os << str.c_str();
	}
}
#endif
