/*
  Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.

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

#include <uuid_gen.h>

#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#include <windows.h>
#include <algorithm>
#endif

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>

/*
Begin of MySQL defs and structs
*/
#ifdef _WIN32

typedef CRITICAL_SECTION pthread_mutex_t;
typedef DWORD		 pthread_t;
typedef struct thread_attr {
    DWORD dwStackSize ;
    DWORD dwCreatingFlag ;
} pthread_attr_t ;

#define pthread_getspecific(A) (TlsGetValue(A))
#define pthread_self() (GetCurrentThreadId())
#define pthread_mutex_init(A,B)  (InitializeCriticalSection(A),0)
#define pthread_mutex_lock(A)	 (EnterCriticalSection(A),0)
#define pthread_mutex_trylock(A) win_pthread_mutex_trylock((A))
#define pthread_mutex_unlock(A)  (LeaveCriticalSection(A), 0)
#define pthread_mutex_destroy(A) (DeleteCriticalSection(A), 0)

#else

#include <unistd.h>
#include <fcntl.h>
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

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef char		my_bool; /* Small bool */

#define UUID_LENGTH_TEXT (8+1+4+1+4+1+4+1+12)

struct rand_struct {
  unsigned long seed1,seed2,max_value;
  double max_value_dbl;
};

static pthread_mutex_t LOCK_uuid_generator, LOCK_sql_rand;


/*
End MySQL defs and structs
*/

/*
  These values are obtained during the live session, but
  here we set them just to non-zero
*/
static unsigned long uuid_seed= 0;

#if defined(_WIN32)
static unsigned long long query_performance_frequency= 0;
static unsigned long long query_performance_offset= 0;
#endif


static unsigned int nanoseq= 0;
static unsigned long long uuid_time=0;
static unsigned long long uuid_time2=0;

/**
  number of 100-nanosecond intervals between
  1582-10-15 00:00:00.00 and 1970-01-01 00:00:00.00.
*/
#define UUID_TIME_OFFSET ((unsigned long long) 141427 * 24 * 60 * 60 * \
                          1000 * 1000 * 10)

#define UUID_MS 10000000

#define UUID_VERSION      0x1000
#define UUID_VARIANT      0x8000

struct rand_struct sql_rand;
struct rand_struct uuid_rand;

/**
  Getting MAC address
*/

#ifdef __FreeBSD__

#include <net/ethernet.h>
#include <sys/sysctl.h>
#include <sys/socket.h>  // for struct sockaddr
#include <net/route.h>
#include <net/if.h>
#include <net/if_dl.h>

/*
  my_gethwaddr - FreeBSD version

  @brief Retrieve MAC address from network hardware

  @param[out]  to MAC address exactly six bytes

  @return Operation status
    @retval 0       OK
    @retval <>0     FAILED
*/
my_bool my_gethwaddr(unsigned char *to)
{
  size_t len;
  char *buf, *next, *end;
  struct if_msghdr *ifm;
  struct sockaddr_dl *sdl;
  int res=1, mib[6]={CTL_NET, AF_ROUTE, 0, AF_LINK, NET_RT_IFLIST, 0};
  char zero_array[ETHER_ADDR_LEN] = {0};

  if (sysctl(mib, 6, NULL, &len, NULL, 0) == -1)
    goto err;
  if (!(buf = (char*)alloca(len)))
    goto err;
  if (sysctl(mib, 6, buf, &len, NULL, 0) < 0)
    goto err;

  end = buf + len;

  for (next = buf ; res && next < end ; next += ifm->ifm_msglen)
  {
    ifm = (struct if_msghdr *)next;
    if (ifm->ifm_type == RTM_IFINFO)
    {
      sdl= (struct sockaddr_dl *)(ifm + 1);
      std::memcpy(to, LLADDR(sdl), ETHER_ADDR_LEN);
      res= std::memcmp(to, zero_array, ETHER_ADDR_LEN) ? 0 : 1;
    }
  }

err:
  return res;
}

#elif __linux__

#include <net/if.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>

#define MAX_IFS 64
/*
  my_gethwaddr - Linux version

  @brief Retrieve MAC address from network hardware

  @param[out]  to MAC address exactly six bytes

  @return Operation status
    @retval 0       OK
    @retval <>0     FAILED
*/
my_bool my_gethwaddr(unsigned char *to)
{
  int fd= -1;
  int res= 1;
  struct ifreq ifr;
  struct ifreq ifs[MAX_IFS];
  struct ifreq *ifri= NULL;
  struct ifreq *ifend= NULL;

  char zero_array[ETHER_ADDR_LEN] = {0};
  struct ifconf ifc;

  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0)
    return 1;

  /* Retrieve interfaces */
  ifc.ifc_len= sizeof(ifs);
  ifc.ifc_req= ifs;
  if (ioctl(fd, SIOCGIFCONF, &ifc) < 0)
  {
    close(fd);
    return 1;
  }

  /* Initialize out parameter */
  std::memcpy(to, zero_array, ETHER_ADDR_LEN);

  /* Calculate first address after array */
  ifend= ifs + (ifc.ifc_len / sizeof(struct ifreq));

  /* Loop over all interfaces */
  for (ifri= ifc.ifc_req; ifri < ifend; ifri++)
  {
    if (ifri->ifr_addr.sa_family == AF_INET)
    {
      /* Reset struct, copy interface name */
      std::memset(&ifr, 0, sizeof(ifr));
      std::strncpy(ifr.ifr_name, ifri->ifr_name, sizeof(ifr.ifr_name));

      /* Get HW address, break if not 0 */
      if (ioctl(fd, SIOCGIFHWADDR, &ifr) >= 0)
      {
        std::memcpy(to, &ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN);
        if (std::memcmp(to, zero_array, ETHER_ADDR_LEN))
        {
          res= 0;
          break;
        }
      }
    }
  }
  close(fd);
  return res;
}

