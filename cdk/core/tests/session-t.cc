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


#include "test.h"
#include "session_test.h"

#include <iostream>
#include <mysql/cdk.h>


using ::std::cout;
using ::std::endl;
using namespace ::cdk;

class Session_core
    : public cdk::test::Core_test
    , public cdk::test::Row_processor<cdk::Meta_data>
{
public:

  static void do_sql(Session &sess, const string &query)
  {
    Reply r;
    r = sess.sql(query);
    r.wait();
    if (0 < r.entry_count())
      r.get_error().rethrow();
  }
};



/*
  Basic test that connects to the test server, sends a message and
  reads server's reply.

  Note: Test server should be started before running this test.
*/

using ::std::cout;
using ::std::endl;
using namespace ::cdk;
using namespace ::cdk::test;


TEST_F(Session_core, basic)
{
  SKIP_IF_NO_XPLUGIN;

  try
  {

    ds::TCPIP ds("localhost", m_port);
    ds::Options options("root");
    cdk::Session s1(ds, options);


    if (!s1.is_valid())
      FAIL() << "Invalid Session created";

    if (!s1.check_valid())
      FAIL() << "Invalid Session created";

    s1.close();

    if (s1.is_valid())
      FAIL() << "Invalid Session after close()";

    if (s1.check_valid())
      FAIL() << "Invalid Session after close()";

  }
  catch (Error &e)
  {
    FAIL() << "Connection error: " << e << endl;
  }

}


TEST_F(Session_core, default_schema)
{
  SKIP_IF_NO_XPLUGIN;

  try
  {

    ds::TCPIP ds("localhost", m_port);
    ds::Options options("root");
    options.set_database("test");

    cdk::Session s(ds, options);


    if (!s.is_valid())
      FAIL() << "Invalid Session created";

    if (!s.check_valid())
      FAIL() << "Invalid Session created";

    Reply r(s.sql(L"SELECT DATABASE()"));
    r.wait();

    Cursor c(r);

    struct : cdk::Row_processor
    {
      // Row_processor callbacks

      virtual bool row_begin(row_count_t row)
      {
        return true;
      }
      virtual void row_end(row_count_t row)
      {}

      virtual void field_null(col_count_t pos)
      {}

      virtual size_t field_begin(col_count_t pos, size_t)
      {
        return  SIZE_MAX;
      }

      size_t field_data(col_count_t pos, bytes data)
      {
        EXPECT_EQ(0, pos);

        // We expect string with current schema name

        cdk::foundation::Codec<cdk::foundation::Type::STRING> codec;
        cdk::string db;

        // Trim trailing \0
        bytes d1(data.begin(), data.end() - 1);
        codec.from_bytes(d1, db);

        cout << "current schema: " << db << endl;
        EXPECT_EQ(string("test"),db);

        return 0;
      }

      virtual void field_end(col_count_t /*pos*/)
      {}

      virtual void end_of_data()
      {}
    }
    prc;

    set_meta_data(c);
    c.get_rows(prc);
    c.wait();

  }
  catch (Error &e)
  {
    FAIL() << "CDK error: " << e << endl;
  }

}


