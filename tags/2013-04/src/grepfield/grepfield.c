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

#include <crush/general.h>
#include "grepfield_main.h"
#include "grepfield.h"

/** @brief
  *
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int grepfield(struct cmdargs *args, int argc, char *argv[], int optind) {

  char default_delim[2] = { 0xfe, 0x00 }; /* default field separator */

  regex_t pattern;
  int reg_flags;                /* flags to pass to regcomp() */
  int err_code;                 /* holds the return value of regcomp() */

  FILE *in, *out;               /* input & output files */
  dbfr_t *in_reader;
  int field_no;                 /* the field number specified by the user */

  char *fieldval = NULL;        /* buffer for the field to scan & its size */
  ssize_t fldsz = 0;

  /* function to pull the field out of the input line
     using a pointer so the case of no field can be treated
     the same as the normal case inside the file reading loop.
   */
  char *(*field_to_scan) (char **, ssize_t *, char *, char *, int f);

  if (optind >= argc) {
    usage(argv[0]);
    exit(EXIT_HELP);
  }

  if (!args->delim) {
    args->delim = getenv("DELIMITER");
    if (!args->delim)
      args->delim = default_delim;
  }
  expand_chars(args->delim);

  reg_flags = REG_EXTENDED;
  if (args->ignore_case)
    reg_flags |= REG_ICASE;

  err_code = regcomp(&pattern, argv[optind++], reg_flags);
  if (err_code != REG_OK) {
    re_perror(err_code, pattern);
    return EXIT_HELP;
  }

  if (args->outfile) {
    if ((out = fopen(args->outfile, "w")) == NULL) {
      perror(args->outfile);
      exit(EXIT_FILE_ERR);
    }
  } else {
    out = stdout;
  }

  if (optind < argc)
    in = nextfile(argc, argv, &optind, "r");
  else
    in = stdin;
  if (! in)
    return EXIT_FILE_ERR;
  in_reader = dbfr_init(in);

  if (args->field) {
    field_no = atoi(args->field) - 1;
    field_to_scan = scan_field;
    if (field_no < 0) {
      fprintf(stderr, "%s: %d: invalid field number.\n", getenv("_"), field_no);
      return EXIT_HELP;
    }
  } else if (args->field_label) {
    field_no = field_str(args->field_label, in_reader->next_line, args->delim);
    field_to_scan = scan_field;
    if (field_no < 0) {
      fprintf(stderr, "%s: %s: invalid field label.\n",
              getenv("_"), args->field_label);
      return EXIT_HELP;
    }
    args->preserve_header = 1;
  } else {
    field_no = -1;
    field_to_scan = scan_wholeline;
  }


  /* set the flags variable to the expected return value
     of regexec() */
  if (args->invert)
    reg_flags = REG_NOMATCH;
  else
    reg_flags = 0;

  if (args->preserve_header) {
    if (dbfr_getline(in_reader) > 0) {
      fputs(in_reader->current_line, out);
    }
  }

  while (in != NULL) {
    while (dbfr_getline(in_reader) > 0) {
      if (field_to_scan(&fieldval, &fldsz, in_reader->current_line,
                        args->delim, field_no) == NULL)
        continue;
      if (regexec(&pattern, fieldval, 0, NULL, 0) == reg_flags)
        fputs(in_reader->current_line, out);
    }

    dbfr_close(in_reader);
    if((in = nextfile(argc, argv, &optind, "r"))) {
      in_reader = dbfr_init(in);
      /* discard header from subsequent files */
      if (args->preserve_header)
        dbfr_getline(in_reader);
    }
  }

  regfree(&pattern);
  fclose(out);

  return EXIT_OKAY;
}

/* just points the field buffer at the original line */
char *scan_wholeline(char **field_buffer, ssize_t * field_buffer_size,
                     char *orig_line, char *delim, int field_no) {
  *field_buffer = orig_line;
  return *field_buffer;
}

/* (re)allocates memory for the field buffer as necessary and copies
 * the desired field into it.
 */
char *scan_field(char **field_buffer, ssize_t * field_buffer_size,
                 char *orig_line, char *delim, int field_no) {
  if (*field_buffer_size < strlen(orig_line)) {
    char *tmp;
    if (*field_buffer)
      tmp = xrealloc(*field_buffer, strlen(orig_line));
    else
      tmp = xmalloc(strlen(orig_line));
    *field_buffer = tmp;
  }

  if (get_line_field
      (*field_buffer, orig_line, *field_buffer_size, field_no, delim) < 0)
    return NULL;

  return *field_buffer;
}

void re_perror(int err_code, regex_t pattern) {
  size_t len;
  char *buf;
  len = regerror(err_code, &pattern, NULL, 0);
  buf = xmalloc(len);
  regerror(err_code, &pattern, buf, len);
  fprintf(stderr, "%s: %s\n", getenv("_"), buf);
  free(buf);
}
