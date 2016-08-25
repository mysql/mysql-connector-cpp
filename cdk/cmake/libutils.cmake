# Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
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
if(POLICY CMP0026)
  cmake_policy(SET CMP0026 OLD)
endif()


#
# Add interface link libraries to a target, even if this is object
# library target.
#

function(lib_interface_link_libraries TARGET)

  get_target_property(target_type ${TARGET} TYPE)

  if(target_type STREQUAL "OBJECT_LIBRARY")
    set_property(TARGET ${TARGET}
      APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${ARGN}
    )
  else()
    target_link_libraries(${TARGET} INTERFACE ${ARGN})
  endif()

endfunction(lib_interface_link_libraries)


#
# Create static or shared library like add_library(). But library can be
# built not only from source files but also from other libraries (object
# or static).
#
# add_library_ex(TARGET TYPE
#   [<sources>]
#   [OBJECTS <object libs>]
#   [LIBS    <static libs>]
# )
#
# If static library is created then specified <static libs> are merged
# with the main library (so that resulting library does not depend on
# them at compile time).
#

function(add_library_ex TARGET)

  set(type)
  set(srcs)
  set(objs)
  set(libs)

  list(GET ARGN 0 arg)
  list(REMOVE_AT ARGN 0)

  if(arg STREQUAL "STATIC")
    #message("- creating static library")
    set(type "STATIC")
    list(GET ARGN 0 arg)
    list(REMOVE_AT ARGN 0)
  endif()

  if(arg STREQUAL "SHARED")
    #message("- creating shared library")
    set(type "SHARED")
    list(GET ARGN 0 arg)
    list(REMOVE_AT ARGN 0)
  endif()

  while(1)

    if(arg STREQUAL "OBJECTS" OR arg STREQUAL "LIBS")
      break()
    endif()

    #message("- processing source: ${arg}")
    list(APPEND srcs ${arg})

    if(NOT ARGN)
      break()
    endif()
    list(GET ARGN 0 arg)
    list(REMOVE_AT ARGN 0)

  endwhile()

  if(arg STREQUAL "OBJECTS")
  while(ARGN)

    list(GET ARGN 0 arg)
    list(REMOVE_AT ARGN 0)

    if(arg STREQUAL "LIBS")
      break()
    endif()

    #message("- processing object lib: ${arg}")
    list(APPEND objs ${arg})

  endwhile()
  endif()

  if(arg STREQUAL "LIBS")
  while(ARGN)

    list(GET ARGN 0 arg)
    list(REMOVE_AT ARGN 0)

    #message("- processing library: ${arg}")
    list(APPEND libs ${arg})

  endwhile()
  endif()

  foreach(obj ${objs})
    list(APPEND srcs $<TARGET_OBJECTS:${obj}>)
  endforeach()

  add_library(${TARGET} ${type} ${srcs})
  #message("- added ${type} library: ${TARGET}")

  foreach(obj ${objs})

    target_link_libraries(${TARGET}
      INTERFACE $<TARGET_PROPERTY:${obj},INTERFACE_LINK_LIBRARIES>
    )

    target_include_directories(${TARGET}
      INTERFACE $<TARGET_PROPERTY:${obj},INTERFACE_INCLUDE_DIRECTORIES>
    )

  endforeach()

  if(libs)
    if(${type} STREQUAL "STATIC")
        merge_static_libraries(${TARGET} ${libs})
        add_dependencies(${TARGET} ${libs})
    else()
      target_link_libraries(${TARGET} PRIVATE ${libs})
    endif()
  endif()

endfunction(add_library_ex)



GET_FILENAME_COMPONENT(MYSQL_CMAKE_SCRIPT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

#IF(WIN32 OR CYGWIN OR APPLE OR WITH_PIC OR DISABLE_SHARED OR NOT CMAKE_SHARED_LIBRARY_C_FLAGS)
# SET(_SKIP_PIC 1)
#ENDIF()


# Merge static libraries into a big static lib. The resulting library
# should not not have dependencies on other static libraries.
# We use it in MySQL to merge mysys,dbug,vio etc into mysqlclient

function(merge_static_libraries TARGET)

  set(libs ${ARGN})

  #
  # Location and name of the generated source file used to create
  # merged library.
  #
  set(source_file ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_depends.c)

  set(oslibs)
  set(libs_to_merge)

  foreach(lib ${libs})

    #message("- processing ${lib}")

    if(TARGET ${lib})

      get_target_property(lib_location ${lib} LOCATION)
      get_target_property(lib_type ${lib} TYPE)
      #message("-- target ${lib_type}: ${lib_location}")

      if(NOT lib_location)

         # 3rd party library like libz.so. Make sure that everything
         # that links to our library links to this one as well.

         list(APPEND oslibs ${lib})

      else()

        # This is a target in current project
        # (can be a static or shared lib)

        if(lib_type STREQUAL "STATIC_LIBRARY")
          list(APPEND libs_to_merge ${lib_location})
        else()
          # This is a shared library our static lib depends on.
          list(APPEND oslibs ${lib})
        endif()

      endif()

    else()

        #message("-- explicitly specified static library: ${lib}")
        list(APPEND libs_to_merge ${lib})

    endif()

  endforeach()

  message("Merging static libraries into ${TARGET}:")
  foreach(lib ${libs_to_merge})
    message(" - ${lib}")
  endforeach()

  if(oslibs)
    list(REMOVE_DUPLICATES oslibs)
    message(STATUS "Library ${TARGET} depends on OSLIBS ${oslibs}")
  endif()

  target_link_libraries(${TARGET} ${oslibs})

  #
  #  Now merge all the libraries into one.
  #

  if(MSVC)

    # To merge libs, just pass them to lib.exe command line via
    # STATIC_LIBRARY_FLAGS property.

    set(LINKER_EXTRA_FLAGS "")
    foreach(lib ${libs_to_merge})
      set(LINKER_EXTRA_FLAGS "${LINKER_EXTRA_FLAGS} ${lib}")
    endforeach()
    set_target_properties(${TARGET} PROPERTIES STATIC_LIBRARY_FLAGS
      "${LINKER_EXTRA_FLAGS}")

    # Disable "empty translation unit warning"

    target_compile_options(${TARGET} PRIVATE /wd4206)

    add_custom_target(merge-test
      COMMAND ${CMAKE_COMMAND}
        -D FOO="Ala ma kota"
        -P ${MYSQL_CMAKE_SCRIPT_DIR}/merge_archives.cmake
    )

  else()

    get_target_property(target_location ${TARGET} LOCATION)

    if(APPLE)

      # Use OSX's libtool to merge archives (it handles universal
      # binaries properly)

      # TODO: We hide errors about duplicate input source file names
      # in the library (this might be a problem when debugging).
      # TODO: find libtool instead of assuming its location

      add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND mv ${target_location} ${target_location}.main
        COMMAND /usr/bin/libtool 2>/dev/null -static -o ${target_location}
        ${target_location}.main ${libs_to_merge}
      )

    else()

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

    endif()
  endif()

endfunction()


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

  # In some versions of cmake dependent libraries are listed
  # in INTERFACE_LINK_LIBRARIES as $<LINK_ONLY:lib>. Detect
  # such entries here and extract the bare name of the dependent
  # library.

  string(REGEX MATCH "^\\$<LINK_ONLY:(.*)>$" link_only ${first})
  if (link_only)
    set(first ${CMAKE_MATCH_1})
  endif()

  if(TARGET ${first})
  get_target_property(LIBS ${first} INTERFACE_LINK_LIBRARIES)
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
