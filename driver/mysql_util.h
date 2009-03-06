/*
   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_UTIL_H_
#define _MYSQL_UTIL_H_

#include <cppconn/config.h>

#ifndef UL64
#ifdef _WIN32
#define UL64(x) x##ui64
#else
#define UL64(x) x##ULL
#endif
#endif

#ifndef L64
#ifdef _WIN32
#define L64(x) x##i64
#else
#define L64(x) x##LL
#endif
#endif


#ifndef _WIN32
#  include <stdlib.h>
#  ifdef __hpux
#    ifdef _PA_RISC2_0
#      define atoll(__a) atol((__a))
#      define strtoull(__a, __b, __c) strtoul((__a), (__b), (__c))
#    else
#      define atoll(__a) strtoimax((__a), NULL, 10)
#      define strtoull(__a, __b, __c) strtoumax((__a), (__b), (__c))
#    endif
#  endif
#else
#  define atoll(x) _atoi64((x))
#  define strtoull(x, e, b) _strtoui64((x), (e), (b))
#endif	//	_WIN32


#ifndef HAVE_STRTOLD
#define strold(a, b) strtod((a), (b))
#endif

#include "mysql_private_iface.h"

namespace sql {
namespace mysql {
namespace util {

int mysql_string_type_to_datatype(const std::string & name);
int mysql_type_to_datatype(const MYSQL_FIELD * const field);
const char * mysql_type_to_string(const MYSQL_FIELD * const field);

char * utf8_strup(const char * const src, size_t srclen);


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