TEST_F(Session_core, sql_basic)
{
  try {
    SKIP_IF_NO_XPLUGIN;

  Session s(this);

    // No results
    {
      Reply rp;
      rp = s.sql(L"select * from mysql.user where invalid query :) ;");
      EXPECT_FALSE(rp.has_results());
    }

    for (int i = 0; i >=0 ;i++)
    {
      Reply rp;
      rp = s.sql(L"select * from mysql.user;");

      switch (i)
      {

      case 0:
        cout <<endl <<"== Case 0 ==" <<endl;
        {
          /*
             Normal results treatment
          */

          EXPECT_TRUE(rp.has_results());
          Cursor cursor(rp);

          set_meta_data(cursor);
          cursor.get_rows(*this);
          cursor.wait();

          EXPECT_FALSE(rp.has_results());

        }
        break;

      case 1:
        cout <<endl <<"== Case 1 ==" <<endl;
        {
          /*
             Second attempt to read cursor without results
          */

          EXPECT_TRUE(rp.has_results());
          Cursor cursor(rp);

          EXPECT_FALSE(rp.has_results());
          cursor.close();

          try
          {
            Cursor cr2(rp);
            FAIL() << "Should throw exception because there are no results now";
          }
          catch(const cdk::Error& e)
          {
            cout << "Expected CDK Error: " << e << std::endl;
          }

        }
        break;

      case 2:
        cout <<endl <<"== Case 2 ==" <<endl;
        {
          /*
             Skip Result
          */

          rp.skip_result();

          EXPECT_FALSE(rp.has_results());

          try
          {
            Cursor c2(rp);
            FAIL() << "No exception thrown";
          }
          catch(const cdk::Error& e)
          {
            cout << "Expected CDK Error: " << e << std::endl;
          }

        }
        break;

      case 3:
        cout <<endl <<"== Case 3 ==" <<endl;
        {
          /*
             Skip Result cannot be done when cursor exists
          */

          Cursor cursor(rp);

          try
          {
            rp.skip_result();

            FAIL() << "No exception thrown";
          }
          catch(const cdk::Error& e)
          {
            cout << "Expected CDK Error: " << e << std::endl;
          }

        }
        break;


      case 4:
        cout <<endl <<"== Case 4 ==" <<endl;
        {
          /*
             Discard Result
          */

          rp.discard();

          EXPECT_FALSE(rp.has_results());
          try
          {
            Cursor c2(rp);
            FAIL() << "No exception thrown";
          }
          catch(const cdk::Error& e)
          {
            cout << "Expected CDK Error: " << e << std::endl;
          }
        }
        break;

      case 5:
        cout <<endl <<"== Case 5 ==" <<endl;
        {
          /*
             Discard Result cannot be done when cursor exists
          */

          Cursor cursor(rp);

          try
          {
            rp.discard();

            FAIL() << "No exception thrown";
          }
          catch(const cdk::Error& e)
          {
            cout << "Expected CDK Error: " << e << std::endl;
          }

        }
        break;

      case 6:
        cout <<endl <<"== Case 6 ==" <<endl;
        {
          rp.skip_result();

          EXPECT_EQ(0U, rp.affected_rows());
        }
        break;

      case 7:
        cout <<endl <<"== Case 7 ==" <<endl;
        {
          Cursor cursor(rp);

          try {

            rp.affected_rows();
            FAIL() << "Affected Rows before end of cursor";
          }
          catch(const cdk::Error& e)
          {
            cout << "Expected CDK Error: " << e << std::endl;
          }

        }
        break;


      default:
        //break;
        i = -2;
        break;
      }
    }

    cout <<endl <<"== Diagnostics ==" <<endl;
    //TODO: Where should erros on queries stay?
    //      Since processors are all on

    for (Session::Diagnostics::Iterator &it= s.get_entries();
         it.next();)
    {
      FAIL() << it.entry().description();
    }

    cout <<"Done!" <<endl;

  }
  CATCH_TEST_GENERIC
}



TEST_F(Session_core, sql_args)
{
  try {
  SKIP_IF_NO_XPLUGIN;

  Session s(this);

  if (!s.is_valid())
    FAIL() << "Invalid Session!";

  struct Args
    : public Any_list
  {
    void process(Processor &prc) const
    {
      Safe_prc<Processor> sprc(prc);
      sprc->list_begin();
      sprc->list_el()->scalar()->num((uint64_t)7);
      sprc->list_el()->scalar()->num((int64_t)-7);
      sprc->list_el()->scalar()->num((double)3.141592);
      sprc->list_el()->scalar()->str("Hello World!");
      sprc->list_end();
    }
  }
  args;

  {
    Reply rp;
    rp = s.sql(L"select ? as A, ? as B, ? as C, ? as D;", &args);

    EXPECT_TRUE(rp.has_results());
    Cursor cursor(rp);

    set_meta_data(cursor);
    cursor.get_rows(*this);
    cursor.wait();

    EXPECT_FALSE(rp.has_results());
  }

  cout <<"Done!" <<endl;

}
  CATCH_TEST_GENERIC
}


