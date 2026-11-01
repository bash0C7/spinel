/* newlib: no sockets. The BSD socket API is declared so lib/*.c compiles;
   sp_newlib.c answers ENOSYS, and a platform with a network stack (lwIP on
   ESP-IDF) supplies the real header and calls (see sys/dirent.h here). */
#ifndef SP_NEWLIB_NETDB_H
#define SP_NEWLIB_NETDB_H
#if defined(__has_include_next) && __has_include_next(<netdb.h>)
#include_next <netdb.h>
#else
#include <sys/socket.h>
struct addrinfo {
  int ai_flags, ai_family, ai_socktype, ai_protocol;
  socklen_t ai_addrlen;
  struct sockaddr *ai_addr;
  char *ai_canonname;
  struct addrinfo *ai_next;
};
#define AI_PASSIVE     0x01
#define AI_CANONNAME   0x02
#define AI_NUMERICHOST 0x04
#define AI_NUMERICSERV 0x400
#define NI_MAXHOST 1025
#define NI_MAXSERV 32
#define NI_NUMERICHOST 1
#define NI_NUMERICSERV 2
#define EAI_AGAIN    (-3)
#define EAI_FAIL     (-4)
#define EAI_FAMILY   (-6)
#define EAI_NONAME   (-2)
#define EAI_SERVICE  (-8)
#define EAI_SOCKTYPE (-7)
int getaddrinfo(const char *, const char *, const struct addrinfo *, struct addrinfo **);
void freeaddrinfo(struct addrinfo *);
const char *gai_strerror(int);
int getnameinfo(const struct sockaddr *, socklen_t, char *, socklen_t, char *, socklen_t, int);
#endif
#endif
