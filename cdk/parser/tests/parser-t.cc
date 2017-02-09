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

#include <gtest/gtest.h>
#include "../json_parser.h"
#include "../expr_parser.h"
#include "../uri_parser.h"

#include <cstdarg>  // va_arg()

/*
  TODO:
  - checking document structure with assertions
  - more sample JSON documents
*/

using namespace ::std;
using namespace ::parser;
using cdk::string;
using cdk::JSON;


struct Printer_base
{
  ostream &m_out;
  unsigned m_indent;

  Printer_base(ostream &out, unsigned ind =0)
    : m_out(out), m_indent(ind)
  {}

  ostream& out_ind()
  {
    for (unsigned i=0; i < 2*m_indent; ++i)
      m_out.put(' ');
    return m_out;
  }
};



class JSON_printer
  : public JSON::Processor
{
  Printer_base m_pb;
  //cdk::string m_op_name;

public:

  // TODO: Warning when initializiding with *this

  JSON_printer(ostream &out, unsigned ind =0)
    : m_pb(out, ind), m_any_printer(*this)
  {}

  ostream& out_key(const cdk::string &key)
  {
    return m_pb.out_ind() <<key <<": ";
  }

  struct Scalar_printer : public Any_prc::Scalar_prc
  {
    ostream &m_out;

    Scalar_printer(ostream &out)
      : m_out(out)
    {}

    void null() { m_out << "null" << endl; }
    void str(const cdk::string &val) { m_out <<val <<endl; }
    void num(uint64_t val) { m_out <<val <<endl; }
    void num(int64_t val) { m_out <<val <<endl; }
    void num(float val) { m_out <<val <<endl; }
    void num(double val) { m_out <<val <<endl; }
    void yesno(bool val) { m_out <<(val ? "true" : "false") <<endl; }
  };

  struct Any_printer
    : public Any_prc
    , public JSON::Processor
    , public JSON::List::Processor
  {
    JSON_printer    &m_parent;
    Scalar_printer  m_scalar_printer;

    Any_printer(JSON_printer &parent)
      : m_parent(parent)
      , m_scalar_printer(parent.m_pb.m_out)
    {}

    Scalar_prc* scalar()
    {
      return &m_scalar_printer;
    }

    Doc_prc *doc()
    { return this; }

    List_prc *arr()
    {
      return this;
    }

    // printing list

    void list_begin()
    {
      m_parent.m_pb.m_out << "<array>" <<endl;
      m_parent.m_pb.m_indent++;
      m_parent.m_pb.out_ind() << "[" <<endl;
      m_parent.m_pb.m_indent++;
    }

    void list_end()
    {
      m_parent.m_pb.m_indent--;
      m_parent.m_pb.out_ind() << "]" <<endl;
      m_parent.m_pb.m_indent--;
    }

    Any_prc* list_el()
    {
      m_parent.m_pb.out_ind();
      return this;
    }

    // printing sub-document

    void doc_begin()
    {
      m_parent.m_pb.m_out << "<document>" <<endl;
      m_parent.m_pb.m_indent++;
      m_parent.doc_begin();
    }

    void doc_end()
    {
      m_parent.doc_end();
      m_parent.m_pb.m_indent--;
    }

    Any_prc* key_val(const string &key)
    {
      m_parent.out_key(key);
      return this;
    }
  }
  m_any_printer;

  // JSON processor

  void doc_begin()
  {
    m_pb.out_ind() <<"{" <<endl;
    m_pb.m_indent++;
  }

  void doc_end()
  {
    m_pb.m_indent--;
    m_pb.out_ind() <<"}" <<endl;
  }

  Any_prc* key_val(const cdk::string &key)
  {
    out_key(key);
    return &m_any_printer;
  }

};


const wchar_t *docs[] =
{
  L"{'str': 'foo', 'num': 123, 'bool': true, 'float': 12.4}",
  L"{'str': 'bar', 'doc': {'str': 'foo', 'num': -123, 'bool': true}}",
  L"{'str': 'bar', 'arr': ['foo', 123, true, -12.4, {'str': 'foo', 'num': 123, 'bool': true}] }",
  L"{'null': null }"
};


