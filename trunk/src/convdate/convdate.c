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
#include "convdate_main.h"
#include "convdate.h"

/** @brief  
  * 
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int convdate(struct cmdargs *args, int argc, char *argv[], int optind) {

  char default_delim[2] = { 0xfe, 0x00 }; /* default field separator */
  char default_input_format[] = "%m-%d-%Y-%T";
  char default_output_format[] = "%Y-%m-%d-%T";

  /* input & output files */
  FILE *in = stdin;
  dbfr_t *in_reader;
  FILE *out = stdout;
  unsigned long lineno = 0;

  int field_no; /* the field number specified by the user */

  struct tm storage = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };  /* storage for the time */

  char date[64] = "";

  size_t size = 0;
  int start = 0;
  int end = 0;
  int result = 0;

  in_reader = dbfr_init(in);

  // Set default delimiter if necessary.
  if (!args->delim) {
    args->delim = getenv("DELIMITER");
    if (!args->delim)
      args->delim = default_delim;
  }

  expand_chars(args->delim);
  // Set default value for the field if necessary.
  if (! args->field && ! args->field_label) {
    field_no = 0;
  } else if (args->field) {
    field_no = atoi(args->field) - 1;
  } else if (args->field_label) {
    field_no = field_str(args->field_label, in_reader->next_line, args->delim);
    args->preserve_header = 1;
  }

  if (field_no < 0) {
    fprintf(stderr, "%s: %d: invalid field number.\n", getenv("_"), field_no);
    return EXIT_HELP;
  }

  // Set default input date format if necessary.
  if (!args->input_format) {
    args->input_format = default_input_format;
  }
  if (!args->output_format) {
    args->output_format = default_output_format;
  }

  if (args->preserve_header) {
    if (dbfr_getline(in_reader) > 0) {
      fprintf(out, "%s", in_reader->current_line);
      lineno++;
    }
  }

  while (dbfr_getline(in_reader) > 0) {
    lineno++;
    result = get_line_pos(in_reader->current_line, field_no,
                          args->delim, &start, &end);
    if (result) {
      if (strptime(in_reader->current_line + start, args->input_format,
                   &storage)) {
        size = strftime(date, 64, args->output_format, &storage);
        in_reader->current_line[start] = '\0';
        fprintf(out, "%s%s%s", in_reader->current_line, date,
                in_reader->current_line + end + 1);
      } else {
        if (args->verbose) {
          fprintf(stderr, "%s: line %lu: could not convert date \"%.*s\"\n",
                  getenv("_"), lineno, end - start + 1,
                  in_reader->current_line + start);
        }
        fprintf(out, "%s", in_reader->current_line);
      }

    } else {  /* get_line_pos returned false - pass line through */
      if (args->verbose) {
        fprintf(stderr, "%s: line %lu: did not find the field at %i\n",
                getenv("_"), lineno, field_no + 1);
      }

      fprintf(out, "%s", in_reader->current_line);
    }
  }

  dbfr_close(in_reader);

  return EXIT_OKAY;
}
