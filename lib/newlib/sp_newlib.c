#include <stdio.h>
#ifdef __NEWLIB__
#include <errno.h>
#include <limits.h>
#include <stdint.h>
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
#include "../sp_compat.h"

#define PATH_MAX 4096
#define SP_NEWLIB_NOSYS(...) { (void)(__VA_ARGS__); errno = ENOSYS; return -1; }

SP_WEAK int clock_gettime(clockid_t id, struct timespec *ts) {
  (void)id;
  struct timeval tv;
  if (gettimeofday(&tv, NULL) != 0) return -1;
  ts->tv_sec = tv.tv_sec;
  ts->tv_nsec = (long)tv.tv_usec * 1000;
  return 0;
}
SP_WEAK int clock_getres(clockid_t id, struct timespec *ts) {
  (void)id;
  if (ts) { ts->tv_sec = 0; ts->tv_nsec = 1000; }
  return 0;
}
SP_WEAK int nanosleep(const struct timespec *req, struct timespec *rem) {
  struct timeval t0, t;
  if (gettimeofday(&t0, NULL) != 0) return -1;
  int64_t want = (int64_t)req->tv_sec * 1000000 + req->tv_nsec / 1000;
  do {
    if (gettimeofday(&t, NULL) != 0) return -1;
  } while ((int64_t)(t.tv_sec - t0.tv_sec) * 1000000 + (t.tv_usec - t0.tv_usec) < want);
  if (rem) { rem->tv_sec = 0; rem->tv_nsec = 0; }
  return 0;
}

SP_WEAK ssize_t getdelim(char **buf, size_t *cap, int delim, FILE *fp) { return __getdelim(buf, cap, delim, fp); }
SP_WEAK ssize_t getline(char **buf, size_t *cap, FILE *fp) { return __getline(buf, cap, fp); }
SP_WEAK size_t __freadahead(FILE *fp) { return fp->_r > 0 ? (size_t)fp->_r : 0; }
SP_WEAK void flockfile(FILE *fp) { (void)fp; }
SP_WEAK void funlockfile(FILE *fp) { (void)fp; }
SP_WEAK FILE *popen(const char *c, const char *m) { (void)c; (void)m; errno = ENOSYS; return NULL; }
SP_WEAK int pclose(FILE *fp) SP_NEWLIB_NOSYS(fp)
SP_WEAK char *crypt(const char *k, const char *s) { (void)k; (void)s; errno = ENOSYS; return NULL; }

SP_WEAK void *mmap(void *a, size_t n, int prot, int fl, int fd, off_t off) {
  (void)a; (void)n; (void)prot; (void)fl; (void)fd; (void)off;
  errno = ENOSYS;
  return MAP_FAILED;
}
SP_WEAK int munmap(void *a, size_t n) SP_NEWLIB_NOSYS(a, n)
SP_WEAK int mprotect(void *a, size_t n, int prot) SP_NEWLIB_NOSYS(a, n, prot)
SP_WEAK int madvise(void *a, size_t n, int advice) { (void)a; (void)n; (void)advice; return 0; }

SP_WEAK int sigaction(int s, const struct sigaction *a, struct sigaction *o) SP_NEWLIB_NOSYS(s, a, o)
SP_WEAK void (*signal(int s, void (*f)(int)))(int) { (void)s; (void)f; errno = ENOSYS; return SIG_ERR; }
SP_WEAK int sigprocmask(int h, const sigset_t *s, sigset_t *o) { (void)h; (void)s; (void)o; return 0; }

SP_WEAK int getcontext(ucontext_t *u) SP_NEWLIB_NOSYS(u)
SP_WEAK int setcontext(const ucontext_t *u) SP_NEWLIB_NOSYS(u)
SP_WEAK void makecontext(ucontext_t *u, void (*f)(void), int n, ...) { (void)u; (void)f; (void)n; }
SP_WEAK int swapcontext(ucontext_t *o, const ucontext_t *u) SP_NEWLIB_NOSYS(o, u)

SP_WEAK int getrlimit(int r, struct rlimit *l) SP_NEWLIB_NOSYS(r, l)
SP_WEAK int setrlimit(int r, const struct rlimit *l) SP_NEWLIB_NOSYS(r, l)
SP_WEAK int getpriority(int w, id_t who) { (void)w; (void)who; return 0; }
SP_WEAK int setpriority(int w, id_t who, int p) { (void)w; (void)who; (void)p; return 0; }

