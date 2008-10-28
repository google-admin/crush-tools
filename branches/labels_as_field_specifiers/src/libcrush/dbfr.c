#if HAVE_CONFIG_H
#  include <config.h>
#  ifdef HAVE_UNISTD_H
#    include <unistd.h>
#  endif
#  ifdef HAVE_FCNTL_H
#    include <fcntl.h>
#  endif
#  ifdef HAVE_STDLIB_H
#    include <stdlib.h>
#  endif
#else
#  include <unistd.h>
#  include <fcntl.h>
#  include <stdlib.h>
#endif /* HAVE_CONFIG_H */

#include <dbfr.h>

#ifdef HAVE_FCNTL
int dbfr_is_readable(FILE *fp) {
  int fd = fileno(fp);
  int flags = fcntl(fd, F_GETFL);
  if (! flags & O_RDONLY && ! flags & O_RDWR)
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

dbfr_t * dbfr_open(char *filename) {
  FILE *fp = fopen(filename, "r");
  if (! fp)
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

  reader->next_line_len = getline(&(reader->next_line),
                                  &(reader->next_line_sz),
                                  reader->file);
  return reader;
}

ssize_t dbfr_getline(dbfr_t *reader) {
  ssize_t next_len;
  /* swap buffers, to make the old "next" line the new "current" */
  char *cur = reader->current_line;
  size_t cur_sz = reader->current_line_sz;
  ssize_t cur_len = reader->current_line_len;

  if (reader->next_line_len < 1) {
    /* do not nullify current_line on EOF */
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
    reader->next_line_len = 0;
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

