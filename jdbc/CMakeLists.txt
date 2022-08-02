# Copyright (c) 2008, 2022, Oracle and/or its affiliates.
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


PROJECT(MYSQLCPPCONN)
cmake_minimum_required(VERSION 3.1)

if(POLICY CMP0003)
  cmake_policy(SET CMP0003 NEW)
endif()

if(POLICY CMP0007)
  cmake_policy(SET CMP0007 NEW)
endif()

if(POLICY CMP0015)
  cmake_policy(SET CMP0015 NEW)
endif()

if(POLICY CMP0028)
  cmake_policy(SET CMP0028 NEW)
endif()

#-----------------

include(cmake/setup.cmake)

# Prefer cmake code from parent project, if present
include(../cmake/setup.cmake OPTIONAL)

include(platform)
include(config_options)


#
# Detect if we are configured as stand-alone project, or sub-project.
#

IF (PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
  MESSAGE("Stand-alone configuration")
  MESSAGE("Building on system: ${CMAKE_SYSTEM}")
  SET(jdbc_stand_alone 1)
ELSE()
  SET(jdbc_stand_alone 0)
  INCLUDE(SubProject.cmake OPTIONAL)
ENDIF()

#-----------------

if(jdbc_stand_alone)

  add_config_option(BUILD_STATIC BOOL DEFAULT OFF
    "Build static connector library instead of dynamic one"
  )

  add_config_option(BUNDLE_DEPENDENCIES BOOL ADVANCED DEFAULT OFF
    "copy dependencies to install location"
  )

endif(jdbc_stand_alone)



add_config_option(MYSQLCLIENT_STATIC_LINKING BOOL ADVANCED DEFAULT ON "enable libmysqlclient static linking")
add_config_option(MYSQLCLIENT_STATIC_BINDING BOOL ADVANCED DEFAULT ON "enable libmysqlclient static binding")



if(BUILD_STATIC)
  set(BUILD_SHARED_LIBS OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()

set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS} CACHE INTERNAL "")


#-----------------
#  Version info
#

if(jdbc_stand_alone)

  message(WARNING "Using fake version info for legacy Connector/C++")

  set(CONCPP_VERSION_MAJOR 8)
  set(CONCPP_VERSION_MINOR 0)
  set(CONCPP_VERSION_MICRO 0)
  set(CONCPP_VERSION_LEVEL "")
  set(JDBC_ABI_VERSION_MAJOR 8)

endif(jdbc_stand_alone)

configure_file(
  "VersionInfo.cmake.in"
  "${PROJECT_BINARY_DIR}/VersionInfo.cmake"
  @ONLY
)

INCLUDE("${PROJECT_BINARY_DIR}/VersionInfo.cmake")

#message("Con/C++ version: ${CONNECTOR_VERSION}")
message("Legacy library soversion: ${MYSQLCPPCONN_SOVERSION}")

include(./install_layout.cmake)

#-----------------
#  Dependencies
#

include(dependency)

if(MYSQLCLIENT_STATIC_LINKING)
  # Arrange for MySQL::client to refer to the static library
  set(MYSQL_LIB_STATIC ON)
else()
  # Arrange for MySQL::client to refer to the dynamic library
  set(MYSQL_LIB_STATIC OFF)
endif()


find_dependency(MySQL)
find_dependency(Boost)


#-----------------
# Global build flags


if(NOT DEFINED CMAKE_CXX_FLAGS)
  enable_pic()
  set_visibility(hidden)
endif()

enable_cxx11()


#-----------------
# Warnings
#

if(MSVC)

  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

elseif(SUNPRO)

  add_compile_options(
    -errtags=yes  # show tags that can be used to disable warnings
  )

endif()

#
#  TODO: We should fix the warnings below and then also increase warning
#  level and fix remaining warnings. However, before this is done, we do
#  not show the warnings to not break our release builds which are done
#  in maintainer mode.
#

set(SHOW_JDBC_WARNINGS OFF CACHE BOOL "Show compile warnings for legacy code")
mark_as_advanced(SHOW_JDBC_WARNINGS)

