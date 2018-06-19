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



/* Defines to make different thread packages compatible */

#ifndef _my_pthread_h
#define _my_pthread_h

#ifndef ETIME
#define ETIME ETIMEDOUT				/* For FreeBSD */
#endif

#ifdef  __cplusplus
#define EXTERNC extern "C"
extern "C" {
#else
#define EXTERNC
#endif /* __cplusplus */

#if defined(_WIN32) || defined(_WIN64)
typedef CRITICAL_SECTION pthread_mutex_t;
typedef DWORD		 pthread_t;
typedef struct thread_attr {
    DWORD dwStackSize ;
    DWORD dwCreatingFlag ;
} pthread_attr_t ;

typedef struct { int dummy; } pthread_condattr_t;

/* Implementation of posix conditions */

typedef struct st_pthread_link {
  DWORD thread_id;
  struct st_pthread_link *next;
} pthread_link;

typedef struct {
  uint32 waiting;
  CRITICAL_SECTION lock_waiting;

  enum {
    SIGNAL= 0,
    BROADCAST= 1,
    MAX_EVENTS= 2
  } EVENTS;

  HANDLE events[MAX_EVENTS];
  HANDLE broadcast_block_event;

} pthread_cond_t;


typedef int pthread_mutexattr_t;
#define pthread_self() GetCurrentThreadId()
#define pthread_handler_t EXTERNC void * __cdecl
typedef void * (__cdecl *pthread_handler)(void *);

/*
  Struct and macros to be used in combination with the
  windows implementation of pthread_cond_timedwait
*/

/*
   Declare a union to make sure FILETIME is properly aligned
   so it can be used directly as a 64 bit value. The value
   stored is in 100ns units.
 */
union ft64 {
  FILETIME ft;
  __int64 i64;
};

struct timespec {
  union ft64 tv;
  /* The max timeout value in millisecond for pthread_cond_timedwait */
  long max_timeout_msec;
};

#define set_timespec_time_nsec(ABSTIME,TIME,NSEC) do {          \
  (ABSTIME).tv.i64= (TIME)+(__int64)(NSEC)/100;                 \
  (ABSTIME).max_timeout_msec= (long)((NSEC)/1000000);           \
} while(0)

#define set_timespec_nsec(ABSTIME,NSEC) do {                    \
  union ft64 tv;                                                \
  GetSystemTimeAsFileTime(&tv.ft);                              \
  set_timespec_time_nsec((ABSTIME), tv.i64, (NSEC));            \
} while(0)

int win_pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_create(pthread_t *,pthread_attr_t *,pthread_handler,void *);
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
         struct timespec *abstime);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_attr_init(pthread_attr_t *connect_att);
int pthread_attr_setstacksize(pthread_attr_t *connect_att,DWORD stack);
int pthread_attr_destroy(pthread_attr_t *connect_att);
struct tm *localtime_r(const time_t *timep,struct tm *tmp);
struct tm *gmtime_r(const time_t *timep,struct tm *tmp);

void pthread_exit(void *a);
int pthread_join(pthread_t thread, void **value_ptr);


#define ETIMEDOUT 145		    /* Win32 doesn't have this */
#define HAVE_LOCALTIME_R		1
#define _REENTRANT			1
#define HAVE_PTHREAD_ATTR_SETSTACKSIZE	1


#undef SAFE_MUTEX				/* This will cause conflicts */
#define pthread_key(T,V)  DWORD V
#define pthread_key_create(A,B) ((*A=TlsAlloc())==0xFFFFFFFF)
#define pthread_key_delete(A) TlsFree(A)
#define my_pthread_setspecific_ptr(T,V) (!TlsSetValue((T),(V)))
#define pthread_setspecific(A,B) (!TlsSetValue((A),(B)))
#define pthread_getspecific(A) (TlsGetValue(A))
#define my_pthread_getspecific(T,A) ((T) TlsGetValue(A))
#define my_pthread_getspecific_ptr(T,V) ((T) TlsGetValue(V))

