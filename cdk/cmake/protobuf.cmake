# Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
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
#  [USE_FULL_PROTOBUF()]
#
#  MYSQLX_PROTOBUF_GENERATE_CPP(SRCS HDRS <protobuf definitions>)
#  ADD_LIBRARY(target ... ${SRCS})
#

#
# Configure Protobuf project in protobuf subfolder of build location.
#

message("Configuring Protobuf build using cmake generator: ${CMAKE_GENERATOR}")
file(REMOVE ${PROJECT_BINARY_DIR}/protobuf/CMakeCache.txt)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/protobuf)

# If specified, use the same build type for Protobuf

if(CMAKE_BUILD_TYPE)
  set(set_build_type -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND}
          -G "${CMAKE_GENERATOR}"
          ${set_build_type}
          -DSTATIC_MSVCRT=${STATIC_MSVCRT}
          -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE}
          ${PROJECT_SOURCE_DIR}/protobuf
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/protobuf
  RESULT_VARIABLE protobuf_config
)

if(protobuf_config)
  message(FATAL_ERROR "Could not configure Protobuf build: ${protobuf_config}")
endif()

include(${PROJECT_BINARY_DIR}/protobuf/exports.cmake)

#
# Protobuf library targets imported above (pb_protobuf
# and pb_protobuf-lite) are local to the directory from which
# they were imported. This is not good if cdk is used as
# a sub-project of a parent project, because the parent project
# must have access to these targets.
#
# For that reason blow we create global protobuf/protobuf-lite targets
# and copy their loactions from the imported targets.
#

add_library(protobuf STATIC IMPORTED GLOBAL)
add_library(protobuf-lite STATIC IMPORTED GLOBAL)

foreach(lib protobuf protobuf-lite)
  #message("processing: ${lib}")

   foreach(CONF NOCONFIG DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    #message("- CONF: ${CONF}")

    get_target_property(LOC pb_${lib} IMPORTED_LOCATION_${CONF})
    if(LOC)
      #message("- settig imported location to: ${LOC}")
      set(location "${LOC}")
      set_target_properties(${lib} PROPERTIES
        IMPORTED_LOCATION_${CONF} "${LOC}"
      )
      set_property(TARGET ${lib} APPEND PROPERTY
        IMPORTED_CONFIGURATIONS ${CONF})
    endif()

  endforeach(CONF)

  # For multi-configuration builders like MSVC, set a generic
  # location of the form <prefix>/$(Configuration)/<name> which will
  # work with any configuration choosen at build time.
  # It is constructed from one of per-configurartion locations
  # determined above and saved in ${location}. The logic assumes
  # that the per-configration location is of the form
  # <prefix>/<config>/<name>

  if(CMAKE_CONFIGURATION_TYPES)

    get_filename_component(name "${location}" NAME)
    get_filename_component(LOC "${location}" PATH)
    get_filename_component(LOC "${LOC}" PATH)
    set(LOC "${LOC}/$(Configuration)/${name}")

    message("- setting generic location to: ${LOC}")
    set_target_properties(${lib} PROPERTIES
      IMPORTED_LOCATION "${LOC}"
    )

  endif()

endforeach(lib)


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
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/protobuf
    COMMENT "Building protobuf using configuration: $<CONFIGURATION>"
  )
else()
  add_custom_command(OUTPUT ${PROTOBUF_BUILD_STAMP}
    COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG>
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/protobuf
    COMMENT "Building protobuf using configuration: $<CONFIG>"
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
      DEPENDS ${ABS_FIL} ${PROTOBUF_PROTOC_EXECUTABLE}
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

