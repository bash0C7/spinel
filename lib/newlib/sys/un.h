#ifndef SP_NEWLIB_SYS_UN_H
#define SP_NEWLIB_SYS_UN_H
#include <sys/socket.h>
struct sockaddr_un { sa_family_t sun_family; char sun_path[108]; };
#endif