#elif defined(_WIN32)

/*
  The following typedef is for dynamically loading iphlpapi.dll /
  GetAdaptersAddresses. Dynamic loading is used because
  GetAdaptersAddresses is not available on Windows 2000 which MySQL
  still supports. Static linking would cause an unresolved export.
*/
typedef DWORD (WINAPI *pfnGetAdaptersAddresses)(IN ULONG Family,
    IN DWORD Flags,IN PVOID Reserved,
    OUT PIP_ADAPTER_ADDRESSES pAdapterAddresses,
    IN OUT PULONG pOutBufLen);

/*
  my_gethwaddr - Windows version

  @brief Retrieve MAC address from network hardware

  @param[out]  to MAC address exactly six bytes

  @return Operation status
    @retval 0       OK
    @retval <>0     FAILED
*/
my_bool my_gethwaddr(unsigned char *to)
{
  PIP_ADAPTER_ADDRESSES pAdapterAddresses;
  PIP_ADAPTER_ADDRESSES pCurrAddresses;
  IP_ADAPTER_ADDRESSES  adapterAddresses;
  ULONG                 address_len;
  my_bool               return_val= 1;
  pfnGetAdaptersAddresses fnGetAdaptersAddresses=
                                (pfnGetAdaptersAddresses)-1;

  if(fnGetAdaptersAddresses == (pfnGetAdaptersAddresses)-1)
  {
    /* Get the function from the DLL */
    fnGetAdaptersAddresses= (pfnGetAdaptersAddresses)
                            GetProcAddress(LoadLibrary("iphlpapi.dll"),
                                          "GetAdaptersAddresses");
  }
  if (!fnGetAdaptersAddresses)
    return 1;                                   /* failed to get function */
  address_len= sizeof (IP_ADAPTER_ADDRESSES);

  /* Get the required size for the address data. */
  if (fnGetAdaptersAddresses(AF_UNSPEC, 0, 0, &adapterAddresses, &address_len)
      == ERROR_BUFFER_OVERFLOW)
  {
    pAdapterAddresses= (PIP_ADAPTER_ADDRESSES)malloc(address_len);
    if (!pAdapterAddresses)
      return 1;                                   /* error, alloc failed */
  }
  else
    pAdapterAddresses= &adapterAddresses;         /* one is enough don't alloc */

  /* Get the hardware info. */
  if (fnGetAdaptersAddresses(AF_UNSPEC, 0, 0, pAdapterAddresses, &address_len)
      == NO_ERROR)
  {
    pCurrAddresses= pAdapterAddresses;

    while (pCurrAddresses)
    {
      /* Look for ethernet cards. */
      if (pCurrAddresses->IfType == IF_TYPE_ETHERNET_CSMACD)
      {
        /* check for a good address */
        if (pCurrAddresses->PhysicalAddressLength < 6)
            continue;                           /* bad address */

        /* save 6 bytes of the address in the 'to' parameter */
        memcpy(to, pCurrAddresses->PhysicalAddress, 6);

        /* Network card found, we're done. */
        return_val= 0;
        break;
      }
      pCurrAddresses= pCurrAddresses->Next;
    }
  }

  /* Clean up memory allocation. */
  if (pAdapterAddresses != &adapterAddresses)
    free(pAdapterAddresses);

  return return_val;
}

#else /* __FreeBSD__ || __linux__ || _WIN32 */
/* just fail */
my_bool my_gethwaddr(unsigned char *to __attribute__((unused)))
{
  return 1;
}
#endif


double my_rnd(rand_struct *rand_st)
{
  rand_st->seed1= (rand_st->seed1*3+rand_st->seed2) % rand_st->max_value;
  rand_st->seed2= (rand_st->seed1+rand_st->seed2+33) % rand_st->max_value;
  return (((double) rand_st->seed1) / rand_st->max_value_dbl);
}


unsigned long long my_getsystime()
{
#ifdef HAVE_CLOCK_GETTIME
  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  return (unsigned long long)tp.tv_sec*10000000+(unsigned long long)tp.tv_nsec/100;
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
  gettimeofday(&tv,NULL);
  return (unsigned long long)tv.tv_sec*10000000+(unsigned long long)tv.tv_usec*10;
#endif
}


