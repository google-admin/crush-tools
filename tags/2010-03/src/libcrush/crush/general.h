/** @file general.h
  * @brief provides wrapper functions for dynamic memory allocation which
  * issue an error message and exit on failure.
  */

#include <stdlib.h>
#ifndef GENERAL_H
#define GENERAL_H

/** @brief wraps malloc(3) with error-checking.
  */
void * xmalloc(size_t nbytes);

/** @brief wraps realloc(3) with error-checking.
  */
void * xrealloc(void *ptr, size_t nbytes);

/** @brief wraps calloc(3) with error-checking.
  */
void * xcalloc(size_t nmemb, size_t memb_sz);

/** @brief wraps strdup(3) with error-checking.
  */
char * xstrdup(const char *str);

#endif /* GENERAL_H */
