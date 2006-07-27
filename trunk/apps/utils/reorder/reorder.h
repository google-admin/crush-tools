#include <regex.h>
#include <assert.h>
#include <ffutils.h>
#include <splitter.h>

#ifndef REORDER_H
#define REORDER_H

#define REORDER_TYPE_SWAP 1
#define REORDER_TYPE_MOVE 2

/* validates swap arg from commandline and pushes values into the array
   of swaps to be performed */
int pushswap(char *s, int action_type);

#endif /* REORDER_H */