TEST(Parser, json)
{
  JSON_printer printer(cout, 0);

  for (unsigned i=0; i < sizeof(docs)/sizeof(wchar_t*); i++)
  {
    cout <<endl <<"== doc#" <<i <<" ==" <<endl<<endl;
    cdk::string doc(docs[i]);
    JSON_parser parser(doc);
    parser.process(printer);
  }

  // negative tests

  {
    JSON_parser parser("");
    EXPECT_THROW(parser.process(printer),cdk::Error);
    cout <<"Expected error when parsing empty sting" <<endl;
  }

  {
    const char *json = "invalid";
    JSON_parser parser(json);
    EXPECT_THROW(parser.process(printer),cdk::Error);
    cout <<"Expected error when parsing invalid sting: " << json
         <<endl;
  }

  {
    const char *json = "{ foo: 123, invalid }";
    JSON_parser parser(json);
    EXPECT_THROW(parser.process(printer),cdk::Error);
    cout <<"Expected error when parsing invalid sting: " << json
         <<endl;
  }

  // numeric tests

  static struct num_doc_t {
    const wchar_t *doc;
    double  val;
  }
  num_docs[] = {
    { L"{'float': -123E-1  }", -123E-1 },
    { L"{'float': +12.3e-1  }", +12.3E-1 },
    { L"{'float': -12.3E+1  }", -12.3E+1 },
    { L"{'float': +123e+1  }",  +123E+1 },
    { L"{'float': +.123E+1  }",  +.123E+1 },
    { L"{'float': -.123e-1  }",  -.123E-1 },
  };

  struct : public JSON::Processor
         , public JSON::Processor::Any_prc
         , public JSON::Processor::Any_prc::Scalar_prc
  {
    double m_val;

    // Scalar processor

    void null() { assert(false && "unexpected null value"); }
    void str(const cdk::string &val) { assert(false && "unexpected string value"); }
    void num(uint64_t val) { assert(false && "unexpected uint value"); }
    void num(int64_t val) { assert(false && "unexpeted int value"); }
    void num(float val) { m_val = val; }
    void num(double val) { m_val = val; }
    void yesno(bool val) { assert(false && "unexpected bool value"); }

    // Any processor

    Scalar_prc* scalar()
    {
      return this;
    }

    Doc_prc *doc()
    {
      assert(false && "Unexpected document field value");
      return NULL;
    }

    List_prc *arr()
    {
      assert(false && "Unexpected array field value");
      return NULL;
    }

    // JSON processor

    void doc_begin() {}
    void doc_end() { cout <<"- value: " << m_val << endl; }
    Any_prc* key_val(const cdk::string &key)
    {
      return this;
    }
  }
  checker;

  for (unsigned i=0; i < sizeof(num_docs)/sizeof(num_doc_t); i++)
  {
    cout <<endl <<"== num#" <<i <<" ==" <<endl<<endl;
    cdk::string doc(num_docs[i].doc);
    JSON_parser parser(doc);
    parser.process(checker);
    EXPECT_EQ(num_docs[i].val, checker.m_val);
  }

}



