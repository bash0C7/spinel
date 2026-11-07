#ifndef SP_NEWLIB_ARPA_INET_H
#define SP_NEWLIB_ARPA_INET_H
#if __has_include_next(<arpa/inet.h>)
#include_next <arpa/inet.h>
#else
#include <netinet/in.h>
#define inet_ntop(af, src, dst, n) ((void)(af), (void)(src), (void)(dst), (void)(n), errno = ENOSYS, (const char *)0)
#define inet_pton(af, src, dst) ((void)(af), (void)(src), (void)(dst), errno = ENOSYS, -1)
#endif
#endif
