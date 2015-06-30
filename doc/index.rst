Welcome to MySQL Connector/C++ 2.0 documentation!
=================================================

Connector/C++ 2.0 implements a C++ variant of the new NG DevAPI.

Synopsis
--------

.. note:: This synopsis is just a vision where we are heading. Not everything
  is implemented yet and things might change as we go. See usage section for
  examples of code that works with current implementation of Connector/C++.


.. code-block:: c++

  #include <mysqlxx.h>

  Session s(33060, "user", "password");

  SqlResult r= s.executeSql("SELECT @server_version");

  const StringList &col_names = r.getColumnNames();

  Row *row;
  while (row = r.next())
  {
    cout <<"== next row ==" <<endl;
    for (unsigned i=0; i < r.getColumnCount(); ++i)
    {
      cout <<"col#" <<i <<" (" <<col_names[i] <<")"
           <<" : " <<(*row)[i] <<endl;
    }
  }


Usage
-----

.. toctree::
  :maxdepth: 2

  usage.rst


Implementation
--------------

.. toctree::
   :maxdepth: 2

   coding.rst

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

