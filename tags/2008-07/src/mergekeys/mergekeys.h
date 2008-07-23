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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <ffutils.h>

#ifdef HAVE_FCNTL_H
# include <fcntl.h>             /* open64() */
#elif HAVE_SYS_FCNTL_H
# include <sys/fcntl.h>         /* open64() */
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>            /* off64_t & close() */
#endif

#ifdef HAVE_ERR_H
# include <err.h>               /* warn() */
#endif

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifndef MERGEKEYS_H
#define MERGEKEYS_H

#define MAX_FIELD_LEN 255


/* macros to clarify the semantics of key comparisons. */

/* these are used to compare "keycmp" */
#define LEFT_LT_RIGHT(n) (n) < 0
#define LEFT_LE_RIGHT(n) (n) <= 0
#define LEFT_EQ_RIGHT(n) (n) == 0
#define LEFT_GT_RIGHT(n) (n) > 0
#define LEFT_GE_RIGHT(n) (n) >= 0

/* these are used to assign to keycmp */
#define LEFT_GREATER  1
#define RIGHT_GREATER -1
#define LEFT_RIGHT_EQUAL   0


enum join_type_t {
  join_type_outer,
  join_type_inner,
  join_type_left_outer,
  join_type_right_outer,
};

int merge_files(FILE * a, FILE * b, enum join_type_t join_type, FILE * out,
                struct cmdargs *args);

void classify_fields(char *left_header, char *right_header);
int compare_keys(char *buffer_left, char *buffer_right);
void join_lines(char *left_line, char *right_line, FILE * out);
int peek_keys(char *peek_line, char *current_line);

#endif /* MERGEKEYS_H */
