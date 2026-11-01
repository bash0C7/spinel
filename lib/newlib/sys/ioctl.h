/* newlib: no terminals; ioctl answers ENOTTY's ENOSYS (see dirent.h here). */
#ifndef SP_NEWLIB_SYS_IOCTL_H
#define SP_NEWLIB_SYS_IOCTL_H
#if defined(__has_include_next) && __has_include_next(<sys/ioctl.h>)
#include_next <sys/ioctl.h>
#endif
#ifndef TIOCGWINSZ
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414
struct winsize { unsigned short ws_row, ws_col, ws_xpixel, ws_ypixel; };
int ioctl(int, int, ...);
#endif
#endif