#define pthread_equal(A,B) ((A) == (B))
#define pthread_mutex_init(A,B)  (InitializeCriticalSection(A),0)
#define pthread_mutex_lock(A)	 (EnterCriticalSection(A),0)
#define pthread_mutex_trylock(A) win_pthread_mutex_trylock((A))
#define pthread_mutex_unlock(A)  (LeaveCriticalSection(A),0)
#define pthread_mutex_destroy(A) DeleteCriticalSection(A)
#define pthread_kill(A,B) pthread_dummy((A) ? 0 : ESRCH)


/* Dummy defines for easier code */
#define pthread_attr_setdetachstate(A,B) pthread_dummy(0)
#define pthread_attr_setscope(A,B)
#define pthread_detach_this_thread()
#define pthread_condattr_init(A)
#define pthread_condattr_destroy(A)
#define pthread_yield() SwitchToThread()


#else /* Normal threads */

#ifdef HAVE_rts_threads
#define sigwait org_sigwait
#include <signal.h>
#undef sigwait
#endif
#include <pthread.h>
#ifndef _REENTRANT
#define _REENTRANT
#endif
#ifdef HAVE_THR_SETCONCURRENCY
#include <thread.h>			/* Probably solaris */
#endif
#ifdef HAVE_SCHED_H
#include <sched.h>
#endif
#ifdef HAVE_SYNCH_H
#include <synch.h>
#endif

#ifdef __NETWARE__
void my_pthread_exit(void *status);
#define pthread_exit(A) my_pthread_exit(A)
#endif

#define pthread_key(T,V) pthread_key_t V
#define my_pthread_getspecific_ptr(T,V) my_pthread_getspecific(T,(V))
#define my_pthread_setspecific_ptr(T,V) pthread_setspecific(T,(void*) (V))
#define pthread_detach_this_thread()
#define pthread_handler_t EXTERNC void *
typedef void *(* pthread_handler)(void *);

/* Test first for RTS or FSU threads */

#if defined(PTHREAD_SCOPE_GLOBAL) && !defined(PTHREAD_SCOPE_SYSTEM)
#define HAVE_rts_threads
extern int my_pthread_create_detached;
#define pthread_sigmask(A,B,C) sigprocmask((A),(B),(C))
#define PTHREAD_CREATE_DETACHED &my_pthread_create_detached
#define PTHREAD_SCOPE_SYSTEM  PTHREAD_SCOPE_GLOBAL
#define PTHREAD_SCOPE_PROCESS PTHREAD_SCOPE_LOCAL
#define USE_ALARM_THREAD
#endif /* defined(PTHREAD_SCOPE_GLOBAL) && !defined(PTHREAD_SCOPE_SYSTEM) */

#if defined(_BSDI_VERSION) && _BSDI_VERSION < 199910
int sigwait(sigset_t *set, int *sig);
#endif

#ifndef HAVE_NONPOSIX_SIGWAIT
#define my_sigwait(A,B) sigwait((A),(B))
#else
int my_sigwait(const sigset_t *set,int *sig);
#endif

#ifdef HAVE_NONPOSIX_PTHREAD_MUTEX_INIT
#ifndef SAFE_MUTEX
#define pthread_mutex_init(a,b) my_pthread_mutex_noposix_init((a),(b))
extern int my_pthread_mutex_noposix_init(pthread_mutex_t *mp,
                                         const pthread_mutexattr_t *attr);
#endif /* SAFE_MUTEX */
#define pthread_cond_init(a,b) my_pthread_cond_noposix_init((a),(b))
extern int my_pthread_cond_noposix_init(pthread_cond_t *mp,
                                        const pthread_condattr_t *attr);
#endif /* HAVE_NONPOSIX_PTHREAD_MUTEX_INIT */

