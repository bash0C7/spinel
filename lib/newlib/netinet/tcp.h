#ifndef SP_NEWLIB_NETINET_TCP_H
#define SP_NEWLIB_NETINET_TCP_H
#if __has_include_next(<netinet/tcp.h>)
#include_next <netinet/tcp.h>
#else
#define TCP_NODELAY 1
#define TCP_MAXSEG  2
#define TCP_KEEPIDLE  4
#define TCP_KEEPINTVL 5
#define TCP_KEEPCNT   6
#endif
#endif
