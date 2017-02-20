/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C is licensed under the terms of the GPLv2
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

class Data_holder;
typedef struct mysqlx_result_struct mysqlx_result_t;
typedef struct mysqlx_stmt_struct mysqlx_stmt_t;

/*
  Class representing an entry in the list of values, which could be used as
  a parameter list or a row
*/
class Value_item
{
protected:
  mysqlx_data_type_t m_type;

  std::string m_str;
  cdk::bytes m_bytes;

  union {
    double   v_double;
    float    v_float;
    int64_t  v_sint;
    uint64_t v_uint;
    bool     v_bool;
  } m_val;

public:

  // Construct a NULL item
  Value_item() : m_type(MYSQLX_TYPE_NULL)
  {}

  // Construct an item from a string
  Value_item(const std::string& str) : m_type(MYSQLX_TYPE_STRING), m_str(str)
  {}

  // Construct an item from a signed 64-bit integer
  Value_item(int64_t v) : m_type(MYSQLX_TYPE_SINT)
  { m_val.v_sint = v; }

  // Construct an item from an unsigned 64-bit integer
  Value_item(uint64_t v) : m_type(MYSQLX_TYPE_UINT)
  { m_val.v_uint = v; }

  // Construct an item from a float
  Value_item(float v) : m_type(MYSQLX_TYPE_FLOAT)
  { m_val.v_float = v; }

  // Construct an item from a double
  Value_item(double v) : m_type(MYSQLX_TYPE_DOUBLE)
  { m_val.v_double = v; }

  // Construct an item from a bool
  Value_item(bool v) : m_type(MYSQLX_TYPE_BOOL)
  { m_val.v_bool = v; }

  // Construct an item from bytes
  Value_item(const cdk::bytes &b) : m_type(MYSQLX_TYPE_BYTES), m_bytes(b)
  {}

  uint64_t get_uint() const
  {
    if (m_type != MYSQLX_TYPE_UINT)
      throw Mysqlx_exception("Data cannot be converted to unsigned integer number");
    return m_val.v_uint;
  }

  int64_t get_sint() const
  {
    if (m_type != MYSQLX_TYPE_SINT)
      throw Mysqlx_exception("Data cannot be converted to signed integer number");
    return m_val.v_sint;
  }

  float get_float() const
  {
    if (m_type != MYSQLX_TYPE_FLOAT)
      throw Mysqlx_exception("Data cannot be converted to float number");
    return m_val.v_float;
  }

  double get_double() const
  {
    if (m_type != MYSQLX_TYPE_DOUBLE)
      throw Mysqlx_exception("Data cannot be converted to double number");
    return m_val.v_double;
  }

  cdk::bytes get_bytes() const
  {
    if (m_type != MYSQLX_TYPE_BYTES)
      throw Mysqlx_exception("Data cannot be converted to bytes");
    return m_bytes;
  }

  const std::string &get_string() const
  {
    if (m_type != MYSQLX_TYPE_STRING && m_type != MYSQLX_TYPE_EXPR &&
        m_type != MYSQLX_TYPE_JSON)
      throw Mysqlx_exception("Data cannot be converted to string");
    return m_str;
  }

  mysqlx_data_type_t get_type() const
  {
    return m_type;
  }

  void process_any(cdk::Any::Processor &prc) const;
  void process_val(cdk::Value_processor &prc) const;
};

typedef struct mysqlx_row_struct : public Mysqlx_diag
{
private:
  std::vector<Data_holder*> m_row_data;
  mysqlx_result_t &m_result;

public:
  mysqlx_row_struct(mysqlx_result_t &result) : m_result(result)
  {}

  ~mysqlx_row_struct();

  // Clear the data in the current row
  void clear();

  // Return the result the current row belongs to
  mysqlx_result_t &get_result() { return m_result; }

  // Return the number of columns in the current row
  size_t row_size() { return m_row_data.size(); }

  // add an item to the list of row values (each next call is for the next column)
  void add_field_data(cdk::bytes data, size_t full_len);

  // append data to the existing buffer
  void append_field_data(cdk::col_count_t pos, cdk::bytes data);

  // add a null item to the list of row values
  void add_field_null();

  // get data from the column number pos
  cdk::bytes get_col_data(cdk::col_count_t pos);

} mysqlx_row_t;

