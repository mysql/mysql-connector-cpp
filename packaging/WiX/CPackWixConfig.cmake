# Copyright (c) 2010, 2018, Oracle and/or its affiliates. All rights reserved.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0,
# as published by the Free Software Foundation.
#
# This program is also distributed with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation.  The authors of MySQL hereby grant you an additional
# permission to link the program and your derivative works with the
# separately licensed software that they have included with MySQL.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License, version 2.0, for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

SET(CPACK_COMPONENTS_USED 
    "XDevAPIDll;JDBCDll;XDevAPIDev;JDBCDev;Readme")

# Some components are optional
# We will build MSI without optional components that are not selected for build
#(need to modify CPACK_COMPONENTS_ALL for that)
SET(CPACK_ALL)
FOREACH(comp1 ${CPACK_COMPONENTS_USED})
 SET(found)
 FOREACH(comp2 ${CPACK_COMPONENTS_ALL})
  IF(comp1 STREQUAL comp2)
    SET(found 1)
    BREAK()
  ENDIF()
 ENDFOREACH()
 IF(found)
   SET(CPACK_ALL ${CPACK_ALL} ${comp1})
 ENDIF()
ENDFOREACH()
SET(CPACK_COMPONENTS_ALL ${CPACK_ALL})

# Always install (hidden), includes Readme files
SET(CPACK_COMPONENT_GROUP_ALWAYSINSTALL_HIDDEN 1)
SET(CPACK_COMPONENT_README_GROUP "AlwaysInstall")

# Feature Connector C++
SET(CPACK_COMPONENT_GROUP_CONNECTORCPP_DISPLAY_NAME "Connector C++")
SET(CPACK_COMPONENT_GROUP_CONNECTORCPP_EXPANDED "1")
SET(CPACK_COMPONENT_GROUP_CONNECTORCPP_DESCRIPTION "Install MySQL Connector C++")
  # Subfeature "DLL Libraries"
  SET(CPACK_COMPONENT_GROUP_DLLLIBRARIES_GROUP "ConnectorCpp")
  SET(CPACK_COMPONENT_GROUP_DLLLIBRARIES_DISPLAY_NAME "Dll Libraries")
  SET(CPACK_COMPONENT_GROUP_DLLLIBRARIES_DESCRIPTION 
    "Connector C++ dll libraries and depdnencies" )
    #Subfeature "New XDevAPI/XAPI connector"
    SET(CPACK_COMPONENT_XDEVAPIDLL_GROUP "DLLLibraries")
    SET(CPACK_COMPONENT_XDEVAPIDLL_DISPLAY_NAME "New XDevAPI/XAPI")
    SET(CPACK_COMPONENT_XDEVAPIDLL_DESCRIPTION "Installs new XDevAPI/XAPI dlls")
    #Subfeature "Legacy JDBC API connector"
    SET(CPACK_COMPONENT_JDBCDLL_GROUP "DLLLibraries")
    SET(CPACK_COMPONENT_JDBCDLL_DISPLAY_NAME "Legacy JDBC API connector")
    SET(CPACK_COMPONENT_JDBCDLL_DESCRIPTION "Installs legacy JDBC API connector dlls")
    SET(CPACK_COMPONENT_JDBCDLL_WIX_LEVEL 2)
  # Subfeature "Development Components"
  SET(CPACK_COMPONENT_GROUP_DEV_GROUP "ConnectorCpp")
  SET(CPACK_COMPONENT_GROUP_DEV_DISPLAY_NAME "Development Components")
  SET(CPACK_COMPONENT_GROUP_DEV_DESCRIPTION 
    "Connector C++ headers, static libraries and import libraries" )
    #Subfeature "New XDevAPI/XAPI connector"
    SET(CPACK_COMPONENT_XDEVAPIDEV_GROUP "Dev")
    SET(CPACK_COMPONENT_XDEVAPIDEV_DISPLAY_NAME "New XDevAPI/XAPI")
    SET(CPACK_COMPONENT_XDEVAPIDEV_DESCRIPTION "Installs new XDevAPI/XAPI dev files")
    #Subfeature "Legacy JDBC API connector"
    SET(CPACK_COMPONENT_JDBCDEV_GROUP "Dev")
    SET(CPACK_COMPONENT_JDBCDEV_DISPLAY_NAME "Legacy JDBC API connector")
    SET(CPACK_COMPONENT_JDBCDEV_DESCRIPTION "Installs legacy JDBC API connector dev files")
    SET(CPACK_COMPONENT_JDBCDEV_WIX_LEVEL 2)

