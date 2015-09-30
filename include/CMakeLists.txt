#
# MySQL Connector/C++ Public Headers
#
# see headers.cmake for explanations how project's public
# headers are declared and processed.
#

INCLUDE(headers)

SETUP_HEADERS(${CMAKE_CURRENT_SOURCE_DIR})

SET(headers mysqlx.h common.h result.h task.h document.h)

ADD_HEADERS(${headers})

#IF(cdk_stand_alone)
  ADD_HEADERS_TARGET()
#ENDIF()

INSTALL(FILES ${headers} DESTINATION include)

