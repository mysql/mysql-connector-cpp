# Copyright (c) 2010, 2023, Oracle and/or its affiliates. All rights reserved.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0,
# as published by the Free Software Foundation.
#
# This program is also distributed with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation.  The authors of MySQL hereby grant you an additional
# permission to link the program and your derivative works with the
# separately licensed software that they have included with MySQL.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License, version 2.0, for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

#-------------- FIND WIX_DIR ------------------
# First check if WIX_DIR was given in the command line
IF(NOT DEFINED WIX_DIR)
  # If WIX_DIR is not given in command line check env
  IF(DEFINED $ENV{WIX_DIR})
    SET(WIX_DIR "$ENV{WIX_DIR}")
  ELSE()
    SET(WIX_DIR "$ENV{USERPROFILE}\\.dotnet\\tools")
  ENDIF(DEFINED $ENV{WIX_DIR})
ENDIF(NOT DEFINED WIX_DIR)

#----------------- FIND WIX EXECUTABLE -------------------
FIND_PROGRAM(WIX_EXECUTABLE wix ${WIX_DIR})

IF (EXISTS ${WIX_EXECUTABLE})
  MESSAGE(STATUS "Wix found in ${WIX_DIR}")
ELSE ()
  IF ($ENV{WIX_DIR})
    MESSAGE(FATAL_ERROR "Cannot find Wix in $ENV{WIX_DIR}")
  ELSE ($ENV{WIX_DIR})
    MESSAGE(FATAL_ERROR "Cannot find Wix in ${WIX_DIR}. Please set environment variable WIX_DIR which points to the wix installation directory")
  ENDIF ($ENV{WIX_DIR})
ENDIF ()

