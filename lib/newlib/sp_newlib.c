/* newlib on a board: the POSIX surface the runtime calls and a board's libc
   leaves out, each a weak definition that answers ENOSYS (or what a single
   thread with no OS would answer). A platform that has the real call (lwIP's
   sockets, ESP-IDF's VFS and pthreads, a board's clock) links it instead, so
   this file never shadows one. The clocks read gettimeofday, which every
   newlib board supplies. This directory is on the include path only for a
   newlib build (-Ilib/newlib), so a native build never sees it. */
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>
#include <poll.h>
#include <pwd.h>
#include <ucontext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SP_WEAK_FN __attribute__((weak))
#define SP_ENOSYS(v) do { errno = ENOSYS; return (v); } while (0)

SP_WEAK_FN int clock_gettime(clockid_t id, struct timespec *ts) {
  (void)id;
  struct timeval tv;
  if (gettimeofday(&tv, NULL) != 0) return -1;
  ts->tv_sec = tv.tv_sec;
  ts->tv_nsec = (long)tv.tv_usec * 1000;
  return 0;
}
SP_WEAK_FN int clock_getres(clockid_t id, struct timespec *ts) {
  (void)id;
  if (ts) { ts->tv_sec = 0; ts->tv_nsec = 1000; }
  return 0;
}
SP_WEAK_FN int nanosleep(const struct timespec *req, struct timespec *rem) {
  struct timeval t0, t;
  if (gettimeofday(&t0, NULL) != 0) SP_ENOSYS(-1);
  int64_t want = (int64_t)req->tv_sec * 1000000 + req->tv_nsec / 1000;
  do {
    if (gettimeofday(&t, NULL) != 0) SP_ENOSYS(-1);
  } while ((int64_t)(t.tv_sec - t0.tv_sec) * 1000000 + (t.tv_usec - t0.tv_usec) < want);
  if (rem) { rem->tv_sec = 0; rem->tv_nsec = 0; }
  return 0;
}

SP_WEAK_FN ssize_t getdelim(char **buf, size_t *cap, int delim, FILE *fp) {
  size_t n = 0;
  int c;
  if (!*buf || !*cap) { *cap = 128; *buf = malloc(*cap); if (!*buf) return -1; }
  while ((c = fgetc(fp)) != EOF) {
    if (n + 2 > *cap) {
      char *g = realloc(*buf, *cap * 2);
      if (!g) return -1;
      *buf = g; *cap *= 2;
    }
    (*buf)[n++] = (char)c;
    if (c == delim) break;
  }
  if (n == 0) return -1;
  (*buf)[n] = 0;
  return (ssize_t)n;
}
SP_WEAK_FN ssize_t getline(char **buf, size_t *cap, FILE *fp) { return getdelim(buf, cap, '\n', fp); }
SP_WEAK_FN size_t __freadahead(FILE *fp) { (void)fp; return 0; }
SP_WEAK_FN void flockfile(FILE *fp) { (void)fp; }
SP_WEAK_FN void funlockfile(FILE *fp) { (void)fp; }
SP_WEAK_FN FILE *popen(const char *cmd, const char *mode) { (void)cmd; (void)mode; SP_ENOSYS(NULL); }
SP_WEAK_FN int pclose(FILE *fp) { (void)fp; SP_ENOSYS(-1); }
SP_WEAK_FN char *crypt(const char *key, const char *salt) { (void)key; (void)salt; SP_ENOSYS(NULL); }

SP_WEAK_FN void *mmap(void *a, size_t n, int prot, int flags, int fd, off_t off) {
  (void)a; (void)n; (void)prot; (void)flags; (void)fd; (void)off;
  SP_ENOSYS(MAP_FAILED);
}
SP_WEAK_FN int munmap(void *a, size_t n) { (void)a; (void)n; SP_ENOSYS(-1); }
SP_WEAK_FN int mprotect(void *a, size_t n, int prot) { (void)a; (void)n; (void)prot; SP_ENOSYS(-1); }
SP_WEAK_FN int madvise(void *a, size_t n, int advice) { (void)a; (void)n; (void)advice; return 0; }

