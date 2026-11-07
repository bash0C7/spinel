#ifndef SP_NEWLIB_SYS_IOCTL_H
#define SP_NEWLIB_SYS_IOCTL_H
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414
struct winsize { unsigned short ws_row, ws_col, ws_xpixel, ws_ypixel; };
int ioctl(int, int, ...);
#endif
