#ifndef SP_NEWLIB_SYS_STAT_H
#define SP_NEWLIB_SYS_STAT_H
#include_next <sys/stat.h>
#if !defined(__SPU__) && !defined(__rtems__) && !defined(__CYGWIN__)
#define lstat(p, st) stat(p, st)
#endif
#endif
