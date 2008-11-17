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
#include "funiq_main.h"

#include <ffutils.h>
#include <dbfr.h>

#define FIELD_LEN_LIMIT 255


/** @brief  
  * 
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int funiq(struct cmdargs *args, int argc, char *argv[], int optind) {

  char delim[] = { 0xfe, 0x00 };  /* the delimiter */
  int *fields = NULL;   /* array of field indexes */
  size_t fields_sz = 0; /* the size of the array */
  size_t n_fields;      /* the number of things in the array */

  FILE *in;
  dbfr_t *in_reader;

  char **prev_line;             /* fields from previous line of input */
  char cur_field[FIELD_LEN_LIMIT];

  int i;

  int dup_count = 1;            /* used with -c option */
  char linebreak[3];

  /* use the default delimiter if necessary */
  if (!args->delim) {
    args->delim = getenv("DELIMITER");
    if (!args->delim)
      args->delim = delim;
  }
  expand_chars(args->delim);

  /* get the first file */
  if (optind < argc)
    in = nextfile(argc, argv, &optind, "r");
  else
    in = stdin;
  if (!in) {
    fprintf(stderr, "%s: no valid input files\n", argv[0]);
    return EXIT_HELP;
  }
  in_reader = dbfr_init(in);

  if (args->fields)
    n_fields = expand_nums(args->fields, &fields, &fields_sz);
  else if (args->field_labels)
    n_fields = expand_label_list(args->field_labels, in_reader->next_line,
                                 args->delim, &fields, &fields_sz);
  if (n_fields < 0) {
    fprintf(stderr, "%s: error expanding field list\n", argv[0]);
    return EXIT_HELP;
  }

  /* prepare the array of previous field values */
  prev_line = malloc(sizeof(char *) * n_fields);
  for (i = 0; i < n_fields; i++) {
    prev_line[i] = malloc(sizeof(char *) * FIELD_LEN_LIMIT);
  }

  /* get the first line to seed the prev_line array */
  i = dbfr_getline(in_reader);

  /* preserve input linebreak style.  assume there can only be 1 or 2 chars
   * in a linebreak sequence */
  if (in_reader->current_line[i - 2] == '\r' ||
      in_reader->current_line[i - 2] == '\n') {
    linebreak[0] = in_reader->current_line[i - 2];
    linebreak[1] = in_reader->current_line[i - 1];
    linebreak[2] = '\0';
  } else {
    linebreak[0] = in_reader->current_line[i - 1];
    linebreak[1] = '\0';
  }
  chomp(in_reader->current_line);

  for (i = 0; i < n_fields; i++) {
    get_line_field(prev_line[i], in_reader->current_line, FIELD_LEN_LIMIT - 1,
                   fields[i] - 1, args->delim);
  }
  printf("%s", in_reader->current_line); /* first line is never a dup */

  while (in) {
    int matching_fields;

    while (dbfr_getline(in_reader) > 0) {
      chomp(in_reader->current_line);

      matching_fields = 0;

      for (i = 0; i < n_fields; i++) {
        /* extract the field from the input line */
        get_line_field(cur_field, in_reader->current_line, FIELD_LEN_LIMIT - 1,
                       fields[i] - 1, args->delim);

        /* see if the field is a duplicate */
        if (str_eq(cur_field, prev_line[i]))
          matching_fields++;

        /* store this line's value */
        strcpy(prev_line[i], cur_field);
      }

      /* if not all of the fields matched, the line
         wasn't a duplicate */
      if (matching_fields != n_fields) {

        if (args->count) {
          /* print the number of dups for
           * the previous output line */
          printf("%s%d%s", args->delim, dup_count, linebreak);
        } else {
          /* give the previous output line a linebreak */
          printf("%s", linebreak);
        }
        printf("%s", in_reader->current_line);
        dup_count = 1;
      } else {
        dup_count++;
      }
    }

    dbfr_close(in_reader);
    in = nextfile(argc, argv, &optind, "r");
    if (in)
      in_reader = dbfr_init(in);
  }

  if (args->count) {
    /* print the number of dups for the last output line */
    printf("%s%d%s", args->delim, dup_count, linebreak);
  } else {
    /* give the last output line a linebreak */
    printf("%s", linebreak);
  }

  for (i = 0; i < n_fields; i++) {
    free(prev_line[i]);
  }
  free(prev_line);
  free(fields);
  return EXIT_OKAY;
}
