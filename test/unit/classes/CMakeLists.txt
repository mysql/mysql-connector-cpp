# Copyright (c) 2008, 2020, Oracle and/or its affiliates.
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


SET(test_common_sources
  ../unit_fixture.cpp
  ../main.cpp)

IF(WIN32)
  SET(test_common_sources
    ${test_common_sources}
    ../unit_fixture.h)

ENDIF(WIN32)

SET(test_connection_sources
  ${test_common_sources}
  connection.cpp)

IF(WIN32)
  SET(test_connection_sources
        ${test_connection_sources}
    connection.h)
ENDIF(WIN32)

add_unit_test(connection)


SET(test_databasemetadata_sources
  ../unit_fixture.cpp
  ../main.cpp
  connectionmetadata.cpp)

IF(WIN32)
  SET(test_databasemetadata_sources
        ${test_databasemetadata_sources}
  ../unit_fixture.h
  connectionmetadata.h)
ENDIF(WIN32)

add_unit_test(databasemetadata)


SET(test_resultsetmetadata_sources
  ../unit_fixture.cpp
  ../main.cpp
  resultsetmetadata.cpp)

IF(WIN32)
  SET(test_resultsetmetadata_sources
        ${test_resultsetmetadata_sources}
  ../unit_fixture.h
  resultsetmetadata.h)
ENDIF(WIN32)

add_unit_test(resultsetmetadata)

SET(test_resultset_sources
  ../unit_fixture.cpp
  ../main.cpp
  resultset.cpp)

IF(WIN32)
  SET(test_resultset_sources
        ${test_resultset_sources}
  ../unit_fixture.h
  resultset.h)
ENDIF(WIN32)

add_unit_test(resultset)


SET(test_savepoint_sources
  ../unit_fixture.cpp
  ../main.cpp
  savepoint.cpp)

IF(WIN32)
  SET(test_savepoint_sources
        ${test_savepoint_sources}
  ../unit_fixture.h
  savepoint.h)
ENDIF(WIN32)

add_unit_test(savepoint)


SET(test_preparedstatement_sources
  ../unit_fixture.cpp
  ../main.cpp
  preparedstatement.cpp)

IF(WIN32)
  SET(test_preparedstatement_sources
        ${test_preparedstatement_sources}
  ../unit_fixture.h
  preparedstatement.h)
ENDIF(WIN32)

add_unit_test(preparedstatement)


SET(test_parametermetadata_sources
  ../unit_fixture.cpp
  ../main.cpp
  parametermetadata.cpp)

IF(WIN32)
  SET(test_parametermetadata_sources
        ${test_parametermetadata_sources}
  ../unit_fixture.h
  parametermetadata.h)
ENDIF(WIN32)

add_unit_test(parametermetadata)


SET(test_art_resultset_sources
  ../unit_fixture.cpp
  ../main.cpp
  art_resultset.cpp)

IF(WIN32)
  SET(test_art_resultset_sources
        ${test_art_resultset_sources}
  ../unit_fixture.h
  art_resultset.h)

  ADD_DEFINITIONS("-DCPPCONN_PUBLIC_FUNC=")
ENDIF(WIN32)

add_unit_test(art_resultset)


SET(test_statement_sources
  ../unit_fixture.cpp
  ../main.cpp
  statement.cpp)

IF(WIN32)
  SET(test_statement_sources
        ${test_statement_sources}
  ../unit_fixture.h
  statement.h)
ENDIF(WIN32)

add_unit_test(statement)


SET(test_uri_sources
  ${test_common_sources}
  "${CMAKE_SOURCE_DIR}/jdbc/driver/mysql_uri.cpp"
  "${CMAKE_SOURCE_DIR}/jdbc/driver/mysql_util.cpp"
  uri.cpp)

IF(WIN32)
  SET(test_uri_sources
        ${test_uri_sources}
    "${CMAKE_SOURCE_DIR}/jdbc/driver/mysql_uri.h"
    uri.h)
ENDIF(WIN32)

add_unit_test(uri)
target_link_libraries(test_uri MySQL::client)


SET(test_variant_sources
  ../unit_fixture.cpp
  ../main.cpp
  variant.cpp)

IF(WIN32)
  SET(test_variant_sources
    ${test_variant_sources}
    ../unit_fixture.h
    variant.h)
ENDIF(WIN32)

add_unit_test(variant)