class Expr_printer
  : public cdk::Expression::Processor
{
  Printer_base m_pb;

public:

  Expr_printer(ostream &out, unsigned ind =0)
    : m_pb(out, ind), m_scalar_printer(*this)
  {}

  // Expr processor

  struct Val_printer : public Scalar_prc::Value_prc
  {
    Printer_base &m_pb;

    Val_printer(Printer_base &pb) : m_pb(pb)
    {}

    virtual void null()
    {
      m_pb.out_ind() <<"<null>" <<endl;
    }

    virtual void str(const cdk::string &val)
    {
      m_pb.out_ind() <<"\"" <<val <<"\"" <<endl;
    }

    virtual void num(int64_t val)
    {
      m_pb.out_ind() <<val <<endl;
    }

    virtual void num(uint64_t val)
    {
      m_pb.out_ind() <<"U" <<val <<endl;
    }

    virtual void num(float val)
    {
      m_pb.out_ind() <<"F" <<val <<endl;
    }

    virtual void num(double val)
    {
      m_pb.out_ind() <<"D" <<val <<endl;
    }

    virtual void yesno(bool val)
    {
      m_pb.out_ind() <<(val ? "TRUE" : "FALSE" ) <<endl;
    }

    void value(cdk::Type_info ti,const cdk::Format_info &fi,
               cdk::foundation::bytes data)
    {
      m_pb.out_ind() <<"<value of type " <<(unsigned)ti <<">" <<endl;
    }

  };

  struct Path_printer
    : public cdk::api::Doc_path::Processor
    , cdk::api::Doc_path_processor
  {
    ostream &m_out;
    bool     m_first;

    Path_printer(ostream &out)
      : m_out(out), m_first(true)
    {}

    void list_begin()
    {
      m_first = true;
    }

    Element_prc* list_el()
    {
      return this;
    }

    void member(const string &name)
    {
      if (!m_first)
        m_out << ".";
      m_first = false;
      m_out << name;
    }

    void any_member()
    {
      if (!m_first)
        m_out << ".";
      m_first = false;
      m_out << "*";
    }

    void index(index_t pos)
    {
      m_first = false;
      m_out << "[" << pos << "]";
    }

    void any_index()
    {
      m_first = false;
      m_out << "[*]";
    }

    void any_path()
    {
      m_first = false;
      m_out << "**";
    }
  };

  struct Scalar_printer
    : public Scalar_prc
    , public Scalar_prc::Args_prc
    , public cdk::api::Table_ref
  {
    Expr_printer &m_parent;
    Printer_base &m_pb;
    cdk::string m_op_name;

    Val_printer   m_val_printer;
    Path_printer  m_path_printer;

    Scalar_printer(Expr_printer &parent)
      : m_parent(parent), m_pb(parent.m_pb)
      , m_val_printer(parent.m_pb)
      , m_path_printer(parent.m_pb.m_out)
    {}

    // Table_ref

    const cdk::string  name() const { return m_op_name; }
    const cdk::api::Schema_ref* schema() const { return NULL; }

    // Scalar_prc

    Value_prc* val()
    { return &m_val_printer; }

    Args_prc* op(const char *op_name)
    {
      std::string name_str("operator \"");
      name_str.append(op_name);
      name_str.append("\"");
      m_op_name= name_str;
      return call(*this);
    }

    Args_prc* call(const cdk::api::Table_ref &db_obj)
    {
      ostream &out = m_pb.out_ind();

      if (db_obj.schema())
        out << db_obj.schema()->name() << "." << db_obj.name();
      else
        out << db_obj.name();

      return this;
    }

    void list_begin()
    {
      m_pb.m_out <<" (" <<endl;
      m_parent.m_pb.m_indent++;
    }

    void list_end()
    {
      m_parent.m_pb.m_indent--;
      m_pb.out_ind() <<")" <<endl;
    }

    Element_prc* list_el()
    {
      return &m_parent;
    }

    virtual void var(const cdk::string &var_name)
    {
      m_pb.out_ind() <<"@" <<var_name <<endl;
    }

    virtual void ref(const cdk::Doc_path &path)
    {
      path.process(m_path_printer);
      m_pb.m_out << endl;
    }

    virtual void ref(const cdk::api::Column_ref &col, const cdk::Doc_path *path)
    {
      ostream &out = m_pb.out_ind();

      if (col.table())
      {
        if (col.table()->schema())
          out <<"`" <<col.table()->schema()->name() <<"`.";
        out <<"`" <<(col.table()->name()) <<"`.";
      }
      out <<"`" <<col.name() <<"`";

      if (path)
      {
        out <<"->$.";
        path->process(m_path_printer);
      }

      out <<endl;
    }

    virtual void placeholder()
    {
      m_pb.out_ind() <<"?" <<endl;
    }

    virtual void param(const cdk::string &pname)
    {
      m_pb.out_ind() <<":" <<pname <<endl;
    }

    virtual void param(uint16_t pos)
    {
      m_pb.out_ind() <<":" <<pos <<endl;
    }
  }
  m_scalar_printer;


  Scalar_prc* scalar()
  {
    return &m_scalar_printer;
  }

  List_prc* arr()
  {
    m_pb.out_ind() << "<array>" <<endl;
    return NULL;
  }

  Doc_prc* doc()
  {
    m_pb.out_ind() << "<document>" <<endl;
    return NULL;
  }

};


// TODO: more extensive testing when expr parser is completed
// TODO: check if parsing is correct

struct Expr_Test{ parser::Parser_mode::value mode; const wchar_t *txt;} ;