SP_WEAK DIR *opendir(const char *p) { (void)p; errno = ENOSYS; return NULL; }
SP_WEAK DIR *fdopendir(int fd) { (void)fd; errno = ENOSYS; return NULL; }
SP_WEAK struct dirent *readdir(DIR *d) { (void)d; errno = ENOSYS; return NULL; }
SP_WEAK int closedir(DIR *d) SP_NEWLIB_NOSYS(d)
SP_WEAK void rewinddir(DIR *d) { (void)d; }
SP_WEAK long telldir(DIR *d) SP_NEWLIB_NOSYS(d)
SP_WEAK void seekdir(DIR *d, long l) { (void)d; (void)l; }
SP_WEAK int dirfd(DIR *d) SP_NEWLIB_NOSYS(d)

SP_WEAK int lstat(const char *p, struct stat *st) { return stat(p, st); }
SP_WEAK int chdir(const char *p) SP_NEWLIB_NOSYS(p)
SP_WEAK int fchdir(int fd) SP_NEWLIB_NOSYS(fd)
SP_WEAK char *getcwd(char *b, size_t n) {
  if (b && n > 1) { b[0] = '/'; b[1] = 0; return b; }
  errno = ERANGE;
  return NULL;
}
SP_WEAK int chmod(const char *p, mode_t m) SP_NEWLIB_NOSYS(p, m)
SP_WEAK int chown(const char *p, uid_t u, gid_t g) SP_NEWLIB_NOSYS(p, u, g)
SP_WEAK int mkdir(const char *p, mode_t m) SP_NEWLIB_NOSYS(p, m)
SP_WEAK int rmdir(const char *p) SP_NEWLIB_NOSYS(p)
SP_WEAK int mkfifo(const char *p, mode_t m) SP_NEWLIB_NOSYS(p, m)
SP_WEAK int symlink(const char *a, const char *b) SP_NEWLIB_NOSYS(a, b)
SP_WEAK ssize_t readlink(const char *p, char *b, size_t n) SP_NEWLIB_NOSYS(p, b, n)
SP_WEAK char *realpath(const char *p, char *out) {
  struct stat st;
  size_t n = 0;
  if (!p) { errno = EINVAL; return NULL; }
  if (stat(p, &st) != 0) return NULL;
  char *r = out ? out : malloc(PATH_MAX);
  if (!r) return NULL;
  if (*p != '/') {
    if (!getcwd(r, PATH_MAX)) { if (!out) free(r); return NULL; }
    n = strlen(r);
    while (n > 0 && r[n - 1] == '/') n--;
  }
  while (*p) {
    while (*p == '/') p++;
    const char *e = p;
    while (*e && *e != '/') e++;
    size_t len = (size_t)(e - p);
    if (len == 2 && p[0] == '.' && p[1] == '.') {
      while (n > 0 && r[--n] != '/') {}
    } else if (len > 0 && !(len == 1 && p[0] == '.')) {
      if (n + 1 + len >= PATH_MAX) { if (!out) free(r); errno = ENAMETOOLONG; return NULL; }
      r[n++] = '/';
      memcpy(r + n, p, len);
      n += len;
    }
    p = e;
  }
  if (n == 0) r[n++] = '/';
  r[n] = 0;
  return r;
}
SP_WEAK mode_t umask(mode_t m) { (void)m; return 022; }
SP_WEAK int faccessat(int fd, const char *p, int m, int fl) SP_NEWLIB_NOSYS(fd, p, m, fl)
SP_WEAK int utimensat(int fd, const char *p, const struct timespec t[2], int fl) SP_NEWLIB_NOSYS(fd, p, t, fl)
SP_WEAK int fsync(int fd) { (void)fd; return 0; }
SP_WEAK int flock(int fd, int op) { (void)fd; (void)op; return 0; }
SP_WEAK ssize_t pread(int fd, void *b, size_t n, off_t off) SP_NEWLIB_NOSYS(fd, b, n, off)
SP_WEAK ssize_t pwrite(int fd, const void *b, size_t n, off_t off) SP_NEWLIB_NOSYS(fd, b, n, off)
SP_WEAK int dup(int fd) SP_NEWLIB_NOSYS(fd)
SP_WEAK int dup2(int a, int b) SP_NEWLIB_NOSYS(a, b)
SP_WEAK int pipe(int fds[2]) SP_NEWLIB_NOSYS(fds)
SP_WEAK int ioctl(int fd, int req, ...) { (void)fd; (void)req; errno = ENOTTY; return -1; }
SP_WEAK int gethostname(char *b, size_t n) {
  if (b && n > 0) { strncpy(b, "localhost", n); b[n - 1] = 0; return 0; }
  errno = EINVAL;
  return -1;
}
SP_WEAK int select(int n, fd_set *r, fd_set *w, fd_set *e, struct timeval *t) SP_NEWLIB_NOSYS(n, r, w, e, t)
SP_WEAK int poll(struct pollfd *fds, nfds_t n, int timeout) SP_NEWLIB_NOSYS(fds, n, timeout)

