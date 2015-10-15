#
# Generate project documentation using Doxygen.
#

IF(NOT DEFINED WITH_DOC)
  OPTION(WITH_DOC "Build Project's documentation" OFF)
ENDIF()

IF(WITH_DOC)

set(WITH_DOC OFF)

# Find Doxygen

if(DEFINED WITH_DOXYGEN)

  find_program(DOXYGEN_EXECUTABLE
    NAMES doxygen
    PATHS ${WITH_DOXYGEN}
    DOC "Doxygen documentation generation tool"
  )

  if(NOT DOXYGEN_EXECUTABLE)
    message(WARNING "Could not find Doxygen at specified location: ${WITH_DOXYGEN},"
            " will not build documentation")
    return()
  endif()

  set(DOXGEN_FOUND YES)
  execute_process(COMMAND ${DOXYGEN_EXECUTABLE} "--version" OUTPUT_VARIABLE DOXYGEN_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

else()

  find_package(Doxygen)

  if(NOT DOXYGEN_FOUND)
    message(WARNING "Could not find Doxygen, will not build documentation."
            " Custom Doxygen location can be specified using WITH_DOXYGEN option")
    set(WITH_DOXYGEN "" CACHE PATH "Doxygen install location")
    return()
  endif()

endif()

set(WITH_DOC ON)
MESSAGE("Building C/C++ documentation")


#
# TODO: remove conflict with CDK's build_docs target
#

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
configure_file(doxygen.cfg.in doxygen.cfg @ONLY)

set(OUTPUT_DIRECTORY "$(OUTPUT_DIRECTORY)")
configure_file(doxygen.cfg.in ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cfg @ONLY)

FILE(GLOB_RECURSE sources *.txt)
LIST(APPEND sources doxygen.cfg.in)


ADD_CUSTOM_TARGET(build_docs
  COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Building project documentation"
  SOURCES ${sources}
)

ENDIF(WITH_DOC)