const Expr_Test exprs[] =
{
  { parser::Parser_mode::DOCUMENT, L"-2*34.1%5"},
  { parser::Parser_mode::DOCUMENT, L"-2*3+4.1%5"},
  { parser::Parser_mode::DOCUMENT, L"-2*3+4.1%5"},
  { parser::Parser_mode::DOCUMENT, L"-2*3+4.1%5 >> 6"},
  { parser::Parser_mode::DOCUMENT, L"-2*3+4.1%5 >> 6 & 7"},
  { parser::Parser_mode::DOCUMENT, L"-2*3+4.1%5 >> 6 & 7 >= 8"},
  { parser::Parser_mode::DOCUMENT, L"-2*3+4.1%5 >> 6 & 7 >= 8 and  not true"},
  { parser::Parser_mode::DOCUMENT, L"-2*3+4.1%5 >> 6 & 7 >= 8 and true or docName like 'foo%'"},
  { parser::Parser_mode::TABLE,    L"-2*3+4.1%5 >> 6 & 7 >= 8 and true or Schema.Table.docName is not true "},
  { parser::Parser_mode::DOCUMENT, L"-2*3+4.1%5 >> 6 & 7 >= 8 and true or docName is not false"},
  { parser::Parser_mode::DOCUMENT, L"-2*3+4.1%5 >> 6 & 7 >= 8 and true or docName is not NULL "},
  { parser::Parser_mode::DOCUMENT, L"-2*3+4.1%5 >> 6 & 7 >= 8 and true or docName not in ('foo%', 'bar%')"},
  { parser::Parser_mode::DOCUMENT, L"-2*3+4.1%5 >> 6 & 7 >= 8 and true or docName not between 'foo%' AND 'bar%'"},
  { parser::Parser_mode::DOCUMENT, L"-2*3+4.1%5 >> 6 & 7 >= 8 and true or docName not regexp 'foo.*'"},
  { parser::Parser_mode::DOCUMENT, L"-2*3+4.1%5 >> 6 & 7 >= 8 and true or Schema.Table.docName = null"},
  { parser::Parser_mode::DOCUMENT, L"not (name <= 'foo' or not bar)"},
  { parser::Parser_mode::DOCUMENT, L"colName.Xpto[1].a[*].* + .1e-2"},
  { parser::Parser_mode::DOCUMENT, L"$.doc_path.Xpto[1].a[*].* + -.1e-2"},
  { parser::Parser_mode::DOCUMENT, L"schemaName.functionX(cast(-2345 AS DECIMAL (2,3)))"},
  { parser::Parser_mode::DOCUMENT, L"schemaName.functionX(cast(-2345 AS DECIMAL (2)))"},
  { parser::Parser_mode::TABLE   , L"schemaName.tableName.columnName->$.doc_path.Xpto[1].a[*].*"},
  { parser::Parser_mode::TABLE   , L"schemaName.tableName.columnName->'$.doc_path.Xpto[1].a[*].*'"},
  { parser::Parser_mode::DOCUMENT, L"age and name"},
  { parser::Parser_mode::DOCUMENT, L"name LIKE :name AND age > :age" },
  { parser::Parser_mode::TABLE   , L"`date`->$.year"},
  { parser::Parser_mode::DOCUMENT, L"count(*)" },
  { parser::Parser_mode::TABLE   , L"~x"},
  { parser::Parser_mode::TABLE   , L"a^22"},
  { parser::Parser_mode::TABLE   , L"a^~22"},
  { parser::Parser_mode::TABLE   , L" a >cast(11 as signed Int)"},
  { parser::Parser_mode::TABLE   , L"c > cast(14.01 as decimal(3,2))"},
  { parser::Parser_mode::TABLE   , L"CHARSET(CHAR(X'65'))"},
  { parser::Parser_mode::TABLE   , L"CHARSET(CHAR(0x65))"},
//  { parser::Parser_mode::TABLE   , L"CHARSET(CHAR(X'65' USING utf8))"},
//  { parser::Parser_mode::TABLE   , L"TRIM(BOTH 'x' FROM 'xxxbarxxx')"},
//  { parser::Parser_mode::TABLE   , L"TRIM(LEADING 'x' FROM 'xxxbarxxx')"},
//  { parser::Parser_mode::TABLE   , L"TRIM(TRAILING 'xyz' FROM 'barxxyz')"},
  { parser::Parser_mode::TABLE   , L"'abc' NOT LIKE 'ABC1'"},
//  { parser::Parser_mode::TABLE   , L"'a' RLIKE '^[a-d]'"},
  { parser::Parser_mode::TABLE   , L"'a' REGEXP '^[a-d]'"},
//  { parser::Parser_mode::TABLE   , L"POSITION('bar' IN 'foobarbar')"},
//  { parser::Parser_mode::TABLE   , L"'Heoko' SOUNDS LIKE 'h1aso'"}

};


TEST(Parser, expr)
{
  Expr_printer printer(cout, 0);

  for (unsigned i=0; i < sizeof(exprs)/sizeof(Expr_Test); i++)
  {
    cout <<endl <<"== expr#" <<i <<" ==" <<endl<<endl;
    cout << (exprs[i].mode == parser::Parser_mode::DOCUMENT ? "DOCUMENT" : "TABLE") << endl;
    cdk::string expr(exprs[i].txt);
    cout <<"expr string: " <<expr <<endl;
    cout <<"----" <<endl;
    Expression_parser parser(exprs[i].mode, expr);
    parser.process(printer);
  }

  // negative tests

  {
    const char *expr = "-23452345243563467456745674567456745674567";
    Expression_parser parser(parser::Parser_mode::DOCUMENT, expr);
    EXPECT_THROW(parser.process(printer), cdk::Error);
    cout << "Expected error when parsing string: " << expr << endl;
  }

  {
    const char *expr = "";
    Expression_parser parser(parser::Parser_mode::DOCUMENT, expr);
    EXPECT_THROW(parser.process(printer), cdk::Error);
    cout << "Expected error when parsing empty string: " << expr << endl;
  }
}