if(NOT SHOW_JDBC_WARNINGS)
  if(MSVC)

    add_compile_options(
      /wd4101  # unreferenced local variable
      /wd4267  # conversion ... possible loss of data
      /wd4800  # forcing value to bool
      /wd4594  # class ... can never be instantiated - indirect virtual base class is inaccesible

      /wd4100  # unreferenced formal parameter
      /wd4458  # declaration hides class member
      /wd4244  # conversion ... possible loss of data
      /wd4245  # conversion ... signed/unsigned missmatch
      /wd4706  # assignment within conditional expression
    )

  elseif(SUNPRO)

    add_compile_options(
      -erroff=wvarhidemem
    )

  else()

    add_compile_options(
      -Wno-unused-parameter
      -Wno-deprecated-declarations
    )

  endif()

endif()


############################################################################
#
#  Main components and connector library target.
#

ADD_SUBDIRECTORY(cppconn)
ADD_SUBDIRECTORY(driver)

#
# Connector library target
#

include(libutils)


# TODO: INFO file

set(INFO "${PROJECT_BINARY_DIR}/INFO_BIN")
file(WRITE "${INFO}" "Connector/C++ build information.\n\n")
set(INFO_PREFIX "jdbc-")

merge_libraries(connector-jdbc jdbc)
target_link_libraries(connector-jdbc PUBLIC Boost::boost)


#
# Install specifications
# ----------------------
#
# Note: Locations and names are configured in install_layout.cmake
#

set_property(TARGET connector-jdbc PROPERTY OUTPUT_NAME ${LIB_NAME})
message("Connector legacy library name: ${LIB_NAME}")

if(NOT BUILD_STATIC)
  set_property(TARGET connector-jdbc PROPERTY ARCHIVE_OUTPUT_NAME ${LIB_NAME_BASE})
endif()


set_target_properties(connector-jdbc
  PROPERTIES
  SOVERSION "${MYSQLCPPCONN_SOVERSION}"
  VERSION   "${MYSQLCPPCONN_SOVERSION}.${CONNECTOR_NUMERIC_VERSION}"
)

#
# Embed rpath information in the connector library.
#

set_property(TARGET connector-jdbc PROPERTY BUILD_WITH_INSTALL_RPATH ON)

# The $ORIGIN/@loader_path entry tells to look for dependent libraries in the
# location where our connector library is stored.

if(APPLE)
  set_property(TARGET connector-jdbc APPEND PROPERTY INSTALL_RPATH "@loader_path")
  if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    set_property(TARGET connector-jdbc APPEND PROPERTY INSTALL_RPATH "@loader_path/../private")
    set_property(TARGET connector-jdbc APPEND PROPERTY INSTALL_RPATH "@loader_path/../plugin")
  else()
    set_property(TARGET connector-jdbc APPEND PROPERTY INSTALL_RPATH "@loader_path/private")
    set_property(TARGET connector-jdbc APPEND PROPERTY INSTALL_RPATH "@loader_path/plugin")
  endif()
elseif(NOT WIN32)
  set_property(TARGET connector-jdbc APPEND PROPERTY INSTALL_RPATH "$ORIGIN")
  if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    set_property(TARGET connector-jdbc APPEND PROPERTY INSTALL_RPATH "$ORIGIN/../private")
    set_property(TARGET connector-jdbc APPEND PROPERTY INSTALL_RPATH "$ORIGIN/../plugin")
  else()
    set_property(TARGET connector-jdbc APPEND PROPERTY INSTALL_RPATH "$ORIGIN/private")
    set_property(TARGET connector-jdbc APPEND PROPERTY INSTALL_RPATH "$ORIGIN/plugin")
  endif()

endif()



install(TARGETS connector-jdbc
  CONFIGURATIONS Release RelWithDebInfo
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR_STATIC}" COMPONENT JDBCDev
  RUNTIME DESTINATION "${INSTALL_LIB_DIR}" COMPONENT JDBCDll
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT JDBCDll
)

install(TARGETS connector-jdbc
  CONFIGURATIONS Debug
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR_STATIC}/debug" COMPONENT JDBCDev
  RUNTIME DESTINATION "${INSTALL_LIB_DIR}/debug" COMPONENT JDBCDll
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}/debug" COMPONENT JDBCDll
)


