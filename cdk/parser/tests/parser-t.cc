/*
 * Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
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


#define EXPECT_ERROR(Code) \
try { \
  Code; FAIL() << "Expected an error"; \
} catch (const cdk::Error &e) \
{ \
  cout << "Expected error: " << e << endl; \
} \


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
    EXPECT_ERROR(parser.process(printer));
  }

  {
    const char *json = "invalid";
    JSON_parser parser(json);
    EXPECT_ERROR(parser.process(printer));
  }

  {
    const char *json = "{ foo: 123, invalid }";
    JSON_parser parser(json);
    EXPECT_ERROR(parser.process(printer));
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
    , cdk::api::Doc_path_element_processor
  {
    using Element_prc::string;
    using Element_prc::index_t;

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

    void whole_document()
    {
      m_first = false;
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
  { parser::Parser_mode::TABLE   , L"'abc' NOT LIKE 'ABC1'"},
  { parser::Parser_mode::TABLE   , L"'a' REGEXP '^[a-d]'"},
  { parser::Parser_mode::TABLE   , L"'a' NOT RLIKE '^[a-d]'"},
  { parser::Parser_mode::TABLE   , L"POSITION('bar' IN 'foobarbar')"},
  { parser::Parser_mode::TABLE   , L"TRIM('barxxyz')"},
  { parser::Parser_mode::DOCUMENT, L"1 IN field.array"},
  { parser::Parser_mode::DOCUMENT, L"1 NOT IN field.array"},
  { parser::Parser_mode::DOCUMENT, L"field IN [1,2,3]"},
  { parser::Parser_mode::DOCUMENT, L"field NOT IN [1,2,3, NULL]"},
  { parser::Parser_mode::DOCUMENT, L"{\"a\":1, \"b\":null } IN $"},
  { parser::Parser_mode::DOCUMENT, L"{\"a\":1} NOT IN $"},
  { parser::Parser_mode::DOCUMENT, L"$.field1 IN $.field2"},
  { parser::Parser_mode::DOCUMENT, L"$.field1 NOT IN $.field2"},
  { parser::Parser_mode::DOCUMENT, L"a IN (b)"},
  { parser::Parser_mode::TABLE   , L"cast(column as json) IN doc->'$.field.array'"},
  { parser::Parser_mode::TABLE   , L"cast(column as json) NOT IN doc->'$.field.array'"},
  { parser::Parser_mode::TABLE   , L"column->'$.field' IN [1,2,3]"},
  { parser::Parser_mode::TABLE   , L"column->'$.field' NOT IN [1,2,3]"},
  { parser::Parser_mode::TABLE   , L"{\"a\":1} IN doc->'$'"},
  { parser::Parser_mode::TABLE   , L"{\"a\":1} NOT IN doc->'$'"},
  { parser::Parser_mode::TABLE   , L"tab1.doc->'$.field1' IN tab2.doc->'$.field2'"},
  { parser::Parser_mode::TABLE   , L"tab1.doc->'$.field1' NOT IN tab2.doc->'$.field2'"},

  //Tests from devcocs:
  //http://devdocs.no.oracle.com/mysqlx/latest/devapi-docs/refguide2/DataTypes/expression.html#expr

  { parser::Parser_mode::DOCUMENT, L"(1 in (1,2,3)) = TRUE"},
  { parser::Parser_mode::DOCUMENT, L"(1 not in (1,2,3)) = FALSE"},
  { parser::Parser_mode::DOCUMENT, L"{\"foo\" : \"bar\", \"baz\": [1,2,[3],{}, TRUE, true, false, False, null, NULL, Null]}"},
  { parser::Parser_mode::DOCUMENT, L"\"foo'bar\""},
  { parser::Parser_mode::DOCUMENT, L"\"foo''bar\""},
  { parser::Parser_mode::DOCUMENT, L"\"foo\\\"bar\""},
  { parser::Parser_mode::DOCUMENT, L"\"foo\"\"bar\""},
  { parser::Parser_mode::DOCUMENT, L"'foo\"bar'"},
  { parser::Parser_mode::DOCUMENT, L"'foo\"\"bar'"},
  { parser::Parser_mode::DOCUMENT, L"'foo\\'bar'"},
  { parser::Parser_mode::DOCUMENT, L"'foo''bar'"},
  { parser::Parser_mode::DOCUMENT, L"''''"},
  { parser::Parser_mode::DOCUMENT, L"\"\"\"\""},
  { parser::Parser_mode::DOCUMENT, L"\"\""},
  { parser::Parser_mode::DOCUMENT, L"''"},
  { parser::Parser_mode::DOCUMENT, L"'\\\\'"},
  { parser::Parser_mode::DOCUMENT, L"\"\\\\\""},
// discarded from grammar
//  { parser::Parser_mode::DOCUMENT, L"[<foo.bar>]"},
//  { parser::Parser_mode::DOCUMENT, L"[<\"foo\">]"},
//  { parser::Parser_mode::DOCUMENT, L"{<foo, bar>}"},
//  { parser::Parser_mode::DOCUMENT, L"[<{\"foo\":bar}>]"},

  // Following items were not included in original EBNF, but are valid
  { parser::Parser_mode::DOCUMENT, L"1 <> 2"},
  { parser::Parser_mode::DOCUMENT, L"4 % 2"},
  { parser::Parser_mode::DOCUMENT, L"[]"},
  { parser::Parser_mode::DOCUMENT, L"{}"},

    // Document Only
  { parser::Parser_mode::DOCUMENT, L"1 in [1,2,3]"},
  { parser::Parser_mode::DOCUMENT, L"[1] in [[1],[2],[3]]"},
  { parser::Parser_mode::DOCUMENT, L"foo = bar.baz"},
  { parser::Parser_mode::DOCUMENT, L"foo**.bar"},
  { parser::Parser_mode::DOCUMENT, L"foo[*].bar"},
  { parser::Parser_mode::DOCUMENT, L"_**._"},
  { parser::Parser_mode::DOCUMENT, L"_**[*]._"},
  { parser::Parser_mode::DOCUMENT, L"_**[*]._**._"},
  { parser::Parser_mode::DOCUMENT, L"$.foo.bar[*]"},
  { parser::Parser_mode::DOCUMENT, L"$ = {\"a\":1}"},
  { parser::Parser_mode::DOCUMENT, L"$.\" \".bar"},
  { parser::Parser_mode::DOCUMENT, L"$.a[0].b[0]"},
  { parser::Parser_mode::DOCUMENT, L"$.a[0][0]"},
  { parser::Parser_mode::DOCUMENT, L"$.a[*][*]"},
  { parser::Parser_mode::DOCUMENT, L"$.a[*].z"},
  { parser::Parser_mode::DOCUMENT, L"$.\"foo bar\".\"baz**\" = $"},
  { parser::Parser_mode::DOCUMENT, L"$.foo**.bar"},
  { parser::Parser_mode::DOCUMENT, L"$.\"foo bar\"**.baz"},
  { parser::Parser_mode::DOCUMENT, L"$.\"foo\"**.\"bar\""},
  { parser::Parser_mode::DOCUMENT, L"$.\"foo.\"**.\"bar\""},
  { parser::Parser_mode::DOCUMENT, L"$.\"foo.\"**.\".bar\""},
  { parser::Parser_mode::DOCUMENT, L"$.\"\""},
  { parser::Parser_mode::DOCUMENT, L"$**.bar"},
  { parser::Parser_mode::DOCUMENT, L"$**[0]"},
  { parser::Parser_mode::DOCUMENT, L"$**.bar"},
  { parser::Parser_mode::DOCUMENT, L"$**.foo"},
  { parser::Parser_mode::DOCUMENT, L"$.a**.bar"},
  { parser::Parser_mode::DOCUMENT, L"$.a**[0]"},
  { parser::Parser_mode::DOCUMENT, L"$.a**[*]"},
  { parser::Parser_mode::DOCUMENT, L"$.a**.bar"},
  { parser::Parser_mode::DOCUMENT, L"$.a**.foo"},

  //Relational
  //http://devdocs.no.oracle.com/mysqlx/latest/devapi-docs/refguide2/DataTypes/expression.html#id8

  { parser::Parser_mode::TABLE, L"1 in (1,2,3)"},
  { parser::Parser_mode::TABLE, L"{\"foo\" : \"bar\", \"baz\": [1,2,[3],{}, TRUE, true, false, False, null, NULL, Null]}"},
//  { parser::Parser_mode::TABLE, L"[<doc->'$.foo'>]"},
//  { parser::Parser_mode::TABLE, L"[<\"foo\">]"},
//  { parser::Parser_mode::TABLE, L"{<key, value>}"},
//  { parser::Parser_mode::TABLE, L"{<\"x\", value>}"},
//  { parser::Parser_mode::TABLE, L"[<{key:value}>]"},

  // Following items were not included in original EBNF, but is MySQL syntax
  { parser::Parser_mode::TABLE, L"1 <> 2"},
  { parser::Parser_mode::TABLE, L"4 % 2"},
  { parser::Parser_mode::TABLE, L"doc->>'$.foo'"},

  { parser::Parser_mode::TABLE, L"[]"},
  { parser::Parser_mode::TABLE, L"{}"},

  // Relational Only
  { parser::Parser_mode::TABLE, L"doc->'$.foo.bar[*]'"},
  { parser::Parser_mode::TABLE, L"doc->'$.\" \".bar'"},
  { parser::Parser_mode::TABLE, L"doc->'$.a[0].b[0]'"},
  { parser::Parser_mode::TABLE, L"doc->'$.a[0][0]'"},
  { parser::Parser_mode::TABLE, L"`x`->'$.a[*][*]'"},
  { parser::Parser_mode::TABLE, L"`''`->'$.a[*].z'"},
  { parser::Parser_mode::TABLE, L"doc->'$.\"foo bar\".\"baz**\"'"},
  { parser::Parser_mode::TABLE, L"doc->'$.foo**.bar'"},
  { parser::Parser_mode::TABLE, L"doc->'$.\"foo bar\"**.baz'"},
  { parser::Parser_mode::TABLE, L"doc->'$.\"foo\"**.\"bar\"'"},
  { parser::Parser_mode::TABLE, L"doc->'$.\"foo.\"**.\"bar\"'"},
  { parser::Parser_mode::TABLE, L"doc->'$.\"foo.\"**.\".bar\"'"},
  { parser::Parser_mode::TABLE, L"doc->'$.\"\"'"},
  { parser::Parser_mode::TABLE, L"doc->'$**.bar'"},
  { parser::Parser_mode::TABLE, L"doc->'$**[0]'"},
  { parser::Parser_mode::TABLE, L"doc->'$**.bar'"},
  { parser::Parser_mode::TABLE, L"doc->'$**.foo'"},
  { parser::Parser_mode::TABLE, L"foo.doc->'$.a**.bar'"},
  { parser::Parser_mode::TABLE, L"foo.bar.doc->'$.a**[0]'"},
  { parser::Parser_mode::TABLE, L"`foo`.doc->'$.a**[*]'"},
  { parser::Parser_mode::TABLE, L"`foo.bar`.doc->'$.a**.bar'"},
  { parser::Parser_mode::TABLE, L"`->`.doc->'$.a**.foo'"}

};

const Expr_Test negative_exprs[] =
{
  { parser::Parser_mode::TABLE   , L"-23452345243563467456745674567456745674567"},
  { parser::Parser_mode::TABLE   , L""},
  { parser::Parser_mode::TABLE   , L"CHARSET(CHAR(X'65' USING utf8))"},
  { parser::Parser_mode::TABLE   , L"TRIM(BOTH 'x' FROM 'xxxbarxxx')"},
  { parser::Parser_mode::TABLE   , L"TRIM(LEADING 'x' FROM 'xxxbarxxx')"},
  { parser::Parser_mode::TABLE   , L"TRIM(TRAILING 'xyz' FROM 'barxxyz')"},
  { parser::Parser_mode::TABLE   , L"TRIM('xyz' FROM 'barxxyz')"},
  { parser::Parser_mode::TABLE   , L"'Heoko' SOUNDS LIKE 'h1aso'"},
  { parser::Parser_mode::TABLE   , L"foo+"},

  //Tests from devcocs:
  //http://devdocs.no.oracle.com/mysqlx/latest/devapi-docs/refguide2/DataTypes/expression.html#invalid

  { parser::Parser_mode::DOCUMENT, L"$."                 },
  { parser::Parser_mode::DOCUMENT, L".doc"               },
  { parser::Parser_mode::DOCUMENT, L"**"                 },
  { parser::Parser_mode::DOCUMENT, L"**foo"              },
  { parser::Parser_mode::DOCUMENT, L"_**"                },
  { parser::Parser_mode::DOCUMENT, L"_**[*]_**._"        },
  { parser::Parser_mode::DOCUMENT, L"_**[*]._.**._"      },
  { parser::Parser_mode::DOCUMENT, L"_**[*]_.**._"       },
  { parser::Parser_mode::DOCUMENT, L"$.foo**"            },
  { parser::Parser_mode::DOCUMENT, L"$.foo.**.bar"       },
//  { parser::Parser_mode::DOCUMENT, L"$.foo.*.bar"        },
  { parser::Parser_mode::DOCUMENT, L"$.foo[**]"          },
  { parser::Parser_mode::DOCUMENT, L"$**"                },
  { parser::Parser_mode::DOCUMENT, L"$.**"               },
  { parser::Parser_mode::DOCUMENT, L"$.**bar"            },
  { parser::Parser_mode::DOCUMENT, L"$.**\".bar\""       },
  { parser::Parser_mode::DOCUMENT, L"$.**.bar"           },
  { parser::Parser_mode::DOCUMENT, L"$.foo..bar"         },
//  { parser::Parser_mode::DOCUMENT, L"foo[*].\"bar\""     },
  { parser::Parser_mode::DOCUMENT, L"\"foo\".bar"        },
  { parser::Parser_mode::DOCUMENT, L"$**.bar()"          },
  { parser::Parser_mode::DOCUMENT, L"[<foo, bar>]"       },
  { parser::Parser_mode::DOCUMENT, L"[<\"foo\", 1>]"     },
  { parser::Parser_mode::DOCUMENT, L"{<foobar>}"         },

  // Invalid that was wrongly included in parser (not MySQL syntax)
//  { parser::Parser_mode::DOCUMENT, L"1 == 1"             },

 // Relational Only
  { parser::Parser_mode::DOCUMENT, L"doc->'$.foo'"           },
  { parser::Parser_mode::DOCUMENT, L"foo.bar->'$.foo'"       },

  //http://devdocs.no.oracle.com/mysqlx/latest/devapi-docs/refguide2/DataTypes/expression.html#id9

  { parser::Parser_mode::TABLE, L"doc->'foo**.bar'"           },
  { parser::Parser_mode::TABLE, L"doc->'foo[*].bar'"          },
  { parser::Parser_mode::TABLE, L"doc->'_**._'"               },
  { parser::Parser_mode::TABLE, L"doc->'_**[*]._'"            },
  { parser::Parser_mode::TABLE, L"doc->_**[*]._**._'"         },
  { parser::Parser_mode::TABLE, L"[<doc->'$.foo', bar>]"      },
  { parser::Parser_mode::TABLE, L"[<\"foo\", 1>]"             },
  { parser::Parser_mode::TABLE, L"{<doc->'$.foobar'>}"        },

  // Document Only
//  { parser::Parser_mode::TABLE, L"1 in [1,2,3]"               },
//  { parser::Parser_mode::TABLE, L"[1] in [[1],[2],[3]]"       },
//  { parser::Parser_mode::TABLE, L"foo = bar.baz"              },
  { parser::Parser_mode::TABLE, L"foo**.bar"                  },
  { parser::Parser_mode::TABLE, L"foo[*].bar"                 },
  { parser::Parser_mode::TABLE, L"_**._"                      },
  { parser::Parser_mode::TABLE, L"_**[*]._"                   },
  { parser::Parser_mode::TABLE, L"_**[*]._**._"               },
  { parser::Parser_mode::TABLE, L"$.foo.bar[*]"               },
  { parser::Parser_mode::TABLE, L"$ = {\"a\":1}"              },
  { parser::Parser_mode::TABLE, L"$.\" \".bar"                },
  { parser::Parser_mode::TABLE, L"$.a[0].b[0]"                },
  { parser::Parser_mode::TABLE, L"$.a[0][0]"                  },
  { parser::Parser_mode::TABLE, L"$.a[*][*]"                  },
  { parser::Parser_mode::TABLE, L"$.a[*].z"                   },
  { parser::Parser_mode::TABLE, L"$.\"foo bar\".\"baz**\" = $"},
  { parser::Parser_mode::TABLE, L"$.foo**.bar"                },
  { parser::Parser_mode::TABLE, L"$.\"foo bar\"**.baz"        },
  { parser::Parser_mode::TABLE, L"$.\"foo\"**.\"bar\""        },
  { parser::Parser_mode::TABLE, L"$.\"foo.\"**.\"bar\""       },
  { parser::Parser_mode::TABLE, L"$.\"foo.\"**.\".bar\""      },
  { parser::Parser_mode::TABLE, L"$.\"\""                     },
  { parser::Parser_mode::TABLE, L"$**.bar"                    },
  { parser::Parser_mode::TABLE, L"$**[0]"                     },
  { parser::Parser_mode::TABLE, L"$**.bar"                    },
  { parser::Parser_mode::TABLE, L"$**.foo"                    },
  { parser::Parser_mode::TABLE, L"$.a**.bar"                  },
  { parser::Parser_mode::TABLE, L"$.a**[0]"                   },
  { parser::Parser_mode::TABLE, L"$.a**[*]"                   },
  { parser::Parser_mode::TABLE, L"$.a**.bar"                  },
  { parser::Parser_mode::TABLE, L"$.a**.foo"                  }
};


TEST(Parser, expr)
{
  Expr_printer printer(cout, 0);

  for (unsigned i=0; i < sizeof(exprs)/sizeof(Expr_Test); i++)
  {
    const Expr_Test &test = exprs[i];
    cout <<endl <<"== expr#" <<i <<" ==" <<endl<<endl;
    cout << (test.mode == parser::Parser_mode::DOCUMENT ? "DOCUMENT" : "TABLE") << endl;
    cdk::string expr(test.txt);
    cout <<"expr string: " <<expr <<endl;
    cout <<"----" <<endl;
    Expression_parser parser(test.mode, expr);
    parser.process(printer);
  }

  cout << endl << "=== NEGATIVE TESTS ===" << endl;

  for (unsigned i=0; i < sizeof(negative_exprs)/sizeof(Expr_Test); i++)
  {
    const Expr_Test &test = negative_exprs[i];
    cout <<endl <<"== expr#" <<i <<" ==" <<endl<<endl;
    cout << (test.mode == parser::Parser_mode::DOCUMENT ? "DOCUMENT" : "TABLE") << endl;
    cdk::string expr(test.txt);
    cout <<"expecting error when parsing string: " <<expr <<endl;
    cout <<"----" <<endl;
    EXPECT_ERROR(Expression_parser(test.mode, expr).process(printer));
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
    cout << "Expecting error when parsing string: " << expr << endl;
    EXPECT_ERROR(parser.process(printer));
  }

  {
    const char *expr = "age ASC year";
    Order_parser parser(parser::Parser_mode::DOCUMENT, expr);
    cout << "Expecting error when parsing string: " << expr << endl;
    EXPECT_ERROR(parser.process(printer));
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
    cout << "Expecting error when parsing string: " << expr << endl;
    EXPECT_ERROR(parser.process(printeDocument));
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

  const wchar_t* negative[] =
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

    EXPECT_ERROR(doc_path.process(path));
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

struct Host
{
  typedef std::string string;

  Host(unsigned short _priority, const string& _name, unsigned short _port)
    : priority(_priority), port(_port), name(_name)
  {}

  Host(const string& _name, unsigned short _port)
    : priority(0), port(_port), name(_name)
  {}

  Host(unsigned short _priority,const string& _name)
    : priority(_priority), port(0), name(_name)
  {}

  Host(const string& _name)
    : priority(0), port(0), name(_name)
  {}

  bool operator == ( const Host other) const
  {
    return priority == other.priority &&
           port     == other.port     &&
           name     == other.name     &&
           type     == other.type;
  }

  unsigned short  priority;
  unsigned short      port;
  string     name;

  enum {ADDRESS, SOCKET, PIPE} type = ADDRESS;
};

struct Pipe : public Host
{
  Pipe(unsigned short priority, const std::string &pipe)
    : Host(priority, pipe)
  {
    type = PIPE;
  }

  Pipe(const std::string &pipe)
    : Host(pipe)
  {
    type = PIPE;
  }
};

struct Unix_socket : public Host
{
  Unix_socket(unsigned short priority, const std::string &socket)
    : Host(priority, socket)
  {
    type = SOCKET;
  }

  Unix_socket(const std::string &socket)
    : Host(socket)
  {
    type = SOCKET;
  }
};

struct Query
{
  Query(const std::string &_key)
    : key(_key)
  {}

  Query(const std::string &_key, const std::string &_val)
  : key(_key), val(_val)
  {}

  bool operator == ( const Query other) const
  {
    return key == other.key &&
           val == other.val;
  }

  std::string key;
  std::string val;
};

struct URI_parts
{
  typedef std::string string;
  typedef std::map<string, string_opt> query_t;

  query_t query;

  URI_parts()
  {}

  template<typename...Options>
  URI_parts(Options...opt)
  {
    add(opt...);
  }


  template<typename Option, typename...Rest>
  void add(Option &opt, Rest...r)
  {
    set(opt);
    add(r...);
  }

  template<typename Options>
  void add(Options opt)
  {
    set(opt);
  }

  void set(const std::string &_path)
  {
    path = _path;
  }

  void set(Host &host)
  {
    hosts.push_back(host);
  }

  void set(Query &qry)
  {
    query[qry.key] = qry.val;
  }


  std::vector<Host> hosts;

  string_opt user;
  string_opt pwd;
  string_opt path;


  bool operator==(const URI_parts &other) const
  {
    return user == other.user
      && pwd == other.pwd
      && hosts == other.hosts
      && path == other.path
      && query == other.query;
  }
};

std::ostream& operator<<(std::ostream &out, URI_parts &data)
{
  if (data.user)
    cout << " user: " << data.user << endl;
  if (data.pwd)
    cout << "  pwd: " << data.pwd << endl;
  cout << " [" << endl;
  for ( auto el : data.hosts)
  {
    switch (el.type)
    {
    case Host::ADDRESS:
        cout << " host: " << el.name << endl;
        cout << " port: " << el.port << endl;
        break;
      case Host::PIPE:
        cout << " pipe: " << el.name << endl;
        break;
      case Host::SOCKET:
        cout << " socket: " << el.name << endl;
        break;
    }
  }
  cout << " ]" << endl;
  if (data.path)
    cout << " path: " << data.path << endl;
  if (data.query.size() != 0)
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
  EXPECT_EQ((A).hosts,(B).hosts);  \
  EXPECT_EQ((A).path,(B).path);  \
  EXPECT_EQ((A).query,(B).query); \


/*
  URI processor used for tests. It stores reported URI data in
  an URI_parts structure.
*/

