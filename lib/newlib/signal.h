/* newlib: the signal surface a board does not deliver. SA_* and si_addr are
   for handlers the runtime only installs on a host OS; SP_NO_SIGNALS keeps
   Fiber's stack-overflow report out (see sys/dirent.h here). */
#ifndef SP_NEWLIB_SIGNAL_H
#define SP_NEWLIB_SIGNAL_H
#include_next <signal.h>
#ifndef SP_NO_SIGNALS
#define SP_NO_SIGNALS 1
#endif
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
