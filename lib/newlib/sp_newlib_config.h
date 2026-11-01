/* The runtime's table sizes and first collection triggers for a board with a
   few hundred KB of RAM, force-included ahead of every source of a newlib
   build (-include lib/newlib/sp_newlib_config.h). The host defaults size the
   tables for millions of objects (about 850 KB of .bss) and first collect at
   256 KB to 1 MB, more than such a board's whole heap. Each value is only a
   default: a -D on the command line wins.
   - SP_GC_STACK_MAX: the in-place GC root table; a deeper program spills the
     rest to a heap block (sp_gc_root_push_slow), so a small one costs speed,
     not correctness.
   - SP_GC_REMEMBERED_MAX, SP_GC_PINNED_MAX: an overflow falls back to a full
     collection.
   - SP_EXC_STACK_MAX: nested begin/rescue frames; past it the program stops
     with "stack level too deep".
   - SP_DYN_SYMS_MAX: Symbols interned at run time.
   - SP_ALLOC_STATS, SP_ALLOC_NAMES, SP_STR_SHAPE_MAX: diagnostics only. */
#ifndef SP_NEWLIB_CONFIG_H
#define SP_NEWLIB_CONFIG_H
#ifndef SP_GC_STACK_MAX
#define SP_GC_STACK_MAX 256
#endif
#ifndef SP_GC_REMEMBERED_MAX
#define SP_GC_REMEMBERED_MAX 256
#endif
#ifndef SP_GC_PINNED_MAX
#define SP_GC_PINNED_MAX 128
#endif
#ifndef SP_EXC_STACK_MAX
#define SP_EXC_STACK_MAX 16
#endif
#ifndef SP_DYN_SYMS_MAX
#define SP_DYN_SYMS_MAX 64
#endif
#ifndef SP_ALLOC_STATS
#define SP_ALLOC_STATS 16
#endif
#ifndef SP_ALLOC_NAMES
#define SP_ALLOC_NAMES 16
#endif
#ifndef SP_STR_SHAPE_MAX
#define SP_STR_SHAPE_MAX 16
#endif
#ifndef SP_GC_THRESHOLD_INIT
#define SP_GC_THRESHOLD_INIT (16 * 1024)
#endif
#ifndef SP_STR_THRESHOLD_INIT
#define SP_STR_THRESHOLD_INIT (8 * 1024)
#endif
#ifndef SP_STR_OLD_THRESHOLD_INIT
#define SP_STR_OLD_THRESHOLD_INIT (32 * 1024)
#endif
#endif