#if defined(HAVE_SIGTHREADMASK) && !defined(HAVE_PTHREAD_SIGMASK)
#define pthread_sigmask(A,B,C) sigthreadmask((A),(B),(C))
#endif

#if !defined(HAVE_SIGWAIT) && !defined(HAVE_rts_threads) && !defined(sigwait) && !defined(alpha_linux_port) && !defined(HAVE_NONPOSIX_SIGWAIT) && !defined(HAVE_DEC_3_2_THREADS) && !defined(_AIX)
int sigwait(sigset_t *setp, int *sigp);		/* Use our implemention */
#endif


/*
  We define my_sigset() and use that instead of the system sigset() so that
  we can favor an implementation based on sigaction(). On some systems, such
  as Mac OS X, sigset() results in flags such as SA_RESTART being set, and
  we want to make sure that no such flags are set.
*/
#if defined(HAVE_SIGACTION) && !defined(my_sigset)
#define my_sigset(A,B) do { struct sigaction l_s; sigset_t l_set; int l_rc; \
                            DBUG_ASSERT((A) != 0);                          \
                            sigemptyset(&l_set);                            \
                            l_s.sa_handler = (B);                           \
                            l_s.sa_mask   = l_set;                          \
                            l_s.sa_flags   = 0;                             \
                            l_rc= sigaction((A), &l_s, (struct sigaction *) NULL);\
                            DBUG_ASSERT(l_rc == 0);                         \
                          } while (0)
#elif defined(HAVE_SIGSET) && !defined(my_sigset)
#define my_sigset(A,B) sigset((A),(B))
#elif !defined(my_sigset)
#define my_sigset(A,B) signal((A),(B))
#endif

#if !defined(HAVE_PTHREAD_ATTR_SETSCOPE) || defined(HAVE_DEC_3_2_THREADS)
#define pthread_attr_setscope(A,B)
#undef	HAVE_GETHOSTBYADDR_R			/* No definition */
#endif

#if defined(HAVE_BROKEN_PTHREAD_COND_TIMEDWAIT) && !defined(SAFE_MUTEX)
extern int my_pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, struct timespec *abstime);
#define pthread_cond_timedwait(A,B,C) my_pthread_cond_timedwait((A),(B),(C))
#endif

#if !defined( HAVE_NONPOSIX_PTHREAD_GETSPECIFIC)
#define my_pthread_getspecific(A,B) ((A) pthread_getspecific(B))
#else
#define my_pthread_getspecific(A,B) ((A) my_pthread_getspecific_imp(B))
void *my_pthread_getspecific_imp(pthread_key_t key);
#endif

#ifndef HAVE_LOCALTIME_R
struct tm *localtime_r(const time_t *clock, struct tm *res);
#endif

#ifndef HAVE_GMTIME_R
struct tm *gmtime_r(const time_t *clock, struct tm *res);
#endif

#ifdef HAVE_PTHREAD_CONDATTR_CREATE
/* DCE threads on HPUX 10.20 */
#define pthread_condattr_init pthread_condattr_create
#define pthread_condattr_destroy pthread_condattr_delete
#endif

/* FSU THREADS */
#if !defined(HAVE_PTHREAD_KEY_DELETE) && !defined(pthread_key_delete)
#define pthread_key_delete(A) pthread_dummy(0)
#endif

