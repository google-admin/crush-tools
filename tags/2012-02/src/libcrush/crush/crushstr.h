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

/** @file crushstr.h
  * @brief A dynamic string library.
  */
#include <stdlib.h>

#ifndef CRUSHSTR_H
#define CRUSHSTR_H

/** @brief a dynamically sized string. */
typedef struct {
  char *buffer;  /**< @brief Dynamically resized buffer to hold a string. */
  size_t length;  /**< @brief Length of the string in the buffer. */
  size_t capacity;  /**< @brief The number of bytes allocated for the buffer. */
} crushstr_t;


/** @brief initializes a new crushstr_t.
  *
  * @param str The crushstr to be initialized.
  * @param capacity The initial capacity of the crushstr.
  * @return 0 on success.
  */
int crushstr_init(crushstr_t *str, size_t capacity);

/** @brief deallocates and zeros out a crushstr.
  *
  * Note that the crushstr object itself will not be deallocated - just its
  * internal resources.
  *
  * @param str the string to be deallocated.
  */
void crushstr_destroy(crushstr_t *str);

/** @brief increases the capacity of a crushstr.
  *
  * If the new capacity is less than the current capacity, no change is made.
  *
  * @param str the crushstr to be resized.
  * @param capacity the new capacity in bytes.
  *
  * @return str on success, or NULL on failure.
  */
crushstr_t* crushstr_resize(crushstr_t *str, size_t capacity);

/** @brief copies a string into a crushstr.
  *
  * @param target the crushstr into which the content should be copied.
  * @param source the string to be copied.
  *
  * @return str on success, or NULL on failure.
  */
crushstr_t* crushstr_copy(crushstr_t *target, const char *source);

/** @brief appends a string into a crushstr.
  *
  * @param target the crushstr into which the content should be appended.
  * @param source the string to be appended.
  *
  * @return str on success, or NULL on failure.
  */
crushstr_t* crushstr_append(crushstr_t *target, const char *source);

#endif /* CRUSHSTR_H */
