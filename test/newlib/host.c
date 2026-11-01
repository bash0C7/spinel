/* newlib-test's host: calls each entry of test/newlib/kernel.rb through the
   emitted header, the strings through a checksum, enough times that the
   board-sized first triggers (lib/newlib/sp_newlib_config.h) collect. */
#include <stdio.h>
#include "k.h"

static unsigned long djb(const char *s, size_t n) {
  unsigned long h = 5381;
  for (size_t i = 0; i < n; i++) h = h * 33 + (unsigned char)s[i];
  return h & 0xffffffffUL;
}

typedef struct { int which; unsigned long out; } job;

static void run(void *p) {
  job *j = p;
  if (j->which == 0) {
    static char raw[2048];
    for (int i = 0; i < 2048; i++) raw[i] = "\x12\x34\xab\xcd"[i & 3];
    unsigned long acc = 0;
    for (int r = 0; r < 50; r++) acc += djb(sp_NewlibKernel_s_ulaw_decode(SP_STR_HDL(sp_str_from_bytes(raw, 2048))), 4096);
    j->out = acc & 0xffffffffUL;
  } else if (j->which == 1) {
    static const int v[16] = { 0x0F0F, 0x3C3C, 0xFFFF, 0x0000, 0xAAAA, 0x5555, 0x1234, 0x8001,
                               0x7FFE, 0x0F0F, 0x3C3C, 0xFFFF, 0x0000, 0xAAAA, 0x5555, 0x1234 };
    sp_IntArray *rows = sp_IntArray_new(); SP_GC_ROOT(rows);
    for (int i = 0; i < 16; i++) sp_IntArray_push(rows, v[i]);
    unsigned long acc = 0;
    for (int r = 0; r < 2000; r++) acc += djb(sp_NewlibKernel_s_glyph16(16, 0xF800, 0x001F, rows), 512);
    j->out = acc & 0xffffffffUL;
  } else if (j->which == 2) {
    j->out = (unsigned long)sp_NewlibKernel_s_spin(7, 1000000);
  } else {
    j->out = (unsigned long)sp_NewlibKernel_s_must_pos(-1);
  }
}

int main(void) {
  static const char *const names[4] = { "ulaw_decode", "glyph16", "spin", "must_pos" };
  Init_newlib_kernel();
  for (int w = 0; w < 4; w++) {
    job j = { w, 0 };
    const char *cls, *msg;
    if (Init_newlib_kernel_try(run, &j, &cls, &msg)) printf("%s raised %s: %s\n", names[w], cls, msg);
    else printf("%s %lu\n", names[w], j.out);
  }
  printf("collected %s\n", sp_gc_stat_collections > 0 ? "yes" : "no");
  printf("pointer %d bits\n", (int)(sizeof(void *) * 8));
  return 0;
}
