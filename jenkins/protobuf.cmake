# Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
#
# The MySQL Connector/C++ is licensed under the terms of the GPLv2
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
# MySQL Connectors. There are special exceptions to the terms and
# conditions of the GPLv2 as it is applied to this software, see the
# FLOSS License Exception
# <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published
# by the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

#
# Download protobuf dependency
#
# -DPROTBUF_URL=<url>
#

if(NOT PROTOBUF_URL)
  message(FATAL_ERROR "Specify URL to load from with PROTOBUF_URL")
endif()

if(NOT EXISTS protobuf-2.6.0)

if(NOT EXISTS protobuf-2.6.0.tar.gz)

message("Downloading protobuf from: ${PROTOBUF_URL}")

file(DOWNLOAD  ${PROTOBUF_URL} ./protobuf-2.6.0.tar.gz TIMEOUT 600 STATUS status)
message("status: ${status}")
list(GET status 0 status_code)

if(status_code)
  message(FATAL_ERROR "Failed to download")
endif()

endif()

message("uncompressing")
execute_process(COMMAND tar xzf protobuf-2.6.0.tar.gz RESULT_VARIABLE uncompress_result)
message("uncompress_result: ${uncompress_result}")

if(uncompress_result)
  message(FATAL_ERROR "Could not uncompress: ${uncompress_result}")
endif()

# rename protobuf directory

file(GLOB protobuf_dir "protobuf-2.6.0-*")

if(NOT protobuf_dir)
  message(FATAL_ERROR "Could not find prtobuf folder after decompression")
endif()

file(RENAME ${protobuf_dir} "protobuf-2.6.0")

endif()