TEST_F(Session_core, meta_data)
{
  // TODO: More extensive tests checking that meta-data reported
  // by CDK is as expected.

  try {
  SKIP_IF_NO_XPLUGIN;

  Session s(this);

  if (!s.is_valid())
    FAIL() << "Invalid Session!";

  struct Tables
  {
    Session &m_sess;

    Tables(Session &sess) : m_sess(sess)
    {
      drop_tables();
      do_sql(m_sess, L"USE test");
      do_sql(m_sess, L"CREATE TABLE meta_data1 (col1 JSON)");
      do_sql(m_sess, L"CREATE TABLE meta_data2 (col1 CHAR(16))");
    }

    ~Tables()
    {
      drop_tables();
    }

    void drop_tables()
    {
      do_sql(m_sess, L"USE test");
      do_sql(m_sess, L"DROP TABLE IF EXISTS meta_data1");
      do_sql(m_sess, L"DROP TABLE IF EXISTS meta_data2");
    }
  }
  create_tables(s);

  {
    Reply rp;
    rp = s.sql(L"SELECT col1 FROM meta_data1");

    EXPECT_TRUE(rp.has_results());
    Cursor cursor(rp);

    EXPECT_EQ(TYPE_DOCUMENT, cursor.type(0));
  }

  {
    Reply rp;
    rp = s.sql(L"SELECT col1 FROM meta_data2");

    EXPECT_TRUE(rp.has_results());
    Cursor cursor(rp);

    EXPECT_EQ(TYPE_STRING, cursor.type(0));
  }

  cout <<"Done!" <<endl;

}
  CATCH_TEST_GENERIC
}


TEST_F(Session_core, affected)
{
  try {
    SKIP_IF_NO_XPLUGIN;

    Session s(this);

    if (!s.is_valid())
      FAIL() << "Invalid Session!";

    do_sql(s, L"USE test");

    cout << "Current schema: " << s.current_schema() << endl;

    do_sql(s, L"DROP TABLE IF EXISTS affected");
    do_sql(s, L"CREATE TABLE affected (c0 INT)");

    Table_ref tbl("affected", "test");

    struct : public Row_source
    {
      int m_val;

      void process(Processor &prc) const
      {
        Safe_prc<Processor> sprc(prc);

        prc.list_begin();
        sprc->list_el()->scalar()->val()->num((int64_t)m_val);
        prc.list_end();
      }

      bool next()
      {
        if (m_val <= 0)
          return false;
        m_val--;
        return true;
      }
    }
    data;
    data.m_val = 7;

    {
      cout << "inserting data into table" << endl;

      Reply rp(s.table_insert(tbl, data, NULL, NULL));
      rp.wait();
      cout << "affected rows: " << rp.affected_rows() << endl;
      EXPECT_EQ(7, rp.affected_rows());

      rp.discard();

      /*
        After discarding reply, affected rows count is no
        longer available.
      */

      EXPECT_THROW(rp.affected_rows(), Error);
    }

    {
      cout << "fetching data from table" << endl;

      Reply rp(s.table_select(tbl, NULL));
      rp.wait();

      /*
        Affected rows count is available only for statements
        that do not produce results.
      */

      EXPECT_THROW(rp.affected_rows(), Error);

      rp.discard();
    }

    cout << "Done!" << endl;

  }
  CATCH_TEST_GENERIC
}


/*
  Test handling of multi-result-sets
*/

