#if HAVE_CONFIG_H
#  include <config.h>
#  ifdef HAVE_UNISTD_H
#    include <unistd.h>
#  endif
#  ifdef HAVE_FCNTL_H
#    include <fcntl.h>
#  elif HAVE_SYS_FCNTL_H
#    include <sys/fcntl.h>
#  endif
#  ifdef HAVE_STDLIB_H
#    include <stdlib.h>
#  endif
#  ifdef HAVE_STDIO_H
#    include <stdio.h>
#  endif
#  ifdef HAVE_STRING_H
#    include <string.h>
#  endif
#  ifdef HAVE_SYS_TYPES_H
#    include <sys/types.h>
#  endif
#  ifdef HAVE_SYS_STAT_H
#    include <sys/stat.h>
#  endif
#  ifndef HAVE_OPEN64
#    define open64 open
#  endif
#else
#  define HAVE_FCNTL_H 1
#  include <unistd.h>
#  include <fcntl.h>
#  include <stdlib.h>
#  include <stdio.h>
#  include <string.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#endif /* HAVE_CONFIG_H */

#include <crush/dbfr.h>

#if defined HAVE_FCNTL_H || defined HAVE_SYS_FCNTL_H
int dbfr_is_readable(FILE *fp) {
  int fd = fileno(fp);
  int flags = fcntl(fd, F_GETFL);
  if (flags & O_WRONLY)
    return 0;
  return 1;
}
#else
int dbfr_is_readable(FILE *fp) {
  return 1;
}
#endif

static void * xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (! ptr) {
    fprintf(stderr, "%s: out of memory\n", getenv("_"));
    exit(EXIT_FAILURE);
  }
  return ptr;
}

dbfr_t * dbfr_open(const char *filename) {
  int fd, flags;
  FILE *fp;
  flags = O_RDONLY;
#ifdef O_LARGEFILE
  flags |= O_LARGEFILE;
#endif
  if (filename == NULL || strcmp(filename, "-") == 0)
    return dbfr_init(stdin);
  if ((fd = open64(filename, flags)) < 0)
    return NULL;
  if ((fp = fdopen(fd, "r")) == NULL)
    return NULL;
  return dbfr_init(fp);
}

dbfr_t * dbfr_init(FILE *fp) {
  dbfr_t *reader;
  if (fp == NULL || ! dbfr_is_readable(fp))
    return NULL;
  reader = xmalloc(sizeof(dbfr_t));
  memset(reader, 0, sizeof(*reader));
  reader->file = fp;

  if ((reader->next_line_len = getline(&(reader->next_line),
                                      &(reader->next_line_sz),
                                      reader->file)) <= 0) {
    reader->eof = 1;
  }
  return reader;
}

ssize_t dbfr_getline(dbfr_t *reader) {
  /* swap buffers, to make the old "next" line the new "current" */
  char *cur = reader->current_line;
  size_t cur_sz = reader->current_line_sz;
  ssize_t cur_len = reader->current_line_len;

  if (reader->next_line_len < 1) {
    /* do not nullify current_line on EOF */
    reader->eof = 1;
    return reader->next_line_len;
  }

  reader->current_line = reader->next_line;
  reader->current_line_sz = reader->next_line_sz;
  reader->current_line_len = reader->next_line_len;

  reader->next_line = cur;
  reader->next_line_sz = cur_sz;
  reader->next_line_len = cur_len;

  /* read in the new "next" line */
  reader->next_line_len = getline(&(reader->next_line),
                                  &(reader->next_line_sz),
                                  reader->file);
  if (reader->next_line_len < 1) {
    free(reader->next_line);
    reader->next_line = NULL;
    reader->next_line_sz = 0;
  }
  reader->line_no++;
  return reader->current_line_len;
}

void dbfr_close(dbfr_t *reader) {
  if (! reader)
    return;
  if (reader->next_line)
    free(reader->next_line);
  if (reader->current_line)
    free(reader->current_line);
  if (reader->file)
    fclose(reader->file);
  free(reader);
}