const Expr_Test order_exprs[] =
{
  { parser::Parser_mode::DOCUMENT, L"$.age"},
  { parser::Parser_mode::DOCUMENT, L"$.age ASC"},
  { parser::Parser_mode::DOCUMENT, L"$.age DESC"},
  { parser::Parser_mode::DOCUMENT, L"$.year-age"},
  { parser::Parser_mode::DOCUMENT, L"$.year-age  ASC "},
  { parser::Parser_mode::DOCUMENT, L"$.year-age    DESC "},
  { parser::Parser_mode::DOCUMENT, L"$.doc_path.Xpto[1].a[*].* + -.1e-2"},
  { parser::Parser_mode::DOCUMENT, L"$.doc_path.Xpto[1].a[*].* + -.1e-2 ASC"},
  { parser::Parser_mode::DOCUMENT, L"$.doc_path.Xpto[1].a[*].* + -.1e-2 DESC"},
  { parser::Parser_mode::TABLE   , L"`date`->$.year"},
  { parser::Parser_mode::TABLE   , L"`date`->$.year ASC"},
  { parser::Parser_mode::TABLE   , L"`date`->$.year DESC"},

};


struct Order_printer
    : public Expr_printer
    , public cdk::api::Order_expr<cdk::Expression>::Processor

{

  Printer_base m_pb;

  Order_printer(ostream &out, unsigned ind =0)
    : Expr_printer(out, ind+1)
    , m_pb(out, ind)
  {}

  Expr_prc* sort_key(cdk::api::Sort_direction::value dir)
  {
    m_pb.out_ind() << "Order "
                   << (dir == cdk::api::Sort_direction::ASC ? "ASC" : "DESC")
                   << endl;

    return this;
  }

};


TEST(Parser, order_expr)
{

  Order_printer printer(cout, 0);

  for (unsigned i=0; i < sizeof(order_exprs)/sizeof(Expr_Test); i++)
  {
    cout <<endl <<"== expr#" <<i <<" ==" <<endl<<endl;
    cout << (order_exprs[i].mode == parser::Parser_mode::DOCUMENT ? "DOCUMENT" : "TABLE") << endl;
    cdk::string expr(order_exprs[i].txt);
    cout <<"Order expr string: " <<expr <<endl;
    cout <<"----" <<endl;
    Order_parser parser(order_exprs[i].mode, expr);
    parser.process(printer);
  }

  // negative tests

  {
    const char *expr = "age ASC DESC";
    Order_parser parser(parser::Parser_mode::DOCUMENT, expr);
    EXPECT_THROW(parser.process(printer), cdk::Error);
    cout << "Expected error when parsing string: " << expr << endl;
  }

  {
    const char *expr = "age ASC year";
    Order_parser parser(parser::Parser_mode::DOCUMENT, expr);
    EXPECT_THROW(parser.process(printer), cdk::Error);
    cout << "Expected error when parsing string: " << expr << endl;
  }

}


class Proj_Document_printer
    : public Expr_printer
    , public cdk::Expression::Document::Processor
{

  Printer_base m_pb;

public:
  Proj_Document_printer(ostream &out, unsigned ind =0)
    : Expr_printer(out, ind+1)
    , m_pb(out, ind)
  {}

  cdk::Expression::Document::Processor::Any_prc* key_val(const string &alias)
  {
    m_pb.out_ind() << "alias \""
                   << alias
                   << "\""
                   << endl;
    return this;
  }

};

class Proj_Table_printer
    : public Expr_printer
    , public cdk::api::Projection_expr<Expression>::Processor
{

  Printer_base m_pb;

public:
  Proj_Table_printer(ostream &out, unsigned ind =0)
    : Expr_printer(out, ind+1)
    , m_pb(out, ind)
  {}

  cdk::api::Projection_expr<Expression>::Processor::Expr_prc* expr()
  {
    return this;
  }

  void alias(const cdk::string& alias)
  {
    m_pb.out_ind() << "alias \""
                   << alias
                   << endl;
  }

};


const Expr_Test proj_exprs[] =
{
  { parser::Parser_mode::DOCUMENT, L"$.age AS new_age"},
  { parser::Parser_mode::DOCUMENT, L"2016-$.age AS birthyear"},
  { parser::Parser_mode::DOCUMENT, L"HEX(1) AS `HEX`"},
  { parser::Parser_mode::TABLE   , L"`date`->$.year"},
  { parser::Parser_mode::TABLE   , L"`date`->$.year AS birthyear"},
  { parser::Parser_mode::TABLE   , L"2016-`date`->$.year AS birthyear"},
  { parser::Parser_mode::TABLE   , L"HEX(1) AS `HEX`"},
};


