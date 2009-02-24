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

#ifndef _MYSQL_UTIL_H_
#define _MYSQL_UTIL_H_


#if !defined(_WIN32) && !defined(_WIN64)
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#else
typedef __int8            int8_t;
#ifndef uint8_t
typedef unsigned __int8   uint8_t;
#endif
typedef __int16           int16_t;
typedef unsigned __int16  uint16_t;
#ifndef int32_t
typedef __int32           int32_t;
#endif
#ifndef uint32_t
typedef unsigned __int32  uint32_t;
#endif
#endif	//	_WIN32

#ifndef _WIN32
#include <stdlib.h>
#else
#define atoll(x) _atoi64((x))
#define strtoull(x, e, b) _strtoui64((x), (e), (b))
#endif	//	_WIN32


#include "mysql_private_iface.h"

namespace sql {
namespace mysql {
namespace util {

int mysql_string_type_to_datatype(const std::string & name);
int mysql_type_to_datatype(const MYSQL_FIELD * const field);
const char * mysql_type_to_string(const MYSQL_FIELD * const field);


typedef struct st_our_charset
{
	unsigned int	nr;
	const char		*name;
	const char		*collation;
	unsigned int	char_minlen;
	unsigned int	char_maxlen;
	const char		*comment;
	unsigned int 	(*mb_charlen)(unsigned int c);
	unsigned int 	(*mb_valid)(const char *start, const char *end);
} OUR_CHARSET;

const OUR_CHARSET * find_charset(unsigned int charsetnr);


template<typename T>
class my_shared_ptr
{
public:
	my_shared_ptr(): _ptr(NULL), ref_count(0) { }

	my_shared_ptr(T * __p): ref_count(1), _ptr(__p) { }

	void reset() { delete * _ptr; _ptr = NULL; }
	void reset(T * new_p) { delete * _ptr; _ptr = new_p; }

	T * get() const throw() { return _ptr; }

	my_shared_ptr< T > * getReference() throw() { ++ref_count; return this; }
	void freeReference() {  if (ref_count && !--ref_count) { delete this;} }


protected:
	unsigned int ref_count;
	T * _ptr;

	~my_shared_ptr() { delete _ptr; }
};

}; /* namespace util */
}; /* namespace mysql */
}; /* namespace sql */

#endif /* _MYSQL_UTIL_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
