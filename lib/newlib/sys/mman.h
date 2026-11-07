#ifndef SP_NEWLIB_SYS_MMAN_H
#define SP_NEWLIB_SYS_MMAN_H
#if __has_include_next(<sys/mman.h>)
#include_next <sys/mman.h>
#else
#include <sys/types.h>
#include <errno.h>
#define PROT_NONE     0x0
#define PROT_READ     0x1
#define PROT_WRITE    0x2
#define PROT_EXEC     0x4
#define MAP_SHARED    0x01
#define MAP_PRIVATE   0x02
#define MAP_FIXED     0x10
#define MAP_ANONYMOUS 0x20
#define MAP_ANON      MAP_ANONYMOUS
#define MAP_FAILED    ((void *)-1)
#define MADV_NORMAL   0
#define MADV_DONTNEED 4
#define mmap(a, n, p, f, fd, o) ((void)(a), (void)(n), (void)(p), (void)(f), (void)(fd), (void)(o), errno = ENOSYS, MAP_FAILED)
#define munmap(a, n) ((void)(a), (void)(n), errno = ENOSYS, -1)
#define mprotect(a, n, p) ((void)(a), (void)(n), (void)(p), errno = ENOSYS, -1)
#define madvise(a, n, f) ((void)(a), (void)(n), (void)(f), errno = ENOSYS, -1)
#endif
#endif