TEST(Parser, projection_expr)
{
  Proj_Document_printer printeDocument(cout, 0);
  Proj_Table_printer printTable(cout, 0);


  for (unsigned i=0; i < sizeof(proj_exprs)/sizeof(Expr_Test); i++)
  {
    cout <<endl <<"== expr#" <<i <<" ==" <<endl<<endl;
    cout << (proj_exprs[i].mode == parser::Parser_mode::DOCUMENT ? "DOCUMENT" : "TABLE") << endl;
    cdk::string expr(proj_exprs[i].txt);
    cout <<"expr string: " <<expr <<endl;
    cout <<"----" <<endl;
    Projection_parser parser(proj_exprs[i].mode, expr);

    if (proj_exprs[i].mode == parser::Parser_mode::DOCUMENT)
      parser.process(printeDocument);
    else
      parser.process(printTable);
  }

  // negative tests

  {
    const char *expr = "age";
    Projection_parser parser(parser::Parser_mode::DOCUMENT, expr);
    EXPECT_THROW(parser.process(printeDocument), cdk::Error);
    cout << "Expected error when parsing string: " << expr << endl;
  }

}


TEST(Parser, doc_path)
{
  {
    cdk::string test = L"$**.date[*]";

    cout << "parsing path: " << test << endl;

    cdk::Doc_path_storage path;
    Doc_field_parser doc_path(test);
    doc_path.process(path);

    EXPECT_EQ(3, path.length());
    EXPECT_EQ(path.DOUBLE_ASTERISK, path.get_el(0).m_type);
    EXPECT_EQ(path.MEMBER, path.get_el(1).m_type);
    EXPECT_EQ(cdk::string(L"date"), path.get_el(1).m_name);
    EXPECT_EQ(path.ARRAY_INDEX_ASTERISK, path.get_el(2).m_type);
  }

  {
    cdk::string test = L"**.date[*]";

    cout << "parsing path: " << test << endl;

    cdk::Doc_path_storage path;
    Doc_field_parser doc_path(test);
    doc_path.process(path);

    EXPECT_EQ(3, path.length());
    EXPECT_EQ(path.DOUBLE_ASTERISK, path.get_el(0).m_type);
    EXPECT_EQ(path.MEMBER, path.get_el(1).m_type);
    EXPECT_EQ(cdk::string(L"date"), path.get_el(1).m_name);
    EXPECT_EQ(path.ARRAY_INDEX_ASTERISK, path.get_el(2).m_type);
  }

  {
    cdk::string test = L"$.date.date[*]";

    cout << "parsing path: " << test << endl;

    cdk::Doc_path_storage path;
    Doc_field_parser doc_path(test);
    doc_path.process(path);

    EXPECT_EQ(3, path.length());
    EXPECT_EQ(path.MEMBER, path.get_el(0).m_type);
    EXPECT_EQ(cdk::string(L"date"), path.get_el(0).m_name);
    EXPECT_EQ(path.MEMBER, path.get_el(1).m_type);
    EXPECT_EQ(cdk::string(L"date"), path.get_el(1).m_name);
    EXPECT_EQ(path.ARRAY_INDEX_ASTERISK, path.get_el(2).m_type);
  }

  cout << endl << "== Negative tests ==" << endl << endl;

  wchar_t* negative[] =
  {
    L"date.date[*].**",
    L"date.date[*]**",
    L"[*].foo",
    L"[1][2]",
    L"$foo",
    NULL
  };

  for (unsigned pos = 0; NULL != negative[pos]; ++pos)
  {
    cdk::string test = negative[pos];
    cout << "parsing path: " << test << endl;

    cdk::Doc_path_storage path;
    Doc_field_parser doc_path(test);

    EXPECT_THROW(doc_path.process(path), cdk::Error);
  }

}


/*
  Tests for URI parser
  ====================
*/

/*
  Helper "optional string" type. It helps distinguishing null string
  from empty one.
*/

struct string_opt : public std::string
{
  bool m_is_null;

  string_opt() : m_is_null(true)
  {}

  template <typename T>
  string_opt(T arg)
    : std::string(arg), m_is_null(false)
  {}

  operator bool() const
  {
    return !m_is_null;
  }

  bool operator==(const string_opt &other) const
  {
    if (m_is_null)
      return other.m_is_null;
    return 0 == this->compare(other);
  }
};

static const string_opt none;


/*
  Helper structure to hold result of URI parsing.
*/

struct URI_parts
{
  typedef std::string string;
  typedef std::map<string, string_opt> query_t;