SP_WEAK_FN int sigaction(int sig, const struct sigaction *act, struct sigaction *old) {
  (void)sig; (void)act;
  if (old) memset(old, 0, sizeof *old);
  return 0;
}
SP_WEAK_FN int sigprocmask(int how, const sigset_t *set, sigset_t *old) {
  (void)how; (void)set;
  if (old) memset(old, 0, sizeof *old);
  return 0;
}

SP_WEAK_FN int getcontext(ucontext_t *u) { (void)u; SP_ENOSYS(-1); }
SP_WEAK_FN int setcontext(const ucontext_t *u) { (void)u; SP_ENOSYS(-1); }
SP_WEAK_FN void makecontext(ucontext_t *u, void (*fn)(void), int argc, ...) { (void)u; (void)fn; (void)argc; }
SP_WEAK_FN int swapcontext(ucontext_t *a, const ucontext_t *b) { (void)a; (void)b; SP_ENOSYS(-1); }

SP_WEAK_FN int getrlimit(int r, struct rlimit *rl) { (void)r; (void)rl; SP_ENOSYS(-1); }
SP_WEAK_FN int setrlimit(int r, const struct rlimit *rl) { (void)r; (void)rl; SP_ENOSYS(-1); }
SP_WEAK_FN int getpriority(int which, int who) { (void)which; (void)who; return 0; }
SP_WEAK_FN int setpriority(int which, int who, int prio) { (void)which; (void)who; (void)prio; SP_ENOSYS(-1); }

SP_WEAK_FN DIR *opendir(const char *p) { (void)p; SP_ENOSYS(NULL); }
SP_WEAK_FN DIR *fdopendir(int fd) { (void)fd; SP_ENOSYS(NULL); }
SP_WEAK_FN struct dirent *readdir(DIR *d) { (void)d; SP_ENOSYS(NULL); }
SP_WEAK_FN int closedir(DIR *d) { (void)d; SP_ENOSYS(-1); }
SP_WEAK_FN void rewinddir(DIR *d) { (void)d; }
SP_WEAK_FN long telldir(DIR *d) { (void)d; SP_ENOSYS(-1); }
SP_WEAK_FN void seekdir(DIR *d, long l) { (void)d; (void)l; }
SP_WEAK_FN int dirfd(DIR *d) { (void)d; SP_ENOSYS(-1); }