#ifdef HAVE_CTHREADS_WRAPPER			/* For MacOSX */
#define pthread_cond_destroy(A) pthread_dummy(0)
#define pthread_mutex_destroy(A) pthread_dummy(0)
#define pthread_attr_delete(A) pthread_dummy(0)
#define pthread_condattr_delete(A) pthread_dummy(0)
#define pthread_attr_setstacksize(A,B) pthread_dummy(0)
#define pthread_equal(A,B) ((A) == (B))
#define pthread_cond_timedwait(a,b,c) pthread_cond_wait((a),(b))
#define pthread_attr_init(A) pthread_attr_create(A)
#define pthread_attr_destroy(A) pthread_attr_delete(A)
#define pthread_attr_setdetachstate(A,B) pthread_dummy(0)
#define pthread_create(A,B,C,D) pthread_create((A),*(B),(C),(D))
#define pthread_sigmask(A,B,C) sigprocmask((A),(B),(C))
#define pthread_kill(A,B) pthread_dummy((A) ? 0 : ESRCH)
#undef	pthread_detach_this_thread
#define pthread_detach_this_thread() { pthread_t tmp=pthread_self() ; pthread_detach(&tmp); }
#endif

#ifdef HAVE_DARWIN5_THREADS
#define pthread_sigmask(A,B,C) sigprocmask((A),(B),(C))
#define pthread_kill(A,B) pthread_dummy((A) ? 0 : ESRCH)
#define pthread_condattr_init(A) pthread_dummy(0)
#define pthread_condattr_destroy(A) pthread_dummy(0)
#undef	pthread_detach_this_thread
#define pthread_detach_this_thread() { pthread_t tmp=pthread_self() ; pthread_detach(tmp); }
#endif

#if ((defined(HAVE_PTHREAD_ATTR_CREATE) && !defined(HAVE_SIGWAIT)) || defined(HAVE_DEC_3_2_THREADS)) && !defined(HAVE_CTHREADS_WRAPPER)
/* This is set on AIX_3_2 and Siemens unix (and DEC OSF/1 3.2 too) */
#define pthread_key_create(A,B) \
    pthread_keycreate(A,(B) ?\
          (pthread_destructor_t) (B) :\
          (pthread_destructor_t) pthread_dummy)
#define pthread_attr_init(A) pthread_attr_create(A)
#define pthread_attr_destroy(A) pthread_attr_delete(A)
#define pthread_attr_setdetachstate(A,B) pthread_dummy(0)
#define pthread_create(A,B,C,D) pthread_create((A),*(B),(C),(D))
#ifndef pthread_sigmask
#define pthread_sigmask(A,B,C) sigprocmask((A),(B),(C))
#endif
#define pthread_kill(A,B) pthread_dummy((A) ? 0 : ESRCH)
#undef	pthread_detach_this_thread
#define pthread_detach_this_thread() { pthread_t tmp=pthread_self() ; pthread_detach(&tmp); }
#elif !defined(__NETWARE__) /* HAVE_PTHREAD_ATTR_CREATE && !HAVE_SIGWAIT */
#define HAVE_PTHREAD_KILL
#endif

#endif /* defined(__WIN__) */

#if defined(HPUX10) && !defined(DONT_REMAP_PTHREAD_FUNCTIONS)
#undef pthread_cond_timedwait
#define pthread_cond_timedwait(a,b,c) my_pthread_cond_timedwait((a),(b),(c))
int my_pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
            struct timespec *abstime);
#endif

#if defined(HPUX10)
#define pthread_attr_getstacksize(A,B) my_pthread_attr_getstacksize(A,B)
void my_pthread_attr_getstacksize(pthread_attr_t *attrib, size_t *size);
#endif

#if defined(HAVE_POSIX1003_4a_MUTEX) && !defined(DONT_REMAP_PTHREAD_FUNCTIONS)
#undef pthread_mutex_trylock
#define pthread_mutex_trylock(a) my_pthread_mutex_trylock((a))
int my_pthread_mutex_trylock(pthread_mutex_t *mutex);
#endif

#if !defined(HAVE_PTHREAD_YIELD_ONE_ARG) && !defined(HAVE_PTHREAD_YIELD_ZERO_ARG)
/* no pthread_yield() available */
#ifdef HAVE_SCHED_YIELD
#define pthread_yield() sched_yield()
#elif defined(HAVE_PTHREAD_YIELD_NP) /* can be Mac OS X */
#define pthread_yield() pthread_yield_np()
#elif defined(HAVE_THR_YIELD)
#define pthread_yield() thr_yield()
#endif
#endif

