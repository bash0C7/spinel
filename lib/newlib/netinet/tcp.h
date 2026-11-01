/* newlib: no sockets. The BSD socket API is declared so lib/*.c compiles;
   sp_newlib.c answers ENOSYS, and a platform with a network stack (lwIP on
   ESP-IDF) supplies the real header and calls (see sys/dirent.h here). */
#ifndef SP_NEWLIB_NETINET_TCP_H
#define SP_NEWLIB_NETINET_TCP_H
#if defined(__has_include_next) && __has_include_next(<netinet/tcp.h>)
#include_next <netinet/tcp.h>
#else
#define TCP_NODELAY 1
#define TCP_MAXSEG  2
#define TCP_KEEPIDLE  4
#define TCP_KEEPINTVL 5
#define TCP_KEEPCNT   6
#endif
#endif
