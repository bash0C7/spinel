#ifndef SP_NEWLIB_TIME_H
#define SP_NEWLIB_TIME_H
#include_next <time.h>
#if !defined(_POSIX_TIMERS)
#include <sys/_timeval.h>
#include <stdint.h>
int gettimeofday(struct timeval *, void *);
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC CLOCK_REALTIME
#endif
#ifndef CLOCK_PROCESS_CPUTIME_ID
#define CLOCK_PROCESS_CPUTIME_ID CLOCK_REALTIME
#endif
#ifndef CLOCK_THREAD_CPUTIME_ID
#define CLOCK_THREAD_CPUTIME_ID CLOCK_REALTIME
#endif
static inline int sp_newlib_clock_gettime(clockid_t id, struct timespec *ts) {
  struct timeval tv;
  (void)id;
  if (gettimeofday(&tv, 0) != 0) return -1;
  ts->tv_sec = tv.tv_sec;
  ts->tv_nsec = (long)tv.tv_usec * 1000;
  return 0;
}
static inline int sp_newlib_clock_getres(clockid_t id, struct timespec *ts) {
  (void)id;
  if (ts) { ts->tv_sec = 0; ts->tv_nsec = 1000; }
  return 0;
}
static inline int sp_newlib_nanosleep(const struct timespec *req, struct timespec *rem) {
  struct timeval t0, t;
  int64_t want = (int64_t)req->tv_sec * 1000000 + req->tv_nsec / 1000;
  if (gettimeofday(&t0, 0) != 0) return -1;
  do {
    if (gettimeofday(&t, 0) != 0) return -1;
  } while ((int64_t)(t.tv_sec - t0.tv_sec) * 1000000 + (t.tv_usec - t0.tv_usec) < want);
  if (rem) { rem->tv_sec = 0; rem->tv_nsec = 0; }
  return 0;
}
#define clock_gettime(id, ts) sp_newlib_clock_gettime(id, ts)
#define clock_getres(id, ts) sp_newlib_clock_getres(id, ts)
#define nanosleep(req, rem) sp_newlib_nanosleep(req, rem)
#endif
#endif
