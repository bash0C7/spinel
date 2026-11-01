/* newlib: no ucontext. Fiber's portable context switch is declared so
   sp_fiber.c compiles; with no mmap a fiber gets no stack, so its first
   resume raises FiberError before any switch (see sys/dirent.h here). */
#ifndef SP_NEWLIB_UCONTEXT_H
#define SP_NEWLIB_UCONTEXT_H
#if defined(__has_include_next) && __has_include_next(<ucontext.h>)
#include_next <ucontext.h>
#else
#include <signal.h>
typedef struct ucontext_t {
  struct ucontext_t *uc_link;
  stack_t uc_stack;
  int uc_mcontext;
} ucontext_t;
int getcontext(ucontext_t *);
int setcontext(const ucontext_t *);
void makecontext(ucontext_t *, void (*)(void), int, ...);
int swapcontext(ucontext_t *, const ucontext_t *);
#endif
#endif
