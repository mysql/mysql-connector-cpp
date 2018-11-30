# Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0, as
# published by the Free Software Foundation.
#
# This program is also distributed with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation.  The authors of MySQL hereby grant you an
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


#
# Pick install location for the main library
# ------------------------------------------
#
# On Windows the install layout is as follows, where NN is the MSVC version
# used to build the connector, A is the major ABI version:
#
#  {lib,lib64}/mysqlcppconn-A-vsNN.dll            <-- shared library
#  {lib,lib64}/vsNN/mysqlcppconn-static.lib       <-- static with /MD
#  {lib,lib64}/vsNN/mysqlcppconn-static-mt.lib    <-- static with /MT
#  {lib,lib64}/vsNN/mysqlcppconn.lib              <-- import library for DLL
#
# On Linux it is as follows, where X.Y.Z is the connector version
#
#  {lib,lib64}/libmysqlcppconn.so.A.X.Y.Z         <-- shared library
#  {lib,lib64}/libmysqlcppconn.so.A               <-- soname link
#  {lib,lib64}/libmysqlcppconn.so                 <-- development link
#  {lib,lib64}/libmysqlcppconn-static.a          <-- static library
#
# Additionally, if connector is built in debug mode, the libraries are installed
# in debug/ subfolder of {lib,lib64}/ or {lib,lib64}/vsNN/.
#
# Note: We expect VS variable to hold the "vsNN" bit on Windows.
#

if(jdbc_stand_alone)
  # TODO: Manage install locations for stand-alone build.
  return()
endif()


if(NOT INSTALL_INCLUDE_DIR OR NOT INSTALL_LIB_DIR OR NOT INSTALL_DOC_DIR)

  message(FATAL_ERROR
    "Install locations not set when configuring legacy connector."
    " This should not happen!"
  )

endif()


set(INSTALL_INCLUDE_DIR "${INSTALL_INCLUDE_DIR}/jdbc")


#
# Library names
#
# The library name base is mysqlcppconn. Static library has -static
# suffix added to the base name.
#
# On Windows we add major ABI version to the shared library name, so that
# different ABI versions of the library can be installed next to each other.
# Also, on Windows we distinguish the MSVC version used to build the library
# (as this determines the runtime version). The shared libraries use
# -vsNN suffix, the import library does not have the suffix but is installed
# to a vsNN/ subfolder of the library install location (see install layout
# below). For static libraries, we add -mt suffix if it is linked with
# static runtime.
#

set(LIB_NAME_BASE "mysqlcppconn")
set(LIB_NAME_STATIC "${LIB_NAME_BASE}-static")

if(WIN32 AND STATIC_MSVCRT)
  set(LIB_NAME_STATIC "${LIB_NAME}-mt")
endif()

if(BUILD_STATIC)

  set(LIB_NAME ${LIB_NAME_STATIC})

else()

  set(LIB_NAME "${LIB_NAME_BASE}")
  if(WIN32)
    set(LIB_NAME "${LIB_NAME}-${JDBC_ABI_VERSION_MAJOR}")
  endif()
  if(VS)
    set(LIB_NAME "${LIB_NAME}-${VS}")
  endif()

endif()