if(MSVC AND NOT BUILD_STATIC)

  install(FILES $<TARGET_PDB_FILE:connector-jdbc>
    CONFIGURATIONS RelWithDebInfo
    DESTINATION "${INSTALL_LIB_DIR}"
    COMPONENT Debuginfo
  )

  install(FILES $<TARGET_PDB_FILE:connector-jdbc>
    CONFIGURATIONS Debug
    DESTINATION "${INSTALL_LIB_DIR}/debug"
    COMPONENT Debuginfo
  )

endif()

# Install some MySQL specific headers
SET(MYSQLCPPCONN_SPECIFIC_INSTALL_HEADERS
  driver/mysql_connection.h
  driver/mysql_driver.h
  driver/mysql_error.h)

INSTALL(FILES ${MYSQLCPPCONN_SPECIFIC_INSTALL_HEADERS}
  DESTINATION "${INSTALL_INCLUDE_DIR}"
  COMPONENT JDBCDev
)

# Note: Copy to build location so that test code can use headers without
# installing them first.

file(COPY ${MYSQLCPPCONN_SPECIFIC_INSTALL_HEADERS}
  DESTINATION "${CMAKE_BINARY_DIR}/include/jdbc"
)

#  export(TARGETS mysqlcppconn
#    APPEND FILE "${PROJECT_BINARY_DIR}/exports.cmake"
#  )

#
#  Add command to show rpath information
#

if(0)

if(APPLE)
  set(list_rpath_cmd otool -l $<TARGET_FILE:connector-jdbc> "|" grep RPATH -A2 "||" true)
elseif(NOT WIN32)
  set(list_rpath_cmd objdump -x $<TARGET_FILE:connector-jdbc> "|" grep RPATH -A2 "||" true)
endif()

add_custom_command(TARGET connector-jdbc POST_BUILD
  COMMAND ${list_rpath_cmd}
  COMMENT "RPATH setting for: $<TARGET_FILE_NAME:connector-jdbc>"
)

endif()


#
# Bundle 3rd party dependencies if needed
# =======================================

# List client-side plugins, their dependencies and dependencies of the client
# library that are bundled with the server and should be bundled with
# connector. Also list plugins and libraries that can be found with the server
# but which should be ignored.
#
# When building in MAINTAINER_MODE cmake will report error if it finds
# a plugin or 3rd party librariy which is not listed here. When that happens
# the lists should be updated.

set(AUTH_PLUGINS
  fido
  kerberos
  ldap_sasl
  oci
)


# Plugin dependencies.
#
# Warning: If one library name is a prefix of the other, the longer name
# should be listed first, otherwise the logic detecting missing dependencies
# will break... For example: `krb5support` must go before `krb5`

set(AUTH_DEPS_fido fido2)

# Note: Windows implementation does not depend on MIT Kerberos library, it uses
# native Win APIs

if(NOT WIN32)
  set(AUTH_DEPS_kerberos gssapi_krb5 k5crypto krb5support krb5 com_err)
endif()

set(AUTH_DEPS_ldap_sasl libsasl ${AUTH_DEPS_kerberos})
if(WIN32)
  list(APPEND AUTH_DEPS_ldap_sasl saslSCRAM)
endif()

# additional client-side plugins (if any)

set(PLUGINS)

# additional bundled dependencies of the client library other than the openssl
# libraries that are handled separately (in DepFindSSL.cmake)

set(BUNDLED_LIBS)

set(IGNORED_PLUGINS qa_auth_client)

set(IGNORED_LIBS
  libmysql lber libprotobuf
  ldap  # note: this is needed only by server-side plugin
  libcurl libmecab zlib
  jemalloc
  libssl libcrypto
)


# -------------

function(bundle_lib lib)

  get_filename_component(lib_name ${lib} NAME)

  if(bundled_${lib_name})
    return()
  endif()
  set(bundled_${lib_name} true PARENT_SCOPE)

  message("Bundling library: ${lib}")

  if(WIN32)
    install(FILES ${lib}
      DESTINATION "bin"
      COMPONENT JDBCDll
    )
  else()
    if(APPLE)
    # On Apple, its on the lib dir
    install(FILES ${lib}
      DESTINATION "${INSTALL_LIB_DIR}"
      COMPONENT JDBCDll
    )
    else()
      install(FILES ${lib}
        DESTINATION "${INSTALL_LIB_DIR}/private"
        COMPONENT JDBCDll
      )
    endif()
  endif()

