if(NOT DEFINED JDBC_DIR)
  message(FATAL_ERROR "JDBC_DIR not specified")
endif()

if(NOT DEFINED JDBC_INCLUDES)
  message(FATAL_ERROR "JDBC_INCLUDES not specified")
endif()

if(NOT DEFINED CMAKE_GENERATOR)
  message(FATAL_ERROR "CMAKE_GENERATOR not specified")
endif()

if(NOT EXISTS "${JDBC_DIR}/install/include/jdbc/mysql_driver.h")
  message(FATAL_ERROR "JDBC headers could not be found at ${JDBC_DIR}/install/include/jdbc. Have you run build_jdbc target?")
endif()

if(NOT EXISTS "${HEADERS_DIR}")
  message(FATAL_ERROR "Location of header checks infrastructure not specified with HEADERS_DIR or it could not be found there (${HEADERS_DIR})")
endif()

if(NOT DEFINED CHECK_DIR)
  set(CHECK_DIR "${JDBC_DIR}/headers_check")
endif()

if(NOT EXISTS "${CHECK_DIR}")
  file(MAKE_DIRECTORY "${CHECK_DIR}")
endif()


message("Checking JDBC public headers in: ${JDBC_DIR}")
message("configuring header checks in: ${CHECK_DIR}")

#
# Generate CMakeLists.txt for the check project.
#

set(hdr_include_dir ${JDBC_INCLUDES})

configure_file(${HEADERS_DIR}/check.cmake.in
               ${CHECK_DIR}/CMakeLists.txt @ONLY)

#
# Collect all headers and generate check sources for each of them.
#

file(GLOB headers "${JDBC_DIR}/install/include/jdbc/*.h")
set(all_headers ${headers})
set(checks)

foreach(hdr ${headers})

  get_filename_component(HEADERN "${hdr}" NAME_WE)
  #message("processing header: ${HEADERN}.h")

  set(HEADER "jdbc/${HEADERN}.h")

  configure_file(${HEADERS_DIR}/check.source.in
                 ${CHECK_DIR}/${HEADERN}.cc @ONLY)

  list(APPEND checks ${HEADERN}.cc)

endforeach()

file(APPEND "${CHECK_DIR}/CMakeLists.txt"
  "add_library(check_jdbc STATIC ${checks})\n"
)


file(GLOB headers "${JDBC_DIR}/install/include/jdbc/cppconn/*.h")
list(APPEND all_headers ${headers})
set(checks)

foreach(hdr ${headers})

  get_filename_component(HEADERN "${hdr}" NAME_WE)
  #message("processing header: cppconn/${HEADERN}.h")

  set(HEADER "jdbc/cppconn/${HEADERN}.h")

  configure_file(${HEADERS_DIR}/check.source.in
                 ${CHECK_DIR}/cppconn_${HEADERN}.cc @ONLY)

  list(APPEND checks cppconn_${HEADERN}.cc)

endforeach()

file(APPEND "${CHECK_DIR}/CMakeLists.txt"
  "add_library(check_jdbc_cppconn STATIC ${checks})\n"
)

#
# Configure check project with cmake and then build it.
#

message("Configuring JDBC header checks using cmake generator: ${CMAKE_GENERATOR}")

execute_process(
  COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  WORKING_DIRECTORY ${CHECK_DIR}
)

message("Running header checks")

execute_process(
  COMMAND ${CMAKE_COMMAND} --build . --clean-first
  WORKING_DIRECTORY ${CHECK_DIR}
)


