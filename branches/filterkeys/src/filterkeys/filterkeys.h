#ifndef FILTERKEYS_H
#define FILTERKEYS_H

#include <crush/bstree.h>

struct fkeys_conf {
  ssize_t key_count;

  size_t field_size;

  size_t size;
  int *indexes;
  
  bstree_t ftree;
};

#endif // FILTERKEYS_H
