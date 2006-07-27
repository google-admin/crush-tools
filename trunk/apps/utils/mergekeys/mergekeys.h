#include <ffutils.h>
#include <fcntl.h>	/* open64() */
#include <unistd.h>	/* off64_t & close() */

#ifndef MERGEKEYS_H
#define MERGEKEYS_H

#define MAX_FIELD_LEN 63

int merge_files( FILE *a, FILE *b, FILE *out, struct cmdargs *args ) ;

#endif /* MERGEKEYS_H */
