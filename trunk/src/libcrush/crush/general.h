/*****************************************
   Copyright 2010 Google Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 *****************************************/

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
