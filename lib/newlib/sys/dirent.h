#ifndef SP_NEWLIB_SYS_DIRENT_H
#define SP_NEWLIB_SYS_DIRENT_H
#ifndef SP_NO_DIRENT
#include_next <sys/dirent.h>
#else
#include <sys/types.h>
#include <errno.h>
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
#define opendir(p) ((void)(p), errno = ENOSYS, (DIR *)0)
#define fdopendir(fd) ((void)(fd), errno = ENOSYS, (DIR *)0)
#define readdir(d) ((void)(d), errno = ENOSYS, (struct dirent *)0)
#define closedir(d) ((void)(d), errno = ENOSYS, -1)
#define rewinddir(d) ((void)(d))
#define telldir(d) ((void)(d), errno = ENOSYS, -1L)
#define seekdir(d, l) ((void)(d), (void)(l))
#define dirfd(d) ((void)(d), errno = ENOSYS, -1)
#endif
#endif
