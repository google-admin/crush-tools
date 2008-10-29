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
#include "mergekeys_main.h"
#include "mergekeys.h"

#ifndef HAVE_OPEN64
# define open64 open
#endif

char *delim;
size_t nfields_left;
size_t nfields_right;

/* arrays of fields common to both files */
int *left_keyfields = NULL;
int *right_keyfields = NULL;
size_t nkeys;

/* fields only in RIGHT or LEFT file */
int *left_mergefields = NULL;
int *right_mergefields = NULL;
size_t left_ntomerge, right_ntomerge;



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
int mergekeys(struct cmdargs *args, int argc, char *argv[], int optind) {
  char default_delimiter[] = { 0xfe, 0x00 };
  FILE *left, *right, *out;     /* the two inputs & the output file ptrs */
  int fd_tmp, retval;           /* file descriptor and return value */

  enum join_type_t join_type;

  if (args->left_keys && ! args->right_keys ||
      ! args->left_keys && args->right_keys) {
    fprintf(stderr, "%s: if -a or -b is specified, the other must be also.\n",
            argv[0]);
    return EXIT_HELP;
  }

  if (argc - optind != 2) {
    fprintf(stderr,
            "%s: missing file arguments.  see %s -h for usage information.\n",
            argv[0], argv[0]);
    return EXIT_HELP;
  } else if (str_eq(argv[optind], argv[optind + 1])) {
    /* TODO: it would be safer to convert these to absolute
       paths first.  "mergekeys file ./file" would still
       go through.
     */
    fprintf(stderr,
            "%s: both input files are the same. see %s -h for usage information.\n",
            argv[0], argv[0]);
    return EXIT_HELP;
  }

  if (str_eq(argv[optind], "-"))
    left = stdin;
  else {
    if ((fd_tmp = open64(argv[optind], O_RDONLY)) < 0) {
      perror(argv[optind]);
      return EXIT_FILE_ERR;
    }
    if ((left = fdopen(fd_tmp, "r")) == NULL) {
      perror(argv[optind]);
      return EXIT_FILE_ERR;
    }
  }

  if (str_eq(argv[optind + 1], "-"))
    right = stdin;
  else {
    if ((fd_tmp = open64(argv[optind + 1], O_RDONLY)) < 0) {
      perror(argv[optind + 1]);
      return EXIT_FILE_ERR;
    }
    if ((right = fdopen(fd_tmp, "r")) == NULL) {
      perror(argv[optind + 1]);
      return EXIT_FILE_ERR;
    }
  }

  if (!args->outfile)
    out = stdout;
  else {
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

  if (!args->merge_default) {
    args->merge_default = "";
  }
  if (args->inner)
    join_type = join_type_inner;
  else if (args->left)
    join_type = join_type_left_outer;
  else if (args->right)
    join_type = join_type_right_outer;
  else
    join_type = join_type_outer;


  /* set locale with values from the environment so strcoll()
     will work correctly. */
  setlocale(LC_ALL, "");
  setlocale(LC_COLLATE, "");

  retval = merge_files(left, right, join_type, out, args);

  fclose(left);
  fclose(right);
  fclose(out);

  return retval;
}


int merge_files(FILE * left, FILE * right, enum join_type_t join_type,
                FILE * out, struct cmdargs *args) {

  int retval = EXIT_OKAY;

  /* input line buffers for first & second files */
  char *buffer_left = NULL;
  char *buffer_right = NULL;
  char *peek_buffer_left = NULL;
  char *peek_buffer_right = NULL;

  /* size of the buffers */
  size_t buffer_left_size = 0;
  size_t buffer_right_size = 0;
  size_t peek_buffer_left_size = 0;
  size_t peek_buffer_right_size = 0;

  /* end of file flags */
  int eof_left = 0;
  int eof_right = 0;

  /* buffer for holding field values */
  char field_right[MAX_FIELD_LEN + 1];

  /* general-purpose counter */
  int i;

  /** @todo take into account that files a & b might have the same fields in
	  * a different order.
	  */
  int keycmp = 0;

  if (getline(&buffer_left, &buffer_left_size, left) <= 0) {
    fprintf(stderr, "%s: no header found in left-hand file\n", getenv("_"));
    retval = EXIT_FILE_ERR;
    goto cleanup;
  }
  if (getline(&buffer_right, &buffer_right_size, right) <= 0) {
    fprintf(stderr, "%s: no header found in right-hand file\n", getenv("_"));
    retval = EXIT_FILE_ERR;
    goto cleanup;
  }

  chomp(buffer_left);
  chomp(buffer_right);

  nfields_left = fields_in_line(buffer_left, delim);
  nfields_right = fields_in_line(buffer_right, delim);

  if (args->verbose) {
    fprintf(stderr,
            "VERBOSE: fields in left: %s\nVERBOSE: fields in right: %s\n",
            buffer_left, buffer_right);
  }

  if ((left_keyfields = malloc(sizeof(int) * nfields_left)) == NULL) {
    warn("allocating key array");
    retval = EXIT_MEM_ERR;
    goto cleanup;
  }
  if ((right_keyfields = malloc(sizeof(int) * nfields_right)) == NULL) {
    warn("allocating key array");
    retval = EXIT_MEM_ERR;
    goto cleanup;
  }

  if ((left_mergefields = malloc(sizeof(int) * nfields_left)) == NULL) {
    warn("allocating merge-fields array");
    retval = EXIT_MEM_ERR;
    goto cleanup;
  }
  if ((right_mergefields = malloc(sizeof(int) * nfields_right)) == NULL) {
    warn("allocating merge-fields array");
    retval = EXIT_MEM_ERR;
    goto cleanup;
  }

  if (args->left_keys && args->right_keys) {
    if (set_field_types(args->left_keys, args->right_keys) < 0) {
      retval = EXIT_FAILURE;
      goto cleanup;
    }
  } else {
    /* use headers to figure out which fields are keys or need to be merged */
    classify_fields(buffer_left, buffer_right);
  }


  if (nkeys == 0) {
    fprintf(stderr, "%s: no common fields found\n", getenv("_"));
    retval = EXIT_HELP;
    goto cleanup;
  }

  if (args->verbose) {
    fprintf(stderr, "VERBOSE: # key fields:       %d\n", nkeys);
    fprintf(stderr, "VERBOSE: left merge fields:  %d\n", left_ntomerge);
    fprintf(stderr, "VERBOSE: right merge fields: %d\n", right_ntomerge);
  }

  /* print the headers which were already read in above */
  extract_and_print_fields(buffer_left, left_keyfields, nkeys, delim, out);
  if (left_ntomerge > 0)
    fputs(delim, out);
  extract_and_print_fields(buffer_left, left_mergefields, left_ntomerge,
                           delim, out);
  if (right_ntomerge > 0)
    fputs(delim, out);
  extract_and_print_fields(buffer_right, right_mergefields, right_ntomerge,
                           delim, out);
  fputc('\n', out);

  /* we need to have the buffers cleared before the first call to my_getline */
  free(buffer_left);
  buffer_left = NULL;
  buffer_left_size = 0;
  free(buffer_right);
  buffer_right = NULL;
  buffer_right_size = 0;

  /* force a line-read from RIGHT the first time around.
     if eof is reached here, we still need to process
     the left-hand file.
   */
  keycmp = LEFT_RIGHT_EQUAL;
  my_getline(&buffer_right, &buffer_right_size,
             &peek_buffer_right, &peek_buffer_right_size, right, &eof_right);

left_file_loop:

  while (!eof_left) {
    int left_line_printed = 0;

    if (LEFT_LE_RIGHT(keycmp)) {
      if (my_getline(&buffer_left, &buffer_left_size, &peek_buffer_left,
                     &peek_buffer_left_size, left, &eof_left) <= 0) {
        keycmp = compare_keys(buffer_left, buffer_right);
        goto right_file_loop;
      }
    }

    keycmp = compare_keys(buffer_left, buffer_right);

    if (LEFT_LT_RIGHT(keycmp)) {
      if (join_type == join_type_outer || join_type == join_type_left_outer)
        join_lines(buffer_left, NULL, args->merge_default, out);
      goto left_file_loop;
    }

    if (LEFT_EQ_RIGHT(keycmp)) {
      /* everybody likes an inner join */
      join_lines(buffer_left, buffer_right, args->merge_default, out);

      if (peek_keys(peek_buffer_left, buffer_left) == 0) {
        /* the keys in the next line of LEFT are the same.
           handle "many:1"
         */
        goto left_file_loop;
      }

      left_line_printed = 1;
    }

  right_file_loop:

    while (!eof_right) {
      if (LEFT_GT_RIGHT(keycmp)) {
        if (join_type == join_type_outer || join_type == join_type_right_outer)
          join_lines(NULL, buffer_right, args->merge_default, out);
      }

      my_getline(&buffer_right, &buffer_right_size, &peek_buffer_right,
                 &peek_buffer_right_size, right, &eof_right);
      keycmp = compare_keys(buffer_left, buffer_right);

      if (LEFT_LT_RIGHT(keycmp)) {

        if ((!left_line_printed) &&
            (join_type == join_type_outer
             || join_type == join_type_left_outer)) {

          join_lines(buffer_left, NULL, args->merge_default, out);
        }

        goto left_file_loop;
      }

      if (LEFT_EQ_RIGHT(keycmp)) {
        join_lines(buffer_left, buffer_right, args->merge_default, out);
        left_line_printed = 1;


        /* if the keys in the next line of LEFT are the same,
           handle "many:1".
         */
        int peek_cmp = peek_keys(peek_buffer_left, buffer_left);
        if ((args->inner && peek_cmp <= 0) || peek_cmp == 0) {
          goto left_file_loop;
        }

        /* if the keys in the next line of RIGHT are the same,
           handle "1:many" by staying in this inner loop.  otherwise,
           go back to the outer loop. */

        if (peek_keys(peek_buffer_right, buffer_right) != 0) {
          /* need a new line from RIGHT */
          my_getline(&buffer_right, &buffer_right_size, &peek_buffer_right,
                     &peek_buffer_right_size, right, &eof_right);
          goto left_file_loop;
        }
      }
    } /* feof( right ) */
  } /* feof( left ) */

cleanup:
  if (buffer_left)
    free(buffer_left);
  if (buffer_right)
    free(buffer_right);
  if (peek_buffer_left)
    free(peek_buffer_left);
  if (peek_buffer_right)
    free(peek_buffer_right);
  if (left_keyfields)
    free(left_keyfields);
  if (right_keyfields)
    free(right_keyfields);
  if (left_mergefields)
    free(left_mergefields);
  if (right_mergefields)
    free(right_mergefields);

  return retval;
}


/* wrapper for get line with peek */
int my_getline(char **buffer, size_t *size, char **peek_buffer,
               size_t *peek_size, FILE *in, int *eof_flag) {

  /* if both buffers are empty, we are at the very beginning */
  if (*peek_buffer == NULL && *buffer == NULL) {

    /* first get the actual line to work with */
    if (getline(buffer, size, in) <= 0) {
      free(*buffer);
      *buffer = NULL;
      *eof_flag = 1;
      return 0;
    }
    chomp(*buffer);

    /* then get the peek line */
    if (getline(peek_buffer, peek_size, in) <= 0) {
      free(*peek_buffer);
      *peek_buffer = NULL;
    } else {
      chomp(*peek_buffer);
    }

    return 1;
  }
  /* if the peek buffer is not empty, we are right in the middle */
  else if (*peek_buffer != NULL) {

    /* make sure we have enough memory for the copy */
    if (*size < *peek_size) {
      char *tmp;
      tmp = (char *) realloc(*buffer, *peek_size);
      if (tmp == NULL) {
        warn("error reallocating the current-line buffer.");
        exit(2);
      }
      *buffer = tmp;
      *size = *peek_size;
    }

    /* first copy the peek line into the actual line buffer */
    memcpy(*buffer, *peek_buffer, *peek_size);

    /* then read a new peek line */
    if (getline(peek_buffer, peek_size, in) <= 0) {
      free(*peek_buffer);
      *peek_buffer = NULL;
    } else {
      chomp(*peek_buffer);
    }

    return 1;
  }
  /* here we are at the end of the file */
  else {
    if (*buffer) {
      free(*buffer);
      *buffer = NULL;
    }
    *eof_flag = 1;
    return 0;
  }
}


/* extract each element of fields from line and print them, separated by delim.
   the delimiter will not be printed after the last field. */
static void extract_and_print_fields(char *line, int *field_list,
                                     size_t nfields, char *delim, FILE *out) {
  int i;
  char field_buffer[MAX_FIELD_LEN + 1];
  for (i = 0; i < nfields - 1; i++) {
    field_buffer[0] = '\0';
    get_line_field(field_buffer, line, MAX_FIELD_LEN, field_list[i], delim);
    fputs(field_buffer, out);
    fputs(delim, out);
  }
  get_line_field(field_buffer, line, MAX_FIELD_LEN, field_list[i], delim);
  fputs(field_buffer, out);
}


/* merge and print two lines. */
void join_lines(char *left_line, char *right_line, char *merge_default,
                FILE * out) {

  int i;
  char field_right[MAX_FIELD_LEN + 1];

  if (left_line == NULL && right_line == NULL)
    return;

  if (right_line == NULL) {
    /* just print LEFT line with empty RIGHT merge fields */
    extract_and_print_fields(left_line, left_keyfields, nkeys, delim, out);
    if (left_ntomerge > 0)
      fputs(delim, out);
    extract_and_print_fields(left_line, left_mergefields, left_ntomerge,
                             delim, out);
    for (i = 0; i < right_ntomerge; i++) {
      fprintf(out, "%s%s", delim, merge_default);
    }
  } else if (left_line == NULL) {
    /* print fields from RIGHT with empty fields from LEFT */
    extract_and_print_fields(right_line, right_keyfields, nkeys, delim, out);
    for (i = 0; i < left_ntomerge; i++)
      fprintf(out, "%s%s", delim, merge_default);
    if (right_ntomerge > 0)
      fputs(delim, out);
    extract_and_print_fields(right_line, right_mergefields, right_ntomerge,
                             delim, out);
  } else {
    /* keys are equal. */
    extract_and_print_fields(left_line, left_keyfields, nkeys, delim, out);
    if (left_ntomerge > 0)
      fputs(delim, out);
    extract_and_print_fields(left_line, left_mergefields, left_ntomerge,
                             delim, out);
    if (right_ntomerge > 0)
      fputs(delim, out);
    extract_and_print_fields(right_line, right_mergefields, right_ntomerge,
                             delim, out);
  }

  fputc('\n', out);
}


static int Expand_nums(const char *str, int **arr,
                       size_t *sz, const char *desc) {
  ssize_t retval = expand_nums(str, arr, sz);
  switch (retval) {
    case -1: fprintf(stderr, "%s: out of memory\n", getenv("_"));
             break;
    case  0:
    case -2: fprintf(stderr, "%s: bad %s key string: \"%s\"\n",
                     getenv("_"), desc, str);
             break;
  }
  return retval;
}


static void decrement_each(int *array, size_t n) {
  int i;
  for (i = 0; i < n; i++) {
    array[i]--;
  }
}


int set_field_types(const char *left_keys, const char *right_keys) {
  ssize_t nkeys_left, nkeys_right;
  int i;

  nkeys_left = Expand_nums(left_keys, &left_keyfields,
                           &nfields_left, "left");
  if (nkeys_left < 1)
    return -1;

  nkeys_right = Expand_nums(right_keys, &right_keyfields,
                            &nfields_right, "right");
  if (nkeys_right < 1)
    return -1;

  if (nkeys_left != nkeys_right) {
    fprintf(stderr,
            "%s: left and right files must have the same number of keys.\n",
            getenv("_"));
    return -1;
  }

  decrement_each(left_keyfields, nkeys_left);
  decrement_each(right_keyfields, nkeys_right);

  nkeys = nkeys_left;
  left_ntomerge = right_ntomerge = 0;

  for (i = 0; i < nfields_left; i++) {
    int j, found = 0;
    for (j = 0; j < nkeys_left; j++) {
      if (left_keyfields[j] == i) {
        found = 1;
        break;
      }
    }
    if (! found)
      left_mergefields[left_ntomerge++] = i;
  }

  for (i = 0; i < nfields_right; i++) {
    int j, found = 0;
    for (j = 0; j < nkeys_right; j++) {
      if (right_keyfields[j] == i) {
        found = 1;
        break;
      }
    }
    if (! found)
      right_mergefields[right_ntomerge++] = i;
  }

  return 0;
}


/* identifies fields as keys or to-be-merged */
void classify_fields(char *left_header, char *right_header) {

  int i, j;

  char label_left[MAX_FIELD_LEN + 1], label_right[MAX_FIELD_LEN + 1];

  nkeys = left_ntomerge = right_ntomerge = 0;

  j = (nfields_left < nfields_right ? nfields_left : nfields_right);

  /* find the keys common to both files & the ones that need merged */
  for (i = 0; i < j; i++) {

    get_line_field(label_left, left_header, MAX_FIELD_LEN, i, delim);
    get_line_field(label_right, right_header, MAX_FIELD_LEN, i, delim);

    if (str_eq(label_left, label_right)) {
      left_keyfields[nkeys] = i;
      right_keyfields[nkeys] = i;
      nkeys++;
    } else {
      left_mergefields[left_ntomerge] = i;
      right_mergefields[right_ntomerge] = i;
      left_ntomerge++;
      right_ntomerge++;
    }
  }

  j = i;
  /* if LEFT had more fields than RIGHT, all of those need merged */
  for (; i < nfields_left; i++) {
    left_mergefields[left_ntomerge] = i;
    left_ntomerge++;
  }

  i = j;
  /* if RIGHT had more fields than LEFT, all of those need merged */
  for (; i < nfields_right; i++) {
    right_mergefields[right_ntomerge] = i;
    right_ntomerge++;
  }
}


int compare_keys(char *buffer_left, char *buffer_right) {
  int keycmp = 0;
  int i;
  char field_left[MAX_FIELD_LEN + 1];
  char field_right[MAX_FIELD_LEN + 1];

  if (buffer_left == NULL && buffer_right == NULL)
    return LEFT_RIGHT_EQUAL;

  /* these special cases may seem counter-intuitive, but saying that
     a NULL line is greater than a non-NULL line results in
     the non-NULL line getting printed and a new line read in.
   */
  if (buffer_left == NULL)
    return LEFT_GREATER;

  if (buffer_right == NULL)
    return RIGHT_GREATER;

  for (i = 0; i < nkeys; i++) {
    get_line_field(field_left, buffer_left, MAX_FIELD_LEN,
                   left_keyfields[i], delim);
    get_line_field(field_right, buffer_right, MAX_FIELD_LEN,
                   right_keyfields[i], delim);
    if ((keycmp = strcoll(field_left, field_right)) != 0)
      break;
  }
  return keycmp;
}


/* compares kees of the current and the next line */
int peek_keys(char *peek_line, char *current_line) {
  int result = 1;

  if (peek_line != NULL) {
    result = compare_keys(current_line, peek_line);
  }

  return result;
}
