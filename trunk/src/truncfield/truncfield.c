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
#include "truncfield_main.h"
#include <err.h>

#include <ffutils.h>
#include <qsort_helper.h>
#include <dbfr.h>

/** @brief  
  * 
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int truncfield(struct cmdargs *args, int argc, char *argv[], int optind) {

  char default_delim[] = { 0xfe, 0x00 };
  FILE *in;
  dbfr_t *in_reader;
  int *field_list = NULL;       /* list of fields to remove */
  size_t field_list_sz = 0;

  size_t n_fields = 0;          /* the number of fields from input
                                   file */
  if (! args->fields && ! args->field_labels) {
    fprintf(stderr, "%s: -f or -F must be specified.\n", argv[0]);
    return EXIT_HELP;
  }
  if (args->output_fname) {
    if (!freopen(args->output_fname, "w", stdout)) {
      warn(args->output_fname);
      return EXIT_FILE_ERR;
    }
  } else if (args->append_fname) {
    if (!freopen(args->append_fname, "a", stdout)) {
      warn(args->append_fname);
      return EXIT_FILE_ERR;
    }
  }

  if (!args->delim) {
    args->delim = getenv("DELIMITER");
    if (!args->delim)
      args->delim = default_delim;
  }
  expand_chars(args->delim);

  if (optind < argc)
    in = nextfile(argc, argv, &optind, "r");
  else
    in = stdin;
  if (! in) {
    fprintf(stderr, "%s: no valid input files.\n", argv[0]);
    return EXIT_FILE_ERR;
  }
  in_reader = dbfr_init(in);

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

    int next_field_to_trunc;    /* index into field_list */
    int i;                      /* index of current input field */
    int f_first, f_last;        /* position of first and last char
                                   in a field */
    int first_field_printed;    /* used to control delimiter output */

    while (dbfr_getline(in_reader) > 0) {
      next_field_to_trunc = 0;
      n_fields = fields_in_line(in_reader->current_line, args->delim);
      first_field_printed = 0;

      for (i = 0; i < n_fields; i++) {
        if (field_list[next_field_to_trunc] == i + 1) {
          ++next_field_to_trunc;
          if (i > 0)
            printf("%s", args->delim);
          first_field_printed = 1;
          continue;
        }
        if (!get_line_pos(in_reader->current_line, i, args->delim,
                          &f_first, &f_last))
          continue;

        if (first_field_printed)
          printf("%s", args->delim);

        printf("%.*s", f_last - f_first + 1,
               &(in_reader->current_line[f_first]));
        first_field_printed = 1;
      }

      /* print everything after the last field in the
       * line (preserves input line-break style) */
      get_line_pos(in_reader->current_line, n_fields - 1,
                   args->delim, &f_first, &f_last);
      printf("%s", &(in_reader->current_line[f_last + 1]));

    }
    dbfr_close(in_reader);
    in = nextfile(argc, argv, &optind, "r");
    if (in)
      in_reader = dbfr_init(in);
  }

  free(field_list);

  return EXIT_OKAY;
}
