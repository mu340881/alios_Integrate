/*
 * time.h
 * 
 * Struct and function declarations for dealing with time.
 */

#ifndef _TIME_H_
#define _TIME_H_

#include <stdio.h>

//#define __need_size_t
//#define __need_NULL
//#include <stddef.h>
#include "_ansi.h"

/* Get _CLOCKS_PER_SEC_ */
#include <machine/time.h>
#include <sys/times.h>

//#include "_ansi.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
/*
 * Structure returned by gettimeofday(2) system call,
 * and used in other calls.
 */
struct timeval {
    long    tv_sec;     /* seconds */
    long    tv_usec;    /* and microseconds */
};
#endif /* _TIMEVAL_DEFINED */

#ifndef _TIMESPEC_DEFINED
#define _TIMESPEC_DEFINED
/*
 * Structure defined by POSIX.1b to be like a timeval.
 */
struct timespec {
    time_t  tv_sec;     /* seconds */
    long    tv_nsec;    /* and nanoseconds */
};
#endif /* _TIMESPEC_DEFINED */ 

struct timezone {
  int tz_minuteswest;   /* minutes west of Greenwich */
  int tz_dsttime;       /* type of dst correction */
};

int gettimeofday(struct timeval *tp, void *ignore);


#ifndef _CLOCKS_PER_SEC_
#define _CLOCKS_PER_SEC_ 1000
#endif

#define CLOCKS_PER_SEC _CLOCKS_PER_SEC_
#define CLK_TCK CLOCKS_PER_SEC

typedef unsigned long useconds_t;
typedef long suseconds_t;

//#include <sys/types.h>

//_BEGIN_STD_C

struct tm
{
  int	tm_sec;
  int	tm_min;
  int	tm_hour;
  int	tm_mday;
  int	tm_mon;/*0~11*/
  int	tm_year;
  int	tm_wday;/*0~6,sunday==0,monday==1*/
  int	tm_yday;
  int	tm_isdst;
#ifdef __TM_GMTOFF
  long	__TM_GMTOFF;
#endif
#ifdef __TM_ZONE
  const char *__TM_ZONE;
#endif
};


//_END_STD_C

#ifndef _REENT_ONLY
char	  *_EXFUN(asctime,  (const struct tm *_tblock));
char	  *_EXFUN(ctime,    (const time_t *_time));
struct tm *_EXFUN(gmtime,   (const time_t *_timer));
struct tm *_EXFUN(localtime,(const time_t *_timer));
#endif

size_t	   _EXFUN(strftime, (char *__restrict _s,
			     size_t _maxsize, const char *__restrict _fmt,
			     const struct tm *__restrict _t));


#ifdef __cplusplus
}
#endif

#endif /* _TIME_H_ */

