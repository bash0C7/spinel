# A board's newlib (`lib/newlib`)

The runtime builds for a microcontroller's C library, newlib, with the board's
own cross compiler: an Arm Cortex-M or -A toolchain, Espressif's for ESP32, any
`*-elf` GCC. Spinel compiles the program on the host as usual (`-c`, typically
with `--ext-init` and `--ext-entry`, see [spin.md](spin.md)); the board's build
compiles the generated C and the runtime sources together.

```sh
spinel kernel.rb -c --ext-init kernel_init --ext-entry Kernel.run -o kernel.c
$CC -c $BOARD_FLAGS -D_DEFAULT_SOURCE \
    -include lib/newlib/sp_newlib_config.h \
    -Ilib/newlib -Ilib -Ilib/regexp -Ilib/regexp/shim \
    $(spinel kernel.rb --print-build | sed -n 's/^define //p') \
    kernel.c lib/*.c lib/regexp/*.c lib/newlib/*.c
```

## What the flags do

- `-Ilib/newlib` puts the stand-ins for the POSIX headers newlib leaves out
  ahead of the system ones: sockets, `poll`, `mmap`, `ucontext`, resource
  limits, terminals and `<dirent.h>`, which newlib ships
  as an `#error` on a board without a filesystem layer. A header the platform
  has is read instead (`#include_next`), so lwIP's sockets on ESP-IDF stay
  lwIP's.
- `lib/newlib/sp_newlib.c` defines what those headers declare, each weakly:
  the call answers `ENOSYS` (or what one thread with no OS answers), and a
  platform that has the real one links it in its place. The clocks read
  `gettimeofday`, which every newlib board has. It also supplies the GCC
  atomic helpers (`__atomic_fetch_add_8`, ...) a core without the
  instructions calls -- Cortex-M0+ has none, Cortex-M3/M4/M33 none for 8
  bytes -- as plain read-modify-writes: the runtime is single-threaded, so
  that is atomic as long as no interrupt handler shares the word.
- `-include lib/newlib/sp_newlib_config.h` sizes the runtime's tables and its
  first collection triggers for a few hundred KB of RAM. The host defaults
  size the tables for millions of objects (about 850 KB of `.bss`) and first
  collect at 256 KB to 1 MB, more than such a board's heap. Each value is a
  default only; a `-D` wins. The file says what each one bounds.
- `-D_DEFAULT_SOURCE` makes newlib declare the POSIX calls it has. GCC 14
  rejects an implicit declaration, so the stand-ins declare the rest
  (`getline`, `clock_gettime` where the board has no POSIX timers, `lstat`).
- The `define` lines of `--print-build` (`-DSP_ONE_STRING`, the overflow mode)
  must reach the runtime sources as they reach the generated C.

Link with `--gc-sections`: what the program never reaches (the socket layer,
`Dir`, `Process`) is dropped. A link without it resolves too.

## What is different on this target

- **`Integer` is the pointer width**, 32 bits on a 32-bit board, as on
  wasm32 ([wasm.md](wasm.md), [int-overflow.md](int-overflow.md)).
- **No virtual memory.** `mmap` answers `MAP_FAILED`, so the slab allocator
  turns itself off and every block is a `malloc`, and an `IO::Buffer` mapping
  raises.
- **No signals.** `<signal.h>` here defines `SP_NO_SIGNALS`: the stack
  overflow report is not installed, and an overflow is the platform's (a
  FreeRTOS stack canary, a hard fault).
- **No `Fiber`, `Enumerator#next` or green threads**: a fiber has no stack to
  run on, and its first resume raises `FiberError` ("failed to allocate fiber
  stack"), which a `rescue` catches.
- **No sockets, processes, terminals or user database** unless the platform
  provides them: each call raises a `SystemCallError` (`Errno::ENOSYS`, or what
  the board's own call answers first: `Dir.entries` is `Errno::ENOENT` on a
  board whose `stat` finds nothing).
- **No directories** unless the platform has them. ESP-IDF's VFS does: define
  `SP_NEWLIB_HAS_DIRENT` to read its `<sys/dirent.h>` instead of the one here.

## Testing

`make newlib-test` builds `test/newlib/kernel.rb` and every runtime source
with `NEWLIB_CC` (default `arm-none-eabi-gcc`, Debian's `gcc-arm-none-eabi` and
`libnewlib-arm-none-eabi`) for `NEWLIB_ARCH` (default a 32-bit Cortex-A9;
`NEWLIB_ARCH="-mcpu=cortex-m33 -mthumb"` builds for an RP2350's core and
`-mcpu=cortex-m0plus -mthumb` for an RP2040's),
links the host in `test/newlib/host.c` with and without `--gc-sections`, and
runs both under `NEWLIB_RUN` (default `qemu-arm`, semihosting through
`rdimon.specs`). The output must be CRuby's, and the board-sized triggers
must have collected. It skips without the compiler and builds without
running without the emulator.
