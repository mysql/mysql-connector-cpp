/*
Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.

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



#ifndef _SQL_VARIANT_H_
#define _SQL_VARIANT_H_

#include <string>
#include <map>
#include <algorithm>
#include <typeinfo>

#include "build_config.h"
#include "sqlstring.h"
#include "exception.h"

namespace sql
{

class BaseVariantImpl
{
public:
	BaseVariantImpl (void *ptr, sql::SQLString vtype) :
		cvptr(ptr),
		vTypeName(vtype)
	{}

	virtual ~BaseVariantImpl() {
		cvptr= NULL;
	}

	virtual BaseVariantImpl* Clone() = 0;

	template<class T>
	T* get() const {
		if (typeid(T).name() == typeid(void).name()) {
			return static_cast< T * > (cvptr);
		}

		if ((vTypeName == typeid(std::string).name() &&
			typeid(T).name() == typeid(sql::SQLString).name()) ||
			(vTypeName == typeid(sql::SQLString).name() &&
			typeid(T).name() == typeid(std::string).name())    ||
			(vTypeName == typeid(std::map< std::string, std::string >).name() &&
			typeid(T).name() ==
				typeid(std::map< sql::SQLString, sql::SQLString >).name()) ||
			(vTypeName ==
				typeid(std::map< sql::SQLString, sql::SQLString >).name() &&
			typeid(T).name() ==
				typeid(std::map< std::string, std::string >).name()))
		{
			return static_cast< T * > (cvptr);
		}

		if (typeid(T).name() != vTypeName) {
			throw sql::InvalidArgumentException("Variant type doesn't match.");
		}

		return static_cast< T * > (cvptr);
    }

protected:
	void *cvptr;
	sql::SQLString vTypeName;
};


template<class T>
class  VariantImpl : public BaseVariantImpl
{
public:
	VariantImpl(T i) : BaseVariantImpl(new T(i), typeid(i).name()) {}

	~VariantImpl() {
		destroy_content();
	}

	VariantImpl(VariantImpl& that) : BaseVariantImpl(that) {
		copy_content(that);
	}

	VariantImpl& operator=(VariantImpl& that) {
		if (this != &that) {
			destroy_content();
			if (cvptr == NULL) {
				copy_content(that);
			}
		}
		return *this;
	}

	virtual BaseVariantImpl* Clone() {
		return new VariantImpl(*this);
	}

private:

	void destroy_content() {
		T *tmp= static_cast< T * >(cvptr);
		if (tmp) {
			delete tmp;
			cvptr= NULL;
		}
	}

	void copy_content(BaseVariantImpl& that) {
		cvptr= new T (*(static_cast< T * > (that.get< void >())));
	}
};


template<class T>
class  VariantMap : public VariantImpl<T>
{
public:
	VariantMap(T i) : VariantImpl<T>(i) {}

	~VariantMap() {
		destroy_content();
	}

	VariantMap(VariantMap& that) :
		VariantImpl<T>(that)
	{
		copy_content(that);
	}

	VariantMap& operator=(VariantMap& that) {
		if (this != &that) {
			destroy_content();
			copy_content(that);
		}
		return *this;
	}

	virtual BaseVariantImpl* Clone() {
		return new VariantMap(*this);
	}


private:
	void destroy_content()
	{
		void *cvptr= BaseVariantImpl::get< void >();
		T *tmp=  static_cast< T *> (cvptr);
		if (tmp) {
			tmp->erase(tmp->begin(), tmp->end());
		}
	}

	void copy_content(VariantMap& var)
	{
		void *cvptr= BaseVariantImpl::get< void >();
		T *tmp=  static_cast< T *> (var.BaseVariantImpl::get< void >());
		if (tmp) {
			cvptr= new T();
			typename T::const_iterator cit = tmp->begin();
			while(cit != tmp->end()) {
				std::pair< sql::SQLString, sql::SQLString >
							map_pair (cit->first, cit->second);
				(static_cast< T * >(cvptr))->insert(map_pair);
				++cit;
			}
		}
	}
};


class CPPCONN_PUBLIC_FUNC Variant
{
public:
	Variant(const int &i = 0) :
		variant(new VariantImpl< int >(i)) {}

	Variant(const double &i) :
		variant(new VariantImpl< double >(i)) {}

	Variant(const bool &i) :
		variant(new VariantImpl< bool >(i)) {}

	Variant(const std::string &i) :
		variant(new VariantImpl< std::string >(i)) {}

	Variant(const sql::SQLString &i) :
		variant(new VariantImpl< sql::SQLString >(i)) {}

	Variant(const std::map< std::string, std::string > &i) :
		variant(new VariantMap< std::map< std::string, std::string > >(i)) {}

	Variant(const std::map< sql::SQLString, sql::SQLString > &i) :
		variant(new VariantMap< std::map< sql::SQLString, sql::SQLString > >(i)) {}

	~Variant() {
		if (variant) {
			delete variant;
			variant= 0;
		}
	}

	Variant(const Variant& that) {
		variant= that.variant->Clone();
	}

	Variant& operator=(const Variant& that) {
		if (this != &that) {
			delete variant;
			variant= that.variant->Clone();
		}
		return *this;
	}

	template<class T>
	T* get() const {
		return variant->get<T>();
	}

private:
	BaseVariantImpl *variant;
};


} /* namespace sql */

#endif /* _SQL_VARIANT_H_ */
