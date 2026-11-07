#ifndef SP_NEWLIB_SIGNAL_H
#define SP_NEWLIB_SIGNAL_H
#include_next <signal.h>
#define SA_RESTART 0x10000000
#define SA_ONSTACK 0x08000000
#define SA_NODEFER 0x40000000
#endif
