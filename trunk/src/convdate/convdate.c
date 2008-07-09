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

  FILE *in = stdin;
  FILE *out = stdout;           /* input & output files */
  unsigned long lineno = 0;

  int field_no;                 /* the field number specified by the user */

  char *buffer = NULL;          /* buffer for file input & its size */
  ssize_t bufsz = 0;

  struct tm storage = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };  /* storage for the time */

  char date[64] = "";

  size_t size = 0;
  int start = 0;
  int end = 0;
  int result = 0;

  // Set default value for the field if necessary.
  if (!args->field) {
    field_no = 1;
  } else {
    field_no = atoi(args->field);
    if (field_no < 0) {
      fprintf(stderr, "%d: invalid field number.\n", field_no);
      return EXIT_HELP;
    }
  }

  // Set default input date format if necessary.
  if (!args->input_format) {
    args->input_format = default_input_format;
  }
  if (!args->output_format) {
    args->output_format = default_output_format;
  }
  // Set default delimiter if necessary.
  if (!args->delim) {
    args->delim = getenv("DELIMITER");
    if (!args->delim)
      args->delim = default_delim;
  }
  expand_chars(args->delim);

  // Process the input stream
  if (in != NULL) {

    // Should we preserve the header line?
    if (args->preserve_header) {
      // Yes => Do we have a line at all?
      if (getline(&buffer, &bufsz, in) > 0) {
        // Yes => Just print this to the output file
        fprintf(out, "%s", buffer);
        lineno++;
      }
    }
    // Process each line
    while (getline(&buffer, &bufsz, in) > 0) {

      // Increase line number
      lineno++;

      // Find the field
      result = get_line_pos(buffer, field_no - 1, args->delim, &start, &end);
      if (result) {

        // Yes => Convert input date into a time value. Success?
        if (strptime(buffer + start, args->input_format, &storage) != NULL) {

          // Yes => Convert time value into a string
          size = strftime(date, 64, args->output_format, &storage);

          // Cut off the first part before the field.
          buffer[start] = '\0';

          // Now write the first part, the date itself and the remainder of the line
          fprintf(out, "%s%s%s", buffer, date, buffer + end + 1);
        } else {

          // No => print unmodified line
          if (args->verbose) {
            fprintf(stderr, "line %lu: could not convert date \"%.*s\"\n",
                    lineno, end - start + 1, buffer + start);
          }

          fprintf(out, "%s", buffer);
        }

      } else {                  /* get_line_pos returned false */

        // pass the line through if we have not found the field.
        if (args->verbose) {
          fprintf(stderr, "line %lu: did not find the field at %i\n", lineno,
                  field_no);
        }

        fprintf(out, "%s", buffer);

      }
    }
  }

  if (buffer) {
    free(buffer);
  }

  return EXIT_OKAY;
}
