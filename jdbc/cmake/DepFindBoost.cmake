# Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0, as
# published by the Free Software Foundation.
#
# This program is also distributed with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation. The authors of MySQL hereby grant you an
# additional permission to link the program and your derivative works
# with the separately licensed software that they have included with
# MySQL.
#
# Without limiting anything contained in the foregoing, this file,
# which is part of MySQL Connector/C++, is also subject to the
# Universal FOSS Exception, version 1.0, a copy of which can be found at
# http://oss.oracle.com/licenses/universal-foss-exception.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License, version 2.0, for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

##########################################################################
#
# Input:
#
#  WITH_BOOST  - location where to look for Boost. If not set,
#                system-wide locations are searched
#
# Output:
#
#  BOOST_ROOT
#
#  Boost::boost  - library target to link against.
#

add_config_option(WITH_BOOST PATH "Boost install location")

if(NOT DEFINED BOOST_ROOT)
  set(BOOST_ROOT ${WITH_BOOST})
endif()


# Do not use system paths if user provided location.

if(BOOST_ROOT)
  set(Boost_NO_SYSTEM_PATHS ON)
endif()

set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS TRUE CACHE BOOL "Link with static Boost libraries")

if(NOT TARGET Boost::boost)

  # Note: FindBoost does not honor QUIET option
  find_package(Boost)

  # Older FindBoost modules do not create this target

  if(Boost_FOUND AND NOT TARGET Boost::boost)
    add_library(boost INTERFACE)
    target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIRS})
    add_library(Boost::boost ALIAS boost)
  endif()

endif()

if(NOT TARGET Boost::boost)
  message(FATAL_ERROR "Boost was not found. You can set WITH_BOOST to point at Boost install location.")
endif()

set(BOOST_ROOT "${Boost_INCLUDE_DIRS}" CACHE INTERNAL "")
