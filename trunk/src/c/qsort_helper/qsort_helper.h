#include <stdlib.h>
#include <string.h>

#ifndef QSORT_HELPER_H
#define QSORT_HELPER_H

typedef int(*qsort_cmp_func_t)(const void *, const void *);

int qsort_strcmp ( const char **a, const char **b );

int qsort_intcmp ( const int *a, const int *b );

int qsort_uintcmp ( const unsigned int *a, const unsigned int *b );

#endif /* QSORT_HELPER_H */
