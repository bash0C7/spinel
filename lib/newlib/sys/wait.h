#ifndef SP_NEWLIB_SYS_WAIT_H
#define SP_NEWLIB_SYS_WAIT_H
#include_next <sys/wait.h>
#define WCOREDUMP(s) 0
#endif
