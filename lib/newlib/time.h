#ifndef SP_NEWLIB_TIME_H
#define SP_NEWLIB_TIME_H
#include_next <time.h>
#define CLOCK_MONOTONIC CLOCK_REALTIME
#define CLOCK_PROCESS_CPUTIME_ID CLOCK_REALTIME
#define CLOCK_THREAD_CPUTIME_ID CLOCK_REALTIME
int clock_gettime(clockid_t, struct timespec *);
int clock_getres(clockid_t, struct timespec *);
int nanosleep(const struct timespec *, struct timespec *);
#endif
