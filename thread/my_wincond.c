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
** The following is a simple implementation of posix conditions
*****************************************************************************/
#if defined(_WIN32)

#undef SAFE_MUTEX			/* Avoid safe_mutex redefinitions */
#include <process.h>
#include <sys/timeb.h>

#include <my_global.h>
#include "my_pthread.h"

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
  cond->waiting= 0;
  InitializeCriticalSection(&cond->lock_waiting);

  cond->events[SIGNAL]= CreateEvent(NULL,  /* no security */
                                    FALSE, /* auto-reset event */
                                    FALSE, /* non-signaled initially */
                                    NULL); /* unnamed */

  /* Create a manual-reset event. */
  cond->events[BROADCAST]= CreateEvent(NULL,  /* no security */
                                       TRUE,  /* manual-reset */
                                       FALSE, /* non-signaled initially */
                                       NULL); /* unnamed */


  cond->broadcast_block_event= CreateEvent(NULL,  /* no security */
                                           TRUE,  /* manual-reset */
                                           TRUE,  /* signaled initially */
                                           NULL); /* unnamed */

  if( cond->events[SIGNAL] == NULL ||
      cond->events[BROADCAST] == NULL ||
      cond->broadcast_block_event == NULL )
    return ENOMEM;
  return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
  DeleteCriticalSection(&cond->lock_waiting);

  if (CloseHandle(cond->events[SIGNAL]) == 0 ||
      CloseHandle(cond->events[BROADCAST]) == 0 ||
      CloseHandle(cond->broadcast_block_event) == 0)
    return EINVAL;
  return 0;
}


int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
  return pthread_cond_timedwait(cond,mutex,NULL);
}


int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                           struct timespec *abstime)
{
  int result;
  long timeout;
  union ft64 now;

  if( abstime != NULL )
  {
    GetSystemTimeAsFileTime(&now.ft);

    /*
      Calculate time left to abstime
      - subtract start time from current time(values are in 100ns units)
      - convert to millisec by dividing with 10000
    */
    timeout= (long)((abstime->tv.i64 - now.i64) / 10000);

    /* Don't allow the timeout to be negative */
    if (timeout < 0)
      timeout= 0L;

    /*
      Make sure the calucated timeout does not exceed original timeout
      value which could cause "wait for ever" if system time changes
    */
    if (timeout > abstime->max_timeout_msec)
      timeout= abstime->max_timeout_msec;

  }
  else
  {
    /* No time specified; don't expire */
    timeout= INFINITE;
  }

  /*
    Block access if previous broadcast hasn't finished.
    This is just for safety and should normally not
    affect the total time spent in this function.
  */
  WaitForSingleObject(cond->broadcast_block_event, INFINITE);

  EnterCriticalSection(&cond->lock_waiting);
  cond->waiting++;
  LeaveCriticalSection(&cond->lock_waiting);

  LeaveCriticalSection(mutex);

  result= WaitForMultipleObjects(2, cond->events, FALSE, timeout);

  EnterCriticalSection(&cond->lock_waiting);
  cond->waiting--;

  if (cond->waiting == 0)
  {
    /*
      We're the last waiter to be notified or to stop waiting, so
      reset the manual event.
    */
    /* Close broadcast gate */
    ResetEvent(cond->events[BROADCAST]);
    /* Open block gate */
    SetEvent(cond->broadcast_block_event);
  }
  LeaveCriticalSection(&cond->lock_waiting);

  EnterCriticalSection(mutex);

  return result == WAIT_TIMEOUT ? ETIMEDOUT : 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
  EnterCriticalSection(&cond->lock_waiting);

  if(cond->waiting > 0)
    SetEvent(cond->events[SIGNAL]);

  LeaveCriticalSection(&cond->lock_waiting);

  return 0;
}


int pthread_cond_broadcast(pthread_cond_t *cond)
{
  EnterCriticalSection(&cond->lock_waiting);
  /*
     The mutex protect us from broadcasting if
     there isn't any thread waiting to open the
     block gate after this call has closed it.
   */
  if(cond->waiting > 0)
  {
    /* Close block gate */
    ResetEvent(cond->broadcast_block_event);
    /* Open broadcast gate */
    SetEvent(cond->events[BROADCAST]);
  }

  LeaveCriticalSection(&cond->lock_waiting);

  return 0;
}


int pthread_attr_init(pthread_attr_t *connect_att)
{
  connect_att->dwStackSize	= 0;
  connect_att->dwCreatingFlag	= 0;
  return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *connect_att,DWORD stack)
{
  connect_att->dwStackSize=stack;
  return 0;
}

int pthread_attr_destroy(pthread_attr_t *connect_att)
{
  memset((void*) connect_att, 0, sizeof(*connect_att));
  return 0;
}

/****************************************************************************
** Fix localtime_r() to be a bit safer
****************************************************************************/

struct tm *localtime_r(const time_t *timep,struct tm *tmp)
{
  if (*timep == (time_t) -1)			/* This will crash win32 */
  {
    memset((void*)tmp, 0, sizeof(*tmp));
  }
  else
  {
    struct tm *res=localtime(timep);
    if (!res)                                   /* Wrong date */
    {
      memset((void*)tmp, 0, sizeof(*tmp));                  /* Keep things safe */
      return 0;
    }
    *tmp= *res;
  }
  return tmp;
}
#endif /* __WIN__ */
