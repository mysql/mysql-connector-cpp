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

#ifndef CDK_API_OBJ_REF_H
#define CDK_API_OBJ_REF_H

#include "../foundation.h" // for string

namespace cdk {
namespace api {

/*
  Classes for describing database object references of the form:

   [[<catalog>.<schema>.]<table>.]<column>

*/

class Ref_base
{
public:

  virtual ~Ref_base() {}

  virtual const string name() const =0;
  virtual const string orig_name() const { return name(); }
};


class Schema_ref
    : public Ref_base
{
public:

  virtual const Ref_base* catalog() const { return NULL; }
};

class Object_ref
    : public Ref_base
{
public:
  virtual const Schema_ref* schema() const =0;
};


typedef Object_ref Table_ref;


class Column_ref
    : public Ref_base
{
public:
  virtual const Table_ref* table() const =0;
};


}}  // cdk::api


#endif
