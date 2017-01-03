/*
  Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include "uuid_gen.h"

#ifdef _WIN32
#include <windows.h>
#include <time.h>
#else
#include <cstring>
#endif
#include <algorithm>
#include <stdexcept>

#ifdef _WIN32

typedef CRITICAL_SECTION pthread_mutex_t;
typedef DWORD		 pthread_t;
typedef struct thread_attr {
  DWORD dwStackSize;
  DWORD dwCreatingFlag;
} pthread_attr_t;

#define pthread_getspecific(A) (TlsGetValue(A))
#define pthread_self() (GetCurrentThreadId())
#define pthread_mutex_init(A,B)  (InitializeCriticalSection(A),0)
#define pthread_mutex_lock(A)	 (EnterCriticalSection(A),0)
#define pthread_mutex_trylock(A) win_pthread_mutex_trylock((A))
#define pthread_mutex_unlock(A)  (LeaveCriticalSection(A), 0)
#define pthread_mutex_destroy(A) (DeleteCriticalSection(A), 0)

#else

#include <unistd.h>
#include <fstream>
#include <sys/time.h>

#endif

#define MY_MUTEX_INIT_FAST   NULL

/*
  The macros below are borrowed from include/linux/compiler.h in the
  Linux kernel. Use them to indicate the likelyhood of the truthfulness
  of a condition. This serves two purposes - newer versions of gcc will be
  able to optimize for branch predication, which could yield siginficant
  performance gains in frequently executed sections of the code, and the
  other reason to use them is for documentation
*/

#if !defined(__GNUC__) || (__GNUC__ == 2 && __GNUC_MINOR__ < 96)
#define __builtin_expect(x, expected_value) (x)
#endif

#define likely(x)	__builtin_expect((x),1)
#define unlikely(x)	__builtin_expect((x),0)

static pthread_mutex_t LOCK_uuid_generator;

/* The seed and the random value are stored in uuid_seed */
static uint16_t uuid_seed = 0;

/* Needed for the time counting */
static unsigned int nanoseq = 0;

/* The last used time value is stored here */
static unsigned long long uuid_time2 = 0;

#if defined(_WIN32)
static unsigned long long query_performance_frequency = 0;
static unsigned long long query_performance_offset = 0;
#endif

static uint16_t time_seq_global = 0;

/*
  The randomly generated node part is stored here.
  It is generated only when the seed is changed.
  All other times it is just re-used.
*/
unsigned char node_global[6];

/**
  number of 100-nanosecond intervals between
  1582-10-15 00:00:00.00 and 1970-01-01 00:00:00.00.
*/
#define UUID_TIME_OFFSET ((unsigned long long) 141427 * 24 * 60 * 60 * \
  1000 * 1000 * 10)

#define UUID_MS 10000000

#define UUID_VERSION      0x1000
#define UUID_VARIANT      0x8000

/* Generate pseudo-random values using Fibonaccy sequence */
uint16_t rand_fibonacci()
{
  uint16_t bit = ((uuid_seed >> 0) ^ (uuid_seed >> 2) ^
                  (uuid_seed >> 3) ^ (uuid_seed >> 5)) & 1;
  uuid_seed = (uuid_seed >> 1) | (bit << 15);
  return uuid_seed;
}


unsigned long long my_getsystime()
{
#ifdef HAVE_CLOCK_GETTIME
  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  return (unsigned long long)tp.tv_sec * 10000000 + (unsigned long long)tp.tv_nsec / 100;
#elif defined(_WIN32)
  LARGE_INTEGER t_cnt;
  if (query_performance_frequency)
  {
    QueryPerformanceCounter(&t_cnt);
    return ((t_cnt.QuadPart / query_performance_frequency * 10000000) +
            ((t_cnt.QuadPart % query_performance_frequency) * 10000000 /
            query_performance_frequency) + query_performance_offset);
  }
  return 0;
#else
  /* TODO: check for other possibilities for hi-res timestamping */
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (unsigned long long)tv.tv_sec * 10000000 + (unsigned long long)tv.tv_usec * 10;
#endif
}

/* Get process id */
static uint32_t get_proc_id()
{
#ifdef _WIN32
  return (uint32_t)GetCurrentProcessId();
#else
  return (uint32_t)getpid();
#endif
}

/* Do initialization for UUID generator */
void init_uuid()
{
  pthread_mutex_init(&LOCK_uuid_generator, MY_MUTEX_INIT_FAST);
  uuid_seed = 0;

#if defined(_WIN32)
  FILETIME ft;
  LARGE_INTEGER li, t_cnt;

  if (QueryPerformanceFrequency((LARGE_INTEGER *)&query_performance_frequency) == 0)
    query_performance_frequency = 0;
  else
  {
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    query_performance_offset = li.QuadPart - UUID_TIME_OFFSET;
    QueryPerformanceCounter(&t_cnt);
    query_performance_offset -= (t_cnt.QuadPart /
                                 query_performance_frequency * UUID_MS +
                                 t_cnt.QuadPart %
                                 query_performance_frequency * UUID_MS /
                                 query_performance_frequency);
  }
#endif

}


