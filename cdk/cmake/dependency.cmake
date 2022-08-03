# Copyright (c) 2008, 2021, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0, as
# published by the Free Software Foundation.
#
# This program is also distributed with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation. The authors of MySQL hereby grant you an
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

#include(CMakeParseArguments)
include(ProcessorCount)

##########################################################################
#
# find_dependency(XXX) command.
#
# Currently it is looking for DepFindXXX.cmake script that should perform
# all steps required to locate given dependency and make it available in
# the project. It is a layer on top of find_module().
#

function(find_dependency NAME)

  # TODO: Fallback to find_module()

  include(DepFind${NAME})

endfunction(find_dependency)


# Variables to forward from this build configuration to external
# build configuration

# TODO: complete the list

set(EXT_FWD
  CMAKE_BUILD_TYPE
  CMAKE_SYSTEM_NAME CMAKE_SYSTEM_VERSION
  CMAKE_SYSTEM_PROCESSOR
  CMAKE_C_COMPILER CMAKE_CXX_COMPILER
)

set(EXT_DIR ${CMAKE_CURRENT_LIST_DIR}/ext CACHE INTERNAL "external project utils location")

##########################################################################
#
# add_ext(name header [tgt_name tgt])
#
# Add an external dependency(such as a 3rd party library) with name `name`.
# There are two cases(where NNN is the specified name) :
#
# 1. Either location of the dependency is given using WTTH_NNN or related
#    options (see below), or
#
# 2. Dependency is built from bundled sources located at extra/NNN in the source
#    tree if no options are specified.
#
# Parameter`header` names a file to be located at the dependency location to
# check
#
# if it is correct. Variable `NNN_FOUND` is set to true/false to indicate if
# dependency was correctly located.
#
# To use the dependency in the project some targets need to be created for it
# using `add_ext_targets() `.
#
# If`tgt_name` and `tgt` parameters are given then it is assumed that the
# dependency is a single library and the target is created for it already here
# by a call to `add_ext_targets(name LIBRARY tgt_name tgt) `.
#
# If the dependency is built from bundled sources, targets `NNN-build` and
# `NNN-rebuild` are created to build or re-build the dependency if needed.
#

