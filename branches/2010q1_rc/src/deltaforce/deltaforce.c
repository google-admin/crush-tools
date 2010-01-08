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
#include "deltaforce_main.h"
#include "deltaforce.h"

#ifndef HAVE_OPEN64
# define open64 open
#endif


#define Fputs(s,f)					\
	do { if ( fputs((s), (f)) == EOF ) {		\
		warn("error writing to output:");	\
		return EXIT_FILE_ERR;			\
	} } while ( 0 )

char *delim;

int *keyfields = NULL;          /* array of fields common to both files */
size_t keyfields_sz = 0;
ssize_t nkeys;

/** @brief opens all the files necessary, sets a default
  * delimiter if none was specified, and calls the
  * merge_files() function.
  *
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  *
  * @return exit status for main() to return.
  */
int deltaforce(struct cmdargs *args, int argc, char *argv[], int optind) {
  char default_delimiter[] = { 0xfe, 0x00 };
  FILE *left, *right, *out;     /* the two inputs & the output file ptrs */
  dbfr_t *left_reader, *right_reader;
  int fd_tmp, retval;           /* file descriptor and return value */
  int i;

  if (argc - optind != 2) {
    fprintf(stderr,
            "%s: missing file arguments.  see %s -h for usage information.\n",
            argv[0], argv[0]);
    return EXIT_HELP;
  }

  if (str_eq(argv[optind], argv[optind + 1])) {
    fprintf(stderr, "%s: 2 input files cannot be the same\n", argv[0]);
    return EXIT_HELP;
  }

  if (str_eq(argv[optind], "-")) {
    left = stdin;
  } else {
    if ((fd_tmp = open64(argv[optind], O_RDONLY)) < 0) {
      perror(argv[optind]);
      return EXIT_FILE_ERR;
    }
    if ((left = fdopen(fd_tmp, "r")) == NULL) {
      perror(argv[optind]);
      return EXIT_FILE_ERR;
    }
  }
  left_reader = dbfr_init(left);

  if (str_eq(argv[optind + 1], "-")) {
    right = stdin;
  } else {
    if ((fd_tmp = open64(argv[optind + 1], O_RDONLY)) < 0) {
      perror(argv[optind + 1]);
      return EXIT_FILE_ERR;
    }
    if ((right = fdopen(fd_tmp, "r")) == NULL) {
      perror(argv[optind + 1]);
      return EXIT_FILE_ERR;
    }
  }
  right_reader = dbfr_init(right);

  if (!args->outfile) {
    out = stdout;
  } else {
    if ((fd_tmp = open64(args->outfile, O_WRONLY | O_CREAT | O_TRUNC)) < 0) {
      perror(args->outfile);
      return EXIT_FILE_ERR;
    }
    fchmod(fd_tmp, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if ((out = fdopen(fd_tmp, "w")) == NULL) {
      perror(args->outfile);
      return EXIT_FILE_ERR;
    }
  }

  if (!args->delim) {
    args->delim = getenv("DELIMITER");
    if (!args->delim)
      args->delim = default_delimiter;
  }
  expand_chars(args->delim);
  delim = args->delim;

  if (args->keys) {
    nkeys = expand_nums(args->keys, &keyfields, &keyfields_sz);
  } else if (args->key_labels) {
    nkeys = expand_label_list(args->key_labels, left_reader->next_line, delim,
                              &keyfields, &keyfields_sz);
  } else {
    keyfields = xmalloc(sizeof(int));
    keyfields[0] = 1;
    keyfields_sz = sizeof(int);
    nkeys = 1;
  }

  if (nkeys < 1) {
    fprintf(stderr, "%s: bad key specified: \"%s\"\n",
            getenv("_"), args->keys ? args->keys : args->key_labels);
    return EXIT_HELP;
  }
  for (i = 0; i < nkeys; i++)
    keyfields[i]--;

  /* set locale with values from the environment so strcoll()
     will work correctly. */
  setlocale(LC_ALL, "");
  setlocale(LC_COLLATE, "");

  retval = merge_files(left_reader, right_reader, out, args);

  fclose(left);
  fclose(right);
  fclose(out);

  return retval;
}


int merge_files(dbfr_t *left_reader, dbfr_t *right_reader, FILE * out,
                struct cmdargs *args) {

  int retval = EXIT_OKAY;

  char field_right[MAX_FIELD_LEN + 1];  /* buffer for holding field values */

  int i;                        /* general-purpose counter */

  /** @todo take into account that files a & b might have the same fields in
	  * a different order.
	  */
  int keycmp = 0;

  /* assume that if there is a header line, it exists
     in both files. */

  while (!left_reader->eof) {

    if (left_reader->current_line == NULL ||
        left_reader->current_line[0] == '\0') {
      /* get a line from the full set */
      if (dbfr_getline(left_reader) <= 0) {
        free(left_reader->current_line);
        left_reader->current_line = NULL;
        break;
      }
    }

    if (right_reader->current_line == NULL ||
        right_reader->current_line[0] == '\0') {
      if (right_reader->eof) {
        /* no more delta data to merge in: just dump
           the rest of the full data set. */
        Fputs(left_reader->current_line, out);
        while (dbfr_getline(left_reader) > 0)
          Fputs(left_reader->current_line, out);
        continue;
      }

      /* get a line from the delta set */
      if (dbfr_getline(right_reader) <= 0) {
        free(right_reader->current_line);
        right_reader->current_line = NULL;
        continue;
      }
    }

    keycmp = compare_keys(left_reader->current_line,
                          right_reader->current_line);

    switch (keycmp) {
        /* keys equal - print the delta line and scan
           forward in both files the next time around. */
      case 0:
        Fputs(right_reader->current_line, out);
        right_reader->current_line[0] = '\0';
        left_reader->current_line[0] = '\0';
        break;

        /* delta line greater than full-set line.
           print the full set line and keep the delta
           line for later.
         */
      case -1:
        Fputs(left_reader->current_line, out);
        left_reader->current_line[0] = '\0';
        break;

        /* delta line less than full-set line: the full
           set did not previously contain the key from
           delta. */
      case 1:
        Fputs(right_reader->current_line, out);
        right_reader->current_line[0] = '\0';
        break;
    }
  }

  if (right_reader->current_line != NULL &&
      right_reader->current_line[0] != '\0')
    Fputs(right_reader->current_line, out);

  if (! right_reader->eof) {
    while (dbfr_getline(right_reader) > 0)
      Fputs(right_reader->current_line, out);
  }

  if (keyfields)
    free(keyfields);

  return retval;
}


int compare_keys(char *buffer_left, char *buffer_right) {
  int keycmp = 0;
  int i;
  char field_left[MAX_FIELD_LEN + 1];
  char field_right[MAX_FIELD_LEN + 1];

  // printf("inside compare_keys([%s], [%s])\n", buffer_left, buffer_right);
  if (buffer_left == NULL && buffer_right == NULL) {
    return LEFT_RIGHT_EQUAL;
  }

  /* these special cases may seem counter-intuitive, but saying that
     a NULL line is greater than a non-NULL line results in
     the non-NULL line getting printed and a new line read in.
   */
  if (buffer_left == NULL)
    return LEFT_GREATER;

  if (buffer_right == NULL)
    return RIGHT_GREATER;

  for (i = 0; i < nkeys; i++) {
    get_line_field(field_left, buffer_left, MAX_FIELD_LEN, keyfields[i], delim);
    get_line_field(field_right, buffer_right, MAX_FIELD_LEN, keyfields[i],
                   delim);
    /* printf("Comparing (%s) to (%s) inside compare_keys\n", field_left, field_right); */
    if ((keycmp = strcoll(field_left, field_right)) != 0)
      break;
  }

  /* ensure predictable return values */
  if (keycmp == 0)
    return 0;
  if (keycmp < 0)
    return -1;
  if (keycmp > 0)
    return 1;
}
