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
# used to build the connector:
#
#  {lib,lib64}/mysqlcppconnX-vsNN.dll              <-- shared library
#  {lib,lib64}/vsNN/mysqlcppconnX-static.lib       <-- static with /MD
#  {lib,lib64}/vsNN/mysqlcppconnX-static-mt.lib    <-- static with /MT
#  {lib,lib64}/vsNN/mysqlcppconnX.lib              <-- import library for DLL
#
# On Linux it is as follows, where A.B is the API version number
#
#  {lib,lib64}/libmysqlcppconnX.so.A.B             <-- shared library
#  {lib,lib64}/libmysqlcppconnX.so.A               <-- soname link
#  {lib,lib64}/libmysqlcppconnX.so                 <-- development link
#  {lib,lib64}/libmysqlcppconnX-static.a           <-- static library
#
# Additionally, if connector is built in debug mode, the libraries are installed
# in debug/ subfolder of {lib,lib64}/ or {lib,lib64}/vsNN/.
#
# Note: We expect VS variable to hold the "vsNN" bit on Windows.
#

if(NOT DEFINED IS64BIT)
  message(FATAL_ERROR "IS64BIT not defined!")
endif()

if(IS64BIT)
  if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    set(INSTALL_LIB_DIR "lib")
  else()
    set(INSTALL_LIB_DIR "lib64")
  endif()
else()
  set(INSTALL_LIB_DIR "lib")
endif()


set(INSTALL_LIB_DIR_STATIC "${INSTALL_LIB_DIR}")
if(VS)
  set(INSTALL_LIB_DIR_STATIC "${INSTALL_LIB_DIR_STATIC}/${VS}")
endif()

set(INSTALL_INCLUDE_DIR include)
set(INSTALL_DOC_DIR doc)

#
# Store layout settings in the cache.
#

set(INSTALL_INCLUDE_DIR "${INSTALL_INCLUDE_DIR}"
  CACHE INTERNAL "Install location for include headers"
)

set(INSTALL_DOC_DIR "${INSTALL_DOC_DIR}"
  CACHE INTERNAL "Install location for documentation files"
)

set(INSTALL_LIB_DIR "${INSTALL_LIB_DIR}"
  CACHE INTERNAL "Library install location (relative to install root)"
)

set(INSTALL_LIB_DIR_STATIC "${INSTALL_LIB_DIR_STATIC}"
  CACHE INTERNAL "Install location for static libraries (relative to install root)"
)

