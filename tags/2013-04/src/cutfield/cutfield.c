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
#include "cutfield_main.h"
#include <err.h>

#include <crush/ffutils.h>
#include <crush/dbfr.h>
#include <crush/qsort_helper.h>

/** @brief  
  * 
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int cutfield(struct cmdargs *args, int argc, char *argv[], int optind) {

  char default_delim[] = { 0xfe, 0x00 };

  int *field_list = NULL;       /* list of fields to remove */
  ssize_t field_list_sz = 0;

  FILE *in;
  dbfr_t *in_reader;
  size_t n_fields = 0;  /* the number of fields from input file */
  int field_length;

  if (! (args->fields || args->field_labels)) {
    fprintf(stderr, "%s: -f or -F must be specified.\n", argv[0]);
    return EXIT_HELP;
  }

  if (optind >= argc) {
    in = stdin;
  } else {
    in = nextfile(argc, argv, &optind, "r");
  }
  if (in)
    in_reader = dbfr_init(in);

  if (args->output_fname) {
    if (!freopen(args->output_fname, "w", stdout)) {
      warn("%s", args->output_fname);
      return EXIT_FILE_ERR;
    }
  } else if (args->append_fname) {
    if (!freopen(args->append_fname, "a", stdout)) {
      warn("%s", args->append_fname);
      return EXIT_FILE_ERR;
    }
  }

  if (!args->delim) {
    args->delim = getenv("DELIMITER");
    if (!args->delim)
      args->delim = default_delim;
  }
  expand_chars(args->delim);

  if (args->fields) {
    field_list_sz = expand_nums(args->fields, &field_list, &field_list_sz);
  } else if (args->field_labels) {
    field_list_sz = expand_label_list(args->field_labels, in_reader->next_line,
                                      args->delim, &field_list, &field_list_sz);
  }
  if (field_list_sz < 1) {
    fprintf(stderr, "%s: error expanding field list.\n", argv[0]);
    return EXIT_HELP;
  }
  qsort(field_list, field_list_sz, sizeof(field_list[0]),
        (qsort_cmp_func_t) qsort_intcmp);

  while (in) {
    int next_field_to_skip;     /* index into field_list */
    int i;                      /* index of current input field */
    int f_first, f_last;        /* position of first and last char
                                   in a field */
    int first_field_printed;    /* used to control delimiter output */

    while (dbfr_getline(in_reader) > 0) {
      next_field_to_skip = 0;
      n_fields = fields_in_line(in_reader->current_line, args->delim);
      first_field_printed = 0;

      for (i = 0; i < n_fields; i++) {
        if (field_list[next_field_to_skip] == i + 1) {
          ++next_field_to_skip;
          continue;
        }
        field_length = get_line_pos(in_reader->current_line, i, args->delim,
                                    &f_first, &f_last);
        if (field_length < 0)
          continue;

        if (first_field_printed)
          printf("%s", args->delim);

        if (field_length > 0)
          printf("%.*s", f_last - f_first + 1,
                 &(in_reader->current_line[f_first]));
        first_field_printed = 1;
      }

      /* print everything after the last field in the
       * line (preserves input line-break style) */
      field_length = get_line_pos(in_reader->current_line, n_fields - 1,
                                  args->delim, &f_first, &f_last);
      if (field_length > 0)
        printf("%s", &(in_reader->current_line[f_last + 1]));
      else
        printf("%s", &(in_reader->current_line[f_last]));

    }
    dbfr_close(in_reader);
    in = nextfile(argc, argv, &optind, "r");
    if (in)
      in_reader = dbfr_init(in);
  }

  free(field_list);

  return EXIT_OKAY;
}
