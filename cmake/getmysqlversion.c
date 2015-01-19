/*
Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.

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


#include <mysql.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *fp;
  int i;

  if (argc < 3) {
    return 0;
  }

  if (!(fp = fopen(argv[1],  "w"))) {
    return 0;
  }

  i = 2;
  while (i < argc) {
    if (strcmp(argv[i], "MYSQL_SERVER_VERSION") == 0) {
      fprintf(fp, "\nSET(MYSQL_CPP_SERVER_VERSION \"%s\")\n", MYSQL_SERVER_VERSION);
    } else if (strcmp(argv[i], "MYSQL_VERSION_ID") == 0) {
      fprintf(fp, "\nSET(MYSQL_CPP_SERVER_VERSION_ID %d)\n", MYSQL_VERSION_ID);
    } else if (strcmp(argv[i], "LIBMYSQL_VERSION") == 0) {
      fprintf(fp, "\nSET(LIBMYSQL_CPP_VERSION \"%s\")\n", LIBMYSQL_VERSION);
    } else if (strcmp(argv[i], "LIBMYSQL_VERSION_ID") == 0) {
      fprintf(fp, "\nSET(LIBMYSQL_CPP_VERSION_ID %d)\n", LIBMYSQL_VERSION_ID);
    }
    ++i;
  }

  fclose(fp);
  return 1;
}
