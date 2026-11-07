#ifndef SP_NEWLIB_UCONTEXT_H
#define SP_NEWLIB_UCONTEXT_H
#if __has_include_next(<ucontext.h>)
#include_next <ucontext.h>
#else
#include <signal.h>
#include <errno.h>
typedef struct ucontext_t {
  struct ucontext_t *uc_link;
  stack_t uc_stack;
  int uc_mcontext;
} ucontext_t;
#define getcontext(u) ((void)(u), errno = ENOSYS, -1)
#define setcontext(u) ((void)(u), errno = ENOSYS, -1)
#define makecontext(u, f, ...) ((void)(u), (void)(f))
#define swapcontext(o, u) ((void)(o), (void)(u), errno = ENOSYS, -1)
#endif
#endif
