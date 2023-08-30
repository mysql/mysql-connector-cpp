# Copyright (c) 2010, 2023, Oracle and/or its affiliates. All rights reserved.
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
# The lines above are intentionally left blank

#-------------- FIND WIX_DIR ------------------
IF(DEFINED $ENV{WIX_DIR})
  SET(WIX_DIR "$ENV{WIX_DIR}")
ELSE()
  SET(WIX_DIR "$ENV{USERPROFILE}\\.dotnet\\tools")
ENDIF(DEFINED $ENV{WIX_DIR})

#----------------- FIND WIX EXECUTABLE -------------------
IF (EXISTS "${WIX_DIR}\\wix.exe")
	MESSAGE(STATUS "Wix found in ${WIX_DIR}")
ELSE ()
	IF ($ENV{WIX_DIR})
		MESSAGE(FATAL_ERROR "Cannot find Wix in $ENV{WIX_DIR}")
	ELSE ($ENV{WIX_DIR})
		MESSAGE(FATAL_ERROR "Cannot find Wix in ${WIX_DIR}. Please set environment variable WIX_DIR which points to the wix installation directory")
	ENDIF ($ENV{WIX_DIR})
ENDIF ()

FIND_PROGRAM(WIX_EXECUTABLE wix ${WIX_DIR})