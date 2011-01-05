/*
Copyright (c) 2000, 2011, Oracle and/or its affiliates. All rights reserved.

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




#if !defined(_my_no_pthread_h) && !defined(THREAD)
#define _my_no_pthread_h

/*
  This block is to access some thread-related type definitions
  even in builds which do not need thread functions,
  as some variables (based on these types) are declared
  even in non-threaded builds.
  Case in point: 'mf_keycache.c'
*/
#if defined(_WIN32) || defined(_WIN64)
#else /* Normal threads */
#include <pthread.h>
#endif /* defined(__WIN__) */


/*
  This undefs some pthread mutex locks when one isn't using threads
  to make thread safe code, that should also work in single thread
  environment, easier to use.
*/
#define pthread_mutex_init(A,B)
#define pthread_mutex_lock(A)
#define pthread_mutex_unlock(A)
#define pthread_mutex_destroy(A)

#endif
