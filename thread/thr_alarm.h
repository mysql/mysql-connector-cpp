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



/* Prototypes when using thr_alarm library functions */

#ifndef _thr_alarm_h
#define _thr_alarm_h
#ifdef	__cplusplus
extern "C" {
#endif

#ifndef USE_ALARM_THREAD
#define USE_ONE_SIGNAL_HAND		/* One must call process_alarm */
#endif
#ifdef HAVE_rts_threads
#undef USE_ONE_SIGNAL_HAND
#define USE_ALARM_THREAD
#define THR_SERVER_ALARM SIGUSR1
#else
#define THR_SERVER_ALARM SIGALRM
#endif

typedef struct st_alarm_info
{
  ulong next_alarm_time;
  uint active_alarms;
  uint max_used_alarms;
} ALARM_INFO;

void thr_alarm_info(ALARM_INFO *info);

#if defined(DONT_USE_THR_ALARM) || !defined(THREAD)

#define USE_ALARM_THREAD
#undef USE_ONE_SIGNAL_HAND

typedef my_bool thr_alarm_t;
typedef my_bool ALARM;

#define thr_alarm_init(A) (*(A))=0
#define thr_alarm_in_use(A) (*(A) != 0)
#define thr_end_alarm(A)
#define thr_alarm(A,B,C) ((*(A)=1)-1)
/* The following should maybe be (*(A)) */
#define thr_got_alarm(A) 0
#define init_thr_alarm(A)
#define thr_alarm_kill(A)
#define resize_thr_alarm(N)
#define end_thr_alarm(A)

#else
#if defined(__WIN__)
typedef struct st_thr_alarm_entry
{
  rf_SetTimer crono;
} thr_alarm_entry;

#else /* System with posix threads */

typedef int thr_alarm_entry;

#define thr_got_alarm(thr_alarm) (**(thr_alarm))

#endif /* __WIN__ */

typedef thr_alarm_entry* thr_alarm_t;

typedef struct st_alarm {
  ulong expire_time;
  thr_alarm_entry alarmed;		/* set when alarm is due */
  pthread_t thread;
  my_thread_id thread_id;
  my_bool malloced;
} ALARM;

extern uint thr_client_alarm;
extern pthread_t alarm_thread;

#define thr_alarm_init(A) (*(A))=0
#define thr_alarm_in_use(A) (*(A)!= 0)
void init_thr_alarm(uint max_alarm);
void resize_thr_alarm(uint max_alarms);
my_bool thr_alarm(thr_alarm_t *alarmed, uint sec, ALARM *buff);
void thr_alarm_kill(my_thread_id thread_id);
void thr_end_alarm(thr_alarm_t *alarmed);
void end_thr_alarm(my_bool free_structures);
sig_handler process_alarm(int);
#ifndef thr_got_alarm
my_bool thr_got_alarm(thr_alarm_t *alrm);
#endif


#endif /* DONT_USE_THR_ALARM */

#ifdef	__cplusplus
}
#endif /* __cplusplus */
#endif /* _thr_alarm_h */
