# Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
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
# Code below reads LOCATION property of targets. This has been deprecated
# in later cmake versions. To remove this policy more thought is needed how
# to deal with target locations (which can not be reliably determined at
# build configuration time).
#
if(CMP0026)
  cmake_policy(SET CMP0026 OLD)
endif()

# This file exports macros that emulate some functionality found  in GNU libtool
# on Unix systems. One such feature is convenience libraries. In this context,
# convenience library is a static library that can be linked to shared library
# On systems that force position-independent code, linking into shared library
# normally requires compilation with a special flag (often -fPIC). To enable
# linking static libraries to shared, we compile source files that come into
# static library with the PIC flag (${CMAKE_SHARED_LIBRARY_C_FLAGS} in CMake)
# Some systems, like Windows or OSX do not need special compilation (Windows
# never uses PIC and OSX always uses it).
#
# The intention behind convenience libraries is simplify the build and to reduce
# excessive recompiles.

# Except for convenience libraries, this file provides macros to merge static
# libraries (we need it for mysqlclient) and to create shared library out of
# convenience libraries(again, for mysqlclient)

# Following macros are exported
# - ADD_CONVENIENCE_LIBRARY(target source1...sourceN)
# This macro creates convenience library. The functionality is similar to
# ADD_LIBRARY(target STATIC source1...sourceN), the difference is that resulting
# library can always be linked to shared library
#
# - MERGE_LIBRARIES(target [STATIC|SHARED|MODULE]  [linklib1 .... linklibN]
#  [EXPORTS exported_func1 .... exported_func_N]
#  [OUTPUT_NAME output_name]
# This macro merges several static libraries into a single one or creates a shared
# library from several convenience libraries

# Important global flags
# - WITH_PIC : If set, it is assumed that everything is compiled as position
# independent code (that is CFLAGS/CMAKE_C_FLAGS contain -fPIC or equivalent)
# If defined, ADD_CONVENIENCE_LIBRARY does not add PIC flag to compile flags
#
# - DISABLE_SHARED: If set, it is assumed that shared libraries are not produced
# during the build. ADD_CONVENIENCE_LIBRARY does not add anything to compile flags


