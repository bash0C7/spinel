# Bare-metal boards with newlib (`lib/newlib`)

**Community-maintained, best effort.** It is kept working by its users, not
by Spinel's maintainers: it may lag behind `master`, and nothing in CI waits
on it. It lives in `lib/newlib/`, `test/newlib/` and the `newlib-test`
target, so a maintainer can remove it cheaply if it stands in the way of a
POSIX change.

```sh
spinel --cc=$CC kernel.rb -c --ext-init kernel_init --ext-entry Kernel.run -o kernel.c
$CC -c $BOARD_FLAGS -O2 -ffunction-sections -fdata-sections -Ilib/regexp/shim \
    $(spinel --cc=$CC kernel.rb --print-build | sed -n 's/^cflag //p; s/^define //p; s/^include /-I/p') \
    kernel.c lib/*.c lib/regexp/*.c
$CC $BOARD_FLAGS -Wl,--gc-sections *.o your_board.o -lm -o firmware.elf
```

The runtime builds for a microcontroller whose C library is newlib, with
the board's own cross compiler. Spinel emits the C on the host as usual
(`-c`, typically with `--ext-init` and `--ext-entry`, see
[spin.md](spin.md)), and the board's build compiles it and the runtime
sources together into its firmware.

## What you need

- A cross compiler whose C library is newlib, such as arm-none-eabi-gcc or
  an ESP32 toolchain's gcc.
- The system calls any newlib program needs from the board (`_write`,
  `_read`, `_sbrk`, `_gettimeofday`, ...): the board's own, or semihosting
  (`--specs=rdimon.specs`).
- `--cc=$CC` on both `spinel` runs. Spinel takes the Integer width from
  that compiler, and the flags below from that compiler's newlib.
- The `cflag`, `define` and `include` lines of `--print-build` on every
  file built from Spinel, the generated C and the runtime sources alike: the
  table sizes must agree between them.
- `-Wl,--gc-sections` on the link.

## What `--print-build` adds

For a compiler whose C library is newlib and that predefines none of
`__unix__`, `__APPLE__` and `_WIN32`:

| Line | Effect |
|---|---|
| `include .../lib/newlib` | the headers described below |
| `define -DSP_NO_SIGNALS` | no signal handler is installed: `trap` is accepted and never runs, and a stack overflow is the board's (a stack canary, a hard fault) |
| `define -DSP_NO_TERMIOS` | `IO#raw` and `IO#noecho` raise `Errno::ENOTTY` |
| `define -DSP_NO_DIRENT` | only when that compiler's `<dirent.h>` does not compile: no directory can be opened |
| `cflag -include .../lib/newlib/sp_newlib_config.h` | the table sizes below |

It leaves out `-falign-functions=64` and `-falign-loops=64`, and refuses a
program that uses `Thread`, `Mutex`, `Queue` or `ConditionVariable`: the
runtime there is single-threaded.

## The headers

Each header in `lib/newlib/` reads the environment's header of the same
name first, the toolchain's or an SDK's, and adds only what that header
leaves out. A header the environment does not have is replaced by one whose
calls fail: sockets, `poll`, `mmap`, `ucontext` and `ioctl`. So the slab
allocator is off and every block is a `malloc`, `IO::Buffer.map` raises
`SystemCallError` (`ENOSYS`), and `Fiber.new`, or stepping an
`Enumerator.new` block by `next`, raises `FiberError`. Without
`_POSIX_TIMERS`, every clock reads `gettimeofday`, `CLOCK_MONOTONIC`
included, and `sleep` spins on it.

No file in `lib/newlib/` defines a C library function: what the board
defines is what the firmware links.

## A call the board does not define

newlib declares calls whose definition it leaves to the board, such as
`getcwd`, `getuid`, `pipe` and `fork`. A program that reaches one links
only where the board defines it; elsewhere the link names it
(``undefined reference to `getcwd'``).

## Table sizes

`sp_newlib_config.h` sets these; each is a default, and a `-D` on the
command line wins.

| Macro | Here | Host | What it bounds |
|---|---|---|---|
| `SP_GC_STACK_MAX` | 256 | 65536 | the in-place GC root table; deeper roots spill to the heap |
| `SP_GC_REMEMBERED_MAX` | 256 | 65536 | the remembered set; an overflow takes a full collection |
| `SP_GC_PINNED_MAX` | 128 | 16384 | pinned objects; an overflow takes a full collection |
| `SP_EXC_STACK_MAX` | 16 | 64 | nested `begin` frames and `rescue` bodies |
| `SP_CATCH_STACK_MAX` | 16 | 64 | nested `catch` blocks |
| `SP_FIBER_STACK_CACHE_MAX` | 1 | 1024 | freed fiber stacks kept for reuse |
| `SP_DYN_SYMS_MAX` | 64 | 8192 | Symbols made at run time |
| `SP_ALLOC_STATS`, `SP_ALLOC_NAMES`, `SP_STR_SHAPE_MAX` | 16 | | `SPINEL_ALLOC_REPORT` and string-shape diagnostics |
| `SP_GC_THRESHOLD_INIT` | 16 KB | 256 KB | bytes allocated before the first collection of the object heap |
| `SP_STR_THRESHOLD_INIT` | 8 KB | 256 KB | the same for the string heap |
| `SP_STR_OLD_THRESHOLD_INIT` | 32 KB | 1 MB | the same for the old string generation |

Past `SP_EXC_STACK_MAX`, a `begin` stops the program with "stack level too
deep" and a `rescue` body with "rescue nesting too deep".

`Integer` is the pointer width, 32 bits on a 32-bit board, as on wasm32
([int-overflow.md](int-overflow.md)). A value past it is a Bignum.

## Tests

`make newlib-test` builds every runtime source with `NEWLIB_CC` (default
`arm-none-eabi-gcc`) for `NEWLIB_ARCH` (default `-mcpu=cortex-a9 -marm`),
with the flags `--print-build` gives, and runs these under `NEWLIB_RUN`
(default `qemu-arm`):

- `test/newlib/kernel.rb`, as an `--ext-init` library that
  `test/newlib/host.c` calls, against `test/newlib/expected`.
- Every other `test/newlib/*.rb`, as a program, against its
  `.rb.expected`. `sensor_log.rb` prints what CRuby prints, and
  `differences.rb` prints what the sections above say each method answers.

It also checks that `--print-build` refuses a `Thread` program.

It skips without the compiler, its newlib or the emulator;
`NEWLIB_REQUIRED=1` makes that a failure. An M-profile core (Cortex-M33,
Cortex-M0+) runs as:

```sh
make newlib-test NEWLIB_ARCH="-mcpu=cortex-m33 -mthumb" NEWLIB_RUN="qemu-arm -cpu cortex-m33"
make newlib-test NEWLIB_ARCH="-mcpu=cortex-m0plus -mthumb" NEWLIB_RUN="qemu-arm -cpu cortex-m0"
```
