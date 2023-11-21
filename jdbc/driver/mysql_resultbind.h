/*
 * Copyright (c) 2008, 2023, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */



#ifndef _MYSQL_RESULTBIND_H_
#define _MYSQL_RESULTBIND_H_

#include <memory>
#include <vector>
#include <cassert>

#include <cppconn/prepared_statement.h>
#include <cppconn/parameter_metadata.h>

#include "nativeapi/mysql_private_iface.h"
#include "mysql_util.h"

namespace sql
{
namespace mysql
{
namespace NativeAPI
{
class NativeStatementWrapper;
}

struct MySQL_Bind : public MYSQL_BIND
{
  MySQL_Bind();
  MySQL_Bind(MySQL_Bind&&);
  MySQL_Bind(const MySQL_Bind&) = delete;
  ~MySQL_Bind();


  void setBigInt(const sql::SQLString& value);
  void setBlob(std::istream * blob);
  void setBoolean(bool value);
  void setDateTime(const sql::SQLString& value);
  void setDouble(double value);
  void setInt(int32_t value);
  void setUInt(uint32_t value);
  void setInt64(int64_t value);
  void setUInt64(uint64_t value);
  void setNull();
  void setString(const sql::SQLString& value);

  void clear();
};

class MySQL_ResultBind
{
  unsigned int num_fields;
  std::unique_ptr<my_bool[]> is_null;
  std::unique_ptr<my_bool[]> err;
  std::unique_ptr<unsigned long[]> len;

  std::shared_ptr<NativeAPI::NativeStatementWrapper> proxy;

  std::shared_ptr<MySQL_DebugLogger> logger;

 public:
  std::unique_ptr<MYSQL_BIND[]> rbind;

  MySQL_ResultBind(std::shared_ptr<NativeAPI::NativeStatementWrapper> &_capi,
                   std::shared_ptr<MySQL_DebugLogger> &log);

  ~MySQL_ResultBind();

  void bindResult();

};


/*
  Class to store attribute/parameter names.

  Together with attribute name the status of its value is stored: either
  no value (UNSET) or INTERNAL/EXTERNAL value. The internal/external status
  is used for implementing logic that prevents overwriting external attribute
  values (e.g. set by user) by internal ones (e.g. set by telemetry logic),
  see `MySQL_AttributesBind`.

  Note: New names are added to the list by `getNamePos(name, is_external)`
  method. Once added, the name at given postion can not be changed.
*/

class MySQL_Names
{
protected:

  typedef enum
  {
    UNSET = 0,
    INTERNAL,
    EXTERNAL
  }
  set_type;

public:

  MySQL_Names()
  {}

  const char **getNames();
  size_t size();
  void unset(unsigned int position);
  bool isSet(unsigned int position);
  bool isInternal(unsigned int position);
  bool isExternal(unsigned int position);

  /*
    Return 0-based position of the given name, or -1 if the name is not found.
  */
    
  int getNamePos(const sql::SQLString &name)
  {
    set_type tmp = UNSET;
    return (long)get_name_pos(name, tmp) - 1;
  }

  /*
    Return 0-based position of the given name, adding it to the list
    if needed.

    The in-out parameter `is_external` reports the internal/external status
    of the corresponding attribute and allows upgrading that status
    to "external". See `MySQL_AttributesBing::getBindPos()`.
  */

  unsigned int getNamePos(const sql::SQLString &name, bool &is_external)
  {
    set_type type = is_external ? EXTERNAL : INTERNAL;
    unsigned pos = get_name_pos(name, type);
    assert(pos > 0);
    if (UNSET != type)
      is_external = (type == EXTERNAL);
    return pos-1;
  }

  void clearNames();

 private:

  /*
    Returns 1-based position if given name was found, 0 if not found.

    If the input value of in-out parameter `type` is different from UNSET
    and the name is not found, the name is placed in the first empty slot
    or in a new slot if all slots are occupied (thus non-zero position
    is returned). Also, in this case status of the name slot can be upgraded
    as follows:
    
      current status  status given by `type`
      ==============  ======================
      INTERNAL    --> EXTERNAL
      UNSET       --> INTERNAL
      UNSET       --> EXTERNAL

    For other combinations the current status remains unchanged.    
    
    The output value of `type` is the status of the name slot that was found
    or created (after possible modification).
  */

  unsigned get_name_pos(const sql::SQLString &name, set_type &type);

  // Storage for attribute names.
  std::vector<sql::SQLString> s_names;

  // Storage for attribute value status.
  std::vector<set_type> name_set_type;

  // This is used to return names as array of `const char*` pointers.
  std::vector<const char*> names;
};


/*
  This class stores a list of named query attributes.

  Attribute values are stroed in an array of `MYSQL_BIND` structures returned
  by `getBinds()` method. The correpsonding array of `const char*` pointers
  to attribute names is returned by `getNames()` method.
  
  Attribute values can be marked as "external" or "internal". An internal
  value can not overwrite an external value that was set earlier for
  the attribute (but it can overwrite an earlier internal value).
*/

class MySQL_AttributesBind : private MySQL_Names
{
 public:

  MySQL_AttributesBind();
  ~MySQL_AttributesBind();

  /*
    The `setQueryAttrXXX()` methods either add an attribute with given name
    and value or modify value of an existing attribute with the given name.
    The `is_external` parameter tells if internal or external value is being
    set. Internal value can not overwrite existing external value and will
    be ignored.

    These methods return 1-based position of the attribute that was set or modified.
  */

  int setQueryAttrBigInt(const sql::SQLString &name, const sql::SQLString& value, bool is_external = true);

  int setQueryAttrBoolean(const sql::SQLString &name, bool value, bool is_external = true);

  int setQueryAttrDateTime(const sql::SQLString &name, const sql::SQLString& value, bool is_external = true);

  int setQueryAttrDouble(const sql::SQLString &name, double value, bool is_external = true);

  int setQueryAttrInt(const sql::SQLString &name, int32_t value, bool is_external = true);

  int setQueryAttrUInt(const sql::SQLString &name, uint32_t value, bool is_external = true);

  int setQueryAttrInt64(const sql::SQLString &name, int64_t value, bool is_external = true);

  int setQueryAttrUInt64(const sql::SQLString &name, uint64_t value, bool is_external = true);

  int setQueryAttrNull(const SQLString &name, bool is_external = true);

  int setQueryAttrString(const sql::SQLString &name, const sql::SQLString& value, bool is_external = true);

  void clearAttributes();

  // Number of attributes that were set.

  size_t size();

  // Returns array of pointers to attribute names

  using MySQL_Names::getNames;

  // Returns array of MYSQL_BIND structures with attribute values.

  MYSQL_BIND* getBinds();

 private:

  // Storage for attribute values.

  std::vector<MySQL_Bind> bind;

  /*
    Return 0-based position of attribute with given name. If no such
    attribute exists a new slot is added for it.

    The in-out parameter `is_external` returns the external/internal status
    of the attribute. If new attribute is added the value of 'is_external'
    determines its initial status.
    
    If input value of 'is_external' is true and existing internal attribute
    was found, the attribute is upgraded to external one. On the other hand,
    if input value of `is_external` is false, and existing external attribute
    was found, the attribute remains external and the output value
    of `is_external` is true.
  */

  unsigned get_bind_pos(const SQLString &name, bool &is_external);

  // Like getBindPos(name, is_external) with `is_external` set to true.

  unsigned get_bind_pos(const SQLString &name)
  {
    bool dummy = true;
    return get_bind_pos(name, dummy);
  }
};

} /* namespace mysql */
} /* namespace sql */

#endif /* _MYSQL_RESULTBIND_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

