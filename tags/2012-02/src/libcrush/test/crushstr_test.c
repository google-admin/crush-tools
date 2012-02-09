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

#include <crush/crushstr.h>
#include "unittest.h"

int main (int argc, char *argv[]) {
  crushstr_t str, *str_ptr;
  crushstr_init(&str, 8);
  ASSERT_TRUE(str.buffer != NULL,
              "crushstr_init: buffer initialized to non-null");
  ASSERT_STR_EQ("", str.buffer,
                "crushstr_init: buffer is an empty string");
  ASSERT_LONG_EQ(8L, str.capacity, "crushstr_init: capacity set");
  ASSERT_LONG_EQ(0L, str.length, "crushstr_init: length zeroed out");

  str_ptr = crushstr_copy(&str, "hello world");
  ASSERT_TRUE(str.capacity == strlen("hello world") + 1,
              "crushstr_copy: increased capacity");
  ASSERT_STR_EQ(str.buffer, "hello world", "crushstr_copy: copy to buffer");
  ASSERT_LONG_EQ(strlen("hello world"), str.length, "crushstr_copy: set length");
  ASSERT_TRUE(str_ptr == &str, "crushstr_copy: returned the string object");

  crushstr_copy(&str, "hello world");
  str_ptr = crushstr_append(&str, ", and again I say hello");
  ASSERT_STR_EQ("hello world, and again I say hello", str.buffer, 
                "crushstr_append: appended string");
  ASSERT_TRUE(str_ptr == &str, "crushstr_append: returned the string object");

  crushstr_destroy(&str);
  ASSERT_TRUE(str.buffer == NULL, "crushstr_destroy: buffer nulled out");
  ASSERT_LONG_EQ(0L, str.capacity, "crushstr_destroy: capacity zeroed out");
  ASSERT_LONG_EQ(0L, str.length, "crushstr_destroy: length zeroed out");

  return unittest_has_error;
}
