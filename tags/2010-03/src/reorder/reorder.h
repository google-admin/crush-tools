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

#include <crush/ffutils.h>
#include <crush/dbfr.h>
#include <crush/linklist.h>

#ifndef REORDER_H
#define REORDER_H

#define REORDER_TYPE_SWAP 1
#define REORDER_TYPE_MOVE 2

struct swap_arg {
  char *pair_str;
  int action_type;
};
struct swap_pair {
  int pair[2];
  int action_type;
};

/* validates swap arg from commandline and pushes values into the array
   of swaps to be performed */
int pushswap(char *s, int action_type);

/** @brief copies ct into s, swapping fields in the process.
  * 
  * @param s destination buffer
  * @param ct source buffer
  * @param d delimiter
  */
void doswap(llist_t *list, char *s, char *ct, const char *d);

/** @brief turns a list of swap/move swap_args (commandline option strings)
  * into a list of struct swap_pairs.
  *
  * @param args input linked list of struct swap_arg
  * @param pairs output linked list of struct swap_pair
  * @param header first line of input file for translating labels in the
  *               input args
  * @param delim field separator used in header
  */
int parse_swap_list(llist_t *args, llist_t *pairs,
                    const char *header, const char *delim);

/** @brief creates in s a new string delimited by d, containing the fields
  * from ct in the order specified by o.
  *
  * because a field may be included multiple times in the destination buffer,
  * it is entirely possible for the output to be larger than the input, so
  * this function may dynamically resize s as necessary.
  *
  * @param s address of destination buffer
  * @param ct source buffer
  * @param s_sz address of the size of destination buffer
  * @param d delimiter
  * @param o array of field numbers
  * @param n number of elements in o
  *
  * @return the length of s on success; < 0 on error
  */
int docut(char **s, const char *ct, size_t * s_sz, const char *d,
          const int *order, const size_t n);

#endif /* REORDER_H */
