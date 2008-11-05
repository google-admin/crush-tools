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
#include "aggregate2_main.h"
#include <ffutils.h>
#include <dbfr.h>
#include <err.h>                /* warn() */


static void print_line(FILE * out,
                       const char *keys,
                       const char *delim,
                       const int *counts,
                       size_t ncounts,
                       const double *sums, int nsums, int *sums_precision);

static int extract_keys(char *target, const char *source, const char *delim,
                        int *keys, size_t nkeys, const char *suffix);

static int float_precision(char *n);

/** @brief  
  * 
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int aggregate2(struct cmdargs *args, int argc, char *argv[], int optind) {

  char default_delim[] = { 0xfe, 0x00 };

  FILE *in, *out;
  dbfr_t *in_reader;

  int *keys = NULL, *sums = NULL, *counts = NULL,
      *sums_precision = NULL; /* output the same precision as in the input */
  int cur_precision;
  size_t keys_sz = 0, sums_sz = 0, counts_sz = 0;

  ssize_t nkeys = 0, nsums = 0, ncounts = 0;

  char *cur_keys = NULL, *prev_keys = NULL;
  int prev_keys_initialized = 0;
  size_t keybuf_sz = 0;

  int *cur_counts = NULL;
  double *cur_sums = NULL;

  char field_buf[64];
  double f;                     /* numeric value of sum fields */
  int i;                        /* counter */

  if (! (args->keys || args->key_labels)) {
    fprintf(stderr, "%s: either -k or -K must be specified.\n", argv[0]);
    return EXIT_HELP;
  }

  /* individually these args are not required. */
  if (!(args->sums || args->sum_labels) &&
      !(args->counts || args->count_labels)) {
    fprintf(stderr,
            "%s: at least one of -s/-S and -c/-C must be specified.\n",
            argv[0]);
    return EXIT_HELP;
  }

  if (!args->delim) {
    if ((args->delim = getenv("DELIMITER")) == NULL)
      args->delim = default_delim;
  }
  expand_chars(args->delim);

  if (optind < argc) {
    in = nextfile(argc, argv, &optind, "r");
    if (! in)
      return EXIT_FILE_ERR;
  } else {
    in = stdin;
  }
  in_reader = dbfr_init(in);

  if (args->keys) {
    nkeys = expand_nums(args->keys, &keys, &keys_sz);
  } else {
    nkeys = expand_label_list(args->key_labels, in_reader->next_line,
                              args->delim, &keys, &keys_sz);
    args->preserve_header = 1;
  }
  if (nkeys <= 0) {
    fprintf(stderr, "%s: bad key fields specified\n", argv[0]);
    return EXIT_HELP;
  }
  for (i = 0; i < nkeys; i++)
    keys[i]--;

  if (args->sums) {
    nsums = expand_nums(args->sums, &sums, &sums_sz);
  } else if (args->sum_labels) {
    nsums = expand_label_list(args->sum_labels, in_reader->next_line,
                              args->delim, &sums, &sums_sz);
    args->preserve_header = 1;
  }
  if (nsums < 0) {
    fprintf(stderr, "%s: bad sum fields specified\n", argv[0]);
    return EXIT_HELP;
  } else if (nsums > 0) {
    for (i = 0; i < nsums; i++)
      sums[i]--;
    sums_precision = calloc(nsums, sizeof(int));
  }

  if (args->counts) {
    ncounts = expand_nums(args->counts, &counts, &counts_sz);
  } else if (args->count_labels) {
    ncounts = expand_label_list(args->count_labels, in_reader->next_line,
                                args->delim, &counts, &counts_sz);
    args->preserve_header = 1;
  }
  if (ncounts < 0) {
    fprintf(stderr, "%s: bad count fields specified\n", argv[0]);
    return EXIT_HELP;
  } else if (ncounts > 0) {
    for (i = 0; i < ncounts; i++)
      counts[i]--;
  }

  if (args->outfile) {
    out = fopen(args->outfile, "w");
    if (!out) {
      warn(args->outfile);
      return EXIT_FILE_ERR;
    }
  } else {
    out = stdout;
  }

  if (ncounts > 0)
    cur_counts = calloc(ncounts, sizeof(int));

  if (nsums > 0)
    cur_sums = calloc(nsums, sizeof(double));

  /* this can be resized later */
  cur_keys = malloc(sizeof(char) * 1024);
  prev_keys = malloc(sizeof(char) * 1024);
  keybuf_sz = 1024;

  if (args->labels || args->auto_label)
    args->preserve_header = 1;

  if (args->preserve_header) {
    if (dbfr_getline(in_reader) <= 0) {
      DIE("unexpected end of file");
    }

    chomp(in_reader->current_line);
    if (extract_keys(cur_keys, in_reader->current_line, args->delim,
                     keys, nkeys, NULL) != 0) {
      fprintf(stderr, "%s: malformatted input\n", argv[0]);
      return EXIT_FILE_ERR;
    }
    fputs(cur_keys, out);

    if (args->labels) {
      fprintf(out, "%s%s", args->delim, args->labels);
    } else {
      if (nsums > 0) {
        if (extract_keys(cur_keys, in_reader->current_line, args->delim,
                         sums, nsums, args->auto_label ? "-Sum" : NULL) != 0) {
          fprintf(stderr, "%s: malformatted input\n", argv[0]);
          return EXIT_FILE_ERR;
        }
        fprintf(out, "%s%s", args->delim, cur_keys);
      }

      if (ncounts > 0) {
        if (extract_keys(cur_keys, in_reader->current_line, args->delim,
                         counts, ncounts, args->auto_label ? "-Count" : NULL)
            != 0) {
          fprintf(stderr, "%s: malformatted input\n", argv[0]);
          return EXIT_FILE_ERR;
        }
        fprintf(out, "%s%s", args->delim, cur_keys);
      }
    }
    fputs("\n", out);
  }

  cur_keys[0] = '\0';
  prev_keys[0] = '\0';

  while (in) {
    while (dbfr_getline(in_reader) > 0) {
      chomp(in_reader->current_line);
      if (in_reader->current_line_sz > keybuf_sz) {
        char *tmp;
        tmp = realloc(cur_keys, in_reader->current_line_sz);
        if (!tmp) {
          fprintf(stderr,
                  "%s: out of memory (resizing cur_keys from %d to %d)\n",
                  argv[0], keybuf_sz, in_reader->current_line_sz);
          return EXIT_MEM_ERR;
        }
        cur_keys = tmp;

        tmp = realloc(prev_keys, in_reader->current_line_sz);
        if (!tmp) {
          fprintf(stderr, "%s: out of memory (resizing prev_keys)\n", argv[0]);
          return EXIT_MEM_ERR;
        }
        prev_keys = tmp;
        keybuf_sz = in_reader->current_line_sz;
      }

      if (extract_keys(cur_keys, in_reader->current_line, args->delim,
                       keys, nkeys, NULL) != 0) {
        fprintf(stderr, "%s: malformatted input\n", argv[0]);
        return EXIT_FILE_ERR;
      }

      if (prev_keys_initialized && !str_eq(cur_keys, prev_keys)) {
        print_line(out, prev_keys, args->delim, cur_counts, ncounts, cur_sums,
                   nsums, sums_precision);

        memset(cur_counts, 0, ncounts * sizeof(int));
        memset(cur_sums, 0, nsums * sizeof(double));
      }

      for (i = 0; i < ncounts; i++) {
        get_line_field(field_buf, in_reader->current_line, 63, counts[i],
                       args->delim);
        if (field_buf[0] != '\0') {
          cur_counts[i]++;
        }
      }

      for (i = 0; i < nsums; i++) {
        get_line_field(field_buf, in_reader->current_line, 63, sums[i],
                       args->delim);
        f = atof(field_buf);
        cur_sums[i] += f;

        cur_precision = float_precision(field_buf);

        if (cur_precision > sums_precision[i])
          sums_precision[i] = cur_precision;
      }

      strcpy(prev_keys, cur_keys);
      prev_keys_initialized = 1;
    }
    dbfr_close(in_reader);
    in = nextfile(argc, argv, &optind, "r");
    if (in) {
      in_reader = dbfr_init(in);
    }
  }

  print_line(out, prev_keys, args->delim, cur_counts, ncounts, cur_sums, nsums,
             sums_precision);

  free(keys);
  free(counts);
  free(sums);
  free(sums_precision);
  free(cur_counts);
  free(cur_sums);
  free(cur_keys);
  free(prev_keys);

  return EXIT_OKAY;
}


