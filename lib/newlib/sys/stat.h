#ifndef SP_NEWLIB_SYS_STAT_H
#define SP_NEWLIB_SYS_STAT_H
#include_next <sys/stat.h>
int lstat(const char *, struct stat *);
#endif
