/* newlib: getline is __getline there, and the read-ahead count spinel's
   IO#ungetc path asks musl for has no newlib name (see sys/dirent.h here). */
#ifndef SP_NEWLIB_STDIO_H
#define SP_NEWLIB_STDIO_H
#include_next <stdio.h>
#include <sys/types.h>
ssize_t getline(char **, size_t *, FILE *);
ssize_t getdelim(char **, size_t *, int, FILE *);
size_t __freadahead(FILE *);
#endif
