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
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <crush/dbfr.h>
#include <crush/ffutils.h>
#include <crush/general.h>
#include <crush/hashtbl.h>

#include "hashjoin_main.h"

char default_delim[] = {0xfe, 0x00};

static void extract_fields(int *field_list, size_t n_fields,
                           const char *line, char *target, size_t target_sz,
                           const char *ifs, const char *ofs);

static size_t hash_dimension_file(struct cmdargs *args, hashtbl_t *ht);

static void decrement(int *lst, size_t n);

/** @brief Application entry point.
  *
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  *
  * @return exit status for main() to return.
  */
int hashjoin (struct cmdargs *args, int argc, char *argv[], int optind) {
  hashtbl_t dimension;
  FILE *infile;
  dbfr_t *datareader;

  char *keybuffer = NULL;
  size_t keybuffer_sz = 0;

  char *value, *empty_value;
  size_t n_values, i;

  int *key_fields = NULL;
  size_t n_key_fields = 0;

  if (! args->key_labels &&
      ! (args->data_key_fields && args->dimension_key_fields)) {
    fprintf(stderr, "%s: missing key field argument(s)\n", getenv("_"));
    return EXIT_FAILURE;
  }

  if (! args->dimension_field_labels &&
      ! args->dimension_fields) {
    fprintf(stderr, "%s: missing dimension field argument\n", getenv("_"));
    return EXIT_FAILURE;
  }

  if (! args->delim) {
    args->delim = getenv("DELIMITER");
    if (! args->delim) {
      args->delim = default_delim;
    }
  }
  expand_chars(args->delim);
  if (! args->dimension_delim) {
    args->dimension_delim = args->delim;
  }

  ht_init(&dimension, 1024, NULL, NULL);
  n_values = hash_dimension_file(args, &dimension);

  if (args->default_values) {
    size_t default_len = strlen(args->default_values);
    char *default_buffer = xmalloc(default_len);
    empty_value = xmalloc(default_len + strlen(args->delim) * n_values);
    empty_value[0] = '\0';
    for (i = 0; i < n_values; i++) {
      get_line_field(default_buffer, args->default_values, default_len, i, ",");
      strcat(empty_value, default_buffer);
      if (i < n_values - 1) {
        strcat(empty_value, args->delim);
      }
    }
  } else {
    empty_value = xmalloc(strlen(args->delim) * n_values);
    empty_value[0] = '\0';
    for (i = 0; i < n_values - 1; i++) {
      strcat(empty_value, args->delim);
    }
  }

  if (argc > optind)
    infile = nextfile(argc, argv, &optind, "r");
  else
    infile = stdin;

  if (! args->key_labels) {
    n_key_fields = expand_nums(args->data_key_fields,
                               &key_fields, &n_key_fields);
    decrement(key_fields, n_key_fields);
  }

  while (infile) {
    datareader = dbfr_init(infile);
    if (datareader->eof) {
      infile = nextfile(argc, argv, &optind, "r");
      continue;
    }

    if (args->key_labels) {
      n_key_fields = expand_label_list(args->key_labels, datareader->next_line,
                                       args->delim, &key_fields,
                                       &n_key_fields);
      decrement(key_fields, n_key_fields);
    }

    /* Add user-supplied dimension labels to the header row. */
    if (args->dimension_labels && ! args->dimension_field_labels) {
      dbfr_getline(datareader);
      chomp(datareader->current_line);
      printf("%s%s%s\n", datareader->current_line,
             args->delim, args->dimension_labels);
    }

    /* If the input has only a header row, quit now. */
    if (datareader->next_line == NULL) {
      infile = nextfile(argc, argv, &optind, "r");
      continue;
    }

    if (! keybuffer) {
      keybuffer = xmalloc(datareader->next_line_len);
      keybuffer_sz = datareader->next_line_len;
    }

    while (dbfr_getline(datareader) > 0) {
      if (datareader->current_line_len > keybuffer_sz) {
        keybuffer = xrealloc(keybuffer, datareader->current_line_len);
        keybuffer_sz = datareader->current_line_len;
      }
      chomp(datareader->current_line);
      extract_fields(key_fields, n_key_fields, datareader->current_line,
                     keybuffer, keybuffer_sz,
                     args->delim, args->delim);

      value = ht_get(&dimension, keybuffer);
      if (! value)
        value = empty_value;
      printf("%s%s%s\n", datareader->current_line, args->delim, value);
    }

    infile = nextfile(argc, argv, &optind, "r");
  }

  return EXIT_OKAY;
}


