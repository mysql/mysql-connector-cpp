# Copyright (c) 2021, Oracle and/or its affiliates. All rights reserved.
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

###########################################################################
#
# Commands:
#
# bootstrap()
#
#   Generate build configuration cache to be used by external cmake builds.
#   This should be called before the main project() directive.
#
# init_ext_build(DIR)
#
#   Prepare the given directory for an external build. If configuration cache
#   was generated with bootstrap() then copies the files to the new build
#   location to speed up cmake configuration process. Otherwise does nothing.
#
#   Note: A later cmake invocation in DIR must use build options compatible
#   with the main project (as stored in the cache).
#
# TODO:
#  - Make it work for Ninja builds
#


set(BOOTSTRAP_SRC ${CMAKE_CURRENT_LIST_DIR}/bootstrap CACHE INTERNAL "bootstrap project location")
set(BOOTSTRAP_CACHE ${CMAKE_BINARY_DIR}/platform-cache CACHE INTERNAL "bootstrap project location")

function(bootstrap)
  message("=== Booststrap ===")

  set(bin_dir ${BOOTSTRAP_CACHE})
  set(src_dir ${BOOTSTRAP_SRC})

  #message("bin: ${bin_dir}")
  #message("src: ${src_dir}")

  file(MAKE_DIRECTORY  ${bin_dir})

  message("-- generator: ${CMAKE_GENERATOR}")
  set(cmake_opts)

  if(CMAKE_GENERATOR_PLATFORM)
    message("-- platform: ${CMAKE_GENERATOR_PLATFORM}")
    list(APPEND cmake_opts "-A" ${CMAKE_GENERATOR_PLATFORM})
  endif()

  if(CMAKE_GENERATOR_TOOLSET)
    message("-- toolset: ${CMAKE_GENERATOR_TOOLSET}")
    list(APPEND cmake_opts "-T" ${CMAKE_GENERATOR_TOOLSET})
  endif()

  if(CMAKE_BUILD_TYPE)
    message("-- build type: ${CMAKE_BUILD_TYPE}")
    list(APPEND cmake_opts "-D" "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
  endif()

  message("-- ----")

  execute_process(
    COMMAND ${CMAKE_COMMAND}
    -G ${CMAKE_GENERATOR}
    ${cmake_opts}
    ${src_dir}
    WORKING_DIRECTORY ${bin_dir}
    RESULT_VARIABLE res
  )

  #message(FATAL_ERROR "!!! STOP")

  if(res)
    file(REMOVE_RECURSE ${BOOTSTRAP_CACHE})
    message(WARNING
      "Bootstrap failed, not using it"
    )
    return()
  endif()

  # Copy cmake files that contain detected toolchain info

  file(COPY ${bin_dir}/CMakeFiles DESTINATION ${CMAKE_BINARY_DIR})

  # Write patched version of bootstrap cache that can be used directly
  # with other projects

  file(STRINGS ${bin_dir}/CMakeCache.txt cache_lines REGEX "^[^\;]*$")

  list_filter(
    cache_lines
    "^$|CMAKE_CACHEFILE_DIR|CMAKE_HOME_DIRECTORY|CMAKE_INSTALL_PREFIX"
  )

  file(REMOVE ${bin_dir}/CMakeCache.txt)
  foreach(line ${cache_lines})
    file(APPEND ${bin_dir}/CMakeCache.txt "${line}\n")
  endforeach()

  # Get list of internal variables in the generated cache

  set(internal_vars)
  foreach(line ${cache_lines})
    if(line MATCHES "^([^:]*):INTERNAL=")
      set(var ${CMAKE_MATCH_1})
      # Note: These need to be skipped
      if(NOT var MATCHES "CMAKE_CACHEFILE_DIR|CMAKE_HOME_DIRECTORY")
        #message("- internal: ${var}")
        list(APPEND internal_vars ${var})
      endif()
    endif()
  endforeach()

  # Load bootstrap cache into this project (must include all internal
  # variables)

  load_cache(${bin_dir}  INCLUDE_INTERNALS ${internal_vars})

  message("=== Booststrap done ===")
endfunction(bootstrap)


function(init_ext_build BIN_DIR)

  if(NOT EXISTS ${BOOTSTRAP_CACHE})
    return()
  endif()

  #file(REMOVE_RECURSE ${BIN_DIR}/CMakeCache.txt ${BIN_DIR}/CMakeFiles)
  if(EXISTS ${BIN_DIR}/CMakeCache.txt)
    return()
  endif()
  file(COPY ${BOOTSTRAP_CACHE}/CMakeFiles DESTINATION ${BIN_DIR})
  file(COPY ${BOOTSTRAP_CACHE}/CMakeCache.txt DESTINATION ${BIN_DIR})

endfunction(init_ext_build)


# Note: list(FILTER ...) available only in cmake 3.6+

macro(list_filter list filter)
if(NOT CMAKE_VERSION VERSION_LESS 3.6)

  list(FILTER ${list} EXCLUDE REGEX ${filter})

else()

  set(list_filter_out)
  while(${list})
    list(GET ${list} 0 line)
    list(REMOVE_AT ${list} 0)
    if(NOT line MATCHES ${filter})
      list(APPEND list_filter_out ${line})
    else()
      message("--- filtering out: ${line}")
    endif()
  endwhile()
  set(${list} ${list_filter_out})

endif()
endmacro()
