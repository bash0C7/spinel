#ifndef SP_NEWLIB_SYS_SOCKET_H
#define SP_NEWLIB_SYS_SOCKET_H
#if __has_include_next(<sys/socket.h>)
#include_next <sys/socket.h>
#else
#include <sys/types.h>
#include <stdint.h>
#include <errno.h>
typedef uint32_t socklen_t;
typedef uint16_t sa_family_t;
struct sockaddr { sa_family_t sa_family; char sa_data[14]; };
struct sockaddr_storage { sa_family_t ss_family; char ss_data[126]; };
struct msghdr;
#define AF_UNSPEC 0
#define AF_UNIX   1
#define AF_LOCAL  AF_UNIX
#define AF_INET   2
#define AF_INET6  10
#define PF_UNSPEC AF_UNSPEC
#define PF_UNIX   AF_UNIX
#define PF_INET   AF_INET
#define PF_INET6  AF_INET6
#define SOCK_STREAM 1
#define SOCK_DGRAM  2
#define SOCK_RAW    3
#define SOCK_SEQPACKET 5
#define SOL_SOCKET  1
#define SO_DEBUG     1
#define SO_REUSEADDR 2
#define SO_TYPE      3
#define SO_ERROR     4
#define SO_DONTROUTE 5
#define SO_BROADCAST 6
#define SO_SNDBUF    7
#define SO_RCVBUF    8
#define SO_KEEPALIVE 9
#define SO_OOBINLINE 10
#define SO_LINGER    13
#define SO_REUSEPORT 15
#define SO_RCVLOWAT  18
#define SO_SNDLOWAT  19
#define SO_RCVTIMEO  20
#define SO_SNDTIMEO  21
#define SOMAXCONN 128
#define MSG_OOB       0x01
#define MSG_PEEK      0x02
#define MSG_DONTROUTE 0x04
#define MSG_WAITALL   0x100
#define MSG_DONTWAIT  0x40
#define MSG_NOSIGNAL  0x4000
#define SHUT_RD   0
#define SHUT_WR   1
#define SHUT_RDWR 2
struct linger { int l_onoff; int l_linger; };
#define SP_NEWLIB_NOSOCK(...) ((void)(__VA_ARGS__), errno = ENOSYS, -1)
#define socket(d, t, p) SP_NEWLIB_NOSOCK(d, t, p)
#define socketpair(d, t, p, sv) SP_NEWLIB_NOSOCK(d, t, p, sv)
#define bind(fd, a, l) SP_NEWLIB_NOSOCK(fd, a, l)
#define connect(fd, a, l) SP_NEWLIB_NOSOCK(fd, a, l)
#define listen(fd, b) SP_NEWLIB_NOSOCK(fd, b)
#define accept(fd, a, l) SP_NEWLIB_NOSOCK(fd, a, l)
#define shutdown(fd, h) SP_NEWLIB_NOSOCK(fd, h)
#define send(fd, b, n, f) SP_NEWLIB_NOSOCK(fd, b, n, f)
#define recv(fd, b, n, f) SP_NEWLIB_NOSOCK(fd, b, n, f)
#define sendto(fd, b, n, f, a, l) SP_NEWLIB_NOSOCK(fd, b, n, f, a, l)
#define recvfrom(fd, b, n, f, a, l) SP_NEWLIB_NOSOCK(fd, b, n, f, a, l)
#define getsockopt(fd, lv, o, v, l) SP_NEWLIB_NOSOCK(fd, lv, o, v, l)
#define setsockopt(fd, lv, o, v, l) SP_NEWLIB_NOSOCK(fd, lv, o, v, l)
#define getsockname(fd, a, l) SP_NEWLIB_NOSOCK(fd, a, l)
#define getpeername(fd, a, l) SP_NEWLIB_NOSOCK(fd, a, l)
#endif
#endif
