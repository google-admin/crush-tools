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
#include "pivot_main.h"
#include <dbfr.h>
#include <ffutils.h>
#include <hashtbl.h>
#include <linklist.h>

#include <locale.h>
#include <assert.h>

#define KEY_HASH_SZ 1024
#define PIVOT_HASH_SZ 32
#define MAX_FIELD_LEN 1024

/* holds the expansions of the -f, -p, and -v arguments, or their
 * label counterparts. */
struct pivot_conf {
  int *keys;
  size_t keys_sz;
  ssize_t n_keys;
  int *pivots;
  size_t pivots_sz;
  ssize_t n_pivots;
  int *values;
  size_t values_sz;
  ssize_t n_values;
  int *value_precisions;
};

int configure_pivot(struct pivot_conf *conf, struct cmdargs *args,
                    const char *header, const char *delim);
void decrement_values(int *array, size_t sz);
void *realloc_if_needed(char **target, size_t * cur_sz, const size_t new_sz);
void extract_fields_to_string(char *line, char *destbuf, size_t destbuf_sz,
                              int *fields, size_t nfields, char *delim);
int key_strcmp(char **a, char **b);
int float_str_precision(char *d);

char *delim;

void free_hash(void *t) {
  ht_destroy((hashtbl_t *) t);
  free(t);
}

