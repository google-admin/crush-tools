#ifndef FILTERKEYS_H
#define FILTERKEYS_H

#include <crush/bstree.h>

#define MAX_FIELD_SIZE 64

struct fkeys_conf {
  ssize_t key_count;

  size_t size;
  int *indexes;

  bstree_t ftree;
};

int filterkeys(struct cmdargs *args, int argc, char *argv[], int optind);

#endif // FILTERKEYS_H
