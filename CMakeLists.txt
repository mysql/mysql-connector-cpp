CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(MySQL_CONCPLS)

#
# Set up cmake module search patch.
#

SET(WITH_NGCOMMON $ENV{WITH_NGCOMMON} CACHE PATH "Path to ngcommon sources")

#IF(NOT WITH_NGCOMMON)
#  SET(WITH_NGCOMMON ${PROJECT_SOURCE_DIR}/common)
#ENDIF()

IF (WITH_NGCOMMON AND NOT EXISTS "${WITH_NGCOMMON}/cmake")
MESSAGE(WARNING "Could not find required ngcommon files under location"
                " specified with WITH_NGCOMMON: ${WITH_NGCOMMON}")
ENDIF()

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
#MESSAGE("added module path: ${PROJECT_SOURCE_DIR}/cmake")

IF(WITH_NGCOMMON)
  LIST(APPEND CMAKE_MODULE_PATH ${WITH_NGCOMMON}/cmake)
  #MESSAGE("added module path: ${WITH_NGCOMMON}/cmake")
ENDIF()

#
# Configure static runtime library on Windows if requested
#

OPTION(STATIC_MSVCRT "Use static MSVC runtime library" OFF)

IF(WIN32 AND STATIC_MSVCRT)

message("Using static runtime library")

foreach(LANG C CXX)
  set(CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} /MT")
  foreach(TYPE RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_${LANG}_FLAGS_${TYPE} "${CMAKE_${LANG}_FLAGS_${TYPE}} /MT")
  endforeach()
  set(CMAKE_${LANG}_FLAGS_DEBUG "${CMAKE_${LANG}_FLAGS_DEBUG} /MTd")
endforeach(LANG)

ENDIF()

#message("flags: ${CMAKE_C_FLAGS}")
#message("c++ flags: ${CMAKE_CXX_FLAGS}")
#foreach(TYPE DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
#  message("${TYPE} flags: ${CMAKE_C_FLAGS_${TYPE}}")
#  message("c++ ${TYPE} flags: ${CMAKE_CXX_FLAGS_${TYPE}}")
#endforeach()


#
# Add Boost
#

INCLUDE(boost)
SETUP_BOOST()

#
# CDK
# 
SET(WITH_CDK_DOC 0)
ADD_SUBDIRECTORY(cdk)
INCLUDE_DIRECTORIES(${CDK_INCLUDE_DIR})

#
# Project's public headers
#

ADD_SUBDIRECTORY(include)
INCLUDE_DIRECTORIES(include)


#
# Testing framework
#
#INCLUDE(testing)
#SETUP_TESTING()

#
# Set higher warning level on Windows to catch non-portable
# code.
#

IF(WIN32)

foreach(LANG C CXX)
  set(CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} /W4 /wd4512")
  foreach(TYPE RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_${LANG}_FLAGS_${TYPE} "${CMAKE_${LANG}_FLAGS_${TYPE}} /W4 /wd4512")
  endforeach()
endforeach(LANG)

ENDIF()

#
# CDK components
#

ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(connector)

#
# Target for running all declared tests (run_unit_tests)
#

#IF(WITH_TESTS)
#  ADD_TEST_TARGET()
#  ADD_DEPENDENCIES(run_unit_tests foundation_tests_server ngs_mockup)
#ENDIF()

#
# Sample code to try things out
#

ADD_EXECUTABLE(try try.cc)
TARGET_LINK_LIBRARIES(try concplus)
#  ADD_GCOV(try)