typedef struct mysqlx_doc_struct : public Mysqlx_diag
{
private:

  class JSON_doc
  : public cdk::JSON::Processor
  , public cdk::JSON::Processor::Any_prc
  , public cdk::JSON::Processor::Any_prc::Scalar_prc

  {
  private:
    typedef std::map<cdk::string, Value_item> Doc_map;

    Doc_map m_map;
    cdk::string m_current_key;

    void clear()
    {
      m_map.clear();
    }

  public:

    JSON_doc(cdk::bytes json_data)
    {
      cdk::Codec<cdk::TYPE_DOCUMENT> codec;
      codec.from_bytes(json_data, *this);
    }

    /*
      Processor functions
    */

    void doc_begin() { clear(); }
    void doc_end() {}
    Any_prc* key_val(const string &key)
    {
      m_current_key = key;
      return this;
    }

    List_prc* arr()
    { return NULL; }

    cdk::JSON::Processor::Any_prc::Doc_prc* doc()
    {
      /* TODO: implement later */
      return this;
    }
    cdk::JSON::Processor::Any_prc::Scalar_prc* scalar()
    {
      return this;
    }

    void str(const cdk::string &val)
    { m_map.insert(std::make_pair(m_current_key, Value_item(val))); }

    void num(uint64_t val)
    { m_map.insert(std::make_pair(m_current_key, val)); }

    void num(int64_t val)
    { m_map.insert(std::make_pair(m_current_key, val)); }

    void num(float val)
    { m_map.insert(std::make_pair(m_current_key, val)); }

    void num(double val)
    { m_map.insert(std::make_pair(m_current_key, val)); }

    void yesno(bool val)
    { m_map.insert(std::make_pair(m_current_key, val)); }

    void null()
    { m_map.insert(std::make_pair(m_current_key, Value_item())); }

    /*
      Service functions exported to the classes that use them
    */
    bool key_exists(const cdk::string key)
    {
      return m_map.find(key) != m_map.end();
    }

    Value_item& get_val(const cdk::string key)
    {
      if (!key_exists(key))
        throw Mysqlx_exception("Key does not exist!");
      return m_map.at(key);
    }

    size_t count()
    { return m_map.size(); }
  };

  cdk::bytes m_bytes;
  JSON_doc m_json_doc;

public:

  mysqlx_doc_struct(cdk::bytes data);

  uint64_t get_uint(const cdk::string key)
  { return m_json_doc.get_val(key).get_uint(); }

  int64_t get_sint(const cdk::string key)
  { return m_json_doc.get_val(key).get_sint(); }

  float get_float(const cdk::string key)
  { return m_json_doc.get_val(key).get_float(); }

  double get_double(const cdk::string key)
  { return m_json_doc.get_val(key).get_double(); }

  cdk::bytes get_bytes(const cdk::string key)
  { return m_json_doc.get_val(key).get_bytes(); }

  cdk::string get_string(const cdk::string key)
  { return m_json_doc.get_val(key).get_string(); }

  mysqlx_data_type_t get_type(const cdk::string key)
  { return m_json_doc.get_val(key).get_type(); }

  bool key_exists(const cdk::string key)
  { return m_json_doc.key_exists(key); }

  size_t count()
  { return m_json_doc.count(); }

} mysqlx_doc_t;

class Expr_to_doc_prc_converter
  : public cdk::Converter<
      Expr_to_doc_prc_converter,
      cdk::Expression::Processor,
      cdk::Expression::Document::Processor
    >
{
  Doc_prc* doc() { return m_proc; }
  Scalar_prc* scalar() { throw Mysqlx_exception("Document expected"); }
  List_prc* arr() { throw Mysqlx_exception("Document expected"); }
};

class Param_item : public Value_item, public cdk::Any
{
public:

  // Default constructor for NULL values
  Param_item() : Value_item() {}

  // Constructor template for other types
  template <typename T> Param_item(T val) : Value_item(val) {}

  void process(cdk::Any::Processor &prc) const;
};

/*
  A class implementing parameters list for parametrized queries
*/
class Param_list : public cdk::Any_list
{
  typedef std::vector<Param_item> Param_item_vec;
  Param_item_vec m_list;
public:

  // Function to add NULL to the list
  void add_null_value()
  {
    m_list.push_back(Param_item());
  }

  // Template function to add different types of items into the list
  template <typename T> void add_param_value(T val)
  {
    m_list.push_back(val);
  }

  // Clear the list
  void clear() { m_list.clear(); }