SP_WEAK uid_t getuid(void) { return 0; }
SP_WEAK uid_t geteuid(void) { return 0; }
SP_WEAK gid_t getgid(void) { return 0; }
SP_WEAK gid_t getegid(void) { return 0; }
SP_WEAK int getgroups(int n, gid_t *g) { (void)n; (void)g; return 0; }
SP_WEAK int setpgid(pid_t p, pid_t g) SP_NEWLIB_NOSYS(p, g)
SP_WEAK pid_t setsid(void) { errno = ENOSYS; return -1; }
SP_WEAK struct passwd *getpwnam(const char *n) { (void)n; errno = ENOSYS; return NULL; }
SP_WEAK struct passwd *getpwuid(uid_t u) { (void)u; errno = ENOSYS; return NULL; }
SP_WEAK pid_t waitpid(pid_t p, int *st, int fl) { (void)p; (void)st; (void)fl; errno = ECHILD; return -1; }
SP_WEAK int execvp(const char *f, char *const a[]) SP_NEWLIB_NOSYS(f, a)
SP_WEAK int execl(const char *p, const char *a, ...) SP_NEWLIB_NOSYS(p, a)
SP_WEAK int _execve(const char *p, char *const a[], char *const e[]) SP_NEWLIB_NOSYS(p, a, e)
SP_WEAK int _fork(void) { errno = ENOSYS; return -1; }
SP_WEAK int _wait(int *st) { (void)st; errno = ECHILD; return -1; }

SP_WEAK int socket(int d, int t, int p) { (void)d; (void)t; (void)p; errno = ENOTSUP; return -1; }
SP_WEAK int socketpair(int d, int t, int p, int sv[2]) { (void)d; (void)t; (void)p; (void)sv; errno = ENOTSUP; return -1; }
SP_WEAK int bind(int fd, const struct sockaddr *a, socklen_t l) { (void)fd; (void)a; (void)l; errno = ENOTSUP; return -1; }
SP_WEAK int connect(int fd, const struct sockaddr *a, socklen_t l) { (void)fd; (void)a; (void)l; errno = ENOTSUP; return -1; }
SP_WEAK int listen(int fd, int b) { (void)fd; (void)b; errno = ENOTSUP; return -1; }
SP_WEAK int accept(int fd, struct sockaddr *a, socklen_t *l) { (void)fd; (void)a; (void)l; errno = ENOTSUP; return -1; }
SP_WEAK int shutdown(int fd, int h) { (void)fd; (void)h; errno = ENOTSUP; return -1; }
SP_WEAK ssize_t send(int fd, const void *b, size_t n, int fl) { (void)fd; (void)b; (void)n; (void)fl; errno = ENOTSUP; return -1; }
SP_WEAK ssize_t recv(int fd, void *b, size_t n, int fl) { (void)fd; (void)b; (void)n; (void)fl; errno = ENOTSUP; return -1; }
SP_WEAK ssize_t sendto(int fd, const void *b, size_t n, int fl, const struct sockaddr *a, socklen_t l) { (void)fd; (void)b; (void)n; (void)fl; (void)a; (void)l; errno = ENOTSUP; return -1; }
SP_WEAK ssize_t recvfrom(int fd, void *b, size_t n, int fl, struct sockaddr *a, socklen_t *l) { (void)fd; (void)b; (void)n; (void)fl; (void)a; (void)l; errno = ENOTSUP; return -1; }
SP_WEAK int getsockopt(int fd, int lv, int o, void *v, socklen_t *l) { (void)fd; (void)lv; (void)o; (void)v; (void)l; errno = ENOTSUP; return -1; }
SP_WEAK int setsockopt(int fd, int lv, int o, const void *v, socklen_t l) { (void)fd; (void)lv; (void)o; (void)v; (void)l; errno = ENOTSUP; return -1; }
SP_WEAK int getsockname(int fd, struct sockaddr *a, socklen_t *l) { (void)fd; (void)a; (void)l; errno = ENOTSUP; return -1; }
SP_WEAK int getpeername(int fd, struct sockaddr *a, socklen_t *l) { (void)fd; (void)a; (void)l; errno = ENOTSUP; return -1; }
SP_WEAK int getaddrinfo(const char *n, const char *s, const struct addrinfo *h, struct addrinfo **r) { (void)n; (void)s; (void)h; (void)r; return EAI_FAIL; }
SP_WEAK void freeaddrinfo(struct addrinfo *a) { (void)a; }
SP_WEAK int getnameinfo(const struct sockaddr *a, socklen_t al, char *h, socklen_t hl, char *s, socklen_t sl, int f) { (void)a; (void)al; (void)h; (void)hl; (void)s; (void)sl; (void)f; return EAI_FAIL; }
SP_WEAK const char *gai_strerror(int e) { (void)e; return "name resolution is not available on newlib"; }
SP_WEAK const char *inet_ntop(int af, const void *src, char *dst, socklen_t n) { (void)af; (void)src; (void)dst; (void)n; errno = ENOTSUP; return NULL; }
SP_WEAK int inet_pton(int af, const char *src, void *dst) { (void)af; (void)src; (void)dst; errno = ENOTSUP; return -1; }
SP_WEAK uint32_t htonl(uint32_t v) { return __htonl(v); }
SP_WEAK uint16_t htons(uint16_t v) { return __htons(v); }
SP_WEAK uint32_t ntohl(uint32_t v) { return __ntohl(v); }
SP_WEAK uint16_t ntohs(uint16_t v) { return __ntohs(v); }

