# Install script for directory: /home/carlon/code/mysql-connector-cpp/jdbc

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "JDBCDll" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee]|[Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE SHARED_LIBRARY FILES
      "/home/carlon/code/mysql-connector-cpp/build/jdbc/libmysqlcppconn.so.10.9.4.0"
      "/home/carlon/code/mysql-connector-cpp/build/jdbc/libmysqlcppconn.so.10"
      )
    foreach(file
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libmysqlcppconn.so.10.9.4.0"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libmysqlcppconn.so.10"
        )
      if(EXISTS "${file}" AND
         NOT IS_SYMLINK "${file}")
        if(CMAKE_INSTALL_DO_STRIP)
          execute_process(COMMAND "/usr/bin/strip" "${file}")
        endif()
      endif()
    endforeach()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "JDBCDll" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee]|[Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE SHARED_LIBRARY FILES "/home/carlon/code/mysql-connector-cpp/build/jdbc/libmysqlcppconn.so")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "JDBCDll" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/debug" TYPE SHARED_LIBRARY FILES
      "/home/carlon/code/mysql-connector-cpp/build/jdbc/libmysqlcppconn.so.10.9.4.0"
      "/home/carlon/code/mysql-connector-cpp/build/jdbc/libmysqlcppconn.so.10"
      )
    foreach(file
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/debug/libmysqlcppconn.so.10.9.4.0"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/debug/libmysqlcppconn.so.10"
        )
      if(EXISTS "${file}" AND
         NOT IS_SYMLINK "${file}")
        if(CMAKE_INSTALL_DO_STRIP)
          execute_process(COMMAND "/usr/bin/strip" "${file}")
        endif()
      endif()
    endforeach()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "JDBCDll" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/debug" TYPE SHARED_LIBRARY FILES "/home/carlon/code/mysql-connector-cpp/build/jdbc/libmysqlcppconn.so")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "JDBCDev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/jdbc" TYPE FILE FILES
    "/home/carlon/code/mysql-connector-cpp/jdbc/driver/mysql_connection.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/driver/mysql_driver.h"
    "/home/carlon/code/mysql-connector-cpp/jdbc/driver/mysql_error.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "JDBCTests")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/tests/jdbc" TYPE FILE RENAME "CTestTestfile.cmake" FILES "/home/carlon/code/mysql-connector-cpp/build/jdbc/jdbc_tests.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/carlon/code/mysql-connector-cpp/build/jdbc/extra/otel/cmake_install.cmake")
  include("/home/carlon/code/mysql-connector-cpp/build/jdbc/cppconn/cmake_install.cmake")
  include("/home/carlon/code/mysql-connector-cpp/build/jdbc/driver/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/carlon/code/mysql-connector-cpp/build/jdbc/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
