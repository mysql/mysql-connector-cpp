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
#  add_ext(name)
#
#  Add external cmake project with sources at extra/name.
#  Adds targets (re)build-name that build or re-build this
#  external project.
#
#  Note: add_ext_targets() can be used to import targets from such external
#  project
#

function(add_ext NAME)

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

endfunction(add_ext)

if(NOT TARGET rebuild-ext)
  add_custom_target(rebuild-ext)
  set_target_properties(rebuild-ext PROPERTIES FOLDER "Misc")
endif()


##########################################################################
#
# add_ext_targets(ext name1 tgt1 name2 tgt2 ...)
#
# Defines targets ext::nameK that refer to targets tgtK exported from the
# external project ext.
#
# The external build targets should be exported to file exports.cmake in the
# external build location. Alternatively they can be defined as imported
# targets prior to calling add_ext_targets() (in which case ext is ignored
# but still build-ext will be invoked if it is defined).
#
# The ext::XXX targets will trigger build-ext target when used (if it is
# defined) so that external build is performed before the imported libraries
# are used.
#

function(add_ext_targets EXT)

  set(ext_dir ${CMAKE_CURRENT_BINARY_DIR}/${EXT})

  while(ARGN)

    list(GET ARGN 0 name)
    list(GET ARGN 1 tgt)
    list(REMOVE_AT ARGN 0 1)

    if(NOT TARGET ${tgt} AND EXISTS ${ext_dir}/exports.cmake)
      include(${ext_dir}/exports.cmake)
    endif()

    if(NOT TARGET ${tgt})
      message(FATAL_ERROR "Could not import target ${tgt}")
    endif()

    get_target_property(configs ${tgt} IMPORTED_CONFIGURATIONS)
    get_target_property(type ${tgt} TYPE)

    if(type MATCHES "LIBRARY")
      #message("importing library ${tgt} as ext::${name}")

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
      #message("importing executable ${tgt} as ext::${name}")

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

  endwhile(ARGN)

endfunction(add_ext_targets)
