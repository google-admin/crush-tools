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
#include <crush/ffutils.h>

#include "indexof_main.h"
#include "indexof.h"

int indexof(struct cmdargs *args, int argc, char *argv[], int optind) {
  FILE *in;
  int lineno = 1, curline = 0, index = 1;
  char *buf = NULL, *tok;
  size_t buflen = 0;
  char default_delim[] = { 0xfe, 0x00 };

  if (args->file != NULL) {
    if ((in = fopen(args->file, "r")) == NULL) {
      perror(args->file);
      return EXIT_FILE_ERR;
    }
  } else {
    in = stdin;
  }

  if (!args->delim) {
    args->delim = getenv("DELIMITER");
    if (!args->delim)
      args->delim = default_delim;
  }
  expand_chars(args->delim);

  if (args->row != 0)
    sscanf(args->row, "%d", &lineno);

  if (args->string == NULL) {
    /* read header from file */
    do {
      if (getline(&buf, &buflen, in) == -1) {
        free(buf);
        fprintf(stderr, "%s: out of memory\n", getenv("_"));
        return EXIT_MEM_ERR;
      }
      curline++;
    } while (lineno > curline && !feof(in));

    chomp(buf);

    if (feof(in)) {
      fprintf(stderr, "%s: invalid line number: %d\n", lineno, getenv("_"));
      exit(EXIT_HELP);
    }
  } else {
    /* header string provided on commandline, but strtok needs
       a dynamically allocated copy */
    buf = xmalloc(strlen(args->string) + 1);
    strcpy(buf, args->string);
  }

  tok = strtok(buf, args->delim);
  while (tok != NULL && strcmp(args->label, tok) != 0) {
    tok = strtok(NULL, args->delim);
    index++;
  }
  if (tok == NULL)
    index = 0;

  free(buf);
  printf("%d\n", index);
  return EXIT_OKAY;
}
