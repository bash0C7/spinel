/* newlib ships <termios.h> without the <sys/termios.h> it includes: no
   terminal settings. SP_NO_TERMIOS makes io/console's raw and noecho raise
   ENOTTY as on wasi (see sys/dirent.h here). */
#ifndef SP_NEWLIB_TERMIOS_H
#define SP_NEWLIB_TERMIOS_H
#if defined(__has_include_next) && __has_include_next(<sys/termios.h>)
#include_next <termios.h>
#else
#define SP_NO_TERMIOS 1
#endif
#endif
