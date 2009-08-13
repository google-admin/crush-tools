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
  FILE *out; /* the output file ptrs */
  dbfr_t *left_reader, *right_reader;
  int fd_tmp, retval; /* file descriptor and return value */

  enum join_type_t join_type;

  if ((args->left_keys || args->left_key_labels) &&
      ! (args->right_keys || args->right_key_labels) ||
      ! (args->left_keys || args->left_key_labels) &&
      (args->right_keys || args->right_key_labels)) {
    fprintf(stderr, "%s: if -a/-A or -b/-B is specified, the other must be also.\n",
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

  left_reader = dbfr_open(argv[optind]);
  if (! left_reader) {
    perror(argv[optind]);
    return EXIT_FILE_ERR;
  }

  right_reader = dbfr_open(argv[optind + 1]);
  if (! right_reader) {
    perror(argv[optind + 1]);
    return EXIT_FILE_ERR;
  }

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

  retval = merge_files(left_reader, right_reader, join_type, out, args);

  dbfr_close(left_reader);
  dbfr_close(right_reader);
  fclose(out);

  return retval;
}


int merge_files(dbfr_t *left, dbfr_t *right, enum join_type_t join_type,
                FILE * out, struct cmdargs *args) {

  int retval = EXIT_OKAY;

  /* buffer for holding field values */
  char field_right[MAX_FIELD_LEN + 1];

  /* general-purpose counter */
  int i;

  int keycmp = 0;

  if (dbfr_getline(left) <= 0) {
    fprintf(stderr, "%s: no header found in left-hand file\n", getenv("_"));
    exit(EXIT_FAILURE);
  }
  if (dbfr_getline(right) <= 0) {
    fprintf(stderr, "%s: no header found in right-hand file\n", getenv("_"));
    exit(EXIT_FAILURE);
  }

  chomp(left->current_line);
  chomp(right->current_line);

  nfields_left = fields_in_line(left->current_line, delim);
  nfields_right = fields_in_line(right->current_line, delim);

  if (args->verbose) {
    fprintf(stderr,
            "VERBOSE: fields in left: %s\nVERBOSE: fields in right: %s\n",
            left->current_line, right->current_line);
  }

  left_keyfields = xmalloc(sizeof(int) * nfields_left);
  right_keyfields = xmalloc(sizeof(int) * nfields_right);
  left_mergefields = xmalloc(sizeof(int) * nfields_left);
  right_mergefields = xmalloc(sizeof(int) * nfields_right);

  if ((args->left_keys || args->left_key_labels) &&
      (args->right_keys ||args->right_key_labels)) {
    int has_error = set_key_lists(args, left->current_line,
                                  right->current_line, delim);
    if (has_error) {
      exit(EXIT_FAILURE);
    }
  } else {
    /* use headers to figure out which fields are keys or need to be merged */
    classify_fields(left->current_line, right->current_line);
  }

  if (nkeys == 0) {
    fprintf(stderr, "%s: no common fields found\n", getenv("_"));
    exit(EXIT_FAILURE);
  }

  if (args->verbose) {
    fprintf(stderr, "VERBOSE: # key fields:       %d\n", nkeys);
    fprintf(stderr, "VERBOSE: left merge fields:  %d\n", left_ntomerge);
    fprintf(stderr, "VERBOSE: right merge fields: %d\n", right_ntomerge);
  }

  /* print the headers which were already read in above */
  extract_and_print_fields(left->current_line, left_keyfields, nkeys,
                           delim, out);
  if (left_ntomerge > 0) {
    fputs(delim, out);
    extract_and_print_fields(left->current_line, left_mergefields,
                             left_ntomerge, delim, out);
  }
  if (right_ntomerge > 0) {
    fputs(delim, out);
    extract_and_print_fields(right->current_line, right_mergefields,
                             right_ntomerge, delim, out);
  }
  fputc('\n', out);

  /* force a line-read from LEFT the first time around.
     if eof is reached here, we still need to process
     the left-hand file.
   */
  keycmp = LEFT_RIGHT_EQUAL;
  
  if (dbfr_getline(right) <= 0) {
    free(right->current_line);
    right->current_line = NULL;
  }

left_file_loop:

  while (!left->eof) {
    int left_line_printed = 0;

    if (LEFT_LE_RIGHT(keycmp)) {
      if (dbfr_getline(left) <= 0) {
        if (join_type == join_type_inner || join_type == join_type_left_outer)
          break;
        free(left->current_line);
        left->current_line = NULL;
        keycmp = compare_keys(left->current_line, right->current_line);
        goto right_file_loop;
      }
    }

    keycmp = compare_keys(left->current_line, right->current_line);

    if (LEFT_LT_RIGHT(keycmp)) {
      if (join_type == join_type_outer || join_type == join_type_left_outer)
        join_lines(left->current_line, NULL, args->merge_default, out);
      goto left_file_loop;
    }

    if (LEFT_EQ_RIGHT(keycmp)) {
      /* everybody likes an inner join */
      join_lines(left->current_line, right->current_line,
                 args->merge_default, out);

      if (peek_keys(left->next_line, left->current_line, left_keyfields) == 0) {
        /* the keys in the next line of LEFT are the same.
           handle "many:1"
         */
        goto left_file_loop;
      }

      left_line_printed = 1;
    }

  right_file_loop:

    while (!right->eof) {
      if (LEFT_GT_RIGHT(keycmp)) {
        if (join_type == join_type_outer || join_type == join_type_right_outer)
          join_lines(NULL, right->current_line, args->merge_default, out);
      }

      if(dbfr_getline(right) <= 0) {
        free(right->current_line);
        right->current_line = NULL;
      }
      keycmp = compare_keys(left->current_line, right->current_line);

      if (LEFT_LT_RIGHT(keycmp)) {

        if ((!left_line_printed) &&
            (join_type == join_type_outer
             || join_type == join_type_left_outer)) {

          join_lines(left->current_line, NULL, args->merge_default, out);
        }

        goto left_file_loop;
      }

      if (LEFT_EQ_RIGHT(keycmp)) {
        int peek_cmp;
        join_lines(left->current_line, right->current_line,
                   args->merge_default, out);
        left_line_printed = 1;

        /* if the keys in the next line of LEFT are the same,
           handle "many:1". */
        peek_cmp = peek_keys(left->next_line, left->current_line,
                             left_keyfields);
        if ((args->inner && peek_cmp <= 0) || peek_cmp == 0) {
          goto left_file_loop;
        }

        /* if the keys in the next line of RIGHT are the same,
           handle "1:many" by staying in this inner loop.  otherwise,
           go back to the outer loop. */

        if (peek_keys(right->next_line, right->current_line, right_keyfields)
            != 0) {
          /* need a new line from RIGHT */
          if (dbfr_getline(right) <= 0) {
            free(right->current_line);
            right->current_line = NULL;
          }
          goto left_file_loop;
        }
      }
    } /* feof( right ) */
  } /* feof( left ) */

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


/* extract each element of fields from line and print them, separated by delim.
   the delimiter will not be printed after the last field. */
static void extract_and_print_fields(char *line, int *field_list,
                                     size_t nfields, char *delim, FILE *out) {
  int i;
  char field_buffer[MAX_FIELD_LEN + 1];
  if (nfields == 0)
    return;
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

int set_key_lists(struct cmdargs *args, const char *left_line,
                  const char *right_line, const char *delim) {
  ssize_t nkeys_left, nkeys_right;
  size_t key_array_sz = 0;

  if (args->left_key_labels) {
    nkeys_left = expand_label_list(args->left_key_labels, left_line,
                                   delim, &left_keyfields, &nfields_left);
  } else {
    nkeys_left = expand_nums(args->left_keys, &left_keyfields, &nfields_left);
  }
  if (nkeys_left <= 0) {
    fprintf(stderr, "%s: error parsing left keys\n", getenv("_"));
    return -1;
  }

  if (args->right_key_labels) {
    nkeys_right = expand_label_list(args->right_key_labels, right_line,
                                   delim, &right_keyfields, &nfields_right);
  } else {
    nkeys_right = expand_nums(args->right_keys, &right_keyfields,
                              &nfields_right);
  }
  if (nkeys_right <= 0) {
    fprintf(stderr, "%s: error parsing right keys\n", getenv("_"));
    return -1;
  }

  if (nkeys_left != nkeys_right) {
    fprintf(stderr,
            "%s: left and right files must have the same number of keys.\n",
            getenv("_"));
    return -1;
  }

  decrement_each(left_keyfields, nkeys_left);
  decrement_each(right_keyfields, nkeys_right);
  nkeys = nkeys_left;
  set_field_types();
  return 0;
}

int set_field_types() {
  int i;
  left_ntomerge = right_ntomerge = 0;

  for (i = 0; i < nfields_left; i++) {
    int j, found = 0;
    for (j = 0; j < nkeys; j++) {
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
    for (j = 0; j < nkeys; j++) {
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


/* compares keys of the current and the next line.  Basically the same
 * as compare_keys(), but using the same keyfield list for both lines. */
int peek_keys(char *peek_line, char *current_line, const int *keyfields) {
  int keycmp = 0;
  int i;
  char field_cur[MAX_FIELD_LEN + 1];
  char field_next[MAX_FIELD_LEN + 1];

  /* no next line, so current line's fields are greater. */
  if (peek_line == NULL)
    return 1;

  for (i = 0; i < nkeys; i++) {
    get_line_field(field_cur, current_line, MAX_FIELD_LEN,
                   keyfields[i], delim);
    get_line_field(field_next, peek_line, MAX_FIELD_LEN,
                   keyfields[i], delim);
    if ((keycmp = strcoll(field_cur, field_next)) != 0)
      break;
  }
  return keycmp;
}