function(add_ext NAME HEADER)
  string(TOUPPER ${NAME} EXT_LIB)
  add_config_option(WITH_${EXT_LIB} STRING "Enable, disable or point to ${EXT_LIB} installation.")
  add_config_option(${EXT_LIB}_DIR PATH "Path to ${EXT_LIB} instalation dir.")
  add_config_option(${EXT_LIB}_ROOT_DIR PATH "Path to ${EXT_LIB} instalation dir.")
  add_config_option(${EXT_LIB}_INCLUDE_DIR PATH "Path to ${EXT_LIB} include directory.")
  add_config_option(${EXT_LIB}_LIB_DIR PATH "Path to ${EXT_LIB} lib directory.")
  add_config_option(${EXT_LIB}_BIN_DIR PATH "Path to ${EXT_LIB} bin directory.")
  add_config_option(${EXT_LIB}_LIBRARY STRING "${EXT_LIB} library name")

  set(${EXT_LIB}_FOUND "1" PARENT_SCOPE)

  # If defined, will use external third party dependencies
  if(DEFINED WITH_${EXT_LIB} OR
    DEFINED ${EXT_LIB}_DIR OR
    DEFINED ${EXT_LIB}_ROOT_DIR OR
    DEFINED ${EXT_LIB}_INCLUDE_DIR OR
    DEFINED ${EXT_LIB}_LIB_DIR OR
    DEFINED ${EXT_LIB}_LIBRARY)

    if(DEFINED WITH_${EXT_LIB} AND NOT WITH_${EXT_LIB})
      set(${EXT_LIB}_FOUND "0" PARENT_SCOPE)
      return()
    endif()

    if(DEFINED WITH_${EXT_LIB} AND NOT WITH_${EXT_LIB} STREQUAL "system" AND EXISTS ${WITH_${EXT_LIB}})
      if(DEFINED ${EXT_LIB}_ROOT_DIR)
        message(FATAL_ERROR "Can't specify both WITH_${EXT_LIB} and ${EXT_LIB}_ROOT_DIR")
      endif()

      set(${EXT_LIB}_ROOT_DIR ${WITH_${EXT_LIB}})
    elseif(DEFINED ${EXT_LIB}_DIR)
      if(DEFINED ${EXT_LIB}_ROOT_DIR)
        message(FATAL_ERROR "Can't specify both ${NAME}_DIR and ${NAME}_ROOT_DIR")
      endif()

      set(${EXT_LIB}_ROOT_DIR ${${EXT_LIB}_DIR})
    endif()

    set(${EXT_LIB}_ROOT_DIR ${${EXT_LIB}_ROOT_DIR} PARENT_SCOPE)

    if(DEFINED ${EXT_LIB}_ROOT_DIR AND(DEFINED ${EXT_LIB}_INCLUDE_DIR OR DEFINED ${EXT_LIB}_LIB_DIR OR DEFINED ${EXT_LIB}_LIBRARY))
      message(FATAL_ERROR "Can't specify both ${EXT_LIB} root dir and include/libs parameters")
    endif()

    if(DEFINED ${EXT_LIB}_LIBRARY AND NOT ${ARGC} GREATER 2)
      message(FATAL_ERROR "${EXT_LIB}_LIB_DIR can't be used on ${EXT_LIB} since it has multiple libs")
    endif()

    # Let's find the header
    if(DEFINED ${EXT_LIB}_ROOT_DIR)
      set(suffix PATHS ${${EXT_LIB}_ROOT_DIR}
        PATH_SUFFIXES include
        NO_DEFAULT_PATH
      )

    elseif(DEFINED ${EXT_LIB}_INCLUDE_DIR)
      set(suffix PATHS ${${EXT_LIB}_INCLUDE_DIR}
        NO_DEFAULT_PATH
      )
    endif()

    unset(${EXT_LIB}-include CACHE)

    find_path(${EXT_LIB}-include
      NAMES ${HEADER}
      ${suffix}
      REQUIRED
    )

    message(STATUS "${EXT_LIB} INCLUDES : ${${EXT_LIB}-include}")

  else()
    set(src_dir ${PROJECT_SOURCE_DIR}/extra/${NAME})
    set(bin_dir ${CMAKE_CURRENT_BINARY_DIR}/${NAME})

    file(MAKE_DIRECTORY ${bin_dir})

    #
    # Copy cmake cache and internal files to avoid expensive platform
    # detection/checks (as external projects are built on the same platform
    # as the main project)
    #

    if(COMMAND init_ext_build)
      init_ext_build(${bin_dir})
    endif()

    set(cmake_opts)

    message("== configuring external build of ${NAME}")
    message("-- sources at: ${src_dir}")
    message("-- generator: ${CMAKE_GENERATOR}")

    # Note: if we initialized build location above, there is no need to pass
    # toolset/platform options in cmake invocation (the information is
    # already in the cache). In fact, in this situation cmake errors out if
    # these options are passed.

    if(NOT COMMAND init_ext_build)
      if(CMAKE_GENERATOR_TOOLSET)
        message("-- toolset: ${CMAKE_GENERATOR_TOOLSET}")
        list(APPEND cmake_opts "-T ${CMAKE_GENERATOR_TOOLSET}")
      endif()

      if(CMAKE_GENERATOR_PLATFORM)
        message("-- platform: ${CMAKE_GENERATOR_PLATFORM}")
        list(APPEND cmake_opts "-A ${CMAKETO_GENERATOR_PLATFORM}")
      endif()

    endif()

    foreach(var ${EXT_FWD})
      if(${var})
        message("-- option ${var}: ${${var}}")
        list(APPEND cmake_opts -D${var}=${${var}})
      endif()
    endforeach()

    message("-- ----")

    execute_process(
      COMMAND ${CMAKE_COMMAND}
      -G ${CMAKE_GENERATOR} ${cmake_opts}
      -Wno-dev --no-warn-unused-cli
      ${src_dir}
      WORKING_DIRECTORY ${bin_dir}
      RESULT_VARIABLE res
    )

    if(res)
      message(FATAL_ERROR
        "Failed to configure external build of ${NAME}: ${res}"
      )
    endif()

    message("== done configuring external build of ${NAME}")

    # Option to use parallel builds (much faster!)
    # Note: ninja does that by default
    set(build_opt)
    ProcessorCount(prc_cnt)

    if(prc_cnt AND NOT CMAKE_VERSION VERSION_LESS 3.12)
      list(APPEND build_opt --parallel ${prc_cnt})
    endif()

    add_custom_target(${NAME}-build
      COMMAND ${CMAKE_COMMAND}
      -DBIN_DIR=${bin_dir}
      -DCONFIG=$<CONFIG>
      -DOPTS=${build_opt}
      -P ${EXT_DIR}/ext-build.cmake

      #COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG> ${build_opt}
      #COMMAND ${CMAKE_COMMAND} -E touch ${bin_dir}/build.$<CONFIG>.stamp
      #COMMENT "building ${NAME}"
      WORKING_DIRECTORY ${bin_dir}
    )

    set_target_properties(${NAME}-build PROPERTIES FOLDER "Misc")

    add_custom_target(${NAME}-rebuild
      COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG> --clean-first ${build_opt}
      COMMAND ${CMAKE_COMMAND} -E touch ${bin_dir}/build.$<CONFIG>.stamp
      COMMENT "re-building ${NAME}"
      WORKING_DIRECTORY ${bin_dir}
    )

    set_target_properties(${NAME}-rebuild PROPERTIES FOLDER "Misc")

    add_dependencies(rebuild-ext ${NAME}-rebuild)
  endif()

  if(${ARGC} GREATER 2)
    add_ext_targets(${NAME} LIBRARY ${ARGN})
  endif()
