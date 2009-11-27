#ifndef FILTERKEYS_H
#define FILTERKEYS_H

#define MAX_FIELD_LEN 255

#include <crush/hashtbl.h>

struct fkeys_conf {
  ssize_t key_count;

  size_t key_buffer_sz;
  int *aindexes, *bindexes;
  
  hashtbl_t filter;
};

#endif // FILTERKEYS_H
