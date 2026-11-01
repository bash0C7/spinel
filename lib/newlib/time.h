/* newlib declares the POSIX clocks only where the board says it has them; the
   runtime's GC pacing and Process.clock_gettime read them. Without a
   monotonic clock the realtime one stands in (see sys/dirent.h here). */
#ifndef SP_NEWLIB_TIME_H
#define SP_NEWLIB_TIME_H
#include_next <time.h>
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME ((clockid_t)1)
#endif
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC CLOCK_REALTIME
#endif
#ifndef CLOCK_PROCESS_CPUTIME_ID
#define CLOCK_PROCESS_CPUTIME_ID CLOCK_MONOTONIC
#endif
#ifndef CLOCK_THREAD_CPUTIME_ID
#define CLOCK_THREAD_CPUTIME_ID CLOCK_MONOTONIC
#endif
#ifndef _POSIX_TIMERS
int clock_gettime(clockid_t, struct timespec *);
int clock_getres(clockid_t, struct timespec *);
int nanosleep(const struct timespec *, struct timespec *);
#endif
#endif