/* assumption: target is at least as big as source, so it cannot be
   overflowed.
   TODO(jhinds): this is faulty if the same field is specified multiple times;
   and when auto_labeling, the suffix might cause an overflow.
 */
static int extract_keys(char *target, const char *source, const char *delim,
                        int *keys, size_t nkeys, const char *suffix) {
  int i, s, e; /* iter, start, end */
  int field_len;

  target[0] = '\0';
  for (i = 0; i < nkeys; i++) {
  	field_len = get_line_pos(source, keys[i], delim, &s, &e);
    if (field_len < 0)
      return 1;
    if (field_len > 0)
      strncat(target, source + s, e - s + 1);
    if (suffix)
      strcat(target, suffix);
    if (i != nkeys - 1)
      strcat(target, delim);
  }
  return 0;
}



static void print_line(FILE * out,
                       const char *keys,
                       const char *delim,
                       const int *counts,
                       size_t ncounts,
                       const double *sums, int nsums, int *sums_precision) {
  int i;
  fputs(keys, out);

  for (i = 0; i < nsums; i++) {
    fprintf(out, "%s%.*f", delim, sums_precision[i], sums[i]);
  }

  for (i = 0; i < ncounts; i++) {
    fprintf(out, "%s%d", delim, counts[i]);
  }

  fputs("\n", out);
}


static int float_precision(char *n) {
  char *dot;
  if (n == NULL || n[0] == '\0')
    return 0;
  dot = strchr(n, '.');
  if (dot == NULL)
    return 0;

  return strlen(dot) - 1;
}
