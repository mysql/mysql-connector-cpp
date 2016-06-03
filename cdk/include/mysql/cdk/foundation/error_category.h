/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * This code is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FLOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef CDK_FOUNDATION_ERROR_CATEGORY_H
#define CDK_FOUNDATION_ERROR_CATEGORY_H

#include "types.h"

namespace cdk {
namespace foundation {

class error_condition;

/*
  Class defining a named category of error codes. Such category defines
  descriptions for error codes in the category as well as mapping to
  platform-independent error conditions.
*/

class error_category : nocopy
{
public:
  virtual const char* name() const =0;
  virtual std::string message(int) const =0;
  virtual error_condition default_error_condition(int) const =0;
  virtual bool  equivalent(int, const error_condition&) const =0;

  virtual bool operator==(const error_category &ec) const
  {
    // assumes singleton instances of each category
    return this == &ec;
  }

  virtual bool operator !=(const error_category &ec) const
  {
    return !(*this == ec);
  }
};



/*
  Pre-defined error categories. See "Error codes and categories" in
  doc/designs/errors.rst
*/

const error_category& generic_error_category();
const error_category& system_error_category();
const error_category& std_error_category();
const error_category& posix_error_category();


}}  // cdk::foundation


/*
  Infrastructure for defining new error categories
  ================================================

  See "Defining error category" section in doc/designs/errors.rst.

*/

// Process error declaration using X
#define CDK_ERROR(X,C,N,M) X (C,N,M)


/*
  Generate definition of a structure holding enumeration with error
  code values.
*/

#define CDK_ERROR_ENUM(C,N,M) C=N,

#define CDK_ERROR_CODES(EC,NS) \
  struct NS                             \
  {                                     \
    enum code {                         \
      no_error = 0,                     \
      EC_##EC##_ERRORS(CDK_ERROR_ENUM)  \
    };                                  \
  }


#define CDK_ERROR_CASE(NS)        case NS:: CDK_ERROR_CASE1
#define CDK_ERROR_CASE1(C,N,M) C: return std::string(M);

/*
  Generate switch() statement which returns error description
  corresponding to given error code.
*/

#define CDK_ERROR_SWITCH(NS,EC,code) \
  switch (code)                           \
  {                                       \
    case 0: return "No error";            \
    EC_##EC##_ERRORS(CDK_ERROR_CASE(NS))  \
    default: return "Unknown error";      \
  }

/*
  Generate definition of error category.

  First a structure with error codes is defined using CDK_ERROR_CODES.
  Then a cdk_NNN_error_category class is defined, where NNN is the name
  of the new category, which is a specialization of error_category class
  that defines category name ("cdk-NNN") and descriptions for error
  codes (method message()).

  Then inline functions cdk_NNN_category() and cdk_NNN_error() are
  defined.
*/

#define CDK_ERROR_CATEGORY(EC,NS) \
  CDK_ERROR_CODES(EC,NS);                                                 \
  struct error_category_##EC : public cdk::foundation::error_category     \
  {                                                                       \
    error_category_##EC() {}                                              \
    const char* name() const throw() { return "cdk-" #EC; }               \
    std::string message(int code) const                                   \
    { CDK_ERROR_SWITCH(NS, EC, code); }                                   \
    cdk::foundation::error_condition default_error_condition(int) const;  \
    bool  equivalent(int, const cdk::foundation::error_condition&) const; \
  };                                                                      \
  inline const cdk::foundation::error_category& EC##_error_category()     \
  { static const error_category_##EC instance;                            \
    return instance; }                                                    \
  inline cdk::foundation::error_code EC##_error(int code)                 \
  { return cdk::foundation::error_code(code, EC##_error_category()); }

#endif
