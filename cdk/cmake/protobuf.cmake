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
# Usage:
#
#  INCLUDE(protobuf)
#  [USE_FULL_PROTOBUF()]
#
#  MYSQLX_PROTOBUF_GENERATE_CPP(SRCS HDRS <protobuf definitions>)
#  ADD_LIBRARY(target ... ${SRCS})
#

if(DEFINED WITH_PROTOBUF)

  #
  # If WITH_PROTOBUF is defined, it should point at external location where
  # protobuf libraries were built using our CMakeLists.txt (so that
  # exports.cmake was produced).
  #

  if (NOT EXISTS "${WITH_PROTOBUF}/exports.cmake")
    message(FATAL_ERROR
      "Valid protobuf build not found at the given location"
      " (could not find exports.cmake): ${WITH_PROTOBUF}"
    )
  endif()

  message("Using protobuf build at: ${WITH_PROTOBUF}")

else(DEFINED WITH_PROTOBUF)

  #
  # If external WITH_PROTOBUF location is not given, then we arrange for
  # building of protbuf from bundled sources in ${PROJECT_BINARY_DIR}/protobuf.
  #

  message("Configuring Protobuf build using cmake generator: ${CMAKE_GENERATOR}")
  file(REMOVE "${PROJECT_BINARY_DIR}/protobuf/CMakeCache.txt")
  file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/protobuf")

  #
  # Pick build configuration for the protobuf build. Normally we build using the
  # same build configuration that is used for building CDK (Release/Debug/etc.).
  # But we also support building CDK under non-standard build configuration
  # named 'Static' (this is a dirty trick we use to simplify building our MSIs).
  # Since protobuf does not know 'Static' build configuration, we build protobuf
  # under 'Release' configuration in that case.
  #
  # We need to handle two cases. For some build systems, like Makefiles,
  # the build configuration is specified at cmake time using CMAKE_BUILD_TYPE
  # variable. In that case we also set it during protobuf build configuration.
  # Another case is a multi-configuration build system like MSVC. In this case
  # we use generator expression to pick correct  configuration when the build
  # command is invoked below.
  #

  if(CMAKE_BUILD_TYPE)
    if(CMAKE_BUILD_TYPE MATCHES "[Ss][Tt][Aa][Tt][Ii][Cc]")
      set(set_build_type -DCMAKE_BUILD_TYPE=Release)
    else()
      set(set_build_type -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
    endif()
  endif()

  set(CONFIG_EXPR
    $<$<CONFIG:Static>:Release>$<$<NOT:$<CONFIG:Static>>:$<CONFIG>>
  )

  execute_process(
    COMMAND ${CMAKE_COMMAND}
            -G "${CMAKE_GENERATOR}"
            ${set_build_type}
            -DSTATIC_MSVCRT=${STATIC_MSVCRT}
            -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
            -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
            -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
            -DCMAKE_CXX_FLAGS_RELWITHDEBINFO=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
            -DCMAKE_CXX_FLAGS_MINSIZEREL=${CMAKE_CXX_FLAGS_MINSIZEREL}
            -DCMAKE_STATIC_LINKER_FLAGS=${CMAKE_STATIC_LINKER_FLAGS}
            ${PROJECT_SOURCE_DIR}/protobuf
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/protobuf
    RESULT_VARIABLE protobuf_config
  )

  if(protobuf_config)
    message(FATAL_ERROR "Could not configure Protobuf build: ${protobuf_config}")
  endif()

  set(WITH_PROTOBUF "${PROJECT_BINARY_DIR}/protobuf")

endif(DEFINED WITH_PROTOBUF)

# Import targets exported by protobuf

include(${WITH_PROTOBUF}/exports.cmake)

#
# Protobuf library targets imported above (pb_protobuf
# and pb_protobuf-lite) are local to the directory from which
# they were imported. This is not good if cdk is used as
# a sub-project of a parent project, because the parent project
# must have access to these targets.
#
# For that reason below we create global protobuf/protobuf-lite targets
# and copy their locations from the imported targets.
#
# Note: we can't use ALIAS library because it does not work with imported
# targets
#

add_library(protobuf STATIC IMPORTED GLOBAL)
add_library(protobuf-lite STATIC IMPORTED GLOBAL)

foreach(lib protobuf protobuf-lite)
  #message("processing: ${lib}")

   foreach(CONF NOCONFIG DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    #message("- CONF: ${CONF}")

    get_target_property(LOC pb_${lib} IMPORTED_LOCATION_${CONF})
    if(LOC)
      #message("- setting imported location to: ${LOC}")
      set_target_properties(${lib} PROPERTIES
        IMPORTED_LOCATION_${CONF} "${LOC}"
      )
      set_property(TARGET ${lib} APPEND PROPERTY
        IMPORTED_CONFIGURATIONS ${CONF}
      )
    endif()

  endforeach(CONF)

endforeach(lib)

#
# To support 'Static' build configuration the targets imported from the
# Protobuf project need to have IMPORTED_LOCATION_STATIC defined. We use
# 'Release' locations as Protobuf is built using 'Release' configuration in
# that case.
#

foreach(tgt protobuf protobuf-lite pb_protoc)

  get_target_property(LOC ${tgt} IMPORTED_LOCATION_RELEASE)
  set_property(TARGET ${tgt} PROPERTY IMPORTED_LOCATION_STATIC ${LOC})

endforeach(tgt)


# protobuf depends on protobuf-lite

set_target_properties(protobuf PROPERTIES
  INTERFACE_LINK_LIBRARIES "protobuf-lite"
)


message("Protobuf include path: ${PROTOBUF_INCLUDE_DIR}")

set(PROTOBUF_INCLUDE_DIRS ${PROTOBUF_INCLUDE_DIR}
    CACHE INTERNAL "Protobuf include path" FORCE)
set(PROTOBUF_PROTOC_EXECUTABLE pb_protoc
    CACHE INTERNAL "Protobuf compiler" FORCE)

#
# Custom target build_protobuf ensures that Protobuf project is built.
#
# Note: this depends on Protobuf project generating the stamp file
#

if(NOT DEFINED PROTOBUF_BUILD_STAMP)
  message(FATAL_ERROR "Protobuf build stamp file not defined")
endif()


if(CMAKE_VERSION VERSION_LESS 3.0)
  add_custom_command(OUTPUT ${PROTOBUF_BUILD_STAMP}
    COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIGURATION>
    WORKING_DIRECTORY ${WITH_PROTOBUF}
    COMMENT "Building protobuf using configuration: $(Configuration)"
  )
else()
  add_custom_command(OUTPUT ${PROTOBUF_BUILD_STAMP}
    COMMAND ${CMAKE_COMMAND} --build . --config ${CONFIG_EXPR}
    WORKING_DIRECTORY ${WITH_PROTOBUF}
    COMMENT "Building protobuf using configuration: $(Configuration)"
  )
endif()

add_custom_target(build_protobuf
  DEPENDS ${PROTOBUF_BUILD_STAMP}
)

add_dependencies(protobuf build_protobuf)
add_dependencies(protobuf-lite build_protobuf)
add_dependencies(pb_protoc build_protobuf)

#
#  Choice between full and lite version of the library.
#

option(PROTOBUF_LITE "Using lite version of Protobuf library" ON)
mark_as_advanced(PROTOBUF_LITE)

MACRO(USE_FULL_PROTOBUF)
  set(PROTOBUF_LITE OFF CACHE BOOL "Using lite version of Protobuf library" FORCE)
ENDMACRO()

# Standard PROTOBUF_GENERATE_CPP modified to our usage

FUNCTION(MYSQLX_PROTOBUF_GENERATE_CPP SRCS HDRS)
  IF(NOT ARGN)
    MESSAGE(SEND_ERROR
      "Error: MYSQLX_PROTOBUF_GENERATE_CPP() called without any proto files")
    RETURN()
  ENDIF()

  SET(srcs)
  SET(hdrs)

  FOREACH(FIL ${ARGN})
    GET_FILENAME_COMPONENT(ABS_FIL ${FIL} ABSOLUTE)
    GET_FILENAME_COMPONENT(FIL_WE ${FIL} NAME_WE)
    GET_FILENAME_COMPONENT(ABS_PATH ${ABS_FIL} PATH)

    LIST(APPEND srcs "${CMAKE_CURRENT_BINARY_DIR}/protobuf/${FIL_WE}.pb.cc")
    LIST(APPEND hdrs "${CMAKE_CURRENT_BINARY_DIR}/protobuf/${FIL_WE}.pb.h")

    ADD_CUSTOM_COMMAND(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/protobuf/${FIL_WE}.pb.cc"
             "${CMAKE_CURRENT_BINARY_DIR}/protobuf/${FIL_WE}.pb.h"
      COMMAND ${CMAKE_COMMAND}
            -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/protobuf"
      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --cpp_out "${CMAKE_CURRENT_BINARY_DIR}/protobuf"
           -I ${ABS_PATH} ${ABS_FIL}
           --proto_path=${PROJECT_SOURCE_DIR}/protobuf/protobuf-2.6.1/src
      DEPENDS ${ABS_FIL} #${PROTOBUF_PROTOC_EXECUTABLE}
      COMMENT "Running C++ protocol buffer compiler (${PROTOBUF_PROTOC_EXECUTABLE}) on ${FIL}"
      VERBATIM)

  ENDFOREACH()

  SET_SOURCE_FILES_PROPERTIES(
    ${srcs} ${hdrs}
    PROPERTIES GENERATED TRUE)

  #
  # Disable compile warnings in code generated by Protobuf
  #

  IF(UNIX)
    set_source_files_properties(${srcs}
      APPEND_STRING PROPERTY COMPILE_FLAGS "-w"
    )
  ELSE(WIN32)
    set_source_files_properties(${srcs}
      APPEND_STRING PROPERTY COMPILE_FLAGS
      "/W1 /wd4018 /wd4996 /wd4244 /wd4267"
    )
  ENDIF()


  SET(${SRCS} ${srcs} PARENT_SCOPE)
  SET(${HDRS} ${hdrs} PARENT_SCOPE)

ENDFUNCTION()

