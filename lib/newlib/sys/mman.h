/* newlib: no virtual memory. mmap answers MAP_FAILED, so the slab allocator
   turns itself off and every block is a malloc, and a Fiber stack (FiberError)
   or an IO::Buffer mapping raises (see ../sys/dirent.h here). */
#ifndef SP_NEWLIB_SYS_MMAN_H
#define SP_NEWLIB_SYS_MMAN_H
#if defined(__has_include_next) && __has_include_next(<sys/mman.h>)
#include_next <sys/mman.h>
#else
#include <sys/types.h>
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
void *mmap(void *, size_t, int, int, int, off_t);
int munmap(void *, size_t);
int mprotect(void *, size_t, int);
int madvise(void *, size_t, int);
#endif
#endif