TEST_F(Session_core, sql_multi_rset)
{
  try {
    SKIP_IF_NO_XPLUGIN;

    Session s(this);

    if (!s.is_valid())
      FAIL() << "Invalid Session!";

    do_sql(s, L"DROP PROCEDURE IF EXISTS test.test");
    do_sql(s, L"CREATE PROCEDURE test.test() BEGIN SELECT 1; SELECT 'foo', 2; END");

    {
      Reply rp;
      rp = s.sql(L"CALL test.test()");

      EXPECT_TRUE(rp.has_results());
      {
        cout << "-- next result-set" << endl;
        Cursor cursor(rp);
        set_meta_data(cursor);
        cursor.get_rows(*this);
        cursor.wait();
        EXPECT_EQ(cdk::TYPE_INTEGER, cursor.type(0));
      }

      EXPECT_TRUE(rp.has_results());
      {
        cout << "-- next result-set" << endl;
        Cursor cursor(rp);
        set_meta_data(cursor);
        cursor.get_rows(*this);
        cursor.wait();
        EXPECT_EQ(cdk::TYPE_STRING, cursor.type(0));
      }

      EXPECT_FALSE(rp.has_results());
    }

    cout << "Test discarding of multi-result-set reply" << endl;

    {
      Reply rp;
      rp = s.sql(L"CALL test.test()");

      EXPECT_TRUE(rp.has_results());
      {
        Cursor cursor(rp);
      }

      EXPECT_TRUE(rp.has_results());
    }

    cout << "reply discarded" << endl;

    {
      Reply rp;
      rp = s.sql(L"CALL test.test()");

      EXPECT_TRUE(rp.has_results());
    }

    cout << "reply discarded" << endl;

    // TODO: Test output parameters when xplugin supports it.
    //do_sql(s, L"CREATE PROCEDURE test.test(OUT x INT) BEGIN SELECT 1; SET x = 2; END");
    //rp = s.sql(L"CALL test.test(@ret)");

    cout << "Done!" << endl;

  }
  CATCH_TEST_GENERIC
}



#if 0

parser::JSON_parser m_parser;

  Doc(const string &json)
    : m_parser(json)
  {
    reset(m_parser);
  }

  using Doc_converter::process;
};


struct Doc_list_base
{
  virtual Doc& get_doc(uint32_t) const =0;
};


template <uint32_t N>
struct Doc_list
  : public Doc_list_base
  , public Expr_list
  , public Expression
{

  const wchar_t **m_list;

  Doc_list(const wchar_t *list[N]) : m_list(list)
  {}

  uint32_t count() const { return N; }

  boost::scoped_ptr<Doc> m_doc;

  Doc& get_doc(uint32_t pos) const
  {
    Doc_list *self= const_cast<Doc_list*>(this);
    self->m_doc.reset(new Doc(m_list[pos]));
    return *m_doc;
  }

  uint32_t m_pos;

  const Expression& get(uint32_t pos) const
  {
    Doc_list *self= const_cast<Doc_list*>(this);
    self->m_pos= pos;
    return *this; //static_cast<Expression*>(self);
  }

  void process(Expression::Processor &prc) const
  {
    prc.doc(get_doc(m_pos));
  }
};



TEST_F(Session_core, docs)
{
  // TODO: Share this between different tests
  static const wchar_t *docs[] =
  {
    L"{'_id': 'uuid-1', 'str': 'foo', 'num': 123, 'bool': true}",
    L"{'_id': 'uuid-2', 'str': 'bar', 'doc': {'str': 'foo', 'num': 123, 'bool': true}}",
  };

  try {

    SKIP_IF_NO_XPLUGIN;

    Session &s= get_session();

    Object_ref coll("my_coll", "test");
    Reply r;

    {
      cout <<"== Creating collection" <<endl;
      r= s.admin("create_collection", coll);
      // note: ignoring "collection already exists" error
    }

    {
      cout <<"== Inserting documents" <<endl;
      Doc_list<sizeof(docs)/sizeof(wchar_t*)> list(docs);
      r= s.coll_add(coll, list);
      r.wait();
      if (0 < r.entry_count())
        r.get_error().rethrow();
    }

    {
      cout <<"== Reading results" <<endl;
      r= s.coll_find(coll, NULL);
      Cursor c(r);
      set_meta_data(c);
      c.get_rows(*this);
      c.wait();
    }

    //s.coll_add(coll, docs);
    cout <<"== Done!" <<endl;
  }
  CATCH_TEST_GENERIC
}

#endif

