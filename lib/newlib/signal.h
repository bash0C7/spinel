#ifndef SP_NEWLIB_SIGNAL_H
#define SP_NEWLIB_SIGNAL_H
#include_next <signal.h>
#ifndef SA_RESTART
#define SA_RESTART 0x10000000
#endif
#ifndef SA_ONSTACK
#define SA_ONSTACK 0x08000000
#endif
#ifndef SA_NODEFER
#define SA_NODEFER 0x40000000
#endif
#endif
