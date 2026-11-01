/* newlib: no sockets. The BSD socket API is declared so lib/*.c compiles;
   sp_newlib.c answers ENOSYS, and a platform with a network stack (lwIP on
   ESP-IDF) supplies the real header and calls (see sys/dirent.h here). */
#ifndef SP_NEWLIB_NETINET_IN_H
#define SP_NEWLIB_NETINET_IN_H
#if defined(__has_include_next) && __has_include_next(<netinet/in.h>)
#include_next <netinet/in.h>
#else
#include <sys/socket.h>
typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;
struct in_addr { in_addr_t s_addr; };
struct in6_addr { uint8_t s6_addr[16]; };
struct sockaddr_in { sa_family_t sin_family; in_port_t sin_port; struct in_addr sin_addr; char sin_zero[8]; };
struct sockaddr_in6 { sa_family_t sin6_family; in_port_t sin6_port; uint32_t sin6_flowinfo; struct in6_addr sin6_addr; uint32_t sin6_scope_id; };
#define IPPROTO_IP   0
#define IPPROTO_ICMP 1
#define IPPROTO_TCP  6
#define IPPROTO_UDP  17
#define IPPROTO_IPV6 41
#define IPPROTO_RAW  255
#define INADDR_ANY       ((in_addr_t)0x00000000)
#define INADDR_BROADCAST ((in_addr_t)0xffffffff)
#define INADDR_LOOPBACK  ((in_addr_t)0x7f000001)
#define INADDR_NONE      ((in_addr_t)0xffffffff)
#define INET_ADDRSTRLEN  16
#define INET6_ADDRSTRLEN 46
#define IPV6_V6ONLY 26
uint32_t htonl(uint32_t);
uint16_t htons(uint16_t);
uint32_t ntohl(uint32_t);
uint16_t ntohs(uint16_t);
#endif
#endif
