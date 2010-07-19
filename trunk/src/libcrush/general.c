#include <crush/general.h>

#include <err.h>
#include <stdlib.h>
#include <string.h>

void * xmalloc(size_t nbytes) {
  void *tmp = malloc(nbytes);
  if (! tmp) {
    warn("xmalloc failed to allocate %lu bytes", nbytes);
    exit(EXIT_FAILURE);
  }
  return tmp;
}

void * xrealloc(void *ptr, size_t nbytes) {
  void *tmp = realloc(ptr, nbytes);
  if (! tmp) {
    warn("xrealloc failed to allocate %lu bytes", nbytes);
    exit(EXIT_FAILURE);
  }
  return tmp;
}

void * xcalloc(size_t nmemb, size_t memb_sz) {
  void *tmp = calloc(nmemb, memb_sz);
  if (! tmp) {
    warn("xcalloc failed to allocate %lu bytes", nmemb * memb_sz);
    exit(EXIT_FAILURE);
  }
  return tmp;
}

char * xstrdup(const char *str) {
  char *tmp = strdup(str);
  if (! tmp) {
    warn("xstrdup failed to dup string of length %lu", strlen(str));
    exit(EXIT_FAILURE);
  }
  return tmp;
}
