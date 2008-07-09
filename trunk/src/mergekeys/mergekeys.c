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

int *keyfields = NULL;          /* array of fields common to both files */
size_t nkeys;

int *mergefields = NULL;        /* fields only in RIGHT file - need to be added to those in A */
size_t ntomerge;

size_t nkeys, ntomerge;


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

  if (argc - optind != 2) {
    fprintf(stderr,
            "missing file arguments.  see %s -h for usage information.\n",
            argv[0]);
    return EXIT_HELP;
  } else if (str_eq(argv[optind], argv[optind + 1])) {
    /* TODO: it would be safer to convert these to absolute
       paths first.  "mergekeys file ./file" would still
       go through.
     */
    fprintf(stderr,
            "both input files are the same. see %s -h for usage information.\n",
            argv[0]);
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
    fprintf(stderr, "no header found in left-hand file\n");
    retval = EXIT_FILE_ERR;
    goto cleanup;
  }
  if (getline(&buffer_right, &buffer_right_size, right) <= 0) {
    fprintf(stderr, "no header found in right-hand file\n");
    retval = EXIT_FILE_ERR;
    goto cleanup;
  }

  chomp(buffer_left);
  chomp(buffer_right);

  nfields_left = fields_in_line(buffer_left, delim);
  nfields_right = fields_in_line(buffer_right, delim);

  if (args->verbose)
    printf("fields in left: %s\nfields in right: %s\n", buffer_left,
           buffer_right);

  if ((keyfields = malloc(sizeof(int) * nfields_left)) == NULL) {
    warn("allocating key array");
    retval = EXIT_MEM_ERR;
    goto cleanup;
  }
  if ((mergefields = malloc(sizeof(int) * nfields_right)) == NULL) {
    warn("allocating merge-fields array");
    retval = EXIT_MEM_ERR;
    goto cleanup;
  }

  /* figure out which fields are keys or need to be merged */
  classify_fields(buffer_left, buffer_right);

  if (nkeys == 0) {
    fprintf(stderr, "%s: no common fields found\n", getenv("_"));
    retval = EXIT_HELP;
    goto cleanup;
  }

  /* print the headers which were already read in above */
  fputs(buffer_left, out);
  for (i = 0; i < ntomerge; i++) {
    get_line_field(field_right, buffer_right, MAX_FIELD_LEN, mergefields[i],
                   delim);
    fprintf(out, "%s%s", delim, field_right);
  }
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
      if (my_getline
          (&buffer_left, &buffer_left_size, &peek_buffer_left,
           &peek_buffer_left_size, left, &eof_left) <= 0) {
        keycmp = compare_keys(buffer_left, buffer_right);
        goto right_file_loop;
      }
    }

    keycmp = compare_keys(buffer_left, buffer_right);

    if (LEFT_LT_RIGHT(keycmp)) {
      if (join_type == join_type_outer || join_type == join_type_left_outer)
        join_lines(buffer_left, NULL, out);
      goto left_file_loop;
    }

    if (LEFT_EQ_RIGHT(keycmp)) {
      /* everybody likes an inner join */
      join_lines(buffer_left, buffer_right, out);

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
          join_lines(NULL, buffer_right, out);
      }

      my_getline(&buffer_right, &buffer_right_size, &peek_buffer_right,
                 &peek_buffer_right_size, right, &eof_right);
      keycmp = compare_keys(buffer_left, buffer_right);

      if (LEFT_LT_RIGHT(keycmp)) {

        if ((!left_line_printed) &&
            (join_type == join_type_outer
             || join_type == join_type_left_outer)) {

          join_lines(buffer_left, NULL, out);
        }

        goto left_file_loop;
      }

      if (LEFT_EQ_RIGHT(keycmp)) {
        join_lines(buffer_left, buffer_right, out);
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

    }                           /* feof( right ) */

  }                             /* feof( left ) */

cleanup:
  if (buffer_left)
    free(buffer_left);
  if (buffer_right)
    free(buffer_right);
  if (peek_buffer_left)
    free(peek_buffer_left);
  if (peek_buffer_right)
    free(peek_buffer_right);
  if (keyfields)
    free(keyfields);
  if (mergefields)
    free(mergefields);

  return retval;
}


/* wrapper for get line with peek */
int my_getline(char **buffer, size_t * size, char **peek_buffer,
               size_t * peek_size, FILE * in, int *eof_flag) {

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
        warn
          ("not able to reallocate new memory to copy the peek buffer into the current buffer.");
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


/* merge and print two lines. */
void join_lines(char *left_line, char *right_line, FILE * out) {

  int i, j;
  char field_right[MAX_FIELD_LEN + 1];

  if (left_line == NULL && right_line == NULL)
    return;

  if (right_line == NULL) {
    /* just print LEFT line with empty merge fields */
    fprintf(out, "%s", left_line);
    for (i = 0; i < ntomerge; i++)
      fputs(delim, out);
  } else if (left_line == NULL) {
    /* just print fields from RIGHT with empty fields from LEFT */

    /* first the keys & empty LEFT-only fields */
    j = 0;
    for (i = 0; i < nfields_left; i++) {
      if (keyfields[j] == i) {
        get_line_field(field_right, right_line, MAX_FIELD_LEN, keyfields[j],
                       delim);
        fprintf(out, "%s%s", (j > 0 ? delim : ""), field_right);
        j++;
      } else {
        fputs(delim, out);
      }
    }

    /* now the non-keys */
    for (i = 0; i < ntomerge; i++) {
      get_line_field(field_right, right_line, MAX_FIELD_LEN, mergefields[i],
                     delim);
      fprintf(out, "%s%s", delim, field_right);
    }
  } else {
    /* presumably keys are equal. */
    fprintf(out, "%s", left_line);
    for (i = 0; i < ntomerge; i++) {
      get_line_field(field_right, right_line, MAX_FIELD_LEN, mergefields[i],
                     delim);
      fprintf(out, "%s%s", delim, field_right);
    }
  }

  fputc('\n', out);
}


/* identifies fields as keys or to-be-merged */
void classify_fields(char *left_header, char *right_header) {

  int i, j;

  char label_left[MAX_FIELD_LEN + 1], label_right[MAX_FIELD_LEN + 1];

  nkeys = ntomerge = 0;

  j = (nfields_left < nfields_right ? nfields_left : nfields_right);

  /* find the keys common to both files & the ones that need merged */
  for (i = 0; i < j; i++) {

    get_line_field(label_left, left_header, MAX_FIELD_LEN, i, delim);
    get_line_field(label_right, right_header, MAX_FIELD_LEN, i, delim);

    if (str_eq(label_left, label_right))
      keyfields[nkeys++] = i;
    else
      mergefields[ntomerge++] = i;
  }

  /* if RIGHT had more fields than LEFT, all of those need merged */
  for (; i < nfields_right; i++) {
    mergefields[ntomerge++] = i;
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
    get_line_field(field_left, buffer_left, MAX_FIELD_LEN, keyfields[i], delim);
    get_line_field(field_right, buffer_right, MAX_FIELD_LEN, keyfields[i],
                   delim);
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