#define SP_NEWLIB_ATOMIC(N, T)                                                                 \
  SP_WEAK T sp_newlib_atomic_load_##N(const volatile void *p, int m) __asm__("__atomic_load_" #N); \
  T sp_newlib_atomic_load_##N(const volatile void *p, int m) { (void)m; return *(const volatile T *)p; } \
  SP_WEAK void sp_newlib_atomic_store_##N(volatile void *p, T v, int m) __asm__("__atomic_store_" #N); \
  void sp_newlib_atomic_store_##N(volatile void *p, T v, int m) { (void)m; *(volatile T *)p = v; } \
  SP_WEAK T sp_newlib_atomic_exchange_##N(volatile void *p, T v, int m) __asm__("__atomic_exchange_" #N); \
  T sp_newlib_atomic_exchange_##N(volatile void *p, T v, int m) { (void)m; T o = *(volatile T *)p; *(volatile T *)p = v; return o; } \
  SP_WEAK _Bool sp_newlib_atomic_cas_##N(volatile void *p, void *e, T d, int s, int f) __asm__("__atomic_compare_exchange_" #N); \
  _Bool sp_newlib_atomic_cas_##N(volatile void *p, void *e, T d, int s, int f) {             \
    (void)s; (void)f;                                                                           \
    T o = *(volatile T *)p;                                                                     \
    if (o == *(T *)e) { *(volatile T *)p = d; return 1; }                                       \
    *(T *)e = o; return 0;                                                                      \
  }                                                                                             \
  SP_WEAK T sp_newlib_atomic_fetch_add_##N(volatile void *p, T v, int m) __asm__("__atomic_fetch_add_" #N); \
  T sp_newlib_atomic_fetch_add_##N(volatile void *p, T v, int m) { (void)m; T o = *(volatile T *)p; *(volatile T *)p = (T)(o + v); return o; } \
  SP_WEAK T sp_newlib_atomic_fetch_sub_##N(volatile void *p, T v, int m) __asm__("__atomic_fetch_sub_" #N); \
  T sp_newlib_atomic_fetch_sub_##N(volatile void *p, T v, int m) { (void)m; T o = *(volatile T *)p; *(volatile T *)p = (T)(o - v); return o; } \
  SP_WEAK T sp_newlib_atomic_fetch_or_##N(volatile void *p, T v, int m) __asm__("__atomic_fetch_or_" #N); \
  T sp_newlib_atomic_fetch_or_##N(volatile void *p, T v, int m) { (void)m; T o = *(volatile T *)p; *(volatile T *)p = (T)(o | v); return o; } \
  SP_WEAK T sp_newlib_atomic_fetch_and_##N(volatile void *p, T v, int m) __asm__("__atomic_fetch_and_" #N); \
  T sp_newlib_atomic_fetch_and_##N(volatile void *p, T v, int m) { (void)m; T o = *(volatile T *)p; *(volatile T *)p = (T)(o & v); return o; }
SP_NEWLIB_ATOMIC(1, uint8_t)
SP_NEWLIB_ATOMIC(2, uint16_t)
SP_NEWLIB_ATOMIC(4, uint32_t)
SP_NEWLIB_ATOMIC(8, uint64_t)
#endif
