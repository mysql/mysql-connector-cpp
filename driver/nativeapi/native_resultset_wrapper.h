/*
Copyright (c) 2009, 2011, Oracle and/or its affiliates. All rights reserved.

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



#ifndef _NATIVE_RESULTSET_WRAPPER_H_
#define _NATIVE_RESULTSET_WRAPPER_H_

#include <boost/noncopyable.hpp>
#include <config.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif


struct st_mysql_field;

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

class NativeResultsetWrapper : public boost::noncopyable
{
public:
	virtual ~NativeResultsetWrapper(){}

	virtual void data_seek(uint64_t) = 0;

	virtual ::st_mysql_field * fetch_field() = 0;

	virtual ::st_mysql_field * fetch_field_direct(unsigned int) = 0;

	virtual unsigned long * fetch_lengths() = 0;

	virtual char** fetch_row() = 0;

	virtual unsigned int num_fields() = 0;

	virtual uint64_t num_rows() = 0;
};

} /* namespace NativeAPI*/
} /* namespace mysql */
} /* namespace sql */

#endif // _NATIVE_RESULTSET_WRAPPER_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
