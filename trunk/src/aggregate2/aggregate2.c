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
#include <err.h>                /* warn() */


static void print_line(FILE * out,
                       const char *keys,
                       const char *delim,
                       const int *counts,
                       size_t ncounts,
                       const double *sums, int nsums, int *sums_precision);

static int extract_keys(char *target,
                        const char *source,
                        const char *delim, int *keys, size_t nkeys);


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
  char *input_line = NULL;
  size_t input_line_sz = 0;

  int *keys = NULL, *sums = NULL, *counts = NULL, *sums_precision = NULL; /* output the same precision as
                                                                             in the input */
  int cur_precision;
  size_t keys_sz = 0, sums_sz = 0, counts_sz = 0;

  ssize_t nkeys = 0, nsums = 0, ncounts = 0;

  char *cur_keys = NULL, *prev_keys = NULL;
  size_t keybuf_sz = 0;

  int *cur_counts = NULL;
  double *cur_sums = NULL;

  char field_buf[64];
  double f;                     /* numeric value of sum fields */
  int i;                        /* counter */

  if (!args->delim) {
    if ((args->delim = getenv("DELIMITER")) == NULL)
      args->delim = default_delim;
  }
  expand_chars(args->delim);

  /* individually these args are not required. */
  if (!args->sums && !args->counts) {
    fprintf(stderr,
            "%s: at least one of --sums and --counts must be specified.\n",
            argv[0]);
    return EXIT_HELP;
  }

  nkeys = expand_nums(args->keys, &keys, &keys_sz);
  if (nkeys <= 0) {
    fprintf(stderr, "%s: bad key fields specified\n", argv[0]);
    return EXIT_HELP;
  }
  for (i = 0; i < nkeys; i++)
    keys[i]--;

  if (args->sums) {
    nsums = expand_nums(args->sums, &sums, &sums_sz);
    if (nsums < 0) {
      fprintf(stderr, "%s: bad sum fields specified\n", argv[0]);
      return EXIT_HELP;
    }
    for (i = 0; i < nsums; i++)
      sums[i]--;

    sums_precision = calloc(nsums, sizeof(int));
  }

  if (args->counts) {
    ncounts = expand_nums(args->counts, &counts, &counts_sz);
    if (ncounts < 0) {
      fprintf(stderr, "%s: bad count fields specified\n", argv[0]);
      return EXIT_HELP;
    }
    for (i = 0; i < ncounts; i++)
      counts[i]--;
  }

  if (optind < argc) {
    in = nextfile(argc, argv, &optind, "r");
  } else {
    in = stdin;
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


  if (args->preserve_header) {
    if (getline(&input_line, &input_line_sz, in) > 0) {
      chomp(input_line);
      if (extract_keys(cur_keys, input_line, args->delim, keys, nkeys) != 0) {
        fprintf(stderr, "%s: malformatted input\n", argv[0]);
        return EXIT_FILE_ERR;
      }
      fputs(cur_keys, out);

      if (nsums > 0) {
        if (extract_keys(cur_keys, input_line, args->delim, sums, nsums) != 0) {
          fprintf(stderr, "%s: malformatted input\n", argv[0]);
          return EXIT_FILE_ERR;
        }

        if (ncounts == 0) {
          /* remove trailing delimiter */
          cur_keys[strlen(cur_keys) - strlen(args->delim)] = '\0';
        }
        fputs(cur_keys, out);
      }

      if (ncounts > 0) {
        if (extract_keys(cur_keys, input_line, args->delim, counts, ncounts) !=
            0) {
          fprintf(stderr, "%s: malformatted input\n", argv[0]);
          return EXIT_FILE_ERR;
        }
        /* remove trailing delimiter */
        cur_keys[strlen(cur_keys) - strlen(args->delim)] = '\0';
        fputs(cur_keys, out);
      }

      fputs("\n", out);
    }
  }

  cur_keys[0] = '\0';
  prev_keys[0] = '\0';

  while (in) {
    while (getline(&input_line, &input_line_sz, in) > 0) {
      chomp(input_line);
      if (input_line_sz > keybuf_sz) {
        char *tmp;
        tmp = realloc(cur_keys, input_line_sz);
        if (!tmp) {
          fprintf(stderr,
                  "%s: out of memory (resizing cur_keys from %d to %d)\n",
                  argv[0], keybuf_sz, input_line_sz);
          return EXIT_MEM_ERR;
        }
        cur_keys = tmp;

        tmp = realloc(prev_keys, input_line_sz);
        if (!tmp) {
          fprintf(stderr, "%s: out of memory (resizing prev_keys)\n", argv[0]);
          return EXIT_MEM_ERR;
        }
        prev_keys = tmp;
        keybuf_sz = input_line_sz;
      }

      if (extract_keys(cur_keys, input_line, args->delim, keys, nkeys) != 0) {
        fprintf(stderr, "%s: malformatted input\n", argv[0]);
        return EXIT_FILE_ERR;
      }

      if (prev_keys[0] != '\0' && !str_eq(cur_keys, prev_keys)) {
        print_line(out, prev_keys, args->delim, cur_counts, ncounts, cur_sums,
                   nsums, sums_precision);

        memset(cur_counts, 0, ncounts * sizeof(int));
        memset(cur_sums, 0, nsums * sizeof(double));
      }

      for (i = 0; i < ncounts; i++) {
        get_line_field(field_buf, input_line, 63, counts[i], args->delim);
        if (field_buf[0] != '\0') {
          cur_counts[i]++;
        }
      }

      for (i = 0; i < nsums; i++) {
        get_line_field(field_buf, input_line, 63, sums[i], args->delim);
        f = atof(field_buf);
        cur_sums[i] += f;

        cur_precision = float_precision(field_buf);

        if (cur_precision > sums_precision[i])
          sums_precision[i] = cur_precision;
      }

      strcpy(prev_keys, cur_keys);
    }
    in = nextfile(argc, argv, &optind, "r");
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
   overflowed. */
static int extract_keys(char *target,
                        const char *source,
                        const char *delim, int *keys, size_t nkeys) {
  int i, s, e;

  target[0] = '\0';
  for (i = 0; i < nkeys; i++) {
    if (get_line_pos(source, keys[i], delim, &s, &e)) {
      strncat(target, source + s, e - s + 1);
      strcat(target, delim);
    } else {
      return 1;
    }
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
    fprintf(out, "%.*f", sums_precision[i], sums[i]);
    if (i < nsums - 1)
      fputs(delim, out);
  }

  if (ncounts > 0 && nsums > 0)
    fputs(delim, out);

  for (i = 0; i < ncounts; i++) {
    fprintf(out, "%d", counts[i]);
    if (i < ncounts - 1)
      fputs(delim, out);
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
