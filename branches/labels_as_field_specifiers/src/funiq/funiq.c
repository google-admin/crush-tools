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
  int *fields;                  /* array of field indexes */
  size_t fields_sz;             /* the size of the array */
  size_t n_fields;              /* the number of things in the array */

  FILE *in;                     /* input file pointer */
  char **prev_line;             /* fields from previous line of input */
  char cur_field[FIELD_LEN_LIMIT];

  int i;
  char *buf;
  size_t bufsz;

  int dup_count = 1;            /* used with -c option */
  char linebreak[3];

  /* use the default delimiter if necessary */
  if (!args->delim) {
    args->delim = getenv("DELIMITER");
    if (!args->delim)
      args->delim = delim;
  }
  expand_chars(args->delim);

  /* turn the user-provided list of field numbers
     into an array of ints */
  fields = malloc(sizeof(int) * 32);
  if (!fields) {
    fprintf(stderr, "%s: out of memory\n", getenv("_"));
    return (EXIT_MEM_ERR);
  }
  fields_sz = 32;
  n_fields = expand_nums(args->fields, &fields, &fields_sz);

  /* prepare the array of previous field values */
  prev_line = malloc(sizeof(char *) * n_fields);
  for (i = 0; i < n_fields; i++) {
    prev_line[i] = malloc(sizeof(char *) * FIELD_LEN_LIMIT);
  }

  /* get the first file */
  if (optind < argc)
    in = nextfile(argc, argv, &optind, "r");
  else
    in = stdin;

  /* get the first line to seed the prev_line array */
  buf = NULL;
  bufsz = 0;

  i = getline(&buf, &bufsz, in);

  /* ASSUMPTION: there can only be 1 or 2 chars in a linebreak
   * sequence */
  if (buf[i - 2] == '\r' || buf[i - 2] == '\n') {
    linebreak[0] = buf[i - 2];
    linebreak[1] = buf[i - 1];
    linebreak[2] = '\0';
  } else {
    linebreak[0] = buf[i - 1];
    linebreak[1] = '\0';
  }
  chomp(buf);

  for (i = 0; i < n_fields; i++) {
    get_line_field(prev_line[i], buf, FIELD_LEN_LIMIT - 1,
                   fields[i] - 1, args->delim);
  }
  printf("%s", buf);            /* first line is never a dup */

  while (in) {
    int matching_fields;

    while (getline(&buf, &bufsz, in) > 0) {

      chomp(buf);

      matching_fields = 0;

      for (i = 0; i < n_fields; i++) {
        /* extract the field from the input line */
        get_line_field(cur_field, buf, FIELD_LEN_LIMIT - 1,
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
        printf("%s", buf);
        dup_count = 1;
      } else {
        dup_count++;
      }
    }

    fclose(in);
    in = nextfile(argc, argv, &optind, "r");
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
