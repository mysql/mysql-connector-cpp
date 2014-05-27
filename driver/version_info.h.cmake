/*
Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPLv2
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPLv2 as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

/* @EDIT_WARNING_MESSAGE@ */

#define MYCPPCONN_MAJOR_VERSION @CONNECTOR_MAJOR@
#define MYCPPCONN_MINOR_VERSION @CONNECTOR_MINOR@
#define MYCPPCONN_PATCH_VERSION @CONNECTOR_PATCH@

#define SETUP_VERSION         "@CONNECTOR_MAJOR@.@CONNECTOR_MINOR_PADDED@.@CONNECTOR_PATCH_PADDED@"
#define DRIVER_VERSION        "0" SETUP_VERSION

#define MYCPPCONN_VERSION        SETUP_VERSION
#define MYCPPCONN_FILEVER        @CONNECTOR_MAJOR@,@CONNECTOR_MINOR@,@CONNECTOR_PATCH@,0
#define MYCPPCONN_PRODUCTVER     MYCPPCONN_FILEVER
#define MYCPPCONN_STRFILEVER     "@CONNECTOR_MAJOR@, @CONNECTOR_MINOR@, @CONNECTOR_PATCH@, 0\0"
#define MYCPPCONN_STRPRODUCTVER  MYCPPCONN_STRFILEVER

#define MYCPPCONN_STRSERIES      "@CONNECTOR_MAJOR@.@CONNECTOR_MINOR@"
#define MYCPPCONN_STRQUALITY     "@CONNECTOR_QUALITY@"
#define MYCPPCONN_STRVERSION     "@CONNECTOR_VERSION@"
