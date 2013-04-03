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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include <crush/crushstr.h>
#include <crush/general.h>

#define CRUSHSTR_NOT_INITIALIZED(str) \
  ((str)->buffer == NULL)

int crushstr_init(crushstr_t *str, size_t capacity) {
  memset(str, 0, sizeof(crushstr_t));
  if (capacity > 0) {
    str->buffer = xmalloc(capacity);
    str->buffer[0] = '\0';
    str->capacity = capacity;
  }
  return 0;
}

void crushstr_destroy(crushstr_t *str) {
  free(str->buffer);
  str->buffer = NULL;
  str->capacity = 0;
  str->length = 0;
}

crushstr_t* crushstr_resize(crushstr_t *str, size_t capacity) {
  char *tmp;
  if (CRUSHSTR_NOT_INITIALIZED(str)) {
    crushstr_init(str, capacity);
  } else if (str->capacity < capacity) {
    tmp = xrealloc(str->buffer, capacity);
    if (! tmp)
      return NULL;
    str->buffer = tmp;
    str->capacity = capacity;
  }
  return str;
}

crushstr_t* crushstr_copy(crushstr_t *target, const char *source) {
  size_t source_len = strlen(source);
  if (crushstr_resize(target, source_len + 1) == NULL)
    return NULL;
  strcpy(target->buffer, source);
  target->length = source_len;
  return target;
}

crushstr_t* crushstr_append(crushstr_t *target, const char *source) {
  size_t source_len = strlen(source);
  if (crushstr_resize(target, target->capacity + source_len + 1) == NULL)
    return NULL;
  strcat(target->buffer, source);
  target->length += source_len;
  return target;
}

#undef CRUSHSTR_NOT_INITIALIZED
