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

namespace sql {
namespace mysql {
namespace util {

int mysql_to_datatype(int mysql_type, int flags);

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