  uint32_t count() const { return m_list.size(); }

  void process(cdk::Any_list::Processor& prc) const
  {
    prc.list_begin();
    for (Param_item_vec::const_iterator it = m_list.begin();
         it != m_list.end(); ++it)
    {
      it->process_if(prc.list_el());
    }
    prc.list_end();
  }
};

class Param_source : public cdk::Param_source
{
  typedef std::map<string, Param_item> Param_item_map;
  Param_item_map m_map;
public:

  void clear() { m_map.clear(); }

  void add_null_value(const string &key)
  {
    m_map.insert(std::make_pair(key, Param_item()));
  }

  uint32_t count() const { return m_map.size(); }

  template <typename T> void add_param_value(const std::string &key, T val)
  {
    m_map.insert(std::make_pair(key, val));
  }

  void process(cdk::Param_source::Processor &prc) const
  {
    prc.doc_begin();

    for (Param_item_map::const_iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
      it->second.process_if(prc.key_val(it->first));
    }

    prc.doc_end();
  }
};

class Projection_list : public cdk::Projection, public cdk::Expression::Document
{
  typedef std::vector<cdk::string> Proj_vec;
  mysqlx_op_t m_op_type;
  parser::Parser_mode::value m_mode;
  Proj_vec m_list;
public:

  Projection_list(mysqlx_op_t op_type) : m_op_type(op_type)
  {
    switch(m_op_type)
    {
      case OP_SELECT:
        m_mode = parser::Parser_mode::TABLE;
      break;
      case OP_FIND:
        m_mode = parser::Parser_mode::DOCUMENT;
      break;
      default:
        throw Mysqlx_exception("Wrong operation type! " \
                               "Only OP_SELECT and OP_FIND are supported!");
    }
  }

  // Template function to add different types of items into the list
  void add_value(const char *val)
  {
    m_list.push_back(val);
  }

  // Clear the list
  void clear()
  {
    m_list.clear();
  }

  uint32_t count() const
  {
    return m_list.size();
  }

  // Process method for table projections
  void process(cdk::Projection::Processor & prc) const;

  // Process method for document projections
  void process(cdk::Expression::Document::Processor & prc) const;
};

class Row_item : public Value_item
{
  std::string m_uuid;
  bool m_empty_doc;

public:

  // Constructor template for other types
  template <typename T> Row_item(T val) : Value_item(val), m_empty_doc(false) {}

  // Default constructor for NULL values
  Row_item() : Value_item(), m_empty_doc(false) {}

  void process(cdk::Value_processor &prc) const;

  void generate_uuid();
  std::string get_uuid() const { return m_uuid; };

  bool is_empty_doc() const { return m_empty_doc; }
};

class Source_base
{
protected:
  typedef std::vector<Row_item> Row_item_vec;
  typedef std::vector<Row_item_vec> Rows;
  Rows m_rows;
  size_t m_row_num;
public:

  Source_base() : m_row_num(0)
  {}

  void add_new_row()
  {
    m_rows.push_back(Row_item_vec());
  }

  void add_new_doc()
  {
    add_new_row();
  }

  void remove_last_row()
  {
    if (m_rows.size())
      m_rows.erase(m_rows.begin() + m_rows.size() - 1);
  }

  // Clear the list
  void clear() { m_row_num = 0; m_rows.clear(); }
  size_t row_count() const { return m_rows.size(); }
  size_t col_count() const
  {
    if (m_rows.size()) return m_rows[0].size();
    return 0;
  }

  bool do_next();

  void add_null_value()
  {
    m_rows[m_rows.size() - 1].push_back(Row_item());
  }

  // Template function to add different types of items into the list
  template <typename T> void add_row_value(T val)
  {
    // Always add to the last row
    m_rows[m_rows.size() - 1].push_back(val);
  }

};

class Row_source : public Source_base, public cdk::Row_source
{
public:
  Row_source() : Source_base()
  {}

  virtual void process(Processor &prc) const;
  virtual bool next();

};

class Doc_source : public Source_base, public cdk::Doc_source
{
public:
  Doc_source() : Source_base()
  {}

  virtual void process(Processor &prc) const;
  virtual bool next();

  size_t count() { return m_rows.size(); }

  // Template function to add different types of items into the list
  void add_doc_value(const char *json_doc)
  {
    // Always add to the last row
    m_rows[m_rows.size() - 1].push_back(Row_item(cdk::string(json_doc)));
    m_rows[m_rows.size() - 1][0].generate_uuid();
  }

