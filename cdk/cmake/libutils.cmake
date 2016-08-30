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
# Library utilities:
#
# - convenient building of libraries from object library targets
# - merging static libraries
#

get_filename_component(LIBUTILS_SCRIPT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
set(LIBUTILS_SCRIPT_DIR "${LIBUTILS_SCRIPT_DIR}/libutils")

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

  #
  #  Collect library sources, up to first OBJECTS or LIBS word
  #

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

  #
  #  Collect object libraries, if present
  #

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

  #
  #  Collect static libraries, if present
  #

  if(arg STREQUAL "LIBS")
  while(ARGN)

    list(GET ARGN 0 arg)
    list(REMOVE_AT ARGN 0)

    #message("- processing library: ${arg}")
    list(APPEND libs ${arg})

  endwhile()
  endif()

  #
  # We use different technology for building static library
  # from object libraries on Linux and on Windows or OSX.
  #
  # Because of the way we merge static libraries on Linux it is
  # important that all object files archived in a static library
  # have unique names. This might be not the case if we compose
  # the resulting library from several cmake object libraries. Two
  # different object libraries can have source file with the same
  # name and then the corresponding object files will also have
  # the same name. To go around this, instead of using object
  # libraries we use regular static libraries and then merge them
  # into output library. The static library merging infrastructure
  # ensures that object names are unique (see merge_archives.cmake.in).
  #
  # This is needed only for Linux builds. On Windows and OSX we have
  # tools for merging static libraries which can deal with non-unique
  # object names inside single library. On these platforms we prefer
  # directly using object libraries instead of merging output library
  # from static libs.
  #

  #
  # Add objects of each object library to the "sources" of the
  # output library. This is not done only if building static library
  # on Linux.
  #

  if(type STREQUAL "SHARED" OR MSVC OR APPLE)
    foreach(obj ${objs})
      list(APPEND srcs $<TARGET_OBJECTS:${obj}>)
    endforeach()
  endif()

  add_library(${TARGET} ${type} ${srcs})
  #message("- added ${type} library: ${TARGET}")

  foreach(obj ${objs})

    # If we are building static library on Linux, then for each object
    # library OOO a corresponding static library OOO_objs is created.
    # Then these static libraries are added to the list of libraries
    # that will be merged into the resulting library.

    if(type STREQUAL "STATIC" AND NOT MSVC AND NOT APPLE)
      message(- "adding static library: ${obj}_objs")
      add_library(${obj}_objs STATIC $<TARGET_OBJECTS:${obj}>)
      list(APPEND libs ${obj}_objs)
      add_dependencies(${TARGET} ${obj}_objs)
    endif()

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


#
# Infrastructure for merging static libraries
# ===========================================
#
# Static libraries that will be merged are prepared first. This is
# done by prepere_for_merge() macro. During preparations the library
# is copied to a known location (${MERGELIBS_DIR}) so that the Location
# is not dependent on build configuraton.
#
# On Linux individual objects are also extracted from the library archive.
# This is preliminary step for later merging of the object into single static
# library.
#
#

if(APPLE)
  # TODO: find libtool instead of assuming its location
  set(LIBTOOL_COMMAND /usr/bin/libtool CACHE INTERNAL "location of libtool")
endif()


#
# Merge static libraries into a big static lib. The resulting library
# should not not have dependencies on other static libraries.
# We use it in MySQL to merge mysys,dbug,vio etc into mysqlclient
#

function(merge_static_libraries TARGET)

  set(mergedir ${CMAKE_CURRENT_BINARY_DIR}/libmerge_${TARGET})

  if(MSVC)
    set(libs ${ARGN})
  else()
    set(libs ${TARGET} ${ARGN})
  endif()

  #
  # Prepare list of commands which copy each library file to a known location.
  #

  message("Merging static libraries into ${TARGET}:")
  set(copy_libs)
  foreach(lib ${libs})

    message(" - ${lib}")

    if(NOT TARGET ${lib})
      message(FATAL_ERROR "mergelibs: Trying to merge non target: ${lib}")
    endif()

    get_target_property(lib_type ${lib} TYPE)

    if(NOT lib_type STREQUAL "STATIC_LIBRARY")
      message(FATAL_ERROR "mergelibs: Trying to merge target which is not a static library: ${lib}")
    endif()

    list(APPEND copy_libs COMMAND ${CMAKE_COMMAND} -E copy
      $<TARGET_FILE:${lib}>
      ${mergedir}/${lib}/lib${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

  endforeach()

  #
  #  Now merge all the libraries into one.
  #

  if(MSVC)

    # To merge libs, just pass them to lib.exe command line via
    # STATIC_LIBRARY_FLAGS property.

    set(LINKER_EXTRA_FLAGS "")

    foreach(lib ${ARGN})
      set(LINKER_EXTRA_FLAGS
          "${LINKER_EXTRA_FLAGS} ${mergedir}/${lib}/lib.lib")
    endforeach()

    set_target_properties(${TARGET} PROPERTIES STATIC_LIBRARY_FLAGS
      "${LINKER_EXTRA_FLAGS}"
    )

    # The PRE_BUILD command copies libraries into the predefined location
    # before main library is linked.

    add_custom_command(TARGET ${TARGET} PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E remove_directory ${mergedir}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${mergedir}
      ${copy_libs}
      COMMENT "Preparing libraries for merging into: ${TARGET}"
    )

  else()

    #
    # Generate cmake script which handles merging of the libraries.
    # Apart from global cmake variables, script template uses the follwoing
    # additional variables:
    #
    # STATIC_LIBS  -- static library targets to be merged
    # MERGELIBS_DIR -- location where libraries are prepared for merging
    #

    set(STATIC_LIBS ${libs})
    set(MERGELIBS_DIR ${mergedir})

    CONFIGURE_FILE(
      ${LIBUTILS_SCRIPT_DIR}/merge_archives.cmake.in
      ${CMAKE_CURRENT_BINARY_DIR}/merge_archives_${TARGET}.cmake
      @ONLY
    )

    # The POST_BUILD action on the merged library target copies merged
    # libraries to the predefined location and invokes the merge script.

    #message("- copy libs: ${copy_libs}")

    add_custom_command(TARGET ${TARGET} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E remove_directory ${mergedir}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${mergedir}
      ${copy_libs}
      COMMAND ${CMAKE_COMMAND}
        -D TARGET_LOCATION=$<TARGET_FILE:${TARGET}>
        -P ${CMAKE_CURRENT_BINARY_DIR}/merge_archives_${TARGET}.cmake
      COMMENT "Merging libraries into: ${TARGET}"
    )

  endif()

endfunction()
