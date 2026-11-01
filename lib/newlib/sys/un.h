/* newlib: no sockets. The BSD socket API is declared so lib/*.c compiles;
   sp_newlib.c answers ENOSYS, and a platform with a network stack (lwIP on
   ESP-IDF) supplies the real header and calls (see sys/dirent.h here). */
#ifndef SP_NEWLIB_SYS_UN_H
#define SP_NEWLIB_SYS_UN_H
#if defined(__has_include_next) && __has_include_next(<sys/un.h>)
#include_next <sys/un.h>
#else
#include <sys/socket.h>
struct sockaddr_un { sa_family_t sun_family; char sun_path[108]; };
#endif
#endif