unsigned long sql_rnd_with_mutex()
{
  pthread_mutex_lock(&LOCK_sql_rand);
  unsigned long tmp=(unsigned long) (my_rnd(&sql_rand) * 0xffffffff); /* make all bits random */
  pthread_mutex_unlock(&LOCK_sql_rand);
  return tmp;
}


void randominit(struct rand_struct *rand_st, unsigned long seed1, unsigned long seed2)
{                                               /* For mysql 3.21.# */
#ifdef HAVE_purify
  memset(rand_st, 0, sizeof(*rand_st));       /* Avoid UMC varnings */
#endif
  rand_st->max_value= 0x3FFFFFFFL;
  rand_st->max_value_dbl=(double) rand_st->max_value;
  rand_st->seed1=seed1%rand_st->max_value ;
  rand_st->seed2=seed2%rand_st->max_value;
}


static uint16 get_proc_id()
{
  uint16 proc_id= 0;
#ifdef _WIN32
  proc_id= (uint16)(GetCurrentProcessId() & 0x0000FFFF);
#else
  proc_id= (uint16)(getpid() & 0x0000FFFF);
#endif
  return proc_id;
}


void init_uuid(unsigned long seed)
{
  pthread_mutex_init(&LOCK_uuid_generator, MY_MUTEX_INIT_FAST);
  pthread_mutex_init(&LOCK_sql_rand, MY_MUTEX_INIT_FAST);
  uuid_seed= seed;

#if defined(_WIN32)
  FILETIME ft;
  LARGE_INTEGER li, t_cnt;

  if (QueryPerformanceFrequency((LARGE_INTEGER *)&query_performance_frequency) == 0)
    query_performance_frequency= 0;
  else
  {
    GetSystemTimeAsFileTime(&ft);
    li.LowPart=  ft.dwLowDateTime;
    li.HighPart= ft.dwHighDateTime;
    query_performance_offset= li.QuadPart-UUID_TIME_OFFSET;
    QueryPerformanceCounter(&t_cnt);
    query_performance_offset-= (t_cnt.QuadPart /
                                query_performance_frequency * UUID_MS +
                                t_cnt.QuadPart %
                                query_performance_frequency * UUID_MS /
                                query_performance_frequency);
  }
#endif
}


void end_uuid()
{
  pthread_mutex_destroy(&LOCK_uuid_generator);
  pthread_mutex_destroy(&LOCK_sql_rand);
}

void generate_uuid(uuid_type &uuid)
{

  struct uuid_internal_st
  {
    uint32 time_low;
    uint16 time_mid;
    uint16 time_hi_and_version;
    uint16 process_id;
    unsigned char  hw_mac[6];
  };

  static uuid_internal_st uuid_internal;

  pthread_mutex_lock(&LOCK_uuid_generator);

  /* The thread key address will be used for random number generation */
#ifdef _WIN32
  static DWORD key;
#else
  static pthread_t key;
#endif

  key= pthread_self();
  void *thd = (void*)key;

  int i;

  if (!uuid_time2) /* first UUID() call. initializing data */
  {
    unsigned long client_start_time= time(0);
    randominit(&sql_rand,(unsigned long) client_start_time,(unsigned long) client_start_time/2);
    unsigned long tmp=sql_rnd_with_mutex();
    if (my_gethwaddr(uuid_internal.hw_mac))
    {

      /* purecov: begin inspected */
      /*
        generating random "hardware addr"
        and because specs explicitly specify that it should NOT correlate
        with a clock_seq value (initialized random below), we use a separate
        randominit() here
      */
      randominit(&uuid_rand, tmp + (unsigned long) thd, tmp + (unsigned long)uuid_seed);
      for (i=0; i < (int)sizeof(uuid_internal.hw_mac); i++)
        uuid_internal.hw_mac[i]=(unsigned char)(my_rnd(&uuid_rand)*255);
      /* purecov: end */
    }

    uuid_internal.process_id= get_proc_id();
  }

  unsigned long long tv= my_getsystime() + UUID_TIME_OFFSET + nanoseq;

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
      unsigned long delta= std::min<unsigned long>(nanoseq, (unsigned long) (tv - uuid_time2 -1));
      tv-= delta;
      nanoseq-= delta;
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
      uuid_internal.process_id= get_proc_id();
      tv= my_getsystime() + UUID_TIME_OFFSET;
      nanoseq= 0;
    }
  }

  uuid_time2=tv;
  pthread_mutex_unlock(&LOCK_uuid_generator);

  uuid_internal.time_low=            (uint32) (tv & 0xFFFFFFFF);
  uuid_internal.time_mid=            (uint16) ((tv >> 32) & 0xFFFF);
  uuid_internal.time_hi_and_version= (uint16) ((tv >> 48) | UUID_VERSION);

  memcpy(uuid, &uuid_internal, sizeof(uuid_internal));

  union thread_to_char
  {
    pthread_t thr;
    unsigned char c[sizeof(pthread_t)];
  }t_val;

  t_val.thr = key;
  for (i = 0; i < sizeof(key); ++i)
    uuid[sizeof(uuid) - i - 1] ^= t_val.c[i]; 
}