/*
  The defines set_timespec and set_timespec_nsec should be used
  for calculating an absolute time at which
  pthread_cond_timedwait should timeout
*/

#define set_timespec(ABSTIME,SEC) set_timespec_nsec((ABSTIME),(SEC)*1000000000ULL)

#ifndef set_timespec_nsec
#define set_timespec_nsec(ABSTIME,NSEC)                                 \
  set_timespec_time_nsec((ABSTIME),my_getsystime(),(NSEC))
#endif /* !set_timespec_nsec */

/* adapt for two different flavors of struct timespec */
#ifdef HAVE_TIMESPEC_TS_SEC
#define MY_tv_sec  ts_sec
#define MY_tv_nsec ts_nsec
#else
#define MY_tv_sec  tv_sec
#define MY_tv_nsec tv_nsec
#endif /* HAVE_TIMESPEC_TS_SEC */

#ifndef set_timespec_time_nsec
#define set_timespec_time_nsec(ABSTIME,TIME,NSEC) do {                  \
  ulonglong nsec= (NSEC);                                               \
  ulonglong now= (TIME) + (nsec/100);                                   \
  (ABSTIME).MY_tv_sec=  (now / ULL(10000000));                          \
  (ABSTIME).MY_tv_nsec= (now % ULL(10000000) * 100 + (nsec % 100));     \
} while(0)
#endif /* !set_timespec_time_nsec */

/* safe_mutex adds checking to mutex for easier debugging */

#if defined(__NETWARE__) && !defined(SAFE_MUTEX_DETECT_DESTROY)
#define SAFE_MUTEX_DETECT_DESTROY
#endif
struct st_hash;

typedef struct st_safe_mutex_t
{
  pthread_mutex_t global,mutex;
  const char *file, *name;
  uint line,count;
  myf create_flags, active_flags;
  ulong id;
  pthread_t thread;
  struct st_hash *locked_mutex, *used_mutex;
  struct st_safe_mutex_t *prev, *next;
#ifdef SAFE_MUTEX_DETECT_DESTROY
  struct st_safe_mutex_info_t *info;	/* to track destroying of mutexes */
#endif
} safe_mutex_t;

typedef struct st_safe_mutex_deadlock_t
{
  const char *file, *name;
  safe_mutex_t *mutex;
  uint line;
  ulong count;
  ulong id;
  my_bool warning_only;
} safe_mutex_deadlock_t;

#ifdef SAFE_MUTEX_DETECT_DESTROY
/*
  Used to track the destroying of mutexes. This needs to be a seperate
  structure because the safe_mutex_t structure could be freed before
  the mutexes are destroyed.
*/

typedef struct st_safe_mutex_info_t
{
  struct st_safe_mutex_info_t *next;
  struct st_safe_mutex_info_t *prev;
  const char *init_file;
  uint32 init_line;
} safe_mutex_info_t;
#endif /* SAFE_MUTEX_DETECT_DESTROY */

int safe_mutex_init(safe_mutex_t *mp, const pthread_mutexattr_t *attr,
                    const char *name, myf my_flags,
                    const char *file, uint line);
int safe_mutex_lock(safe_mutex_t *mp, myf my_flags, const char *file,
                    uint line);
int safe_mutex_unlock(safe_mutex_t *mp,const char *file, uint line);
int safe_mutex_destroy(safe_mutex_t *mp,const char *file, uint line);
int safe_cond_wait(pthread_cond_t *cond, safe_mutex_t *mp,const char *file,
       uint line);
int safe_cond_timedwait(pthread_cond_t *cond, safe_mutex_t *mp,
      struct timespec *abstime, const char *file, uint line);
void safe_mutex_global_init(void);
void safe_mutex_end(FILE *file);
void safe_mutex_free_deadlock_data(safe_mutex_t *mp);

  /* Wrappers if safe mutex is actually used */
