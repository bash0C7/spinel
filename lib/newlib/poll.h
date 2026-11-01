/* newlib: no poll. The scheduler's and IO's waits are declared; poll answers
   ENOSYS unless the platform has one (lwIP's on ESP-IDF; see sys/dirent.h
   here). */
#ifndef SP_NEWLIB_POLL_H
#define SP_NEWLIB_POLL_H
#if defined(__has_include_next) && __has_include_next(<poll.h>)
#include_next <poll.h>
#else
#define POLLIN   0x001
#define POLLPRI  0x002
#define POLLOUT  0x004
#define POLLERR  0x008
#define POLLHUP  0x010
#define POLLNVAL 0x020
typedef unsigned int nfds_t;
struct pollfd { int fd; short events; short revents; };
int poll(struct pollfd *, nfds_t, int);
#endif
#endif