GET_FILENAME_COMPONENT(MYSQL_CMAKE_SCRIPT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
IF(WIN32 OR CYGWIN OR APPLE OR WITH_PIC OR DISABLE_SHARED OR NOT CMAKE_SHARED_LIBRARY_C_FLAGS)
 SET(_SKIP_PIC 1)
ENDIF()


# Write content to file, using CONFIGURE_FILE
# The advantage compared to FILE(WRITE) is that timestamp
# does not change if file already has the same content
MACRO(CONFIGURE_FILE_CONTENT content file)
 SET(CMAKE_CONFIGURABLE_FILE_CONTENT
  "${content}\n")
 CONFIGURE_FILE(
  ${MYSQL_CMAKE_SCRIPT_DIR}/configurable_file_content.in
  ${file}
  @ONLY)
ENDMACRO()

# Merge static libraries into a big static lib. The resulting library
# should not not have dependencies on other static libraries.
# We use it in MySQL to merge mysys,dbug,vio etc into mysqlclient

MACRO(MERGE_STATIC_LIBS TARGET OUTPUT_NAME LIBS_TO_MERGE)

  # To produce a library we need at least one source file.
  # It is created by ADD_CUSTOM_COMMAND below and will
  # also help to track dependencies.

  SET(SOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_depends.c)
  ADD_LIBRARY(${TARGET} STATIC ${SOURCE_FILE})
  SET_TARGET_PROPERTIES(${TARGET} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})

  GET_DEPENDENT_LIBS("${LIBS_TO_MERGE}" LIBS)
  #message("dependent libs: ${LIBS}")

  SET(OSLIBS)
  SET(ORIGINAL ${LIBS_TO_MERGE})

  FOREACH(LIB ${LIBS})

    IF(TARGET ${LIB})

      GET_TARGET_PROPERTY(LIB_LOCATION ${LIB} LOCATION)
      GET_TARGET_PROPERTY(LIB_TYPE ${LIB} TYPE)
      #message("- processing ${LIB} (${LIB_TYPE}): ${LIB_LOCATION}")

      IF(NOT LIB_LOCATION)

         # 3rd party library like libz.so. Make sure that everything
         # that links to our library links to this one as well.

         LIST(APPEND OSLIBS ${LIB})

      ELSE()

        # This is a target in current project
        # (can be a static or shared lib)

        IF(LIB_TYPE STREQUAL "STATIC_LIBRARY")
          LIST(APPEND STATIC_LIBS ${LIB_LOCATION})
          ADD_DEPENDENCIES(${TARGET} ${LIB})
        ELSE()
          # This is a shared library our static lib depends on.
          LIST(APPEND OSLIBS ${LIB})
        ENDIF()

      ENDIF()

    ELSE()

      #
      # If non-target library was passed as input, assume it is
      # a static library that should be merged in, otherwise treat
      # non-targets as system libs
      #
      # Note: macro arguments like LIBS_TO_MERGE can not be used
      # with list(FIND ...)
      #

      LIST(FIND ORIGINAL ${LIB} pos)
      IF(pos LESS 0)
        LIST(APPEND OSLIBS ${LIB})
      ELSE()
        LIST(APPEND STATIC_LIBS ${LIB})
      ENDIF()

    ENDIF()

  ENDFOREACH()

  MESSAGE("Merging static libraries into ${TARGET}:")
  FOREACH(LIB ${STATIC_LIBS})
    MESSAGE(" - ${LIB}")
  ENDFOREACH()

  IF(OSLIBS)
    LIST(REMOVE_DUPLICATES OSLIBS)
    TARGET_LINK_LIBRARIES(${TARGET} ${OSLIBS})
    MESSAGE(STATUS "Library ${TARGET} depends on OSLIBS ${OSLIBS}")
  ENDIF()

  # Make the generated dummy source file depended on all static input
  # libs. If input lib changes,the source file is touched
  # which causes the desired effect (relink).

  ADD_CUSTOM_COMMAND(
    OUTPUT  ${SOURCE_FILE}
    COMMAND ${CMAKE_COMMAND}  -E touch ${SOURCE_FILE}
    DEPENDS ${STATIC_LIBS})

  IF(MSVC)
    # To merge libs, just pass them to lib.exe command line.
    SET(LINKER_EXTRA_FLAGS "")
    FOREACH(LIB ${STATIC_LIBS})
      SET(LINKER_EXTRA_FLAGS "${LINKER_EXTRA_FLAGS} ${LIB}")
    ENDFOREACH()
    SET_TARGET_PROPERTIES(${TARGET} PROPERTIES STATIC_LIBRARY_FLAGS
      "${LINKER_EXTRA_FLAGS}")
  ELSE()
    GET_TARGET_PROPERTY(TARGET_LOCATION ${TARGET} LOCATION)
    IF(APPLE)
      # Use OSX's libtool to merge archives (ihandles universal
      # binaries properly)
      ADD_CUSTOM_COMMAND(TARGET ${TARGET} POST_BUILD
        COMMAND rm ${TARGET_LOCATION}
        COMMAND /usr/bin/libtool -static -o ${TARGET_LOCATION}
        ${STATIC_LIBS}
      )
    ELSE()
      # Generic Unix, Cygwin or MinGW. In post-build step, call
      # script, that extracts objects from archives with "ar x"
      # and repacks them with "ar r"
      SET(TARGET ${TARGET})
      CONFIGURE_FILE(
        ${MYSQL_CMAKE_SCRIPT_DIR}/merge_archives_unix.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/merge_archives_${TARGET}.cmake
        @ONLY
      )
      ADD_CUSTOM_COMMAND(TARGET ${TARGET} POST_BUILD
        COMMAND rm ${TARGET_LOCATION}
        COMMAND ${CMAKE_COMMAND} -P
        ${CMAKE_CURRENT_BINARY_DIR}/merge_archives_${TARGET}.cmake
      )
    ENDIF()
  ENDIF()
ENDMACRO()


#
# Recursively extend given list of targets with all libraries
# on which given targets depend (as given by LINK_LIBRARIES target
# property).
#

FUNCTION(GET_DEPENDENT_LIBS targets result)

  # Get dependencies of the first target in the list

  list(GET targets 0 first)
  list(REMOVE_AT targets 0)
  set(LIBS)

  if(TARGET ${first})
  get_target_property(LIBS ${first} LINK_LIBRARIES)
  endif()
  #message("- processing ${first}: ${LIBS}")

  # Add LIBS to the list of remaining targets and
  # call this function recursively to process the new
  # list.

  if(LIBS)
    list(APPEND targets ${LIBS})
  endif(LIBS)

  if(targets)
    #message("rec call: ${targets}")
    GET_DEPENDENT_LIBS("${targets}" ret)
    #message("rec ret: ${ret}")
  endif()

  # Insert the first target back to the result and
  # return it.

  list(INSERT ret 0 ${first})
  list(REMOVE_DUPLICATES ret)
  set(${result} ${ret} PARENT_SCOPE)

ENDFUNCTION(GET_DEPENDENT_LIBS)

