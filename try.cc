#include <iostream>
#include <mysqlx.h>
#include <mysql/cdk.h>
//#include <expr_parser.h>


using ::std::cout;
using ::std::endl;
//using namespace ::cdk;
using namespace ::mysqlx;

#if 0

class Doc_proc : public cdk::Document::Processor
{
  typedef cdk::string string;
  typedef cdk::Document Document;

  void begin() { cout <<"{" <<endl; }
  void end()   { cout <<"}" <<endl; }

  void field(const string &key, const string &val)
  {
    cout <<" " <<key <<": " <<'"' <<val <<'"' <<endl;
  }

  void field(const string &key, const int64_t val)
  {
    cout <<" " <<key <<": " <<val <<endl;
  }

  void field(const string &key, const uint64_t val)
  {
    cout <<" " <<key <<": " <<val <<endl;
  }

  void field(const string &key, const float val)
  {
    cout <<" " <<key <<": " <<val <<endl;
  }

  void field(const string &key, const double val)
  {
    cout <<" " <<key <<": " <<val <<endl;
  }

  void field(const string &key, const bool val)
  {
    cout <<" " <<key <<": " <<val <<endl;
  }

  void field(const string &key, const Document &val)
  {
    cout <<" " <<key <<": <sub-document>" <<endl;
    val.process(*this);
  }
};



class Expr_printer
  : public cdk::Expression::Processor
  , public cdk::api::Table_ref
{
  ostream &m_out;
  unsigned m_indent;
  cdk::string m_op_name;

public:

  Expr_printer(ostream &out, unsigned ind =0)
    : m_out(out), m_indent(ind)
  {}

  // Table_ref

  const cdk::string  name() const { return m_op_name; }
  const cdk::api::Schema_ref* schema() const { return NULL; }

  // Expr processor

  ostream& out_ind()
  {
    for (unsigned i=0; i < 2*m_indent; ++i)
      m_out.put(' ');
    return m_out;
  }

  virtual void null()
  {
    out_ind() <<"<null>" <<endl;
  }

  virtual void str(const cdk::string &val)
  {
    out_ind() <<"\"" <<val <<"\"" <<endl;
  }

  virtual void num(int64_t val)
  {
    out_ind() <<val <<endl;
  }

  virtual void num(uint64_t val)
  {
    out_ind() <<"U" <<val <<endl;
  }

  virtual void num(float val)
  {
    out_ind() <<"F" <<val <<endl;
  }

  virtual void num(double val)
  {
    out_ind() <<"D" <<val <<endl;
  }

  virtual void yesno(bool val)
  {
    out_ind() <<(val ? "TRUE" : "FALSE" ) <<endl;
  }

  virtual void var(const cdk::string &name)
  {
    out_ind() <<"@" <<name <<endl;
  }


  virtual void ref(const cdk::Doc_path &/*path*/)
  {
    out_ind() <<"$<path>" <<endl;
  }

  virtual void ref(const cdk::api::Column_ref &col, const cdk::Doc_path *path)
  {
    out_ind();

    if (col.table())
    {
      if (col.table()->schema())
        m_out <<"`" <<col.table()->schema()->name() <<"`.";
      m_out <<"`" <<(col.table()->name()) <<"`.";
    }
    m_out <<"`" <<col.name() <<"`";

    if (path)
      m_out <<"@<path>";

    m_out <<endl;
  }

  virtual void op(const char *name, const Expr_list &args)
  {
    std::string op_name("operator \"");
    op_name.append(name);
    op_name.append("\"");
    m_op_name= op_name;
    call(*this, args); 
  }

  virtual void call(const cdk::api::Table_ref &db_obj, const Expr_list &args)
  {
    out_ind() << db_obj.name() <<" (" <<endl;
    if (0 == args.count())
    {
      m_out <<")" <<endl;
      return;
    }
    ++m_indent;
    for (unsigned pos=0; pos < args.count(); ++pos)
      args.get(pos).process(*this);
    --m_indent;
    out_ind() <<")" <<endl;
  }

  virtual void placeholder()
  {
    out_ind() <<"?" <<endl;
  }

  virtual void param(const cdk::string &name)
  {
    out_ind() <<":" <<name <<endl;
  }

  virtual void param(unsigned pos)
  {
    out_ind() <<":" <<pos <<endl;
  }
};

#endif


