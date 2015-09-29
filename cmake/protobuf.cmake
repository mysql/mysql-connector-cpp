# Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
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

#
# Usage:
#
#  INCLUDE(protobuf)
#  SETUP_PROTOBUF()
#
#  ADD_LIBRARY(target ...)
#  ADD_PROTOBUF(target foo.proto bar.proto ...)
#
# This will create new library "target_pb" with compiled protobuf code. The original
# target "target" will depend on this "target_pb" library.
#

if(TARGET protobuf)
  return()
endif()

#
# Location where to look for compiled protobuf library. It can be location of binary
# distribution or location where protobuf was built from sources. If not given, system
# default locations will be searched.
#

SET(WITH_PROTOBUF $ENV{WITH_PROTOBUF} CACHE PATH "Protobuf location")

IF(WITH_PROTOBUF)

  IF(MSVC AND EXISTS ${WITH_PROTOBUF}/vsprojects)

    IF(NOT PROTOBUF_SRC_ROOT_FOLDER)
      SET(PROTOBUF_SRC_ROOT_FOLDER "${WITH_PROTOBUF}")
    ENDIF()

    FIND_PROGRAM(PROTOBUF_PROTOC_EXECUTABLE
      NAMES protoc
      DOC "The Google Protocol Buffers Compiler"
      PATHS ${WITH_PROTOBUF}/bin
      NO_DEFAULT_PATH
    )

  ELSE()

    FIND_PATH(PROTOBUF_INCLUDE_DIR
      google/protobuf/service.h
      PATH ${WITH_PROTOBUF}/include
      NO_DEFAULT_PATH
    )

    # Set prefix
    IF(MSVC)
      SET(PROTOBUF_ORIG_FIND_LIBRARY_PREFIXES "${CMAKE_FIND_LIBRARY_PREFIXES}")
      SET(CMAKE_FIND_LIBRARY_PREFIXES "lib" "")
    ENDIF()

    IF(WITH_STATIC_LINKING)
      SET(ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
      IF(NOT WIN32)
        SET(CMAKE_FIND_LIBRARY_SUFFIXES .a)
      ELSE()
        SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib)
      ENDIF()
    ENDIF()

    FIND_LIBRARY(PROTOBUF_LIBRARY
      NAMES protobuf
      PATHS ${WITH_PROTOBUF}/lib ${WITH_PROTOBUF}/lib/sparcv9 ${WITH_PROTOBUF}/lib/amd64
      NO_DEFAULT_PATH
    )
    
    IF(WITH_STATIC_LINKING)
      SET(CMAKE_FIND_LIBRARY_SUFFIXES ${ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
    ENDIF()    

    # Restore original find library prefixes
    IF(MSVC)
      SET(CMAKE_FIND_LIBRARY_PREFIXES "${PROTOBUF_ORIG_FIND_LIBRARY_PREFIXES}")
    ENDIF()

    FIND_PROGRAM(PROTOBUF_PROTOC_EXECUTABLE
      NAMES protoc
      DOC "The Google Protocol Buffers Compiler"
      PATHS ${WITH_PROTOBUF}/bin
      NO_DEFAULT_PATH
    )

  ENDIF()

ENDIF()

FIND_PACKAGE(Protobuf "2.6.0" REQUIRED)

IF(NOT PROTOBUF_FOUND)
  MESSAGE(FATAL_ERROR "Protobuf could not be found")
ENDIF()

#MESSAGE("PROTOBUF_INCLUDE_DIRS: ${PROTOBUF_INCLUDE_DIRS}")
#MESSAGE("PROTOBUF_LIBRARIES: ${PROTOBUF_LIBRARIES}")

#
# Import Protobuf library as imported target so that it can
# participate in merging project libraries.
#
# TODO: Make sure that per-configuration variants of the library
# (debug/release) are correctly used when merging.
#

set(pb_lib_type "optimized")

foreach(LIB ${PROTOBUF_LIBRARIES})

  if(LIB MATCHES "^(optimized|debug)$")

   set(pb_lib_type ${LIB})

  else()

    #message("- processing ${pb_lib_type} pb lib: ${LIB}")
    if(EXISTS ${LIB})
      list(APPEND pb_libs_${pb_lib_type} ${LIB})
    else()
      list(APPEND pb_oslibs ${LIB})
    endif()

  endif()

endforeach()

add_library(protobuf STATIC IMPORTED GLOBAL)
set_target_properties(protobuf PROPERTIES
  IMPORTED_LOCATION "${pb_libs_optimized}"
  IMPORTED_LOCATION_DEBUG "${pb_libs_debug}"
)
target_link_libraries(protobuf LINK_INTERFACE_LIBRARIES ${pb_oslibs})


MACRO(SETUP_PROTOBUF)
  INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIRS})
ENDMACRO()

#
# Usage: ADD_PROTOBUF(<target> <list of protobuf definition files>)
#

MACRO(ADD_PROTOBUF target)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
  #MESSAGE("Generating protobuf sources from: ${ARGN}")
  PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${ARGN})
  #MESSAGE("Generated sources: ${PROTO_SRCS}")
  #MESSAGE("Generated headers: ${PROTO_HDRS}")
  set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)
  IF(UNIX)
    set_source_files_properties(${PROTO_SRCS} PROPERTIES COMPILE_FLAGS "-w")
  ELSE(WIN32)
    set_source_files_properties(${PROTO_SRCS} PROPERTIES COMPILE_FLAGS "/wd4018 /wd4996")
  ENDIF()
  SOURCE_GROUP(Protobuf FILES ${PROTO_SRCS} ${PROTO_HDRS})

  ADD_LIBRARY(${target}_pb STATIC ${PROTO_SRCS} ${PROTO_HDRS})
  TARGET_LINK_LIBRARIES(${target}_pb protobuf)
  TARGET_LINK_LIBRARIES(${target} ${target}_pb)

ENDMACRO()