/** @brief  
  * 
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int pivot(struct cmdargs *args, int argc, char *argv[], int optind) {

  int i, j, tmplen;

  char default_delim[] = { 0xFE, 0x00 };

  hashtbl_t key_hash;           /* outer hash */
  hashtbl_t *pivot_hash;        /* pointer for inner hashes */

  struct pivot_conf conf;
  /* variables for keeping track of the unique pivot strings */
  hashtbl_t uniq_pivots;        /* set of all pivot strings */
  char **pivot_array;           /* list of all pivot keys */
  size_t n_key_strings;         /* number of distinct key strings */
  size_t n_pivot_keys;          /* number of distinct pivot field values */

  double *line_values;          /* array of values */

  char *keystr, *pivstr;        /* hash key strings */
  size_t keystr_sz, pivstr_sz;

  char **headers = NULL;        /* array of header labels */
  size_t n_headers = 0;         /* number of fields */

  char *fieldbuf = NULL;        /* to hold fields extracted from input */
  size_t fieldbuf_sz = 0;       /* size of field buffer */

  FILE *fin;                    /* input file */
  dbfr_t *in_reader;

  char empty_string[] = "";

  if (!args->delim) {
    args->delim = getenv("DELIMITER");
    if (!args->delim)
      args->delim = default_delim;
  }
  expand_chars(args->delim);

  delim = args->delim;

  /* get first input file pointer - either trailing arg or stdin */
  if (optind == argc)
    fin = stdin;
  else
    fin = nextfile(argc, argv, &optind, "r");
  if (!fin) {
    fprintf(stderr, "%s: no valid input files specified.\n", argv[0]);
    return EXIT_FILE_ERR;
  }
  in_reader = dbfr_init(fin);

  /* set locale with values from the environment so strcoll()
     will work correctly. */
  setlocale(LC_ALL, "");
  setlocale(LC_COLLATE, "");

  memset(&conf, 0, sizeof(conf));
  if (configure_pivot(&conf, args, in_reader->next_line, delim) != 0) {
    fprintf(stderr, "%s: error parsing input field arguments.\n", argv[0]);
    return EXIT_HELP;
  }
  if (conf.n_pivots == 0 || conf.n_values == 0) {
    fprintf(stderr, "%s: -p/-P and -v/-A must be specified.\n", argv[0]);
    return EXIT_HELP;
  }

  /* TODO: get rid of this arbirary field length limitation */
  fieldbuf = malloc(MAX_FIELD_LEN);
  if (! fieldbuf) {
    warn("allocating field buffer");
    return EXIT_MEM_ERR;
  }
  fieldbuf_sz = MAX_FIELD_LEN;

  /* extract headers from first line of input if necessary */
  if (args->keep_header) {

    if (dbfr_getline(in_reader) < 1) {
      fprintf(stderr, "%s: unexpected end of file.\n", argv[0]);
      return EXIT_FILE_ERR;
    }
    chomp(in_reader->current_line);
    n_headers = fields_in_line(in_reader->current_line, delim);
    headers = malloc(sizeof(char *) * n_headers);
    if (!headers)
      return EXIT_MEM_ERR;

    for (i = 0; i < n_headers; i++) {
      get_line_field(fieldbuf, in_reader->current_line, fieldbuf_sz - 1,
                     i, delim);
      headers[i] = malloc(sizeof(char *) * strlen(fieldbuf) + 1);
      strcpy(headers[i], fieldbuf);
    }

#ifdef CRUSH_DEBUG
    for (i = 0; i < n_headers; i++) {
      fprintf(stderr, "%s%s", headers[i], i < n_headers - 1 ? args->delim : "");
    }
    fprintf(stderr, "\n");
#endif
  }

  /* these two buffers will have enough capacity to hold the entire input line,
     unless there are no key fields specified, in which case keystr will just
     be set to an empty string.
   */
  keystr = pivstr = NULL;
  keystr_sz = pivstr_sz = 0;

  ht_init(&key_hash, KEY_HASH_SZ, NULL, free_hash);
  ht_init(&uniq_pivots, PIVOT_HASH_SZ, NULL, NULL);
  n_key_strings = 0;
  n_pivot_keys = 0;

  /* no keys specified?  set keystr to an empty string */
  if (!conf.n_keys) {
    keystr = empty_string;
  }

  while (fin != NULL) {

    while (dbfr_getline(in_reader) > 0) {
      int value_in_hash = 1;
      int pivot_in_hash = 1;

      chomp(in_reader->current_line);
      if (conf.n_keys) {
        /* this could validly return NULL if both sizes are 0 the first time thru,
           when keystr is still NULL, but that shouldn't happen */
        if (realloc_if_needed(&keystr, &keystr_sz,
                              in_reader->current_line_sz) == NULL) {
          fprintf(stderr, "%s: out of memory.\n", getenv("_"));
          break;
        }
      }

      if (conf.n_pivots) {
        if (realloc_if_needed(&pivstr, &pivstr_sz,
                              in_reader->current_line_sz) == NULL) {
          fprintf(stderr, "%s: out of memory.\n", getenv("_"));
          break;
        }
      }

      /* make key string from keys[] */
      if (conf.n_keys)
        extract_fields_to_string(in_reader->current_line, keystr, keystr_sz,
                                 conf.keys, conf.n_keys, delim);

      /* make key string from pivots[] */
      extract_fields_to_string(in_reader->current_line, pivstr, pivstr_sz,
                               conf.pivots, conf.n_pivots, delim);

#ifdef CRUSH_DEBUG
      if (n_keys)
        fprintf(stderr, "key string: %s\n", keystr);
      if (n_pivots)
        fprintf(stderr, "pivot string: %s\n", pivstr);
#endif

      /* get hashtable value */
      pivot_hash = (hashtbl_t *) ht_get(&key_hash, keystr);
      if (!pivot_hash) {
        pivot_hash = malloc(sizeof(hashtbl_t));
        ht_init(pivot_hash, PIVOT_HASH_SZ, NULL, free);
        pivot_in_hash = 0;
      }

      line_values = ht_get(pivot_hash, pivstr);
      if (!line_values) {
        line_values = malloc(sizeof(double) * conf.n_values);
        memset(line_values, 0, sizeof(double) * conf.n_values);
        value_in_hash = 0;
      }


      /* add in values */
      for (i = 0; i < conf.n_values; i++) {
        tmplen =
          get_line_field(fieldbuf, in_reader->current_line, fieldbuf_sz - 1,
                         conf.values[i], delim);
        if (tmplen > 0) {
          line_values[i] += atof(fieldbuf);

          /* remember the greatest input floating-point precision for each
           * field */
          tmplen = float_str_precision(fieldbuf);
          if (conf.value_precisions[i] < tmplen) {
#ifdef CRUSH_DEBUG
            fprintf(stderr, "setting precision to %d for field %d\n", tmplen,
                    i);
#endif
            conf.value_precisions[i] = tmplen;
          }
        }
      }

      /* store hashtable value */
      if (!value_in_hash)
        ht_put(pivot_hash, pivstr, line_values);

      if (!pivot_in_hash) {
        ht_put(&key_hash, keystr, pivot_hash);
      }

      /* store the pivot key string for later use */
      ht_put(&uniq_pivots, pivstr, (void *) 1);
    }

    dbfr_close(in_reader);
    fin = nextfile(argc, argv, &optind, "r");
    if (fin) {
      in_reader = dbfr_init(fin);
      /* reconfigure in case the fields are rearranged in the new file */
      if (configure_pivot(&conf, args, in_reader->next_line, delim) != 0) {
        fprintf(stderr, "%s: error parsing input field arguments.\n", argv[0]);
        return EXIT_HELP;
      }
      /* throw out headers from all files after the first. */
      if (args->keep_header)
        dbfr_getline(in_reader);
    }
  }

  n_key_strings = key_hash.nelems;
  n_pivot_keys = uniq_pivots.nelems;

  /* sort the collection of all pivot key strings */
  {
    llist_node_t *node;
    llist_t *pivot_list;
    pivot_array = malloc(sizeof(char *) * n_pivot_keys);
    j = 0;
    for (i = 0; i < uniq_pivots.arrsz; i++) {
      pivot_list = uniq_pivots.arr[i];
      if (pivot_list) {
        for (node = pivot_list->head; node; node = node->next) {
          pivot_array[j] = ((ht_elem_t *) node->data)->key;
          j++;
        }
      }
    }
    qsort(pivot_array, n_pivot_keys, sizeof(char *),
          (int (*)(const void *, const void *)) key_strcmp);
#ifdef CRUSH_DEBUG
    fprintf(stderr, "sorted pivot strings:\n");
    for (i = 0; i < n_pivot_keys; i++) {
      fprintf(stderr, "\t%s\n", pivot_array[i]);
    }
#endif
  }

  /* OUTPUT SECTION */

  /* print headers separate from data if necessary */
  if (args->keep_header) {
    char *pivot_label;

    /* assumption - the largest line of input has a greater length
       than the combined length of all pivot field values and a 3-char
       separator.  safe assumption?  probably not if every input field
       is used as a pivot field. */
    pivot_label = malloc(in_reader->current_line_sz);
    if (! pivot_label) {
      warn("allocating memory for the labels");
      exit(EXIT_MEM_ERR);
    }

    if (conf.n_keys) {
      for (i = 0; i < conf.n_keys; i++)
        printf("%s%s", headers[conf.keys[i]], delim);
    }
    for (i = 0; i < n_pivot_keys; i++) {
      pivot_label[0] = 0x00;

      /* get the current pivot field values & build a label with them */
      for (j = 0; j < conf.n_pivots; j++) {
        get_line_field(fieldbuf, pivot_array[i], fieldbuf_sz - 1, j, delim);
        strcat(pivot_label, fieldbuf);
        if (j != conf.n_pivots - 1)
          strcat(pivot_label, " - ");
      }

      /* get the value field labels & print them with the pivot label */
      for (j = 0; j < conf.n_values; j++) {
        printf("%s: %s", pivot_label, headers[conf.values[j]]);
        if (j != conf.n_values - 1)
          fputs(delim, stdout);
      }
      /* TODO: segfault is happening around here */
      if (i != n_pivot_keys - 1)
        fputs(delim, stdout);

    }
    fputs("\n", stdout);

    free(pivot_label);

    /* free each header string - don't need them anymore */
    for (i = 0; i < n_headers; i++)
      free(headers[i]);
    free(headers);
  }


  {
    char **key_array;
    llist_node_t *key_node;
    llist_t *key_list;
    char *empty_value_string;

    /* construct string for empty value set.  this should be big enough for
       n_values worth of zeros (of the appropriate precision) and delimiters
       in between.  here we'll just guess that a precision of 8 is enough. */
    empty_value_string = malloc((sizeof(char) * conf.n_values * 8) +
                                (strlen(delim) * conf.n_values));
    empty_value_string[0] = 0x00;
    for (i = 0; i < conf.n_values; i++) {
      sprintf(empty_value_string, "%s%.*f", empty_value_string,
              conf.value_precisions[i], 0.0F);
      if (i != conf.n_values - 1)
        strcat(empty_value_string, delim);
    }

    key_array = malloc(sizeof(char *) * n_key_strings);
    if (key_array == NULL) {
      warn("malloc key array");
      return EXIT_MEM_ERR;
    }

    j = 0;
    for (i = 0; i < key_hash.arrsz; i++) {
      key_list = key_hash.arr[i];
      if (key_list) {
        for (key_node = key_list->head; key_node; key_node = key_node->next) {
#ifdef CRUSH_DEBUG
          fprintf(stderr, "got key \"%s\" out of hash.\n",
                  ((ht_elem_t *) key_node->data)->key);
#endif
          key_array[j] = ((ht_elem_t *) key_node->data)->key;
          j++;
        }
      }
    }

    /* j now holds the number of distinct keys to be output */
    assert(j == n_key_strings);

    /* sort the keys */
    qsort(key_array, n_key_strings, sizeof(char *),
          (int (*)(const void *, const void *)) key_strcmp);

    /* loop through all key strings */
    for (i = 0; i < n_key_strings; i++) {
      int k;
      pivot_hash = ht_get(&key_hash, key_array[i]);

      if (n_key_strings > 0)
        printf("%s%s", key_array[i], delim);

      /* loop through all possible pivot-string inner hashtable keys */
      for (k = 0; k < n_pivot_keys; k++) {
        /* loop through all values */
        line_values = ht_get(pivot_hash, pivot_array[k]);
        if (!line_values)
          fputs(empty_value_string, stdout);
        else {
          for (j = 0; j < conf.n_values; j++) {
            printf("%.*f%s", conf.value_precisions[j], line_values[j],
                   j != conf.n_values - 1 ? delim : "");
          }
        }
        if (k != n_pivot_keys - 1)
          fputs(delim, stdout);
      }
      fputs("\n", stdout);

    }
    free(empty_value_string);
    free(key_array);
  }

  /* CLEANUP SECTION */
  ht_destroy(&key_hash);
  ht_destroy(&uniq_pivots);

  if (keystr && keystr != empty_string)
    free(keystr);
  if (pivstr)
    free(pivstr);
  if (pivot_array)
    free(pivot_array);
  if (fieldbuf)
    free(fieldbuf);

  return EXIT_OKAY;
}