SP_WEAK_FN int lstat(const char *p, struct stat *st) { return stat(p, st); }
SP_WEAK_FN int chdir(const char *p) { (void)p; SP_ENOSYS(-1); }
SP_WEAK_FN int fchdir(int fd) { (void)fd; SP_ENOSYS(-1); }
SP_WEAK_FN char *getcwd(char *b, size_t n) { if (b && n > 1) { b[0] = '/'; b[1] = 0; return b; } SP_ENOSYS(NULL); }
SP_WEAK_FN int chmod(const char *p, mode_t m) { (void)p; (void)m; SP_ENOSYS(-1); }
SP_WEAK_FN int chown(const char *p, uid_t u, gid_t g) { (void)p; (void)u; (void)g; SP_ENOSYS(-1); }
SP_WEAK_FN int mkdir(const char *p, mode_t m) { (void)p; (void)m; SP_ENOSYS(-1); }
SP_WEAK_FN int rmdir(const char *p) { (void)p; SP_ENOSYS(-1); }
SP_WEAK_FN int mkfifo(const char *p, mode_t m) { (void)p; (void)m; SP_ENOSYS(-1); }
SP_WEAK_FN int symlink(const char *a, const char *b) { (void)a; (void)b; SP_ENOSYS(-1); }
SP_WEAK_FN ssize_t readlink(const char *p, char *b, size_t n) { (void)p; (void)b; (void)n; SP_ENOSYS(-1); }
SP_WEAK_FN char *realpath(const char *p, char *out) {
  if (!p) SP_ENOSYS(NULL);
  size_t n = strlen(p);
  if (!out) out = malloc(n + 1);
  if (!out) return NULL;
  memcpy(out, p, n + 1);
  return out;
}
SP_WEAK_FN mode_t umask(mode_t m) { (void)m; return 022; }
SP_WEAK_FN int faccessat(int fd, const char *p, int mode, int flags) { (void)fd; (void)p; (void)mode; (void)flags; SP_ENOSYS(-1); }
SP_WEAK_FN int utimensat(int fd, const char *p, const struct timespec t[2], int flags) { (void)fd; (void)p; (void)t; (void)flags; SP_ENOSYS(-1); }
SP_WEAK_FN int fsync(int fd) { (void)fd; return 0; }
SP_WEAK_FN int flock(int fd, int op) { (void)fd; (void)op; return 0; }
SP_WEAK_FN ssize_t pread(int fd, void *b, size_t n, off_t off) { (void)fd; (void)b; (void)n; (void)off; SP_ENOSYS(-1); }
SP_WEAK_FN ssize_t pwrite(int fd, const void *b, size_t n, off_t off) { (void)fd; (void)b; (void)n; (void)off; SP_ENOSYS(-1); }
SP_WEAK_FN int dup(int fd) { (void)fd; SP_ENOSYS(-1); }
SP_WEAK_FN int dup2(int a, int b) { (void)a; (void)b; SP_ENOSYS(-1); }
SP_WEAK_FN int pipe(int fds[2]) { (void)fds; SP_ENOSYS(-1); }
SP_WEAK_FN int ioctl(int fd, int req, ...) { (void)fd; (void)req; errno = ENOTTY; return -1; }
SP_WEAK_FN int gethostname(char *b, size_t n) { if (b && n > 0) { strncpy(b, "localhost", n); b[n - 1] = 0; return 0; } SP_ENOSYS(-1); }
SP_WEAK_FN int select(int n, fd_set *r, fd_set *w, fd_set *e, struct timeval *t) { (void)n; (void)r; (void)w; (void)e; (void)t; SP_ENOSYS(-1); }
SP_WEAK_FN int poll(struct pollfd *fds, nfds_t n, int timeout) { (void)fds; (void)n; (void)timeout; SP_ENOSYS(-1); }

SP_WEAK_FN uid_t getuid(void) { return 0; }
SP_WEAK_FN uid_t geteuid(void) { return 0; }
SP_WEAK_FN gid_t getgid(void) { return 0; }
SP_WEAK_FN gid_t getegid(void) { return 0; }
SP_WEAK_FN int getgroups(int n, gid_t *g) { (void)n; (void)g; return 0; }
SP_WEAK_FN int setpgid(pid_t p, pid_t g) { (void)p; (void)g; SP_ENOSYS(-1); }
SP_WEAK_FN pid_t setsid(void) { SP_ENOSYS(-1); }
SP_WEAK_FN struct passwd *getpwnam(const char *n) { (void)n; SP_ENOSYS(NULL); }
SP_WEAK_FN struct passwd *getpwuid(uid_t u) { (void)u; SP_ENOSYS(NULL); }
SP_WEAK_FN pid_t waitpid(pid_t p, int *s, int o) { (void)p; (void)s; (void)o; errno = ECHILD; return -1; }
SP_WEAK_FN int execvp(const char *f, char *const argv[]) { (void)f; (void)argv; SP_ENOSYS(-1); }
SP_WEAK_FN int execl(const char *p, const char *a, ...) { (void)p; (void)a; SP_ENOSYS(-1); }
SP_WEAK_FN int _execve(const char *p, char *const argv[], char *const envp[]) { (void)p; (void)argv; (void)envp; SP_ENOSYS(-1); }
SP_WEAK_FN int _fork(void) { SP_ENOSYS(-1); }
SP_WEAK_FN int _wait(int *s) { (void)s; errno = ECHILD; return -1; }