  query_t query;

  URI_parts() : port(0), has_query(false)
  {}

  URI_parts(
    const string_opt &_user,
    const string_opt &_pwd,
    const string &_host,
    short _port,
    const string_opt &_path,
    bool _has_query)
    : user(_user)
    , pwd(_pwd)
    , host(_host)
    , port(_port)
    , path(_path)
    , has_query(_has_query)
  {}

  string_opt user;
  string_opt pwd;
  string     host;
  short      port;
  string_opt path;
  bool       has_query;

  bool operator==(const URI_parts &other) const
  {
    return user == other.user
      && pwd == other.pwd
      && host == other.host
      && port == other.port
      && path == other.path
      && has_query == other.has_query;
  }
};

std::ostream& operator<<(std::ostream &out, URI_parts &data)
{
  if (data.user)
    cout << " user: " << data.user << endl;
  if (data.pwd)
    cout << "  pwd: " << data.pwd << endl;
  cout << " host: " << data.host << endl;
  cout << " port: " << data.port << endl;
  if (data.path)
    cout << " path: " << data.path << endl;
  if (data.has_query)
  {
    cout << "query:" << endl;
    for (URI_parts::query_t::const_iterator it = data.query.begin()
        ; it != data.query.end()
        ; ++ it)
    {
      cout << "  " << it->first;
      if (it->second)
        cout << " -> " << it->second;
      cout << endl;
    }
  }
  return out;
}

#define EXPECT_EQ_URI(A,B) \
  EXPECT_EQ((A).user,(B).user);  \
  EXPECT_EQ((A).pwd,(B).pwd);    \
  EXPECT_EQ((A).host,(B).host);  \
  EXPECT_EQ((A).port,(B).port);  \
  EXPECT_EQ((A).path,(B).path);  \
  EXPECT_EQ((A).has_query,(B).has_query)


/*
  URI processor used for tests. It stores reported URI data in
  an URI_parts structure.
*/

struct URI_prc : parser::URI_processor
{
  URI_parts *m_data;

  URI_prc(URI_parts &data) : m_data(&data)
  {}

  void user(const std::string &val)
  {
    m_data->user = val;
  }

  void password(const std::string &val)
  {
    m_data->pwd = val;
  }

  void host(const std::string &val)
  {
    m_data->host = val;
  }

  void port(unsigned short val)
  {
    m_data->port = val;
  }

  void path(const std::string &val)
  {
    m_data->path = val;
  }

  void key_val(const std::string &key)
  {
    m_data->has_query = true;
    m_data->query[key] = string_opt();
  }

  void key_val(const std::string &key, const std::string &val)
  {
    m_data->has_query = true;
    m_data->query[key] = val;
  }

  void key_val(const std::string &key, const std::list<std::string> &val)
  {
    m_data->has_query = true;
    std::string list("['");
    bool start = true;

    for (std::list<std::string>::const_iterator it = val.begin()
        ; it != val.end()
        ; ++it)
    {
      if (start)
        start = false;
      else
        list.append(",'");
      list.append(*it);
      list.append("'");
    }

    list.append("]");
    m_data->query[key] = list;
  }
};