int configure_pivot(struct pivot_conf *conf, struct cmdargs *args,
                    const char *header, const char *delim) {
  conf->n_keys = 0;
  if (args->keys) {
    conf->n_keys = expand_nums(args->keys, &(conf->keys), &(conf->keys_sz));
  } else if (args->key_labels) {
    conf->n_keys = expand_label_list(args->key_labels, header, delim,
                                     &(conf->keys), &(conf->keys_sz));
    args->keep_header = 1;
  }
  if (conf->n_keys < 0)
    return -1;
  else
    decrement_values(conf->keys, conf->n_keys);

  conf->n_pivots = 0;
  if (args->pivots) {
    conf->n_pivots = expand_nums(args->pivots, &(conf->pivots),
                                 &(conf->pivots_sz));
  } else if (args->pivot_labels) {
    conf->n_pivots = expand_label_list(args->pivot_labels, header, delim,
                                       &(conf->pivots), &(conf->pivots_sz));
    args->keep_header = 1;
  }
  if (conf->n_pivots < 0)
    return -1;
  else
    decrement_values(conf->pivots, conf->n_pivots);

  conf->n_values = 0;
  if (args->values) {
    conf->n_values = expand_nums(args->values, &(conf->values),
                                 &(conf->values_sz));
  } else if (args->value_labels) {
    conf->n_values = expand_label_list(args->value_labels, header, delim,
                                       &(conf->values), &(conf->values_sz));
    args->keep_header = 1;
  }
  if (conf->n_values < 0)
    return -1;
  else
    decrement_values(conf->values, conf->n_values);
  conf->value_precisions = calloc(conf->n_values, sizeof(int));
  return 0;
}