#define MYF_TRY_LOCK              1
#define MYF_NO_DEADLOCK_DETECTION 2

#ifdef SAFE_MUTEX
#undef pthread_mutex_init
#undef pthread_mutex_lock
#undef pthread_mutex_unlock
#undef pthread_mutex_destroy
#undef pthread_mutex_wait
#undef pthread_mutex_timedwait
#undef pthread_mutex_t
#undef pthread_cond_wait
#undef pthread_cond_timedwait
#undef pthread_mutex_trylock
#define my_pthread_mutex_init(A,B,C,D) safe_mutex_init((A),(B),(C),(D),__FILE__,__LINE__)
#define pthread_mutex_init(A,B) safe_mutex_init((A),(B),#A,0,__FILE__,__LINE__)
#define pthread_mutex_lock(A) safe_mutex_lock((A), 0, __FILE__, __LINE__)
#define my_pthread_mutex_lock(A,B) safe_mutex_lock((A), (B), __FILE__, __LINE__)
#define pthread_mutex_unlock(A) safe_mutex_unlock((A),__FILE__,__LINE__)
#define pthread_mutex_destroy(A) safe_mutex_destroy((A),__FILE__,__LINE__)
#define pthread_cond_wait(A,B) safe_cond_wait((A),(B),__FILE__,__LINE__)
#define pthread_cond_timedwait(A,B,C) safe_cond_timedwait((A),(B),(C),__FILE__,__LINE__)
#define pthread_mutex_trylock(A) safe_mutex_lock((A), MYF_TRY_LOCK, __FILE__, __LINE__)
#define pthread_mutex_t safe_mutex_t
#define safe_mutex_assert_owner(mp) \
          DBUG_ASSERT((mp)->count > 0 && \
                      pthread_equal(pthread_self(), (mp)->thread))
#define safe_mutex_assert_not_owner(mp) \
          DBUG_ASSERT(! (mp)->count || \
                      ! pthread_equal(pthread_self(), (mp)->thread))
#else
#define my_pthread_mutex_init(A,B,C,D) pthread_mutex_init((A),(B))
#define my_pthread_mutex_lock(A,B) pthread_mutex_lock(A)
#define safe_mutex_assert_owner(mp)
#define safe_mutex_assert_not_owner(mp)
#define safe_mutex_free_deadlock_data(mp)
#endif /* SAFE_MUTEX */

#if defined(MY_PTHREAD_FASTMUTEX) && !defined(SAFE_MUTEX)
typedef struct st_my_pthread_fastmutex_t
{
  pthread_mutex_t mutex;
  uint spins;
  uint rng_state;
} my_pthread_fastmutex_t;
void fastmutex_global_init(void);

int my_pthread_fastmutex_init(my_pthread_fastmutex_t *mp,
                              const pthread_mutexattr_t *attr);
int my_pthread_fastmutex_lock(my_pthread_fastmutex_t *mp);

#undef pthread_mutex_init
#undef pthread_mutex_lock
#undef pthread_mutex_unlock
#undef pthread_mutex_destroy
#undef pthread_mutex_wait
#undef pthread_mutex_timedwait
#undef pthread_mutex_t
#undef pthread_cond_wait
#undef pthread_cond_timedwait
#undef pthread_mutex_trylock
#define pthread_mutex_init(A,B) my_pthread_fastmutex_init((A),(B))
#define pthread_mutex_lock(A) my_pthread_fastmutex_lock(A)
#define pthread_mutex_unlock(A) pthread_mutex_unlock(&(A)->mutex)
#define pthread_mutex_destroy(A) pthread_mutex_destroy(&(A)->mutex)
#define pthread_cond_wait(A,B) pthread_cond_wait((A),&(B)->mutex)
#define pthread_cond_timedwait(A,B,C) pthread_cond_timedwait((A),&(B)->mutex,(C))
#define pthread_mutex_trylock(A) pthread_mutex_trylock(&(A)->mutex)
#define pthread_mutex_t my_pthread_fastmutex_t
#endif /* defined(MY_PTHREAD_FASTMUTEX) && !defined(SAFE_MUTEX) */


