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
# On MacOS we need install_name_tool to do rpath mangling (see below)
#

if(APPLE)

  find_program(INSTALL_NAME_TOOL install_name_tool)

  # If available, otool is used to show runtime dependencies for libraries we
  # build

  find_program(OTOOL otool)

  if(NOT INSTALL_NAME_TOOL)
    message(FATAL_ERROR
      "Could not find install_name_tool required to buld Connector/C++"
    )
  endif()

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


function(lib_link_libraries TARGET)

  get_target_property(target_type ${TARGET} TYPE)

  if(target_type STREQUAL "OBJECT_LIBRARY")
    set_property(TARGET ${TARGET}
      APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${ARGN}
    )
  else()
    target_link_libraries(${TARGET} PRIVATE ${ARGN})
  endif()

endfunction(lib_link_libraries)


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
  # Propagate RPATH_MANGLE property from compound libraries to the target.
  #

  set(mangle_paths)

  foreach(xx ${objs} ${libs})
    get_target_property(xx_mangle_paths ${xx} RPATH_MANGLE)
    #message("== checking mangle paths of ${xx}: ${xx_mangle_paths}")
    if(xx_mangle_paths)
      list(APPEND mangle_paths ${xx_mangle_paths})
    endif()
  endforeach()

  if(mangle_paths)
    list(REMOVE_DUPLICATES mangle_paths)
    #message("== collected mangle paths: ${mangle_paths}")
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

  if(mangle_paths)
    set_property(TARGET ${TARGET} PROPERTY RPATH_MANGLE ${mangle_paths})
  endif()

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
      PRIVATE $<TARGET_PROPERTY:${obj},INTERFACE_LINK_LIBRARIES>
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

  #
  # Perform rpath mangling on MacOS (see below).
  #

  if(${type} STREQUAL "SHARED" AND APPLE)
    mangle_osx_rpaths(${TARGET})
  endif()

endfunction(add_library_ex)


#
# Perform MacOS rptah mangling.
#
# For some libraries we depend on, such as openSSL, we want the link name
# stored in our library to be of the form @rpath/<library name> instead of
# being a fixed path to the location where such external dependencies were
# found at build time.
#
# If target we build has MACOSX_RPATH and RPATH_MANGLE properties set, then
# we perform rpath mangling for that target. The RPATH_MANGLE property is
# a list of link names that should be mangled, that is, the directory prefix
# of the link name should be replaced by @rpath.
#
# We use install_name_tool to change link names stored in the target. For more
# information about @rpath see: <https://developer.apple.com/library/content/documentation/DeveloperTools/Conceptual/DynamicLibraries/100-Articles/RunpathDependentLibraries.html#//apple_ref/doc/uid/TP40008306-SW1>
#

function(mangle_osx_rpaths TARGET)

  if (NOT APPLE)
    return()
  endif()

  get_target_property(use_rpaths ${TARGET} MACOSX_RPATH)

  if (NOT use_rpaths)
    return()
  endif()

  #message("rpath mangling for target: ${TARGET}")

  get_target_property(paths_to_mangle ${TARGET} RPATH_MANGLE)

  set(mangle_commands)

  foreach(path ${paths_to_mangle})
    #message("magling path: ${path}")
    get_filename_component(lib_name "${path}" NAME)
    list(APPEND mangle_commands
      COMMAND ${INSTALL_NAME_TOOL} -change "\"${path}\"" "\"@rpath/${lib_name}\"" "\"$<TARGET_FILE:${TARGET}>\""
    )
  endforeach()

  # If otool is available, also list final dependencies after the mangling
  if(OTOOL)
    list(APPEND mangle_commands COMMAND ${OTOOL} -L "\"$<TARGET_FILE:${TARGET}>\"")
  endif()

  #message("mangle commands: ${mangle_commands}")

  # Invoke mangling commands as a POST_BUILD step for the target.

  add_custom_command(TARGET ${TARGET} POST_BUILD
    ${mangle_commands}
    COMMENT "runtime dependencies of ${TARGET}:"
  )

endfunction(mangle_osx_rpaths)


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

# An IMPORTED library can also be merged.
function(add_imported_library target location)
  ADD_LIBRARY(${target} STATIC IMPORTED)
  SET_TARGET_PROPERTIES(${target} PROPERTIES IMPORTED_LOCATION ${location})
endfunction()