endfunction(add_ext)

if(NOT TARGET rebuild-ext)
  add_custom_target(rebuild-ext)
  set_target_properties(rebuild-ext PROPERTIES FOLDER "Misc")
endif()

##########################################################################
#
# add_ext_lib(EXT target name)
#
# Expects include defined on ${EXT}-include and creates target ext::{target}
# pointing to library with ${name} and located on ${EXT}_ROOT_DIR (suffixes: lib
# lib64 dll) or ${${EXT}_LIB_DIR}.
#
function(add_ext_lib EXT target name)
  # Search for the library
  if(DEFINED ${EXT}_ROOT_DIR)
    set(suffix PATHS ${${EXT}_ROOT_DIR}
      PATH_SUFFIXES lib lib64 dll
      NO_DEFAULT_PATH
    )
  elseif(DEFINED ${EXT}_LIB_DIR)
    set(suffix
      PATHS ${${EXT}_LIB_DIR}
      NO_DEFAULT_PATH
    )
  endif()

  unset(library CACHE)

  find_library(library
    NAMES ${name} lib${name}
    ${suffix}
    REQUIRED
  )

  message(STATUS "${EXT} LIBRARY : ${library}")

  # if(NOT EXISTS ${${EXT}_include} OR NOT EXISTS ${library})
  #   message(FATAL_ERROR "Couldn't find ${EXT} library.")
  # endif()

  add_library(ext::${target} SHARED IMPORTED GLOBAL)
  set_target_properties(ext::${target} PROPERTIES
    IMPORTED_LOCATION ${library}
    IMPORTED_IMPLIB ${library}
    INTERFACE_INCLUDE_DIRECTORIES ${${EXT}-include})
endfunction(add_ext_lib)

##########################################################################
#
# add_ext_exec(EXT target name)
# Creates target ext::{target} pointing to executable with ${name} and located
# on ${EXT}_ROOT_DIR (suffixes: bin) or ${${EXT}_BIN_DIR}.
#
function(add_ext_exec EXT target name)
  # Search for the library
  if(DEFINED ${EXT}_ROOT_DIR)
    set(suffix PATHS ${${EXT}_ROOT_DIR}
      PATH_SUFFIXES bin
      NO_DEFAULT_PATH
    )

  elseif(DEFINED ${EXT}_BIN_DIR)
    set(suffix PATHS ${${EXT_LIB}_BIN_DIR}
      PATH_SUFFIXES bin
      NO_DEFAULT_PATH
    )
  endif()

  find_program(executable
    NAMES ${name}
    ${suffix}
    NO_CACHE
  )

  if(NOT executable)
    message(FATAL_ERROR "Couldn't find ${name} executable.")
  endif()

  message(STATUS "${EXT} BINARY: ${executable}")

  add_executable(ext::${target} IMPORTED GLOBAL)
  set_target_properties(ext::${target} PROPERTIES
    IMPORTED_LOCATION ${executable})
