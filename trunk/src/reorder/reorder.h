/*****************************************
   Copyright 2008 Google Inc.

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

#if HAVE_CONFIG_H
# include <config.h>
#endif

#if HAVE_REGEX_H
# include <regex.h>
#endif

#if HAVE_ASSERT_H
# include <assert.h>
#endif

#include <ffutils.h>

#ifndef REORDER_H
#define REORDER_H

#define REORDER_TYPE_SWAP 1
#define REORDER_TYPE_MOVE 2

/* validates swap arg from commandline and pushes values into the array
   of swaps to be performed */
int pushswap(char *s, int action_type);

#endif /* REORDER_H */