/** @brief Extracts a list of fields from a string and stores them in a target
  * buffer.
  *
  * The field separator used in the input string can be different from the
  * output field separator.
  *
  * @param field_list an array of 0-based indexes.
  * @param n_fields the number of elements in field_list.
  * @param line the input string.
  * @param target the output string buffer.
  * @param target_sz the size of target.
  * @param ifs field separator used in line.
  * @param ofs field separator to use in target.
  */
static void extract_fields(int *field_list, size_t n_fields,
                           const char *line, char *target, size_t target_sz,
                           const char *ifs, const char *ofs) {
  int i;
  size_t target_len = 0,
         field_len,
         ofs_len = strlen(ofs);
  target[0] = '\0';

  for (i=0; i < n_fields; i++) {
    field_len = get_line_field(target + target_len, line,
                               target_sz - target_len, field_list[i], ifs);
    if (field_len < 0) {
      /* TODO(jhinds): Maybe do something better than silently treating missing
       * fields as empty. */
      field_len = 0;
    }
    target_len += field_len;
    if (i < n_fields - 1) {
      strncat(target, ofs, target_sz - target_len);
      target_len += ofs_len;
    }
  }
}


static void decrement(int *lst, size_t n) {
  int i;
  for (i=0; i < n; i++)
    lst[i]--;
}


/** @brief Stores key and value fields from a dimension file in a hashtable.
  *
  * @param args commandline options.
  * @param ht the hashtable to hold the data.
  * @param filename the name of the dimension file.
  *
  * @return the number of value fields.  Hackish, but hashjoin() needs to know
  *         and has no other reason to parse the value arguments.
  */
static size_t hash_dimension_file(struct cmdargs *args, hashtbl_t *ht) {
  char *value;
  char *field_buffer = NULL;
  size_t field_buffer_len = 0,
         field_buffer_sz = 0;
  int *key_fields = NULL,
      *val_fields = NULL;
  size_t key_fields_sz = 0,
         val_fields_sz = 0;
  int n_key_fields = 0,
      n_val_fields = 0;
  dbfr_t *dim_file = dbfr_open(args->dimension_file);

  if (! dim_file) {
    warn(args->dimension_file);
    exit(EXIT_FAILURE);
  }

  if (args->key_labels) {
    n_key_fields = expand_label_list(args->key_labels, dim_file->next_line,
                                     args->dimension_delim, &key_fields,
                                     &key_fields_sz);
  } else {
    n_key_fields = expand_nums(args->dimension_key_fields,
                               &key_fields, &key_fields_sz);
  }

  if (n_key_fields <= 0) {
    fprintf(stderr, "%s: error parsing dimension key field list.\n",
            getenv("_"));
    exit(EXIT_FAILURE);
  }

  if (args->dimension_field_labels) {
    n_val_fields = expand_label_list(args->dimension_field_labels,
                                     dim_file->next_line,
                                     args->dimension_delim, &val_fields,
                                     &val_fields_sz);
  } else {
    n_val_fields = expand_nums(args->dimension_fields,
                               &val_fields, &val_fields_sz);
  }

  if (n_val_fields <= 0) {
    fprintf(stderr, "%s: error parsing dimension value field list.\n",
            getenv("_"));
    exit(EXIT_FAILURE);
  }

  decrement(key_fields, n_key_fields);
  decrement(val_fields, n_val_fields);

  field_buffer = xmalloc(dim_file->next_line_len);
  field_buffer_sz = dim_file->next_line_len;

  while (dbfr_getline(dim_file) > 0) {
    if (dim_file->current_line_len > field_buffer_sz) {
      field_buffer = xrealloc(field_buffer, dim_file->current_line_len);
      field_buffer_sz = dim_file->current_line_len;
    }

    extract_fields(val_fields, n_val_fields, dim_file->current_line,
                   field_buffer, field_buffer_sz,
                   args->dimension_delim, args->delim);
    value = xstrdup(field_buffer);

    extract_fields(key_fields, n_key_fields, dim_file->current_line,
                   field_buffer, field_buffer_sz,
                   args->dimension_delim, args->delim);

    ht_put(ht, field_buffer, value);
  }

  dbfr_close(dim_file);
  free(field_buffer);

  return n_val_fields;
}