SP_WEAK_FN int socket(int d, int t, int p) { (void)d; (void)t; (void)p; SP_ENOSYS(-1); }
SP_WEAK_FN int socketpair(int d, int t, int p, int sv[2]) { (void)d; (void)t; (void)p; (void)sv; SP_ENOSYS(-1); }
SP_WEAK_FN int bind(int s, const struct sockaddr *a, socklen_t n) { (void)s; (void)a; (void)n; SP_ENOSYS(-1); }
SP_WEAK_FN int connect(int s, const struct sockaddr *a, socklen_t n) { (void)s; (void)a; (void)n; SP_ENOSYS(-1); }
SP_WEAK_FN int listen(int s, int b) { (void)s; (void)b; SP_ENOSYS(-1); }
SP_WEAK_FN int accept(int s, struct sockaddr *a, socklen_t *n) { (void)s; (void)a; (void)n; SP_ENOSYS(-1); }
SP_WEAK_FN int shutdown(int s, int h) { (void)s; (void)h; SP_ENOSYS(-1); }
SP_WEAK_FN ssize_t send(int s, const void *b, size_t n, int f) { (void)s; (void)b; (void)n; (void)f; SP_ENOSYS(-1); }
SP_WEAK_FN ssize_t recv(int s, void *b, size_t n, int f) { (void)s; (void)b; (void)n; (void)f; SP_ENOSYS(-1); }
SP_WEAK_FN ssize_t sendto(int s, const void *b, size_t n, int f, const struct sockaddr *a, socklen_t l) { (void)s; (void)b; (void)n; (void)f; (void)a; (void)l; SP_ENOSYS(-1); }
SP_WEAK_FN ssize_t recvfrom(int s, void *b, size_t n, int f, struct sockaddr *a, socklen_t *l) { (void)s; (void)b; (void)n; (void)f; (void)a; (void)l; SP_ENOSYS(-1); }
SP_WEAK_FN int getsockopt(int s, int lv, int o, void *v, socklen_t *n) { (void)s; (void)lv; (void)o; (void)v; (void)n; SP_ENOSYS(-1); }
SP_WEAK_FN int setsockopt(int s, int lv, int o, const void *v, socklen_t n) { (void)s; (void)lv; (void)o; (void)v; (void)n; SP_ENOSYS(-1); }
SP_WEAK_FN int getsockname(int s, struct sockaddr *a, socklen_t *n) { (void)s; (void)a; (void)n; SP_ENOSYS(-1); }
SP_WEAK_FN int getpeername(int s, struct sockaddr *a, socklen_t *n) { (void)s; (void)a; (void)n; SP_ENOSYS(-1); }
SP_WEAK_FN int getaddrinfo(const char *h, const char *s, const struct addrinfo *hint, struct addrinfo **r) { (void)h; (void)s; (void)hint; (void)r; return EAI_FAIL; }
SP_WEAK_FN void freeaddrinfo(struct addrinfo *r) { (void)r; }
SP_WEAK_FN const char *gai_strerror(int e) { (void)e; return "name resolution is not available"; }
SP_WEAK_FN int getnameinfo(const struct sockaddr *a, socklen_t l, char *h, socklen_t hl, char *s, socklen_t sl, int f) { (void)a; (void)l; (void)h; (void)hl; (void)s; (void)sl; (void)f; return EAI_FAIL; }
SP_WEAK_FN const char *inet_ntop(int af, const void *src, char *dst, socklen_t n) { (void)af; (void)src; (void)dst; (void)n; SP_ENOSYS(NULL); }
SP_WEAK_FN int inet_pton(int af, const char *src, void *dst) { (void)af; (void)src; (void)dst; SP_ENOSYS(-1); }

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
SP_WEAK_FN uint32_t htonl(uint32_t v) { return v; }
SP_WEAK_FN uint16_t htons(uint16_t v) { return v; }
#else
SP_WEAK_FN uint32_t htonl(uint32_t v) { return __builtin_bswap32(v); }
SP_WEAK_FN uint16_t htons(uint16_t v) { return __builtin_bswap16(v); }
#endif
SP_WEAK_FN uint32_t ntohl(uint32_t v) { return htonl(v); }
SP_WEAK_FN uint16_t ntohs(uint16_t v) { return htons(v); }

