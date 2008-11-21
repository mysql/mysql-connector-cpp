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

#ifndef _SQL_DATATYPE_H_
#define _SQL_DATATYPE_H_

namespace sql
{

class DataType
{
	DataType();
public:
	enum {
		BIT = -7,
		TINYINT = -6,
		SMALLINT = 5,
		INTEGER = 4,
		BIGINT = -5,
		FLOAT = 6,
		REAL = 7,
		DOUBLE = 8,
		NUMERIC = 2,
		DECIMAL = 3,
		CHAR = 1,
		VARCHAR = 12,
		LONGVARCHAR = -1,
		DATE = 91,
		TIME = 92,
		TIMESTAMP = 93,
		BINARY = -2,
		VARBINARY = -3,
		LONGVARBINARY = -4,
		SQLNULL = 0,
		OTHER = 1111,
		OBJECT = 2000,
		DISTINCT = 2001,
		STRUCT = 2002,
		ARRAY = 2003,
		BLOB = 2004,
		CLOB = 2005,
		REF = 2006,
		BOOLEAN = 16
	};
};

}; /* namespace */

#endif /* _SQL_DATATYPE_H_ */
