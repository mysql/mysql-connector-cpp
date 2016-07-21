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


/*
  Implementation of error handling infrastructure
*/

#include <mysql/cdk/foundation/error.h>

PUSH_SYS_WARNINGS
#include <stdexcept>
#include <sstream>
POP_SYS_WARNINGS

PUSH_BOOST_WARNINGS
#include <boost/system/system_error.hpp>
POP_BOOST_WARNINGS


namespace cdk {
namespace foundation {


/*
  Default prefix added to error description in the string returned by
  what().
*/

const string Error::m_default_prefix = L"CDK Error: ";


void Error::description_materialize() const
{
  if (m_what)
    return;

  std::ostringstream buf;
  describe(buf);

  Error *self= const_cast<Error*>(this);
  self->m_what= new std::string(m_what_prefix);
  self->m_what->append(buf.str());
}



/*
  Generic error category
  ----------------------
  Error codes in this category map to CDK error conditions with the
  same numeric value. Error code description is the same as description
  of the corresponding error condition.
*/

class error_category_generic : public error_category
{
  error_category_generic() {}

  const char* name() const { return "cdk"; }
  std::string message(int) const;

  error_condition default_error_condition(int code) const
  { return error_condition(cdkerrc::code(code)); }

  bool  equivalent(int code, const error_condition &ec) const
  { return ec == default_error_condition(code); }

  friend const error_category& generic_error_category();
};


const error_category& generic_error_category()
{
  static const error_category_generic instance;
  return instance;
}


// Messages for generic errors

#define ERROR_CONDITION_CASE(C,N,D) \
  case cdkerrc::C: return std::string(D);

std::string error_category_generic::message(int code) const
{
  switch (code)
  {
   case 0: return "No error";
   ERROR_CONDITION_LIST(ERROR_CONDITION_CASE)
   default: return "Unknown error condition";
  }
}


/*
  Using boost error categories as CDK ones
  ----------------------------------------
*/

namespace boost {
  using namespace ::boost::system;
}


// Map boost error codes to CDK ones

const error_code boost_error_code(const boost::error_code &ec)
{
  if (ec.category() == boost::system_category())
    return error_code(ec.value(), system_error_category());

  if (ec.category() == boost::generic_category())
    return error_code(ec.value(), std_error_category());

  // Boost POSIX error category is deprecated.
  // if (ec.category() == boost::get_posix_category())
  //   return error_code(ec.value(), posix_error_category());

  // In case we have no category corresponding to the boost one,
  // we map boost error code to CDK generic error.

  return error_code(cdkerrc::generic_error);
}


const boost::error_code boost_error_code(const error_code &ec)
{
  if (ec.category() == system_error_category())
    return boost::error_code(ec.value(), boost::system_category());

  if (ec.category() == std_error_category())
    return boost::error_code(ec.value(), boost::generic_category());

  // Boost POSIX error category is deprecated.
  // if (ec.category() == posix_error_category())
  //   return boost::error_code(ec.value(), boost::get_posix_category());

  // If no good mapping exists, return "no error"

  return boost::error_code();
}


/*
  Adaptor for boost error categories.
*/

class boost_category_wrapper : public error_category
{
  const boost::error_category &m_cat;

protected:

  boost_category_wrapper(const boost::error_category &cat)
    : m_cat(cat)
  {}

  const char* name() const { return m_cat.name(); }
  std::string message(int code) const { return m_cat.message(code); }

  error_condition default_error_condition(int code) const;
  bool equivalent(int, const error_condition&) const;
};


/*
  Map to error conditions using mappings defined by boost error category
*/

error_condition boost_category_wrapper::default_error_condition(int code) const
{
  // Get default condition as dictated by boost category and
  // transform it to CDK one using boost_error_code()

  const boost::error_condition cond= m_cat.default_error_condition(code);

  error_code ec= boost_error_code(boost::error_code(cond.value(), cond.category()));

  return error_condition(ec.value(), ec.category());
}

bool boost_category_wrapper::equivalent(int code, const error_condition &ec) const
{
  // Use boost::system_category to decide the equivalence. For this to work,
  // given error_condition must be first transformed to boost error condition.

  boost::error_code bec = boost_error_code(ec);

  // If given error code does not map to any boost one, then boost_error_code()
  // returns "no error" code with 0 value. In this case we report false unless
  // code is 0 and ec also describes "no error" code.

  if (0 == bec.value())
    return 0 == code && 0 == ec.value();

  // If there was boost error code matching ec then use boost category
  // to decide equivalence.

  return m_cat.equivalent(code, boost::error_condition(bec.value(), bec.category()));
}


/*
  System error category
  ---------------------
  We use the corresponding boost category
*/

class error_category_system : public boost_category_wrapper
{
  error_category_system()
    : boost_category_wrapper(boost::system_category())
  {}

  friend const error_category& system_error_category();
};


const error_category& system_error_category()
{
  static const error_category_system instance;
  return instance;
}


/*
  Standard/POSIX error category
  -----------------------------
  We use boost "generic" and "posix" categories, respectively.
*/

class error_category_std : public boost_category_wrapper
{
  error_category_std()
    : boost_category_wrapper(boost::generic_category())
  {}
  const char* name() const { return "errc"; }
  friend const error_category& std_error_category();
};


const error_category& std_error_category()
{
  static const error_category_std instance;
  return instance;
}


class error_category_posix : public boost_category_wrapper
{
  error_category_posix()
    : boost_category_wrapper(boost::generic_category())
  {}
  const char* name() const { return "posix"; }
  friend const error_category& posix_error_category();
};

const error_category& posix_error_category()
{
  static const error_category_posix instance;
  return instance;
}



/*
  Wrapping external exceptions as CDK errors
  ------------------------------------------
*/

// Boost error wrapper

class Boost_error : public Error_class<Boost_error>
{
  boost::system_error m_err;

public:

  Boost_error(const boost::system_error &e)
    : Error_base(NULL, boost_error_code(e.code()))
    , m_err(e)
  {}

  void do_describe(std::ostream &out) const
  {
    out <<m_err.what();
    out <<" (" <<code() <<")";
  }
};


// Error class that wraps standard exception

class Std_exception : public Error
{

public:

  Std_exception(const std::exception &e)
    : Error(cdkerrc::standard_exception, e.what())
  {}

};


// Error class for unknown exceptions

class Unknown_exception : public Error
{
public:

  Unknown_exception() : Error(cdkerrc::unknown_exception)
  {}
};


void rethrow_error()
{
  try {
    throw;
  }
  catch (const Error&)
  {
    // CDK errors do not need any wrapping
    throw;
  }
  catch (const boost::system_error &e)
  {
    throw Boost_error(e);
  }
  catch (const std::exception &e)
  {
    throw Std_exception(e);
  }
  catch (...)
  {
    throw Unknown_exception();
  }
}


// Throwing POSIX and system errors


void throw_posix_error()
{
  if (errno)
    throw_error(errno, posix_error_category());
}


void throw_system_error()
{

#ifdef _WIN32
  int error= static_cast<int>(GetLastError());
#else
  int error= errno;
#endif

  if (error)
    throw_error(error, system_error_category());
}


bool error_code::operator== (errc::code x) const
{
  return *this == error_condition(x);
}

bool error_code::operator== (cdkerrc::code x) const
{
  return *this == error_code(x);
}

}} // sdk::foundation