/* The GCC atomic helpers a core without the instructions calls (Cortex-M0+
   has no exclusive loads; Cortex-M3/M4/M33 none for 8 bytes) and newlib does
   not ship. The runtime here is single-threaded (no SP_THREADS), so a plain
   read-modify-write is the atomic one, as long as no interrupt handler
   shares the word. Each is bound to the helper's name by an asm label: GCC
   reserves the C names for its builtins. */
#define SP_NEWLIB_ATOMIC(N, T)                                                                 \
  SP_WEAK_FN T sp_newlib_atomic_load_##N(const volatile void *p, int m) __asm__("__atomic_load_" #N); \
  T sp_newlib_atomic_load_##N(const volatile void *p, int m) { (void)m; return *(const volatile T *)p; } \
  SP_WEAK_FN void sp_newlib_atomic_store_##N(volatile void *p, T v, int m) __asm__("__atomic_store_" #N); \
  void sp_newlib_atomic_store_##N(volatile void *p, T v, int m) { (void)m; *(volatile T *)p = v; } \
  SP_WEAK_FN T sp_newlib_atomic_exchange_##N(volatile void *p, T v, int m) __asm__("__atomic_exchange_" #N); \
  T sp_newlib_atomic_exchange_##N(volatile void *p, T v, int m) { (void)m; T o = *(volatile T *)p; *(volatile T *)p = v; return o; } \
  SP_WEAK_FN _Bool sp_newlib_atomic_cas_##N(volatile void *p, void *e, T d, _Bool w, int s, int f) __asm__("__atomic_compare_exchange_" #N); \
  _Bool sp_newlib_atomic_cas_##N(volatile void *p, void *e, T d, _Bool w, int s, int f) { \
    (void)w; (void)s; (void)f;                                                                   \
    T o = *(volatile T *)p;                                                                      \
    if (o == *(T *)e) { *(volatile T *)p = d; return 1; }                                        \
    *(T *)e = o; return 0;                                                                       \
  }                                                                                              \
  SP_WEAK_FN T sp_newlib_atomic_fetch_add_##N(volatile void *p, T v, int m) __asm__("__atomic_fetch_add_" #N); \
  T sp_newlib_atomic_fetch_add_##N(volatile void *p, T v, int m) { (void)m; T o = *(volatile T *)p; *(volatile T *)p = (T)(o + v); return o; } \
  SP_WEAK_FN T sp_newlib_atomic_fetch_sub_##N(volatile void *p, T v, int m) __asm__("__atomic_fetch_sub_" #N); \
  T sp_newlib_atomic_fetch_sub_##N(volatile void *p, T v, int m) { (void)m; T o = *(volatile T *)p; *(volatile T *)p = (T)(o - v); return o; } \
  SP_WEAK_FN T sp_newlib_atomic_fetch_or_##N(volatile void *p, T v, int m) __asm__("__atomic_fetch_or_" #N); \
  T sp_newlib_atomic_fetch_or_##N(volatile void *p, T v, int m) { (void)m; T o = *(volatile T *)p; *(volatile T *)p = (T)(o | v); return o; } \
  SP_WEAK_FN T sp_newlib_atomic_fetch_and_##N(volatile void *p, T v, int m) __asm__("__atomic_fetch_and_" #N); \
  T sp_newlib_atomic_fetch_and_##N(volatile void *p, T v, int m) { (void)m; T o = *(volatile T *)p; *(volatile T *)p = (T)(o & v); return o; }
SP_NEWLIB_ATOMIC(1, uint8_t)
SP_NEWLIB_ATOMIC(2, uint16_t)
SP_NEWLIB_ATOMIC(4, uint32_t)
SP_NEWLIB_ATOMIC(8, uint64_t)