  void copy_doc_ids(std::vector<std::string> &id_list)
  {
    if (m_rows.size() == 0)
      return; // nothing to copy
    
    for (Rows::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it)
    {
      id_list.push_back((*it)[0].get_uuid());
    }
  }
};

class Column_source : public cdk::api::Columns, public cdk::String_list
{
  typedef std::vector<cdk::string> Column_list;
  Column_list m_columns;
  public:

  void add_column(const char *c) { m_columns.push_back(c); }

  void clear() { m_columns.clear(); }

  size_t count() { return m_columns.size(); }

  void process(cdk::api::Columns::Processor& prc) const;
  void process(cdk::String_list::Processor& prc) const;
};

class Modify_spec;
class Update_spec;

class Update_item : public Value_item,
                    public cdk::Expression
{
  protected:

  cdk::string m_field;
  // We need to know if the item has to be treated as expression
  bool m_is_expr;

  public:

  Update_item(cdk::string path) : Value_item(), m_field(path),
                                  m_is_expr(false)
  {}

  /*
    Constructor template for other types
    column - the column name
    is_expr - flag indicating whether the value is expression
    val - value for the item
  */
  template <typename T> Update_item(cdk::string field, bool is_expr,
                                    T val) : Value_item(val),
                                             m_field(field),
                                             m_is_expr(is_expr)
  {}

  /*
    The expressions need special processing, so the item must
    know whether it represents the value or the expression
  */
  bool is_expr() const { return m_is_expr; }

  cdk::string get_expr() const;

  // Processor for the expressions
  void process(cdk::Expression::Processor &prc) const;

  friend class Modify_spec;
  friend class Update_spec;
};

template <class T>
class Update_base : public cdk::Update_spec
{
  protected:
  typedef std::vector<T> Item_vec;
  Item_vec m_items;
  size_t m_item_num;

  public:

  size_t count() { return m_items.size(); }

  void clear() { m_items.clear(); }

  const T* get_cur_item() const { return &m_items[m_item_num - 1]; }

  bool next()
  {
    if (m_item_num >= count())
      return false;
    ++m_item_num;
    return true;
  }

};

class Update_spec : public Update_base<Update_item>
{
public:

  Update_spec()
  { m_item_num = 0; }

  // Add values of various types
  template <typename T> void add_value(cdk::string column, T val)
  {
    m_items.push_back(Update_item(column, false, val));
  }

  // The expression can only be a string
  void add_expr(cdk::string column, cdk::string expr)
  {
    m_items.push_back(Update_item(column, true, expr));
  }

  void add_null_value(cdk::string column);
  virtual void process(Processor &prc) const;
};

class Modify_item : public Update_item
{
  mysqlx_modify_op m_op_type;

  public:

  /*
    This constructor is used for SET(NULL), UNSET and ARRAY_DELETE operations
  */
  Modify_item(mysqlx_modify_op op_type, cdk::string path) :
        Update_item(path), m_op_type(op_type)
  {}

  /*
    Constructor template for other types
    path - the value/document path (can re-use m_column in Update_item)
    is_expr - flag indicating whether the value is expression
    val - value for the item
  */
  template <typename T> Modify_item(mysqlx_modify_op op_type,
                                    cdk::string path,
                                    bool is_expr,
                                    T val) : Update_item(path, is_expr, val),
                                             m_op_type(op_type)
  {}

  mysqlx_modify_op get_op_type() const { return m_op_type; }

  // Processor for the expressions needs to be overriden
  void process(cdk::Expression::Processor &prc) const;
};


class Modify_spec : public Update_base<Modify_item>
{

public:

  Modify_spec()
  { m_item_num = 0; }

  void add_value(mysqlx_modify_op modify_type, cdk::string path)
  {
    m_items.push_back(Modify_item(modify_type, path));
  }

  // Add values of various types
  template <typename T> void add_value(mysqlx_modify_op op_type,
                                       cdk::string path,
                                       bool is_expr,
                                       T val)
  {
    m_items.push_back(Modify_item(op_type, path, is_expr, val));
  }

  void add_null_value(mysqlx_modify_op op_type, cdk::string path)
  {
    m_items.push_back(Modify_item(op_type, path));
  }

  virtual void process(Processor &prc) const;
};
