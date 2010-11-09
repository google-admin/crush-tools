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

#include <stdio.h>
#include <string.h>

int unittest_has_error = 0;

#define FAIL(s, ...) \
  do { \
    fprintf(stderr, "FAIL: (%s:%d): " s "\n", \
            __FILE__, __LINE__, ##__VA_ARGS__); \
    unittest_has_error = 1; \
  } while (0)

#define PASS(s, ...) \
  do { \
    fprintf(stderr, "PASS: (%s:%d): " s "\n", \
            __FILE__, __LINE__, ##__VA_ARGS__); \
  } while (0)

#define ASSERT_(expr, msg, expected, got, fmt) \
  do { \
    if ((expr)) { \
      PASS(msg); \
    } else { \
      FAIL(msg "\n  expected: " fmt "\n  got: " fmt, (expected), (got)); \
    } \
  } while (0)

#define ASSERT_TRUE(expr, msg) \
  do { \
    if ((expr)) { \
      PASS(msg); \
    } else { \
      FAIL(msg "\n  failed expression: %s", #expr); \
    } \
  } while (0)

#define ASSERT_STR_EQ(s1, s2, msg) \
  ASSERT_(strcmp((s1), (s2)) == 0, msg, (s1), (s2), "%s")
 
#define ASSERT_STR_NE(s1, s2, msg) \
  ASSERT_(strcmp((s1), (s2)) != 0, msg, (s1), (s2), "%s")
 
#define ASSERT_STR_LT(s1, s2, msg) \
  ASSERT_(strcmp((s1), (s2)) < 0, msg, (s1), (s2), "%s")
 
#define ASSERT_STR_GT(s1, s2, msg) \
  ASSERT_(strcmp((s1), (s2)) > 0, msg, (s1), (s2), "%s")
 
#define ASSERT_INT_EQ(n1, n2, msg) \
  ASSERT_((n1) == (n2), msg, (n1), (n2), "%d")

#define ASSERT_INT_NE(n1, n2, msg) \
  ASSERT_((n1) != (n2), msg, (n1), (n2), "%d")

#define ASSERT_INT_LT(n1, n2, msg) \
  ASSERT_((n1) < (n2), msg, (n1), (n2), "%d")

#define ASSERT_INT_GT(n1, n2, msg) \
  ASSERT_((n1) > (n2), msg, (n1), (n2), "%d")
