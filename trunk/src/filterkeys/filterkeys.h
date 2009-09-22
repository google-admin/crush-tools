#ifndef FILTERKEYS_H
#define FILTERKEYS_H

#include <crush/bstree.h>

struct fkeys_conf {
  ssize_t key_count;

  size_t key_buffer_sz;

  size_t asize, bsize;
  int *aindexes, *bindexes;
  
  bstree_t ftree;
};

#endif // FILTERKEYS_H
