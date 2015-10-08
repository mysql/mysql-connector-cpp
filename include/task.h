#ifndef MYSQLX_TASK_H
#define MYSQLX_TASK_H

/**
  @file
  Classes to control (asynchronous) execution of commands.

  @note Asynchronous execution is not implemented yet.
*/


#include "common.h"
#include "result.h"

namespace mysqlx {

using std::ostream;

class Session;
class Schema;
class Collection;
class Result;
class Row;
class RowResult;
class DbDoc;
class DocResult;

class Task;
class Executable;


class Task : nocopy
{
protected:

  class Impl;
  Impl  *m_impl;

  Task() : m_impl(NULL)
  {}

  Task(Task &&other) : m_impl(other.m_impl)
  {
    other.m_impl = NULL;
  }

  void reset(Impl*);

public:

  virtual ~Task();
  bool is_completed();
  Result wait();
  void cont();

  friend class Impl;
  friend class Session;
  friend class NodeSession;
  friend class Result;
  friend class Collection;
  friend class Schema;

  struct Access;
  friend struct Access;
  friend class Executable;
};


}  // mysqlx

#endif
