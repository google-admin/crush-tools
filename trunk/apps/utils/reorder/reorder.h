#if HAVE_CONFIG_H
# include <config.h>
#endif

#if HAVE_REGEX_H
# include <regex.h>
#endif

#if HAVE_ASSERT_H
# include <assert.h>
#endif

#if HAVE_FFUTILS_H
# include <ffutils.h>
#endif

#ifndef REORDER_H
#define REORDER_H

#define REORDER_TYPE_SWAP 1
#define REORDER_TYPE_MOVE 2

/* validates swap arg from commandline and pushes values into the array
   of swaps to be performed */
int pushswap(char *s, int action_type);

#endif /* REORDER_H */