int main()
try {

#if 0

#define TEST_ERROR(Code) \
  try { try { Code; } CATCH_AND_WRAP } \
  catch (const Error &e) { std::cout <<"Error: " <<e <<std::endl; } \
  catch (const std::exception &e) { std::cout <<"Std Exception: " <<e.what() <<std::endl; } \
  catch (const char *e) { std::cout <<"Bare string: " <<e <<std::endl; } \
  catch (...) { std::cout <<"Unkown exception" <<std::endl; }

  TEST_ERROR(throw Error("c/c++ error"));
  TEST_ERROR(cdk::throw_error(cdk::cdkerrc::protobuf_error, "foo"));
  TEST_ERROR(throw std::runtime_error("std exception"));
  TEST_ERROR(throw "bare string");
  TEST_ERROR(throw 7);

#else

  /*
    Using CRUD API
    ==============
  */

  {
    cout << "Creating session on localhost..." << endl;

    XSession sess(13010, "root");

    cout <<"Session accepted, creating collection..." <<endl;

    Schema sch= sess.getSchema("test");
    Collection coll= sch.createCollection("c1", true);

    cout <<"inserting document..." <<endl;

    coll.remove().execute();

    {
      Result add;

      add= coll.add("{ \"name\": \"foo\", \"age\": 1 }").execute();
      cout <<"- added doc with id: " <<add.getLastDocumentId() <<endl;

    add= coll.add("{ \"name\": \"bar\", \"age\": 2 }")
             .add("{ \"name\": \"baz\", \"age\": 3, \"date\": { \"day\": 20, \"month\": \"Apr\" }}").execute();
    cout << "- added 2 docs, last id: " << add.getLastDocumentId() << endl;

    add= coll.add("{ \"_id\": \"myuuid-1\", \"name\": \"foo\", \"age\": 7 }",
                  "{ \"name\": \"buz\", \"age\": 17 }").execute();
    cout <<"- added 2 docs, last id: " <<add.getLastDocumentId() <<endl;
    }

    cout <<"Fetching documents..." <<endl;

    DocResult docs = coll.find().execute(); // "age > 1 and name like 'ba%'").execute();

    DbDoc *doc = docs.fetchOne();

    for (int i = 0; doc; ++i, doc = docs.fetchOne())
    {
      cout << "doc#" << i << ": " << *doc << endl;

      for (Field fld : *doc)
      {
        cout << " field `" << fld << "`: " <<(*doc)[fld] << endl;
      }

      string name = (*doc)["name"];
      cout << " name: " << name << endl;

      if (doc->hasField("date") && Value::DOCUMENT == doc->fieldType("date"))
      {
        cout << "- date field" << endl;
        DbDoc date = (*doc)["date"];
        for (Field fld : date)
        {
          cout << "  date `" << fld << "`: " << date[fld] << endl;
        }
        string month = (*doc)["date"]["month"];
        int day = date["day"];
        cout << "  month: " << month << endl;
        cout << "  day: " << day << endl;
      }

      cout << endl;
    }
  }

  /*
    Using SQL
    =========
  */

  {
    cout << "querying collection with SQL ..." << endl;

    NodeSession sess(13010, "root");

    SqlResult res = sess.sql(L"SELECT * FROM test.c1").execute();

    cout << "Query sent, reading rows..." << endl;
    cout << "There are " << res.getColumnCount() << " columns in the result" << endl;
    Row *row;

    while (NULL != (row = res.fetchOne()))
    {
      cout << "== next row ==" << endl;
      for (unsigned i = 0; i < res.getColumnCount(); ++i)
      {
        cout << "col#" << i << ": " << (*row)[i] << endl;
      }
    }
  }

  /*
    Test some SQL types
    ===================
  */

  {
    cout << endl;

    NodeSession sess(13010, "root");

    cout << "Preparing test.types..." << endl;

    sess.sql("DROP TABLE IF EXISTS test.types").execute();
    sess.sql(
      "CREATE TABLE test.types("
      "  c0 INT,"
      "  c1 DECIMAL,"
      "  c2 FLOAT,"
      "  c3 DOUBLE"
      ")").execute();
    sess.sql(
      "INSERT INTO test.types VALUES"
      "(7, 3.14, 3.1415, 3.141592)"
      ).execute();

    cout << "Table prepared, querying it..." << endl;

    SqlResult res = sess.sql(L"SELECT * FROM test.types").execute();

    cout << "Query sent, reading rows..." << endl;
    cout << "There are " << res.getColumnCount() << " columns in the result" << endl;
    Row *row;

    while (NULL != (row = res.fetchOne()))
    {
      cout << "== next row ==" << endl;
      for (unsigned i = 0; i < res.getColumnCount(); ++i)
      {
        cout << "col#" << i << ": " << (*row)[i] << endl;
      }
    }

  }

#endif
  cout << "Done!" << endl;
}
//catch (const cdk::Error &err)
//{
//  // TODO: why these errors are not caught as std::exception?
//  cout <<"CDK ERROR: " <<err <<endl;
//}
catch (const mysqlx::Error &err)
{
  cout <<"ERROR: " <<err <<endl;
}
catch (std::exception &ex)
{
  cout <<"STD EXCEPTION: " <<ex.what() <<endl;
}
catch (const char *ex)
{
  cout <<"EXCEPTION: " <<ex <<endl;
}
