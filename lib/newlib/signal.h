#ifndef SP_NEWLIB_SIGNAL_H
#define SP_NEWLIB_SIGNAL_H
#include_next <signal.h>
#include <errno.h>
#ifndef SA_RESTART
#define SA_RESTART 0x10000000
#endif
#ifndef SA_ONSTACK
#define SA_ONSTACK 0x08000000
#endif
#ifndef SA_NODEFER
#define SA_NODEFER 0x40000000
#endif
#ifdef SP_NO_SIGNALS
#define signal(s, f) ((void)(s), (void)(f), errno = ENOSYS, SIG_ERR)
#define sigaction(s, a, o) ((void)(s), (void)(a), (void)(o), errno = ENOSYS, -1)
#define sigprocmask(h, s, o) ((void)(h), (void)(s), (void)(o), 0)
#endif
#endif