endfunction(add_ext_exec)

##########################################################################
#
# add_ext_targets(ext type1 name1 tgt1 type2 name2 tgt2 ...)
#
# Definestargets `ext::nameK` for an external dependency `ext` that was created
# with `add_ext(ext ...) `. The target can refer to a LIBRARY or EXECUTABLE,
# as given by the `typeK` parameter.
#
# If the external dependency is at locations specified using the corresponding
# WITH_X(or alternative) options then it refers to a library/executable with the
# name `nameK` found in these locations(and `tgtK` argument is ignored). For
# example `add_ext_targets(zlib LIBRARY z ...) ` will create target `ext::z`
# referring to library `libz` found in the external locations specified using
# WITH_ZLIB.
#
# If the dependency is built from bundled sources, then created target
# `ext::nameK` refers to the build target `tgtK` that should be exported to file
# `exports.cmake` by the bundled dependency build system. In the example of
# `add_ext_targets(zlib LIBRARY z ext_zlib) `, in case zlib is built from
# bundled sources the created `ext::z` target will correspond to the target
# `ext_zlib` exported by the bundled zlib build system.
#
# In the case of dependency built from bundled sources the `ext::nameK` target
# will trigger `build-ext` target when used (if it is defined) so that external
# build is performed before the imported libraries are used.

function(add_ext_targets EXT)
  string(TOUPPER ${EXT} EXT_LIB)
  set(ext_dir ${CMAKE_CURRENT_BINARY_DIR}/${EXT})

  while(ARGN)
    list(GET ARGN 0 type)
    list(GET ARGN 1 name)
    list(GET ARGN 2 tgt)
    list(REMOVE_AT ARGN 0 1 2)

    # If defined, will use external third party dependencies
    if(NOT TARGET ${EXT}-build)
      set(target ${name})

      if(DEFINED ${EXT_LIB}_LIBRARY)
        set(name ${${EXT_LIB}_LIBRARY})
      endif()

      if(type STREQUAL "LIBRARY")
        add_ext_lib(${EXT_LIB} ${target} ${name})
      elseif(type STREQUAL "EXECUTABLE")
        add_ext_exec(${EXT_LIB} ${target} ${name})
      endif()

    else()

      # otherwise, will use bundled code
      if(NOT TARGET ${tgt} AND EXISTS ${ext_dir}/exports.cmake)
        include(${ext_dir}/exports.cmake)
      endif()

      if(NOT TARGET ${tgt})
        message(FATAL_ERROR "Could not import target ${tgt}")
      endif()

      get_target_property(configs ${tgt} IMPORTED_CONFIGURATIONS)
      get_target_property(type ${tgt} TYPE)

      if(type MATCHES "LIBRARY")
        # message("importing library ${tgt} as ext::${name}")
        add_library(ext::${name} INTERFACE IMPORTED GLOBAL)
        target_link_libraries(ext::${name} INTERFACE ${tgt})

        if(TARGET ${EXT}-build)
          add_dependencies(ext::${name} ${EXT}-build)
        endif()

        # Touch imported locations which do not exist before external project
        # is built. This is needed for ninja ...
        if(CMAKE_GENERATOR MATCHES "Ninja")
          foreach(conf ${configs})
            get_target_property(loc ${tgt} IMPORTED_LOCATION_${conf})

            if(loc)
              execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${loc})
            endif()
          endforeach(conf)
        endif()
      endif()

      if(type MATCHES "EXECUTABLE")
        # message("importing executable ${tgt} as ext::${name}")
        add_executable(ext::${name} IMPORTED GLOBAL)

        if(TARGET ${EXT}-build)
          add_dependencies(ext::${name} ${EXT}-build)
        endif()

        foreach(conf ${configs})
          get_target_property(loc ${tgt} IMPORTED_LOCATION_${conf})

          if(loc)
            set_target_properties(ext::${name}
              PROPERTIES IMPORTED_LOCATION_${conf} ${loc}
            )
          endif()
        endforeach(conf)
      endif()
    endif()
  endwhile(ARGN)
endfunction(add_ext_targets)
