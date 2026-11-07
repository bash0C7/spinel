#ifndef SP_NEWLIB_STDIO_H
#define SP_NEWLIB_STDIO_H
#include_next <stdio.h>
#define getline(b, n, fp) __getline(b, n, fp)
#define getdelim(b, n, d, fp) __getdelim(b, n, d, fp)
#define __freadahead(fp) ((size_t)((fp)->_r > 0 ? (fp)->_r : 0))
#define flockfile(fp) ((void)(fp))
#define funlockfile(fp) ((void)(fp))
#endif