endfunction(bundle_lib)


# Bundle libraries listed in a list variable ${to_bundle}.
# Libraries that were found and bundled are removed from ${to_bundle} list.
# Other libraries found but not listed in ${to_bundle} are returned
# in ${ignored} variable.
# If additional arguments are given, they are used as glob expressions to find
# the libraries to be bundled, otherwise 3rd parties bundled in with the server
# are searched in ${MYSQL_LIB_DIR} locations.

macro(bundle_libs to_bundle ignored)

  #message("== bundle_libs: ${${to_bundle}}")

  unset(_bundled)
  unset(_bundled1)

  if(${ARGC} GREATER 2)

    file(GLOB _bundled ${ARGN})

  else()

    # On Apple, libs are only on the lib dir, not on private
    if(APPLE)
      file(GLOB _bundled
        "${MYSQL_LIB_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
      )
    else()
      file(GLOB _bundled
        "${MYSQL_LIB_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
        "${MYSQL_LIB_DIR}/private/*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
      )
    endif()

    # On windows, libs are in bin directory

    if(WIN32)
      file(GLOB _bundled1 "${MYSQL_LIB_DIR}/../bin/*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
    endif()

  endif()

  # Process collected libraries
  set(libs ${${to_bundle}})

  foreach(lib IN LISTS _bundled _bundled1)

    get_filename_component(lib_name ${lib} NAME)
    #message("== looking at 3rd party lib: ${lib_name} (${bundled_${lib_name}})")

    # Match library name against names in to_bundle list and in case of match remove that name from the list

    unset(found)
    foreach(name ${libs})

      #message(STATUS "checking lib ${pos}: ${name}")

      if(
        (lib_name MATCHES "^${name}")
        OR ((NOT WIN32) AND (lib_name MATCHES "^lib${name}"))
      )
        #message(STATUS "removing from list: ${name}")
        list(REMOVE_ITEM ${to_bundle} ${name})
        bundle_lib(${lib})
        set(found ${name})
        break()
      endif()

    endforeach()

    if(NOT found)
      #message(STATUS "ignoring it (${lib_name})")
      list(APPEND ${ignored} ${lib_name})
    endif()

  endforeach()

endmacro(bundle_libs)


# Bundle plugins listed in PLUGINS list. Each bundled plugin P is removed from
# the list and its dependedencies listed in DEPS_${P} are also bundled. Client
# side plugins found with the server and not listed in PLUGINS are returned
# in ${ignored} list.

macro(bundle_plugins ignored)

  #message("== PLUGINS: ${PLUGINS}")

  file(GLOB _bundled "${MYSQL_PLUGIN_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*")

  set(plugins ${PLUGINS})

  foreach(lib ${_bundled})

    get_filename_component(lib_name ${lib} NAME_WE)

    if(NOT lib_name MATCHES "_client")
      continue()
    endif()

    #message("== looking at client-side plugin: ${lib_name}")

    # Match plugin name against names in PLUGINS list and in case of match
    # remove that name from the list

    unset(plugin)
    foreach(check ${plugins})

      #message(STATUS "checking plugin ${pos}: ${check}")

      if(
        lib_name MATCHES "^${check}"
      )
        #message(STATUS "plugin name match")
        set(plugin ${check})
        list(REMOVE_ITEM PLUGINS ${plugin})
        break()
      endif()

    endforeach()

    if(NOT plugin)
      #message(STATUS "ignoring it")
      list(APPEND ${ignored} ${lib_name})
      continue()
    endif()

    message("Bundling client plugin: ${lib}")

    install(FILES "${lib}"
      DESTINATION "${INSTALL_LIB_DIR}/plugin"
      COMPONENT JDBCDll
    )

    # See if libsasl is bundled to also bundle sasl plugins
    if("${DEPS_${plugin}}" MATCHES "sasl")
      set(sasl_bundled 1)
    endif()

    # Bundle plugin dependencies

    bundle_libs(DEPS_${plugin} ignored_deps)

    # In MAINTAINER_MODE report error if some plugin dependencies were not found

    if(DEPS_${plugin})

      if("${DEPS_${plugin}}" MATCHES "sasl")
        #message(STATUS "required SASL dependency not found, skipping SASL plugins")
        set(sasl_bundled 0)
      endif()

      if(MAINTAINER_MODE)
        message(SEND_ERROR
          "The following dependencies of bundled plugin ${plugin}"
          " were not found: ${DEPS_${plugin}}"
        )
      endif()

    endif()

  endforeach()

  # Also bundle modules required by the SASL library if they exist
  # and the library was bundled.

  if(
    sasl_bundled AND NOT bundled_sasl_modules
    AND EXISTS "${MYSQL_LIB_DIR}/private/sasl2"
  )

    message("Bundling SASL modules")
    set(bundled_sasl_modules true)
    install(DIRECTORY "${MYSQL_LIB_DIR}/private/sasl2"
      DESTINATION "${INSTALL_LIB_DIR}/private"
      COMPONENT JDBCDll
    )

  endif()

endmacro(bundle_plugins)



if(BUNDLE_DEPENDENCIES)

  message(STATUS "Looking for bundled client-side plugins")

  # add authentication plugins to the list of plugins

  foreach(auth ${AUTH_PLUGINS})
    set(plugin "authentication_${auth}")
    list(APPEND PLUGINS ${plugin})
    set(DEPS_${plugin} ${AUTH_DEPS_${auth}})
    list(APPEND IGNORED_LIBS ${DEPS_${plugin}})
  endforeach()

  # Bundle the plugins and their dependencies.

  unset(ingored)
  bundle_plugins(ignored)

  if(MAINTAINER_MODE)

    foreach(plugin ${PLUGINS})
      message(STATUS "known client-side plugin not found and not bundled: ${plugin}")
    endforeach()

    foreach(plugin ${IGNORED_PLUGINS})
      list(FILTER ignored EXCLUDE REGEX "^${plugin}")
    endforeach()

    if(ignored)
      message(SEND_ERROR
        "Unrecognized client-side plugins bundled with the server were found,"
        " please update definitions in cmake lists to either ignore them"
        " or bundle with the conector: ${ignored}"
      )
    endif()

  endif(MAINTAINER_MODE)


  message(STATUS "Looking for bundled client lib dependencies")

  # Bundle additional libraries listed in BUNDLED_LIBS

  #message(STATUS "bundle other libs")
  unset(extra_libs)
  bundle_libs(BUNDLED_LIBS extra_libs)

  # In MAINTAINER_MODE check whether all 3rd party libs found but not bundled
  # are listed in IGNORED_LIBS

  if(MAINTAINER_MODE)

    # Extend ignore list with libraries that are dependencies of known plugins
    # and are not listed in BUNDLED_LIBS. Otherwise we would get false errors
    # below.

    foreach(plugin PLUGINS)
      list(APPEND IGNORED_LIBS ${DEPS_${plugin}})
    endforeach()

    # Remove from ${extra_libs} the libraries that we know we should ignore.

    #message("== extra_libs: ${extra_libs}")
    foreach(lib ${IGNORED_LIBS})
      #message(STATUS "removing: ${lib}")
      list(FILTER extra_libs EXCLUDE REGEX "^${lib}")
      list(FILTER extra_libs EXCLUDE REGEX "^lib${lib}")
    endforeach()
    #message("== extra_libs: ${extra_libs}")

    if(extra_libs)
      message(SEND_ERROR
        "Unrecognized 3rd party library bundled with the server were found,"
        " please update definitions in cmake lists to either ignore them"
        " or bundle with the conector: ${extra_libs}"
      )
    endif()

  endif()

endif(BUNDLE_DEPENDENCIES)


if(WITH_TESTS)

  if(NOT BUILD_SHARED_LIBS)
    set_property(
      DIRECTORY .
      APPEND PROPERTY COMPILE_DEFINITIONS
      CPPCONN_LIB_BUILD
      )
  endif()

  add_subdirectory(test)
  add_subdirectory(examples)
endif()

if(jdbc_stand_alone)
  show_config_options()
endif()
