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
#ifndef SP_CATCH_STACK_MAX
#define SP_CATCH_STACK_MAX 16
#endif
#ifndef SP_FIBER_STACK_CACHE_MAX
#define SP_FIBER_STACK_CACHE_MAX 1
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