void deinit_uuid()
{
  pthread_mutex_destroy(&LOCK_uuid_generator);
}


/*
  Instance of this class holds the global UUID generator mutex until
  it gets destroyed. An exception-safe way of executing a block of code while
  holding the mutex is like follows:

    {
      Uuid_guard guard;
      // code executed under the guard of the global mutex
    }
*/

struct Uuid_guard
{
  struct Initializer
  {
    Initializer() { init_uuid(); }
    ~Initializer() { deinit_uuid(); }
  };

  Uuid_guard()
  {
    /*
      This static initializer instance gets constructed only once.
      It ensures that uuid module is properly initialized before
      using the mutex.
    */
    static Initializer init;
    pthread_mutex_lock(&LOCK_uuid_generator);
  }

  ~Uuid_guard()
  {
    pthread_mutex_unlock(&LOCK_uuid_generator);
  }
};


/*
  Structure of UUID produced by the generator.
  It already has its components reversed, but we will
  consider the order of components as direct.
*/
struct uuid_internal_st
{
  unsigned char  node[6];
  uint16_t clock_seq;
  uint16_t time_hi_and_version;
  uint16_t time_mid;
  uint32_t time_low;
};

/* Random generation of the node part in UUID */
void generate_node()
{
  uint16_t i = 0;
  uint16_t rand_buf[3];

  /* Run a few steps through the sequence */
  i = rand_fibonacci() & 7;
  while (i && rand_fibonacci()) --i;

  for (i = 0; i < 3; i++)
  {
    rand_buf[i] = rand_fibonacci();
  }

  memcpy(node_global, rand_buf, sizeof(node_global));
  time_seq_global = rand_fibonacci();
}


namespace uuid
{

void generate_uuid(uuid_type &uuid)
{
  if (!uuid_seed)
    throw std::logic_error("The seed must be set for random numbers generator");

  uuid_internal_st uuid_internal;

  Uuid_guard guard;

  unsigned long long tv = my_getsystime() + UUID_TIME_OFFSET + nanoseq;

  if (likely(tv > uuid_time2))
  {
    /*
      Current time is ahead of last timestamp, as it should be.
      If we "borrowed time", give it back, just as long as we
      stay ahead of the previous timestamp.
    */
    if (nanoseq)
    {
      /*
        -1 so we won't make tv= uuid_time for nanoseq >= (tv - uuid_time)
      */
      unsigned long delta = std::min<unsigned long>(nanoseq, (unsigned long)(tv - uuid_time2 - 1));
      tv -= delta;
      nanoseq -= delta;
    }
  }
  else
  {
    if (unlikely(tv == uuid_time2))
    {
      /*
        For low-res system clocks. If several requests for UUIDs
        end up on the same tick, we add a nano-second to make them
        different.
        ( current_timestamp + nanoseq * calls_in_this_period )
        may end up > next_timestamp; this is OK. Nonetheless, we'll
        try to unwind nanoseq when we get a chance to.
        If nanoseq overflows, we'll start over with a new numberspace
        (so the if() below is needed so we can avoid the ++tv and thus
        match the follow-up if() if nanoseq overflows!).
      */
      if (likely(++nanoseq))
        ++tv;
    }

    if (unlikely(tv <= uuid_time2))
    {
      /*
        If the admin changes the system clock (or due to Daylight
        Saving Time), the system clock may be turned *back* so we
        go through a period once more for which we already gave out
        UUIDs.  To avoid duplicate UUIDs despite potentially identical
        times, we make a new random component.
        We also come here if the nanoseq "borrowing" overflows.
        In either case, we throw away any nanoseq borrowing since it's
        irrelevant in the new numberspace.
      */
      tv = my_getsystime() + UUID_TIME_OFFSET;
      time_seq_global = rand_fibonacci() | UUID_VARIANT;

      nanoseq = 0;
    }
  }

  uuid_time2 = tv;

  uuid_internal.time_low = (uint32_t)(tv & 0xFFFFFFFF);
  uuid_internal.time_mid = (uint16_t)((tv >> 32) & 0xFFFF);
  uuid_internal.time_hi_and_version = (uint16_t)((tv >> 48) | UUID_VERSION);
  uuid_internal.clock_seq = time_seq_global;

  memcpy(uuid_internal.node, node_global, sizeof(node_global));
  memcpy(uuid, &uuid_internal, sizeof(uuid_internal));
}


void set_seed(uint16_t seed)
{
  Uuid_guard guard;
  uuid_seed ^= seed;
  generate_node();
}


void set_seed_from_time_pid()
{
  set_seed((uint16_t)(time(0) ^ get_proc_id()));
}


}