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

#include "mysql_private_iface.h"
#include <string>

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
#  ifndef HAVE_STRTOLL
#    ifdef HAVE_STRTOL
#      define strtoll(__a, __b, __c) strtol((__a), (__b), (__c))
#    else
#      ifdef HAVE_STRTOIMAX
#        define strtoll(__a, __b, __c) strtoimax((__a), NULL, 10)
#      else
#        error "Compilation will fail because code does not know an equivalent of strtol/strtoll"
#      endif
#    endif
#    define HAVE_STRTOLL 1
#  endif
#  ifndef HAVE_STRTOULL
#    ifdef HAVE_STRTOUL
#      define strtoull(__a, __b, __c) strtoul((__a), (__b), (__c))
#    else
#      ifdef HAVE_STRTOUMAX
#        define strtoull(__a, __b, __c) strtoumax((__a), NULL, 10)
#      else
#        error Compilation will fail because code does not know an equivalent of strtoul/strtoull
#      endif
#    endif
#    define HAVE_STRTOULL 1
#  endif
#else
#  define strtoll(x, e, b) _strtoi64((x), (e), (b))
#  define strtoull(x, e, b) _strtoui64((x), (e), (b))
#endif	//	_WIN32


namespace sql {
namespace mysql {
namespace util {

int mysql_string_type_to_datatype(const std::string & name);
int mysql_type_to_datatype(const MYSQL_FIELD * const field);
const char * mysql_type_to_string(const MYSQL_FIELD * const field);

char * utf8_strup(const char * const src, size_t srclen);

long double strtold(const char *nptr, char **endptr);

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


template<typename T>
struct my_array_guard
{
	T * ptr;
	my_array_guard(T * p) : ptr(p) {}
	T * get() { return ptr; }
	~my_array_guard() { delete [] ptr; }
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
