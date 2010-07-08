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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <crush/general.h>
#include "buffer_main.h"

/*! initial buffer size */
#define BUFFER_INITIAL_SIZE 4096

/*! main execution function
 *
 * \para args contains the parsed cmd-line options & arguments.
 * \param argc number of arguments
 * \param argv argument vector
 * \param optind index of the first non-option cmd-line argument.
 *
 * \return status indicator
 *	- 0 = success
 *	- 1 = error
 * options are
 *	- <code>-h</code> - prints usage message & exits
 *	- <code>file(s)</code> - specifies input files
 *
 */
int buffer(struct cmdargs *args, int argc, char *argv[], int optind) {
  int opt;                      /*! option index */
  int fi;                       /*! index of current file within argv */
  FILE *fp;                     /*! pointer to input file */

  FILE *out = NULL;             /*! pointer to output file */
  char *outname = NULL;         /*! name of output file */
  int outappend = 0;            /*! indicates whether output should write or append */

  char *buffer;                 /*! buffer to hold input */
  size_t buffer_size = 0;       /*! current buffer size */
  size_t buffer_full = 0;       /*! current number of bytes in buffer */

  size_t amt_read = 0;          /*! number of bytes read from input */
  char tmp[512];                /*! holds input data before moving to buffer */

  buffer = (char *) xmalloc(BUFFER_INITIAL_SIZE);
  buffer_size = BUFFER_INITIAL_SIZE;
  fi = optind;

  do {
    if (optind == argc)
      fp = stdin;
    else if ((fp = fopen(argv[fi], "rb")) == NULL) {
      perror(argv[fi]);
      continue;
    }

    while (!feof(fp) && !ferror(fp)) {
      amt_read = fread(tmp, 1, 512, fp);
      if (amt_read + buffer_full > buffer_size) {
        buffer = (char *) xrealloc(buffer, buffer_size + BUFFER_INITIAL_SIZE);
        buffer_size += BUFFER_INITIAL_SIZE;
      }
      memcpy(buffer + buffer_full, tmp, amt_read);
      buffer_full += amt_read;
    }

    fi++;
  } while (fi < argc);

  if (args->output == NULL)
    out = stdout;
  else if (args->append) {
    if ((out = fopen(args->output, "ab")) == NULL) {
      perror(args->output);
      exit(1);
    }
  } else {
    if ((out = fopen(args->output, "wb")) == NULL) {
      perror(args->output);
      exit(1);
    }
  }

  fwrite(buffer, 1, buffer_full, out);
  fclose(out);
  free(buffer);

  return (0);
}
