#ifndef SP_NEWLIB_POLL_H
#define SP_NEWLIB_POLL_H
#if __has_include_next(<poll.h>)
#include_next <poll.h>
#else
#include <errno.h>
#define POLLIN   0x001
#define POLLPRI  0x002
#define POLLOUT  0x004
#define POLLERR  0x008
#define POLLHUP  0x010
#define POLLNVAL 0x020
typedef unsigned int nfds_t;
struct pollfd { int fd; short events; short revents; };
#define poll(fds, n, t) ((void)(fds), (void)(n), (void)(t), errno = ENOSYS, -1)
#endif
#endif
