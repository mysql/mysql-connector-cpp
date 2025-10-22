# Install script for directory: /home/carlon/code/mysql-connector-cpp/jdbc/cppconn

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local/mysql/connector-cpp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "JDBCDev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/jdbc/cppconn" TYPE FILE FILES
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/build_config.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/callback.h"
    "/home/carlon/code/mysql-connector-cpp/build/jdbc/cppconn/config.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/connection.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/datatype.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/driver.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/exception.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/metadata.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/parameter_metadata.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/prepared_statement.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/resultset.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/resultset_metadata.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/statement.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/sqlstring.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/warning.h"
    "/home/carlon/code/mysql-connector-cpp/build/jdbc/cppconn/version_info.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/cppconn/variant.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/carlon/code/mysql-connector-cpp/build/jdbc/cppconn/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
