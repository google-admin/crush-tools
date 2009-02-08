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
#include <crush/ffutils.h>
#include <crush/dbfr.h>
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

  int key;      /* the index of the key column */
  int keylen;   /* length of the key field in the current input line */

  char cur_key_val[SUBTOT_KEY_MAXLEN],  /* the value of the key field */
       prev_key_val[SUBTOT_KEY_MAXLEN];  /* the value of the previous key field */

  /* the assumption is that input is sorted, so when the key value
   * changes, it's time to print the subtotal line
   */

  size_t nsums = 0;         /* the number of columns to be subtotaled */
  int *sum_cols = NULL,     /* array of column indexes to subtotal */
      *sums = NULL;         /* array to hold the sums */
  size_t sum_cols_sz = 0;   /* capacity of sums_cols */

  size_t n_fields;          /* the number of fields in the input line */
  int current_line, starting_line;
  char default_delim[] = { 0xfe, 0x00 };
  FILE *in, *out;
  dbfr_t *in_reader;

  if (! args->key && ! args->key_label) {
    fprintf(stderr, "%s: -k or -K must be specified.\n", argv[0]);
    return EXIT_HELP;
  }
  if (! args->sum && ! args->sum_labels) {
    fprintf(stderr, "%s: -s or -S must be specified.\n", argv[0]);
    return EXIT_HELP;
  }

  if (! args->delim) {
    args->delim = getenv("DELIMITER");
    if (!args->delim)
      args->delim = default_delim;
  }
  expand_chars(args->delim);

  if (args->start_line) {
    starting_line = atoi(args->start_line);
  } else {
    if (args->key_label || args->sum_labels)
      starting_line = 2;
    else
      starting_line = 1;
  }

  in = stdin;
  if (args->infile) {
    if ((in = fopen(args->infile, "r")) == NULL) {
      perror(args->infile);
      return EXIT_FILE_ERR;
    }
  }
  in_reader = dbfr_init(in);

  out = stdout;
  if (args->outfile) {
    if ((out = fopen(args->outfile, "w")) == NULL) {
      perror(args->outfile);
      return EXIT_FILE_ERR;
    }
  }

  /* if a starting line was specified, skip everything before it.
   * Or if a header line is expected, read up to that point. */
  while (in_reader->line_no < starting_line - 1) {
    if (dbfr_getline(in_reader) > 0)
      fprintf(out, "%s", in_reader->current_line);
  }

  if (args->key) {
    key = atoi(args->key);
    if (key == 0) {
      fprintf(stderr, "%s: invalid key index \"%s\"\n",
              argv[0], args->key);
      return EXIT_HELP;
    }
  } else {
    key = -1;
    key = field_str(args->key_label, in_reader->current_line,
                    args->delim);
    if (key < 0) {
      fprintf(stderr, "%s: error locating field \"%s\"\n",
              argv[0], args->key_label);
      return EXIT_HELP;
    }
    key++;
  }

  if (args->sum) {
    nsums = expand_nums(args->sum, &sum_cols, &sum_cols_sz);
  } else if (args->sum_labels) {
    nsums = expand_label_list(args->sum_labels, in_reader->current_line,
                              args->delim, &sum_cols, &sum_cols_sz);
  }

  if (! nsums) {
    fprintf(stderr, "%s: error expanding sums list \"%s\"\n",
            argv[0], args->sum ? args->sum : args->sum_labels);
    return EXIT_HELP;
  }

  if ((sums = malloc(sizeof(int) * nsums)) == NULL) {
    fprintf(stderr, "%s: out of memory\n", getenv("_"));
    free(sum_cols);
    return EXIT_MEM_ERR;
  }
  memset(sums, 0, sizeof(int) * nsums);

  cur_key_val[0] = '\0';
  prev_key_val[0] = '\0';

  /* get the first line of data.  this way we can compare the
   * previous line & current line in the loop below w/out worrying
   * about whether or not there actually is a previous line. */
  if (dbfr_getline(in_reader) > 0) {
    int i;
    char field[SUBTOT_KEY_MAXLEN];

    keylen =
      get_line_field(prev_key_val, in_reader->current_line,
                     SUBTOT_KEY_MAXLEN, key - 1, args->delim);
    fprintf(out, "%s", in_reader->current_line);

    /* prime the sums array with this line's values */
    for (i = 0; i < nsums; i++) {
      if (get_line_field(field, in_reader->current_line, SUBTOT_KEY_MAXLEN,
                         sum_cols[i] - 1, args->delim) > 0) {
        sums[i] = atoi(field);
      }
    }
  }

  while (dbfr_getline(in_reader) > 0) {
    chomp(in_reader->current_line);

    n_fields = fields_in_line(in_reader->current_line, args->delim);
    keylen =
      get_line_field(cur_key_val, in_reader->current_line, SUBTOT_KEY_MAXLEN,
                     key - 1, args->delim);

    /* assumption: lines without a key field are not
       interesting */
    if (keylen <= 0) {
      fprintf(out, "%s\n", in_reader->current_line);
      continue;
    }

    if (str_eq(cur_key_val, prev_key_val)) {
      /* same key - add in this line's sum fields */
      int i;
      char field[SUBTOT_KEY_MAXLEN];

      for (i = 0; i < nsums; i++) {
        if (get_line_field(field, in_reader->current_line, SUBTOT_KEY_MAXLEN,
                           sum_cols[i] - 1, args->delim) > 0) {
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
        if (get_line_field(field, in_reader->current_line, SUBTOT_KEY_MAXLEN,
                           sum_cols[i] - 1, args->delim) > 0) {
          sums[i] = atoi(field);
        }
      }
    }
    /* print out current line */
    fprintf(out, "%s\n", in_reader->current_line);

    strncpy(prev_key_val, cur_key_val, SUBTOT_KEY_MAXLEN);
  }

  /* print out the final subtotal */
  if (1) {
    int i, n = 1;

    n_fields = fields_in_line(in_reader->current_line, args->delim);

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