#define GETHOSTBYADDR_BUFF_SIZE 2048

#ifndef HAVE_THR_SETCONCURRENCY
#define thr_setconcurrency(A) pthread_dummy(0)
#endif
#if !defined(HAVE_PTHREAD_ATTR_SETSTACKSIZE) && ! defined(pthread_attr_setstacksize)
#define pthread_attr_setstacksize(A,B) pthread_dummy(0)
#endif

/* Define mutex types, see my_thr_init.c */
#define MY_MUTEX_INIT_SLOW   NULL
#ifdef PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP
extern pthread_mutexattr_t my_fast_mutexattr;
#define MY_MUTEX_INIT_FAST &my_fast_mutexattr
#else
#define MY_MUTEX_INIT_FAST   NULL
#endif
#ifdef PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP
extern pthread_mutexattr_t my_errorcheck_mutexattr;
#define MY_MUTEX_INIT_ERRCHK &my_errorcheck_mutexattr
#else
#define MY_MUTEX_INIT_ERRCHK   NULL
#endif

#ifndef ESRCH
/* Define it to something */
#define ESRCH 1
#endif

typedef ulong my_thread_id;

extern void my_threadattr_global_init(void);
extern my_bool my_thread_global_init(void);
extern void my_thread_global_end(void);
extern my_bool my_thread_init(void);
extern void my_thread_end(void);
extern const char *my_thread_name(void);
extern my_thread_id my_thread_dbug_id(void);
extern int pthread_no_free(void *);
extern int pthread_dummy(int);

/* All thread specific variables are in the following struct */

#define THREAD_NAME_SIZE 10
#ifndef DEFAULT_THREAD_STACK
#if SIZEOF_CHARP > 4
/*
  MySQL can survive with 32K, but some glibc libraries require > 128K stack
  To resolve hostnames. Also recursive stored procedures needs stack.
*/
#define DEFAULT_THREAD_STACK	(256*1024L)
#else
#define DEFAULT_THREAD_STACK	(195*1024)
#endif
#endif

#define MY_PTHREAD_LOCK_READ 0
#define MY_PTHREAD_LOCK_WRITE 1

struct st_my_thread_var
{
  int thr_errno;
  pthread_cond_t suspend;
  pthread_mutex_t mutex;
  pthread_mutex_t * volatile current_mutex;
  pthread_cond_t * volatile current_cond;
  pthread_t pthread_self;
  my_thread_id id;
  int cmp_length;
  int volatile abort;
  my_bool init;
  struct st_my_thread_var *next,**prev;
  void *opt_info;
  uint  lock_type; /* used by conditional release the queue */
  void  *stack_ends_here;
  safe_mutex_t *mutex_in_use;
#ifndef DBUG_OFF
  void *dbug;
  char name[THREAD_NAME_SIZE+1];
#endif
};

extern struct st_my_thread_var *_my_thread_var(void) __attribute__ ((const));
extern void **my_thread_var_dbug();
extern safe_mutex_t **my_thread_var_mutex_in_use();
extern uint my_thread_end_wait_time;
extern my_bool safe_mutex_deadlock_detector;
#define my_thread_var (_my_thread_var())
#define my_errno my_thread_var->thr_errno
/*
  Keep track of shutdown,signal, and main threads so that my_end() will not
  report errors with them
*/

/* Which kind of thread library is in use */

#define THD_LIB_OTHER 1
#define THD_LIB_NPTL  2
#define THD_LIB_LT    4

extern uint thd_lib_detected;


#ifdef  __cplusplus
}
#endif
#endif /* _my_ptread_h */
