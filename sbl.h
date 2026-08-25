#ifndef SBL_H
#define SBL_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/* die with an errno-based message and bail (for failed syscalls/libc calls) */
#define DIE(msg)                                                               \
  do {                                                                         \
    perror(msg);                                                               \
    exit(1);                                                                   \
  } while (0)

/* die with a plain message, no errno attached (for bad input/args) */
#define FATAL(msg)                                                             \
  do {                                                                         \
    fprintf(stderr, "sysbloom: %s\n", msg);                                    \
    exit(1);                                                                   \
  } while (0)

/* strip leading whitespace, returns pointer into s (no alloc) */
static inline char *ltrim(char *s) {
  while (*s && isspace((unsigned char)*s))
    s++;
  return s;
}

/* number of utf-8 codepoints in s (display columns for non-wide glyphs,
 * which covers braille/ascii/latin art. doesn't handle CJK double-width). */
static inline int utf8_width(const char *s) {
  int n = 0;
  for (const unsigned char *p = (const unsigned char *)s; *p; p++) {
    if ((*p & 0xC0) != 0x80)
      n++; /* skip utf-8 continuation bytes */
  }
  return n;
}

#endif /* SBL_H */