struct URI_prc : parser::URI_processor
{
  URI_parts *m_data;

  URI_prc(URI_parts &data) : m_data(&data)
  {}

  void user(const std::string &val) override
  {
    m_data->user = val;
  }

  void password(const std::string &val) override
  {
    m_data->pwd = val;
  }

  void host(unsigned short priority, const std::string &host) override
  {
    m_data->hosts.push_back(Host(priority, host));
  }

  void host(unsigned short priority,
            const std::string &host,
            unsigned short port) override
  {
    m_data->hosts.push_back(Host(priority, host, port));
  }

  void socket(unsigned short priority, const std::string &socket_path) override
  {
    m_data->hosts.push_back(Unix_socket(priority, socket_path));
  }

  void pipe(unsigned short priority, const std::string &pipe) override
  {
    m_data->hosts.push_back(Pipe(priority, pipe));
  }

  void schema(const std::string &val) override
  {
    m_data->path = val;
  }

  void key_val(const std::string &key) override
  {
    m_data->query[key] = string_opt();
  }

  void key_val(const std::string &key, const std::string &val) override
  {
    m_data->query[key] = val;
  }

  void key_val(const std::string &key, const std::list<std::string> &val) override
  {
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
  test_uri[] =
  {
    {
      "host",
      URI_parts(Host("host"))
    },
    {
      "[::1]",
      URI_parts(Host("::1"))
    },
    {
      "host:123",
      URI_parts(Host("host", 123))
    },
    {
      "[::1]:123",
      URI_parts(Host("::1", 123))
    },
    {
      "host:0",
      URI_parts(Host("host", 0))
    },
    {
      "host:",  // default port
      URI_parts(Host("host", 0))
    },
    {
      "host/path",
      URI_parts(Host("host", 0), "path")
    },
    {
      "[::1]/path",
      URI_parts(Host("::1", 0), "path")
    },
    {
      "host/",
      URI_parts(Host("host", 0),"")
    },
    {
      "host:123/",
      URI_parts(Host("host", 123), "")
    },
    {
      "host:/db",
      URI_parts(Host("host", 0), "db")
    },
    {
      "host:123/foo?key=val",
      URI_parts(Host("host", 123), "foo", Query("key", "val"))
    },
    {
      "[::1]:123/foo?key=val",
      URI_parts(Host("::1", 123), "foo", Query("key", "val"))
    },
    {
      "host:123?key=val",
      URI_parts(Host("host", 123), Query("key", "val"))
    },
    {
      "host:123/?key=val",
      URI_parts(Host("host", 123), "", Query("key", "val"))
    },
    // host list
    {
      "[127.0.0.1]",
      URI_parts(Host("127.0.0.1"))
    },
    {
      "[[::1]]",
      URI_parts(Host("::1"))
    },
    {
      "[host1]",
      URI_parts(Host("host1"))
    },
    {
      "[127.0.0.1,host,[::1]]",
      URI_parts(Host("127.0.0.1"), Host("host"), Host("::1"))
    },
    {
      "[127.0.0.1,127.0.0.2]/?key1=val1&key2=val2",
      URI_parts(Host("127.0.0.1"), Host("127.0.0.2"),
                Query("key1", "val1"), Query("key2", "val2"))
    },
    {
      "[host1,host2]",
      URI_parts(Host("host1"), Host("host2"))
    },
    {
      "[server.example.com,192.0.2.11:33060,[2001:db8:85a3:8d3:1319:8a2e:370:7348]:1]/database",
      URI_parts(Host("server.example.com"), Host("192.0.2.11",33060),
                Host("2001:db8:85a3:8d3:1319:8a2e:370:7348",1 ), "database")
    },
    {
      "[(Address=127.0.0.1,Priority=2),(Address=example.com,Priority=100)]/database",
      URI_parts(Host(3, "127.0.0.1"), Host(101, "example.com"), "database")
    },
    {
      "\\\\.\\named_pipe.socket",
      URI_parts(Pipe("\\\\.\\named_pipe.socket"))
    },
    {
      "\\\\.\\named%20pipe.socket/database",
      URI_parts(Pipe("\\\\.\\named pipe.socket"), "database")
    },
    {
      "(\\\\.\\named:/?%232[1]@pipe.socket)/database",
      URI_parts(Pipe("\\\\.\\named:/?#2[1]@pipe.socket"), "database")
    },
    {
      "(/mysql:/?%23(2[1)]@socket)/database",
      URI_parts(Unix_socket("/mysql:/?#(2[1)]@socket"), "database")
    },
    {
      ".mysql.sock",
      URI_parts(Unix_socket(".mysql.sock"))
    },
    {
      ".mysql.sock/database?qry=val&qry2=2017",
      URI_parts(Unix_socket(".mysql.sock"), "database",
                Query("qry", "val"),Query("qry2", "2017"))
    }
  };

  //unsigned pos = 23;
  for (unsigned pos = 0; pos < sizeof(test_uri) / sizeof(URI_test); ++pos)
  {
    std::string original_uri = test_uri[pos].uri;


    for (int i = 0 ; i < 4; ++i)
    {
      std::string uri;
      switch (i)
      {
        case 0:
          uri = original_uri;
          test_uri[pos].data.user = none;
          test_uri[pos].data.pwd = none;
          break;
        case 1:
          uri = string("user@") + original_uri;
          test_uri[pos].data.user = "user";
          test_uri[pos].data.pwd = none;
          break;
        case 2:
          uri = string("user:@") + original_uri;
          test_uri[pos].data.user = "user";
          test_uri[pos].data.pwd = "";
          break;
        case 3:
          uri = string("user:pwd@") + original_uri;
          test_uri[pos].data.user = "user";
          test_uri[pos].data.pwd = "pwd";
          break;
      }

      for (unsigned j = 0; j < 2; ++j)
      {
        if (j > 0)
          uri = string("mysqlx://") + uri;

        cout <<endl << "== parsing conn string#" <<pos << ": " << uri << endl;

        URI_parser pp(uri, j>0);

        URI_parts data;
        URI_prc  up(data);

        pp.process(up);
        cout << data;
        EXPECT_EQ_URI(data, test_uri[pos].data);
        cout << "--" << endl;
      }
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
    "host:1234567",
    "host:-127",
    "user@host#",
    "user:pwd@host#",
    "user:pwd@host:foo",
    "host/db#foo",
    "host/db/foo",
    "host/db?query#foo",
    "host/db?a=[a,b,c&b",
    "host/db?a=[a,b,c]foo=bar",
    "host/db?a=[a,b=foo",
    "[::1]:port:123",
    "[::1",
    "<foo.example.com:123/db>"
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

