# Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

# We want boost 1.56.0 in order to build our boost/geometry code.
# The boost tarball is fairly big, and takes several minutes
# to download. So we recommend downloading/unpacking it
# only once, in a place visible from any bzr sandbox.
# We use only header files, so there should be no binary dependencies.

# Downloading the tarball takes about 5 minutes here at the office.
# Here are some size/time data for unpacking the tarball on my desktop:
#  size tarball-name
#  67M boost_1_55_0.tar.gz  unzipping headers    ~2 seconds 117M
#                           unzipping everything ~3 seconds 485M
# 8,8M boost_headers.tar.gz unzipping everything <1 second

MACRO(CHANGE_MD_2_MT)
  # Changes Runtime from Multithreaded DLL to Multithreaded static to be compatible with the default build runtime of MySql Server on Windows.
  foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
     if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
     endif(${flag_var} MATCHES "/MD")
  endforeach(flag_var)
ENDMACRO()