TEST(Parser, uri)
{
  using std::string;

  cout << "---- positive tests ----" << endl;

  static struct URI_test
  {
    std::string  uri;
    URI_parts    data;
  }
  test_uri[] = {
    {
      "host",
      URI_parts(none, none, "host", 0, none, false)
    },
    {
      "[::1]",
      URI_parts(none, none, "::1", 0, none, false)
    },
    {
      "host:123",
      URI_parts(none, none, "host", 123, none, false)
    },
    {
      "[::1]:123",
      URI_parts(none, none, "::1", 123, none, false)
    },
    {
      "host:0",
      URI_parts(none,none, "host", 0, none , false)
    },
    {
      "host/path",
      URI_parts(none , none, "host", 0, "path", false)
    },
    {
      "[::1]/path",
      URI_parts(none , none, "::1", 0, "path", false)
    },
    {
      "host/",
      URI_parts(none , none, "host", 0, "", false)
    },
    {
      "user@host/path",
      URI_parts("user" , none, "host", 0, "path", false)
    },
    {
      "user@[::1]/path",
      URI_parts("user" , none, "::1", 0, "path", false)
    },
    {
      "user%40host%2Fpath",
      URI_parts(none , none, "user@host/path", 0, none, false)
    },
    {
      "user:@host/path",
      URI_parts("user" , "", "host", 0, "path", false)
    },
    {
      "user:pwd@host",
      URI_parts("user" , "pwd", "host", 0, none, false)
    },
    {
      "user:pwd@[::1]",
      URI_parts("user" , "pwd", "::1", 0, none, false)
    },
    {
      "user:pwd@host:123",
      URI_parts("user" , "pwd", "host", 123, none, false)
    },
    {
      "user:pwd@[::1]:123",
      URI_parts("user" , "pwd", "::1", 123, none, false)
    },
    {
      "user:pwd@host:123/",
      URI_parts("user" , "pwd", "host", 123, "", false)
    },
    {
      "user:pwd@host:123/foo?key=val",
      URI_parts("user" , "pwd", "host", 123, "foo", true)
    },
    {
      "user:pwd@[::1]:123/foo?key=val",
      URI_parts("user" , "pwd", "::1", 123, "foo", true)
    },
    {
      "user:pwd@host:123?key=val",
      URI_parts("user" , "pwd", "host", 123, none, true)
    },
    {
      "user:pwd@host:123/?key=val",
      URI_parts("user" , "pwd", "host", 123, none, true)
    },
  };

  //unsigned pos = 3;
  for (unsigned pos = 0; pos < sizeof(test_uri) / sizeof(URI_test); ++pos)
  {
    std::string uri = test_uri[pos].uri;
    cout <<endl << "== parsing conn string#" <<pos << ": " << uri << endl;

    for (unsigned i = 0; i < 2; ++i)
    {
      if (i > 0)
        uri = string("mysqlx://") + uri;

      URI_parser pp(uri, i>0);

      URI_parts data;
      URI_prc  up(data);

      pp.process(up);
      cout << data;
      EXPECT_EQ_URI(data, test_uri[pos].data);
      cout << "--" << endl;
    }
  }


  cout << endl << "---- test queries ----" << endl;

  struct Query_test
  {
    string query;
    typedef std::map<string, string_opt> query_t;

    query_t data;

    Query_test(const char *q, ...)
      : query(q)
    {
      const char *key;
      va_list args;
      va_start(args, q);

      while (NULL != (key = va_arg(args, const char*)))
      {
        const char *val = va_arg(args, const char*);
        data[key] = val ? val : none;
      }

      va_end(args);
    }
  }
  test_q[] =
  {
    Query_test("a=[a,b,c]&b=valB&c",
                "a", "['a','b','c']",
                "b", "valB",
                "c", NULL,
                NULL)
  };


  for (unsigned pos = 0; pos < sizeof(test_q) / sizeof(Query_test); ++pos)
  {
    string uri = "host?";
    uri.append(test_q[pos].query);
    cout <<endl << "== parsing uri#" << pos << ": " << uri << endl;

    URI_parser pp(uri);
    URI_parts  data;
    URI_prc    up(data);

    pp.process(up);
    cout << data;

    for (Query_test::query_t::const_iterator it = test_q[pos].data.begin()
         ; it != test_q[pos].data.end(); ++it)
    {
      EXPECT_EQ(it->second, data.query[it->first]);
    }
  }


  cout << endl << "---- negative tests ----" << endl;

  const char* test_err_uri[] =
  {
    "foobar",
    "myfoobar",
    "my%23oobarbaz",
    "mysqlx",
    "mysqlx//",
    "mysqlx:",
    "mysqlx:/host",
    "mysqlx:host",
  };

  const char* test_err[] =
  {
    "host#",
    "host:foo",
    "host:",
    "host:1234567",
    "host:-127",
    "user@host#",
    "user:pwd@host#",
    "user:pwd@host:foo",
    "user:pwd@host:/db",
    "host/db#foo",
    "host/db/foo",
    "host/db?query#foo",
    "host/db?a=[a,b,c&b",
    "host/db?a=[a,b,c]foo=bar",
    "host/db?a=[a,b=foo",
    "[::1]:port:123",
    "[::1"
    //"host/db?l=[a,b&c]" TODO: should this fail?
    // TODO: allowed chars in host/path component
  };

  for (unsigned i = 0; i < 3; ++i)
  {
    //unsigned pos = 3;
    for (unsigned pos = 0;
         pos < (i==0 ? sizeof(test_err_uri) : sizeof(test_err)) / sizeof(char*);
         ++pos)
    {
      string uri = (i==0? test_err_uri[pos] : test_err[pos]);

      if (2 == i)
        uri = string("mysqlx://") + uri;

      cout << endl << "== parsing string#" << pos << ": " << uri << endl;
      try {
        // require mysqlx scheme only in first iteration
        URI_parser pp(uri, i==0);
        URI_parts  data;
        URI_prc    up(data);
        pp.process(up);
        EXPECT_TRUE(false) << "Expected error when parsing URI";
      }
      catch (const URI_parser::Error &e)
      {
        cout << "Expected error: " << e << endl;
      }
    }
  }

}
