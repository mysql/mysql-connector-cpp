#include <iostream>
#include <mysqlxx.h>
//#include <mysql/cdk.h>
//#include <expr_parser.h>


using namespace ::std;
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

#else

  cout <<"Creating session on localhost..." <<endl;

#if 0

  ds::TCPIP ds("localhost", 13010);
  ds::Options opt("root", NULL);
  //connection::TCPIP conn("localhost", 13010);
  //conn.connect();

  Session sess(ds, opt);

  sess.wait();
  if (!sess.is_valid())
    throw sess.get_error();

  cout <<"Session accepted, sending query..." <<endl;

  //Reply r= sess.sql(L"SELECT 1, 'ala'");
  //Cursor c(r);

#else

  Session sess(13010, "root");

  cout <<"Session accepted, creating collection..." <<endl;

  Schema sch= sess.getSchema("test");
  Collection coll= sch.createCollection("c1", true);

  cout <<"inserting document..." <<endl;

  coll.remove().execute();

#if 1

  {
    Result add;

    add= coll.add("{ \"name\": \"foo\", \"age\": 1 }").execute();
    cout <<"- added doc with id: " <<add.getLastDocumentId() <<endl;

    add= coll.add("{ \"name\": \"bar\", \"age\": 2 }").execute();
    cout <<"- added doc with id: " <<add.getLastDocumentId() <<endl;

    add= coll.add("{ \"name\": \"baz\", \"age\": 3 }").execute();
    cout <<"- added doc with id: " <<add.getLastDocumentId() <<endl;

    add= coll.add("{ \"_id\": \"myuuid-1\", \"name\": \"foo\", \"age\": 7 }").execute();
    cout <<"- added doc with id: " <<add.getLastDocumentId() <<endl;
  }

#endif

  /*
    TODO:

    On Linux and OSX, doing it this way invokes copy constructor:

    Result res= sess.executeSql(...);

    Perhaps using const reference as result initializer will avoid this.
  */

#if 0
  cout <<"querying collection..." <<endl;

#if 0
  RowResult res= sess.executeSql(L"SELECT * FROM test.c1");
#else
  RowResult res= coll.find().execute();
#endif

  cout <<"Query sent, reading rows..." <<endl;
  cout <<"There are " <<res.getColumnCount() <<" columns in the result" <<endl;
  Row *row;

  while (NULL != (row= res.next()))
  {
    cout <<"== next row ==" <<endl;
    for (unsigned i=0; i < res.getColumnCount(); ++i)
    {
      cout <<"col#" <<i <<": " <<(*row)[i] <<endl;
    }
  }
#else

  cout <<"Fetching documents..." <<endl;

  DocResult docs= coll.find("age > 1 and name like 'ba%'").execute();
  cout <<"first doc: " <<docs.first() <<endl;

  DbDoc *doc= docs.next();
  for(int i=0; doc; ++i, doc= docs.next())
  {
    cout <<"doc#" <<i <<": " <<*doc <<endl;
  }


#endif

#endif

#endif

  cout <<"Done!" <<endl;
}
//catch (const cdk::Error &err)
//{
//  // TODO: why these errors are not caught as std::exception?
//  cout <<"CDK ERROR: " <<err <<endl;
//}
//catch (const mysqlx::Error &err)
//{
//  cout <<"ERROR: " <<err <<endl;
//}
catch (std::exception &ex)
{
  cout <<"STD EXCEPTION: " <<ex.what() <<endl;
}
catch (const char *ex)
{
  cout <<"EXCEPTION: " <<ex <<endl;
}
