/* newlib: no sockets. The BSD socket API is declared so lib/*.c compiles;
   sp_newlib.c answers ENOSYS, and a platform with a network stack (lwIP on
   ESP-IDF) supplies the real header and calls (see sys/dirent.h here). */
#ifndef SP_NEWLIB_ARPA_INET_H
#define SP_NEWLIB_ARPA_INET_H
#if defined(__has_include_next) && __has_include_next(<arpa/inet.h>)
#include_next <arpa/inet.h>
#else
#include <netinet/in.h>
const char *inet_ntop(int, const void *, char *, socklen_t);
int inet_pton(int, const char *, void *);
in_addr_t inet_addr(const char *);
#endif
#endif
