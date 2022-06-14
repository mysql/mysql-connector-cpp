/*
 * Copyright (c) 2000, 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */



/*****************************************************************************
** Simulation of posix threads calls for Windows
*****************************************************************************/
#if defined (_WIN32)
/* SAFE_MUTEX will not work until the thread structure is up to date */
#undef SAFE_MUTEX

#include <my_global.h>
#include "my_pthread.h"

#include <process.h>
#include <signal.h>

// re-defining calls to debug functions from libmysql

#ifdef DBUG_PRINT
#undef DBUG_PRINT
#endif
#define DBUG_PRINT(keyword, argslist)

#ifdef DBUG_ENTER
#undef DBUG_ENTER
#endif
#define DBUG_ENTER(p)

#ifdef DBUG_RETURN
#undef DBUG_RETURN
#endif

#define DBUG_RETURN(a1) return(a1)


static void install_sigabrt_handler(void);

struct thread_start_parameter
{
  pthread_handler func;
  void *arg;
};

/**
   Adapter to @c pthread_mutex_trylock()

   @retval 0      Mutex was acquired
   @retval EBUSY  Mutex was already locked by a thread
 */
int
win_pthread_mutex_trylock(pthread_mutex_t *mutex)
{
  if (TryEnterCriticalSection(mutex))
  {
    /* Don't allow recursive lock */
    if (mutex->RecursionCount > 1){
      LeaveCriticalSection(mutex);
      return EBUSY;
    }
    return 0;
  }
  return EBUSY;
}

static unsigned int __stdcall pthread_start(void *p)
{
  struct thread_start_parameter *par= (struct thread_start_parameter *)p;
  pthread_handler func= par->func;
  void *arg= par->arg;
  free(p);
  (*func)(arg);
  return 0;
}


int pthread_create(pthread_t *thread_id, pthread_attr_t *attr,
     pthread_handler func, void *param)
{
  uintptr_t handle;
  struct thread_start_parameter *par;
  unsigned int  stack_size;
  DBUG_ENTER("pthread_create");

  par= (struct thread_start_parameter *)malloc(sizeof(*par));
  if (!par)
   goto error_return;

  par->func= func;
  par->arg= param;
  stack_size= attr?attr->dwStackSize:0;

  handle= _beginthreadex(NULL, stack_size , pthread_start, par, 0, thread_id);
  if (!handle)
    goto error_return;
  DBUG_PRINT("info", ("thread id=%u",*thread_id));

  /* Do not need thread handle, close it */
  CloseHandle((HANDLE)handle);
  DBUG_RETURN(0);

error_return:
  DBUG_PRINT("error",
         ("Can't create thread to handle request (error %d)",errno));
  DBUG_RETURN(-1);
}


void pthread_exit(void *a)
{
  _endthreadex(0);
}

int pthread_join(pthread_t thread, void **value_ptr)
{
  DWORD  ret;
  HANDLE handle;

  handle= OpenThread(SYNCHRONIZE, FALSE, thread);
  if (!handle)
  {
    errno= EINVAL;
    goto error_return;
  }

  ret= WaitForSingleObject(handle, INFINITE);

  if(ret != WAIT_OBJECT_0)
  {
    errno= EINVAL;
    goto error_return;
  }

  CloseHandle(handle);
  return 0;

error_return:
  if(handle)
    CloseHandle(handle);
  return -1;
}

#endif