void decrement_values(int *array, size_t sz) {
  int j;
  if (array == NULL || sz == 0)
    return;
  for (j = 0; j < sz; j++) {
    array[j]--;
  }
}


void *realloc_if_needed(char **target, size_t * cur_sz, const size_t new_sz) {
  void *tmp;

  if (*cur_sz >= new_sz)
    return *target;

  tmp = realloc(*target, new_sz);
  if (!tmp)
    return NULL;

  *target = tmp;
  *cur_sz = new_sz;
  return *target;
}


void extract_fields_to_string(char *line, char *destbuf, size_t destbuf_sz,
                              int *fields, size_t nfields, char *delim) {
  char *pos;
  int i;
  size_t delim_len, field_len;

  delim_len = strlen(delim);
  pos = destbuf;

  for (i = 0; i < nfields; i++) {
    field_len =
      get_line_field(pos, line, destbuf_sz - (pos - destbuf), fields[i], delim);
    pos += field_len;
    if (i != nfields - 1) {
      strcat(pos, delim);
      pos += delim_len;
    }
  }
}

int key_strcmp(char **a, char **b) {
  char fa[256], fb[256];
  int retval = 0;
  int i;
  size_t alen, blen;

  /* avoid comparing nulls */
  if (!*a && !*b)
    return 0;
  if (!*a && *b)
    return -1;
  if (*a && !*b)
    return 1;

  i = 0;
  while (retval == 0) {
    alen = get_line_field(fa, *a, 255, i, delim);
    blen = get_line_field(fb, *b, 255, i, delim);
    if (alen < 0 || blen < 0)
      break;
    retval = strcoll(fa, fb);
    i++;
  }

  return retval;
}

int float_str_precision(char *d) {
  char *p;
  int after_dot;
  if (!d)
    return 0;

  p = strchr(d, '.');
  if (p == NULL) {
    return 0;
  }
  after_dot = p - d + 1;
  return (strlen(d) - after_dot);
}
