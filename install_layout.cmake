# Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
#
# The MySQL Connector/C++ is licensed under the terms of the GPLv2
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
# MySQL Connectors. There are special exceptions to the terms and
# conditions of the GPLv2 as it is applied to this software, see the
# FLOSS License Exception
# <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published
# by the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
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

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(IS64BIT 1)
else()
  set(IS64BIT 0)
endif()

if(IS64BIT)
  set(INSTALL_LIB_DIR "lib64")
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

