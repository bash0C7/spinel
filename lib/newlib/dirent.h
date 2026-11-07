#ifndef SP_NEWLIB_DIRENT_H
#define SP_NEWLIB_DIRENT_H
#ifndef SP_NO_DIRENT
#include_next <dirent.h>
#else
#include <sys/dirent.h>
#endif
#endif
