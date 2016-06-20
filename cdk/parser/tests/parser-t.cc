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

  struct Scalar_printer
    : public Scalar_prc
    , public Scalar_prc::Args_prc
    , public cdk::api::Table_ref
  {
    Expr_printer &m_parent;
    Printer_base &m_pb;
    cdk::string m_op_name;

    Val_printer   m_val_printer;

    Scalar_printer(Expr_printer &parent)
      : m_parent(parent), m_pb(parent.m_pb)
      , m_val_printer(parent.m_pb)
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
      ostream &out = m_pb.out_ind();
      for (unsigned i=0; i < path.length(); ++i)
      {
        if (i > 0)
          out << ".";
        switch(path.get_type(i))
        {
          case Doc_path::MEMBER:
            out << *path.get_name(i);
          break;
          case Doc_path::MEMBER_ASTERISK:
            out << "*";
            break;
          case Doc_path::ARRAY_INDEX:
            out << "[" << *path.get_index(i) << "]";
          break;
          case Doc_path::ARRAY_INDEX_ASTERISK:
            out << "[*]";
          break;
          case Doc_path::DOUBLE_ASTERISK:
            out << "**";
          break;
        }
      }

      out << endl;
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
        out <<"->$";
        for (unsigned i= 0;
             i < path->length();
             ++i)
        {
          switch (path->get_type(i))
          {
            case Doc_path::MEMBER: out <<"." << *path->get_name(i); break;
            case Doc_path::MEMBER_ASTERISK: out << ".*"; break;
            case Doc_path::ARRAY_INDEX: out << "["<< *path->get_index(i) <<"]"; break;
            case Doc_path::ARRAY_INDEX_ASTERISK: out << "[*]"; break;
            case Doc_path::DOUBLE_ASTERISK: out << "**"; break;
          }
        }
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
    Doc_field_parser doc_path("$**.date[*]");
    EXPECT_EQ(3, doc_path.length());
    EXPECT_EQ(cdk::Doc_path::DOUBLE_ASTERISK, doc_path.get_type(0));
    EXPECT_EQ(cdk::Doc_path::MEMBER, doc_path.get_type(1));
    EXPECT_EQ(cdk::string(L"date"), *doc_path.get_name(1));
    EXPECT_EQ(cdk::Doc_path::ARRAY_INDEX_ASTERISK, doc_path.get_type(2));
  }

  {
    Doc_field_parser doc_path("**.date[*]");
    EXPECT_EQ(3, doc_path.length());
    EXPECT_EQ(cdk::Doc_path::DOUBLE_ASTERISK, doc_path.get_type(0));
    EXPECT_EQ(cdk::Doc_path::MEMBER, doc_path.get_type(1));
    EXPECT_EQ(cdk::string(L"date"), *doc_path.get_name(1));
    EXPECT_EQ(cdk::Doc_path::ARRAY_INDEX_ASTERISK, doc_path.get_type(2));
  }
  {
    Doc_field_parser doc_path("date.date[*]");
    EXPECT_EQ(3, doc_path.length());
    EXPECT_EQ(cdk::Doc_path::MEMBER, doc_path.get_type(0));
    EXPECT_EQ(cdk::string(L"date"), *doc_path.get_name(0));
    EXPECT_EQ(cdk::Doc_path::MEMBER, doc_path.get_type(1));
    EXPECT_EQ(cdk::string(L"date"), *doc_path.get_name(1));
    EXPECT_EQ(cdk::Doc_path::ARRAY_INDEX_ASTERISK, doc_path.get_type(2));
  }
  {
    EXPECT_THROW(Doc_field_parser doc_path("date.date[*].**"), cdk::Error);
  }
}
