#ifndef SP_NEWLIB_SYS_IOCTL_H
#define SP_NEWLIB_SYS_IOCTL_H
#if __has_include_next(<sys/ioctl.h>)
#include_next <sys/ioctl.h>
#else
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414
struct winsize { unsigned short ws_row, ws_col, ws_xpixel, ws_ypixel; };
#include <errno.h>
#define ioctl(fd, req, ...) ((void)(fd), (void)(req), errno = ENOTTY, -1)
#endif
#endif
