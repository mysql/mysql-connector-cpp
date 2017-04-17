/*
Copyright (c) 2008, 2012, Oracle and/or its affiliates. All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPLv2
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPLv2 as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/



#ifndef _MYSQL_UTIL_H_
#define _MYSQL_UTIL_H_

#include "nativeapi/mysql_private_iface.h"
#include <cppconn/config.h>
#include <cppconn/sqlstring.h>
#include <cppconn/exception.h>
#include <boost/shared_ptr.hpp>
#include <sstream>


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

#define NULLCSTR static_cast<char *>(0)

#ifndef _WIN32
#  ifndef HAVE_FUNCTION_STRTOLL
#    define strtoll(__a, __b, __c)  static_cast<long long>(sql::mysql::util::strtold((__a), NULL))
#    define HAVE_FUNCTION_STRTOLL 1
#  endif
#  ifndef HAVE_FUNCTION_STRTOULL
#    define strtoull(__a, __b, __c)  static_cast<unsigned long long>(sql::mysql::util::strtold((__a), NULL))
#    define HAVE_FUNCTION_STRTOULL 1
#  endif
#else
# ifndef strtoll
#  define strtoll(x, e, b) _strtoi64((x), (e), (b))
# endif
# ifndef strtoull
#  define strtoull(x, e, b) _strtoui64((x), (e), (b))
# endif
#endif	//	_WIN32


#define bit_uint1korr(A)	(*(((uint8_t*)(A))))

#define bit_uint2korr(A) ((uint16_t) (((uint16_t) (((unsigned char*) (A))[1])) +\
                                   ((uint16_t) (((unsigned char*) (A))[0]) << 8)))
#define bit_uint3korr(A) ((uint32_t) (((uint32_t) (((unsigned char*) (A))[2])) +\
                                   (((uint32_t) (((unsigned char*) (A))[1])) << 8) +\
                                   (((uint32_t) (((unsigned char*) (A))[0])) << 16)))
#define bit_uint4korr(A) ((uint32_t) (((uint32_t) (((unsigned char*) (A))[3])) +\
                                   (((uint32_t) (((unsigned char*) (A))[2])) << 8) +\
                                   (((uint32_t) (((unsigned char*) (A))[1])) << 16) +\
                                   (((uint32_t) (((unsigned char*) (A))[0])) << 24)))
#define bit_uint5korr(A) ((uint64_t)(((uint32_t) (((unsigned char*) (A))[4])) +\
                                    (((uint32_t) (((unsigned char*) (A))[3])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[2])) << 16) +\
                                   (((uint32_t) (((unsigned char*) (A))[1])) << 24)) +\
                                    (((uint64_t) (((unsigned char*) (A))[0])) << 32))
#define bit_uint6korr(A) ((uint64_t)(((uint32_t) (((unsigned char*) (A))[5])) +\
                                    (((uint32_t) (((unsigned char*) (A))[4])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[3])) << 16) +\
                                    (((uint32_t) (((unsigned char*) (A))[2])) << 24)) +\
                        (((uint64_t) (((uint32_t) (((unsigned char*) (A))[1])) +\
                                    (((uint32_t) (((unsigned char*) (A))[0]) << 8)))) <<\
                                     32))
#define bit_uint7korr(A) ((uint64_t)(((uint32_t) (((unsigned char*) (A))[6])) +\
                                    (((uint32_t) (((unsigned char*) (A))[5])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[4])) << 16) +\
                                   (((uint32_t) (((unsigned char*) (A))[3])) << 24)) +\
                        (((uint64_t) (((uint32_t) (((unsigned char*) (A))[2])) +\
                                    (((uint32_t) (((unsigned char*) (A))[1])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[0])) << 16))) <<\
                                     32))
#define bit_uint8korr(A) ((uint64_t)(((uint32_t) (((unsigned char*) (A))[7])) +\
                                    (((uint32_t) (((unsigned char*) (A))[6])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[5])) << 16) +\
                                    (((uint32_t) (((unsigned char*) (A))[4])) << 24)) +\
                        (((uint64_t) (((uint32_t) (((unsigned char*) (A))[3])) +\
                                    (((uint32_t) (((unsigned char*) (A))[2])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[1])) << 16) +\
                                    (((uint32_t) (((unsigned char*) (A))[0])) << 24))) <<\
                                    32))

namespace sql
{
namespace mysql
{

class MySQL_DebugLogger;

namespace NativeAPI
{
class NativeConnectionWrapper;
class NativeStatementWrapper;
} /* namespace NativeAPI */


namespace util {

extern const char *EMPTYSTR;
extern const char *LOCALHOST;

void throwSQLException(::sql::mysql::NativeAPI::NativeConnectionWrapper & proxy);
void throwSQLException(::sql::mysql::NativeAPI::NativeStatementWrapper & proxy);

int mysql_string_type_to_datatype(const sql::SQLString & name);
int mysql_type_to_datatype(const MYSQL_FIELD * const field);
const char * mysql_type_to_string(const MYSQL_FIELD * const field, boost::shared_ptr< sql::mysql::MySQL_DebugLogger > & l);

char * utf8_strup(const char * const src, size_t srclen);

long double strtold(const char *nptr, char **endptr);

/*
  Generic string to number conversion function template.

  Retrurns numeric value after converting given string in a given base,
  which should be either 10, 16 or 8. Throws error if the whole string
  could not be converted to a number.

  Unlike strtod() and friends, this function does not depend on the current
  locale setting but always uses the "C" locale (so that, e.g., decimal point
  character is always '.').
*/

template<
  typename Num_t
>
inline
Num_t strtonum(const std::string &str, int radix = 10)
{
  typedef std::istreambuf_iterator<char> iter_t;
  static std::locale c_locale("C");
  static const std::num_get<char> &cvt
    = std::use_facet<std::num_get<char> >(c_locale);

  std::istringstream inp(str);
  Num_t val;

  inp.imbue(c_locale);

  switch (radix) {
  case 10: inp.setf(std::ios_base::dec, std::ios_base::basefield); break;
  case 16: inp.setf(std::ios_base::hex, std::ios_base::basefield); break;
  case  8: inp.setf(std::ios_base::oct, std::ios_base::basefield); break;
  default:
    inp.setf(std::ios_base::fmtflags(0), std::ios_base::basefield);
    break;
  }

  /*
    Note: We could use istream::operator>>() to do conversion, but then
    there are problems with detecting conversion errors on some platforms
    (OSX). For that reason we instead use a number conversion facet directly.
    This gives direct access to the error information.
  */

  iter_t beg(inp), end;
  std::ios::iostate err = std::ios_base::goodbit;

  iter_t last = cvt.get(beg, end, inp, err, val);

  class NumericConversion
  {
    std::string m_inp;

  public:
    NumericConversion(const std::string &inp)
      : m_inp(inp)
    {}

    std::string msg()
    {
      std::string msg("Not all characters consumed when converting string '");
      msg.append(m_inp);
      msg.append("' to a number");
      return msg;
    }


  };

  if (std::ios_base::goodbit != err && std::ios_base::eofbit != err)
  {
    throw NumericConversionException(NumericConversion(str).msg());
  }

  if (last != end)
    throw NumericConversionException(NumericConversion(str).msg());

  return val;
}

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


} /* namespace util */
} /* namespace mysql */
} /* namespace sql */

#endif /* _MYSQL_UTIL_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
