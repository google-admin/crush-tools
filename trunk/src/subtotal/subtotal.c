/********************************
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
 ********************************/
#include <ffutils.h>
#include "subtotal_main.h"

#define SUBTOT_KEY_MAXLEN 256

/** @brief  
  * 
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int subtotal(struct cmdargs *args, int argc, char *argv[], int optind) {

  int key;                      /* the index of the key column */
  int keylen;                   /* length of the key field in the current input line */

  char cur_key_val[SUBTOT_KEY_MAXLEN],  /* the value of the key field */
    prev_key_val[SUBTOT_KEY_MAXLEN];  /* the value of the previous key field */

  /* the assumption is that input is sorted, so when the key value
   * changes, it's time to print the subtotal line
   */

  size_t nsums;                 /* the number of columns to be subtotaled */
  int *sum_cols,                /* array of column indexes to subtotal */
   *sums;                       /* array to hold the sums */
  size_t sum_cols_sz /* capacity of sums_cols */ ;

  char *inbuf;                  /* input buffer and buffer size */
  ssize_t inbuf_sz;

  size_t n_fields;              /* the number of fields in the input line */
  int current_line, starting_line;
  char default_delim[] = { 0xfe, 0x00 };
  FILE *in, *out;

  key = atoi(args->key);
  if (args->start_line)
    starting_line = atoi(args->start_line);
  else
    starting_line = 0;

  if (!args->delim) {
    args->delim = getenv("DELIMITER");
    if (!args->delim)
      args->delim = default_delim;
  }
  expand_chars(args->delim);

  sums = NULL;
  sum_cols = NULL;
  sum_cols_sz = 0;
  nsums = 0;
  nsums = expand_nums(args->sum, &sum_cols, &sum_cols_sz);

  if (!nsums)
    return EXIT_HELP;

  if ((sums = malloc(sizeof(int) * nsums)) == NULL) {
    fprintf(stderr, "out of memory\n");
    free(sum_cols);
    return EXIT_MEM_ERR;
  }
  memset(sums, 0, sizeof(int) * nsums);

  in = stdin;
  out = stdout;

  if (args->infile) {
    if ((in = fopen(args->infile, "r")) == NULL) {
      perror(args->infile);
      return EXIT_FILE_ERR;
    }
  }
  if (args->outfile) {
    if ((out = fopen(args->outfile, "w")) == NULL) {
      perror(args->outfile);
      return EXIT_FILE_ERR;
    }
  }


  inbuf = NULL;
  inbuf_sz = 0;
  cur_key_val[0] = '\0';
  prev_key_val[0] = '\0';

  /* if a starting line was specified, skip everything before it. */
  current_line = 1;
  while (current_line < starting_line) {
    if (getline(&inbuf, &inbuf_sz, in) > 0)
      fprintf(out, "%s", inbuf);
    current_line++;
  }

  /* get the first line of data.  this way we can compare the
   * previous line & current line in the loop below w/out worrying
   * about whether or not there actually is a previous line. */
  if (getline(&inbuf, &inbuf_sz, in) > 0) {
    int i;
    char field[SUBTOT_KEY_MAXLEN];

    keylen =
      get_line_field(prev_key_val, inbuf, SUBTOT_KEY_MAXLEN, key - 1,
                     args->delim);
    fprintf(out, "%s", inbuf);

    /* prime the sums array with this line's values */
    for (i = 0; i < nsums; i++) {
      if (get_line_field
          (field, inbuf, SUBTOT_KEY_MAXLEN, sum_cols[i] - 1, args->delim) > 0) {
        sums[i] = atoi(field);
      }
    }
  }

  while (getline(&inbuf, &inbuf_sz, in) != -1) {
    chomp(inbuf);

    n_fields = fields_in_line(inbuf, args->delim);
    keylen =
      get_line_field(cur_key_val, inbuf, SUBTOT_KEY_MAXLEN, key - 1,
                     args->delim);

    /* assumption: lines without a key field are not
       interesting */
    if (keylen <= 0) {
      fprintf(out, "%s\n", inbuf);
      continue;
    }

    if (str_eq(cur_key_val, prev_key_val)) {
      /* same key - add in this line's sum fields */
      int i;
      char field[SUBTOT_KEY_MAXLEN];

      for (i = 0; i < nsums; i++) {
        if (get_line_field
            (field, inbuf, SUBTOT_KEY_MAXLEN, sum_cols[i] - 1,
             args->delim) > 0) {
          sums[i] += atoi(field);
        }
      }

    } else {
      /* if the key has changed, print out the subtotals */

      int i, n = 1;
      char field[SUBTOT_KEY_MAXLEN];

      for (i = 0; i < nsums; i++) {
        /* print all non-subtotaled columns between
         * the last one printed and the next */
        for (; n < sum_cols[i]; n++)
          fprintf(out, "%s", args->delim);
        n++;
        fprintf(out, "%d", sums[i]);
        if (sum_cols[i] < n_fields)
          fprintf(out, "%s", args->delim);
      }
      for (; n < n_fields; n++)
        fprintf(out, "%s", args->delim);

      fprintf(out, "\n\n");

      /* zero out the subtotal sums */
      memset(sums, 0, sizeof(int) * nsums);

      /* prime the sums array with this line's values */
      for (i = 0; i < nsums; i++) {
        if (get_line_field
            (field, inbuf, SUBTOT_KEY_MAXLEN, sum_cols[i] - 1,
             args->delim) > 0) {
          sums[i] = atoi(field);
        }
      }
    }
    /* print out current line */
    fprintf(out, "%s\n", inbuf);

    strncpy(prev_key_val, cur_key_val, SUBTOT_KEY_MAXLEN);
  }

  /* print out the final subtotal */
  if (1) {
    int i, n = 1;

    n_fields = fields_in_line(inbuf, args->delim);

    for (i = 0; i < nsums; i++) {
      /* print all non-subtotaled columns between
       * the last one printed and the next */
      for (; n < sum_cols[i]; n++)
        fprintf(out, "%s", args->delim);
      n++;
      fprintf(out, "%d", sums[i]);
      if (sum_cols[i] < n_fields)
        fprintf(out, "%s", args->delim);
    }
    for (; n < n_fields; n++)
      fprintf(out, "%s", args->delim);
    fprintf(out, "\n");

  }

  free(sums);
  free(sum_cols);
  fclose(in);
  fclose(out);

  return EXIT_OKAY;
}
