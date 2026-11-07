# Bare-metal boards with newlib (`lib/newlib`)

**Community-maintained, best effort.** It may lag behind master, CI does
not build it, and nothing in the gate waits on it. It lives in
`lib/newlib/`, `test/newlib/` and the `newlib-test` target, apart from the
runtime's `-D` switches it sets, which change nothing unless a build
defines them, so a maintainer can remove it cheaply if it stands in the way
of a POSIX change.

```sh
spinel --cc=$CC kernel.rb -c --ext-init kernel_init --ext-entry Kernel.run -o kernel.c
$CC -c $BOARD_FLAGS -O2 -ffunction-sections -fdata-sections -ffp-contract=off \
    -Ilib/newlib -DSP_NO_SIGNALS -DSP_NO_TERMIOS \
    -include lib/newlib/sp_newlib_config.h \
    -Ilib -Ilib/regexp -Ilib/regexp/shim \
    $(spinel --cc=$CC kernel.rb --print-build | sed -n 's/^define //p') \
    kernel.c lib/*.c lib/regexp/*.c lib/newlib/*.c
$CC $BOARD_FLAGS -Wl,--gc-sections *.o your_board.o -lm -o firmware.elf
```

The runtime builds for a microcontroller whose C library is newlib, with
the board's own cross compiler: an Arm Cortex-M or -A toolchain, or another
`*-elf` GCC. Spinel emits the C on the host as usual (`-c`, typically with
`--ext-init` and `--ext-entry`, see [spin.md](spin.md)), and the board's
build compiles it and the runtime sources together into its firmware.

## What you need

- A cross compiler whose C library is newlib, such as arm-none-eabi-gcc.
  An SDK that puts its own POSIX layer on the include path, such as ESP-IDF
  with lwIP and its VFS, is not covered, nor is picolibc.
- The system calls any newlib program needs from the board (`_write`,
  `_read`, `_sbrk`, `_gettimeofday`, ...): the board's own, or semihosting
  (`--specs=rdimon.specs`).
- `--cc=$CC` on both `spinel` runs. Spinel takes the Integer width from
  that compiler; without it a literal past `2**31` is a wrong number on a
  32-bit board ([int-overflow.md](int-overflow.md)).
- The flags above, for the generated C and the runtime alike. The table
  sizes must agree between them: `SP_GC_STACK_MAX` sizes a table both
  read, and a different `SP_EXC_STACK_MAX` or `SP_CATCH_STACK_MAX` fails
  the link. Of `--print-build`, pass the `define` lines and
  `-ffp-contract=off`.
- `-Wl,--gc-sections` on the link. Without it the code a program never
  reaches (the socket layer, `Dir`, `Process`) and its tables stay in the
  image and in RAM.

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

## What is different on this target

- **`Integer` is the pointer width**, 32 bits on a 32-bit board, as on
  wasm32 ([int-overflow.md](int-overflow.md)). A value past it is a
  Bignum.
- **No virtual memory.** `mmap` fails, so the slab allocator is off and
  every block is a `malloc`; `IO::Buffer.map` raises `SystemCallError`
  (`ENOSYS`).
- **No signals.** A signal's `trap` is accepted and never runs, and the
  stack-overflow handler is not installed, so an overflow is the board's
  (a stack canary, a hard fault).
- **No `Fiber` or `Thread`.** `Fiber.new` raises `FiberError` ("failed to
  allocate fiber stack"), which a `rescue` catches, and so does stepping
  an `Enumerator.new` block, by `next` or `take`. `next` on an enumerator
  over an Array, a Range or `each_slice` works. A program that uses
  `Thread`, `Mutex`, `Queue` or `ConditionVariable` is refused at
  `--print-build`: newlib has no pthreads.
- **No terminal modes.** `IO#raw` and `IO#noecho` raise `Errno::ENOTTY`.
- **No sockets, processes, directories or user database** unless the board
  defines the calls. A socket raises `Errno::ENOTSUP`, a name lookup
  `Socket::ResolutionError`, `Process.spawn` `SystemCallError` (`ENOSYS`),
  `Process.wait` `Errno::ECHILD`, `Dir.entries` and `Dir.new`
  `Errno::ENOENT`. `Process.uid` and the other ids are 0, and `Dir.pwd` is
  `/`.
- **Clocks.** Every clock reads `gettimeofday`, `CLOCK_MONOTONIC`
  included, and `sleep` spins on it.
- **One thread.** The GCC atomic helpers that cores without atomic
  instructions call (Cortex-M0+, and Cortex-M3/M4/M33 for 8 bytes) are
  plain reads and writes. An interrupt handler or a second core must not
  call into the runtime.

## The pieces

- `lib/newlib/` holds a stand-in for each POSIX header the runtime includes
  that newlib leaves out or leaves short: sockets, `poll`, `mmap`,
  `ucontext`, `ioctl`, resource limits, the clocks, `getline`, `lstat`.
  They are written against the newlib arm-none-eabi-gcc ships. A stand-in
  for a header newlib has reads it with `#include_next` and adds what it
  lacks; `<sys/dirent.h>` does not, because newlib's is an `#error` on a
  board without a filesystem layer. Only a newlib build sees the directory
  (`-Ilib/newlib`).
- `lib/newlib/sp_newlib.c` defines, weakly, the calls a stand-in declares
  and the POSIX calls newlib declares without defining. A board replaces
  one by defining it in an object file on the link line; a definition in
  an archive replaces it only when the linker takes that member for another
  symbol. A board's `opendir` returns `lib/newlib/sys/dirent.h`'s `DIR`.
- `lib/newlib/sp_newlib_config.h` is the table above.

## Tests

`make newlib-test` builds every runtime source with `NEWLIB_CC` (default
`arm-none-eabi-gcc`) for `NEWLIB_ARCH` (default `-mcpu=cortex-a9 -marm`)
and runs these under `NEWLIB_RUN` (default `qemu-arm`):

- `test/newlib/kernel.rb`, as an `--ext-init` library that
  `test/newlib/host.c` calls, linked with and without `--gc-sections`,
  against `test/newlib/expected`.
- Every other `test/newlib/*.rb`, as a program, against its
  `.rb.expected`. `sensor_log.rb` prints what CRuby prints, and
  `differences.rb` prints what the list above says each method answers.

It also checks that `--print-build` refuses a `Thread` program.

It skips without the compiler, its newlib or the emulator;
`NEWLIB_REQUIRED=1` makes that a failure. It is not in the gate or CI. An
M-profile core (Cortex-M33, Cortex-M0+) runs as:

```sh
make newlib-test NEWLIB_ARCH="-mcpu=cortex-m33 -mthumb" NEWLIB_RUN="qemu-arm -cpu cortex-m33"
make newlib-test NEWLIB_ARCH="-mcpu=cortex-m0plus -mthumb" NEWLIB_RUN="qemu-arm -cpu cortex-m0"
```
