/* newlib on a board without a filesystem layer: the <sys/dirent.h> newlib
   ships is an #error, so <dirent.h> fails before lib/*.c compiles. This one
   declares the directory API instead; sp_newlib.c answers ENOSYS. A platform
   that has its own (ESP-IDF's VFS) defines SP_NEWLIB_HAS_DIRENT and gets it. */
#ifndef SP_NEWLIB_SYS_DIRENT_H
#define SP_NEWLIB_SYS_DIRENT_H
#ifdef SP_NEWLIB_HAS_DIRENT
#include_next <sys/dirent.h>
#else
#include <sys/types.h>
#define DT_UNKNOWN 0
#define DT_DIR     4
#define DT_REG     8
#define DT_LNK     10
struct dirent {
  ino_t d_ino;
  unsigned char d_type;
  char d_name[256];
};
typedef struct sp_newlib_dir DIR;
DIR *opendir(const char *);
DIR *fdopendir(int);
struct dirent *readdir(DIR *);
int closedir(DIR *);
void rewinddir(DIR *);
long telldir(DIR *);
void seekdir(DIR *, long);
int dirfd(DIR *);
#endif
#endif
