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
#include <crush/dbfr.h>

#include "aggregate_main.h"
#include "aggregate.h"

#define AGG_TMP_BUF_SIZE 64

char *delim;
struct agg_conf conf;

int configure_aggregation(struct agg_conf *conf, struct cmdargs *args,
                          const char *header, const char *delim) {
  if (args->keys) {
    conf->nkeys = expand_nums(args->keys, &(conf->key_fields),
                              &(conf->key_fields_sz));
  } else if (args->key_labels) {
    conf->nkeys = expand_label_list(args->key_labels, header,
                                    delim, &(conf->key_fields),
                                    &(conf->key_fields_sz));
    args->preserve = 1;
  }
  if (conf->nkeys < 0)
    return conf->nkeys;
  decrement_values(conf->key_fields, conf->nkeys);

  if (args->sums) {
    conf->nsums = expand_nums(args->sums, &(conf->sum_fields),
                              &(conf->sum_fields_sz));
  } else if (args->sum_labels) {
    conf->nsums = expand_label_list(args->sum_labels, header,
                                    delim, &(conf->sum_fields),
                                    &(conf->sum_fields_sz));
    args->preserve = 1;
  }
  if (conf->nsums < 0) {
    return conf->nsums;
  } else if (conf->nsums > 0) {
    decrement_values(conf->sum_fields, conf->nsums);
    conf->sum_precisions = malloc(sizeof(int) * conf->nsums);
    memset(conf->sum_precisions, 0, sizeof(int) * conf->nsums);
  }

  if (args->counts) {
    conf->ncounts = expand_nums(args->counts, &(conf->count_fields),
                                &(conf->count_fields_sz));
  } else if (args->count_labels) {
    conf->ncounts = expand_label_list(args->count_labels, header,
                                      delim, &(conf->count_fields),
                                      &(conf->count_fields_sz));
    args->preserve = 1;
  }
  if (conf->ncounts < 0)
    return conf->ncounts;
  else if (conf->ncounts > 0)
    decrement_values(conf->count_fields, conf->ncounts);

  if (args->averages) {
    conf->naverages = expand_nums(args->averages, &(conf->average_fields),
                                  &(conf->average_fields_sz));
  } else if (args->average_labels) {
    conf->naverages = expand_label_list(args->average_labels, header,
                                        delim, &(conf->average_fields),
                                        &(conf->average_fields_sz));
    args->preserve = 1;
  }
  if (conf->naverages < 0) {
    return conf->naverages;
  } else if (conf->naverages > 0) {
    decrement_values(conf->average_fields, conf->naverages);
    conf->average_precisions = malloc(sizeof(int) * conf->naverages);
    memset(conf->average_precisions, 0, sizeof(int) * conf->naverages);
  }

  if (args->mins) {
    conf->nmins = expand_nums(args->mins, &(conf->min_fields),
                              &(conf->min_fields_sz));
  } else if (args->min_labels) {
    conf->nmins = expand_label_list(args->min_labels, header, delim,
                                    &(conf->min_fields),
                                    &(conf->min_fields_sz));
  }
  if (conf->nmins < 0) {
    return conf->nmins;
  } else if (conf->nmins > 0) {
    decrement_values(conf->min_fields, conf->nmins);
    conf->min_precisions = malloc(sizeof(int) * conf->nmins);
    memset(conf->min_precisions, 0, sizeof(int) * conf->nmins);
  }

  if (args->maxs) {
    conf->nmaxs = expand_nums(args->maxs, &(conf->max_fields),
                              &(conf->max_fields_sz));
  } else if (args->max_labels) {
    conf->nmaxs = expand_label_list(args->max_labels, header, delim,
                                    &(conf->max_fields),
                                    &(conf->max_fields_sz));
  }
  if (conf->nmaxs < 0) {
    return conf->nmaxs;
  } else if (conf->nmaxs > 0) {
    decrement_values(conf->max_fields, conf->nmaxs);
    conf->max_precisions = malloc(sizeof(int) * conf->nmaxs);
    memset(conf->max_precisions, 0, sizeof(int) * conf->nmaxs);
  }

  return 0;
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
int aggregate(struct cmdargs *args, int argc, char *argv[], int optind) {

  int i, n;

  hashtbl_t aggregations;
  llist_t *hash_keys;

  size_t n_hash_elems;

  FILE *in;                     /* input file */
  dbfr_t *in_reader;

  char *outbuf;                 /* buffer for a line of output */
  size_t outbuf_sz;             /* size of the output buffer */

  char default_delim[] = { 0xFE, 0x00 };  /* default delimiter string */

  if (! args->keys && ! args->key_labels) {
    fprintf(stderr, "%s: -k or -K must be specified\n", argv[0]);
    return EXIT_HELP;
  }

  delim = args->delim;
  if (!delim)
    delim = getenv("DELIMITER");

  if (delim)
    expand_chars(delim);
  else
    delim = default_delim;

  if (optind == argc)
    in = stdin;
  else
    in = nextfile(argc, argv, &optind, "r");

  if (in == NULL)
    return EXIT_FILE_ERR;

  in_reader = dbfr_init(in);

  memset(&conf, 0, sizeof(conf));
  if (configure_aggregation(&conf, args, in_reader->next_line, delim) != 0) {
    fprintf(stderr, "%s: error parsing field arguments.\n", argv[0]);
    return EXIT_HELP;
  }

#ifdef CRUSH_DEBUG
  fprintf(stderr, "%d keys: ", conf.nkeys);
  for (i = 0; i < conf.nkeys; i++)
    fprintf(stderr, "%d ", conf.key_fields[i]);
  fprintf(stderr, "\n%d sums: ", conf.nsums);
  for (i = 0; i < conf.nsums; i++)
    fprintf(stderr, "%d ", conf.sum_fields[i]);
  fprintf(stderr, "\n%d averages: ", conf.naverages);
  for (i = 0; i < conf.naverages; i++)
    fprintf(stderr, "%d ", conf.average_fields[i]);
  fprintf(stderr, "\n%d counts: ", conf.ncounts);
  for (i = 0; i < conf.ncounts; i++)
    fprintf(stderr, "%d ", conf.count_fields[i]);
  fprintf(stderr, "\n\n");
#endif

  outbuf = NULL;
  outbuf_sz = 0;

  /* set locale with values from the environment so strcoll()
     will work correctly. */
  setlocale(LC_ALL, "");
  setlocale(LC_COLLATE, "");

  if (args->preserve) {
    size_t str_len;

    if (dbfr_getline(in_reader) <= 0) {
      fprintf(stderr, "%s: unexpected end of file\n", getenv("_"));
      exit(EXIT_FILE_ERR);
    }
    chomp(in_reader->current_line);

    outbuf = malloc(in_reader->current_line_len);
    if (!outbuf) {
      fprintf(stderr, "%s: out of memory.\n", getenv("_"));
      exit(EXIT_MEM_ERR);
    }
    outbuf_sz = in_reader->current_line_len;

    extract_fields_to_string(in_reader->current_line, outbuf, outbuf_sz,
                             conf.key_fields, conf.nkeys, delim, NULL);
    fputs(outbuf, stdout);
    if (args->labels) {
    	printf("%s%s", delim, args->labels);
    } else {
      if (conf.nsums) {
        extract_fields_to_string(in_reader->current_line, outbuf, outbuf_sz,
                                 conf.sum_fields, conf.nsums, delim,
                                 args->auto_label ? "-Sum" : NULL);
        printf("%s%s", delim, outbuf);
      }

      if (conf.ncounts) {
        extract_fields_to_string(in_reader->current_line, outbuf, outbuf_sz,
                                 conf.count_fields, conf.ncounts, delim,
                                 args->auto_label ? "-Count" : NULL);
        printf("%s%s", delim, outbuf);
      }

      if (conf.naverages) {
        extract_fields_to_string(in_reader->current_line, outbuf, outbuf_sz,
                                 conf.average_fields, conf.naverages, delim,
                                 args->auto_label ? "-Average" : NULL);
        printf("%s%s", delim, outbuf);
      }

      if (conf.nmins) {
        extract_fields_to_string(in_reader->current_line, outbuf, outbuf_sz,
                                 conf.min_fields, conf.nmins, delim,
                                 args->auto_label ? "-Min" : NULL);
        printf("%s%s", delim, outbuf);
      }

      if (conf.nmaxs) {
        extract_fields_to_string(in_reader->current_line, outbuf, outbuf_sz,
                                 conf.max_fields, conf.nmaxs, delim,
                                 args->auto_label ? "-Min" : NULL);
        printf("%s%s", delim, outbuf);
      }
    }

    fputs("\n", stdout);
  }

  ht_init(&aggregations, 1024, NULL, (void (*)) free_agg);
  /* ht_init( &aggregations, 1024, NULL, free ); */

  n_hash_elems = 0;

  /* loop through all files */
  while (in != NULL) {
    struct aggregation *value;
    char tmpbuf[AGG_TMP_BUF_SIZE];
    size_t tmplen;
    int in_hash;

    /* loop through each line of the file */
    while (dbfr_getline(in_reader) > 0) {
      chomp(in_reader->current_line);
      if (in_reader->current_line_len > outbuf_sz) {
        char *tmp_outbuf = realloc(outbuf, in_reader->current_line_len + 32);
        if (!tmp_outbuf) {
          fprintf(stderr, "%s: out of memory.\n", getenv("_"));
          return EXIT_MEM_ERR;
        }
        outbuf = tmp_outbuf;
        outbuf_sz = in_reader->current_line_len + 32;
      }

      extract_fields_to_string(in_reader->current_line, outbuf, outbuf_sz,
                               conf.key_fields, conf.nkeys, delim, NULL);

      value = (struct aggregation *) ht_get(&aggregations, outbuf);
      if (!value) {
        in_hash = 0;
        value = alloc_agg(conf.nsums, conf.ncounts, conf.naverages,
                          conf.nmins, conf.nmaxs);
        /* value = malloc(sizeof(struct aggregation));
           memset(value, 0, sizeof(struct aggregation)); */
        if (!value) {
          fprintf(stderr, "%s: out of memory.\n", getenv("_"));
          return EXIT_MEM_ERR;
        }
      } else {
        in_hash = 1;
      }

      /* sums */
      for (i = 0; i < conf.nsums; i++) {
        tmplen =
          get_line_field(tmpbuf, in_reader->current_line,
                         AGG_TMP_BUF_SIZE - 1, conf.sum_fields[i], delim);
        if (tmplen > 0) {
          n = float_str_precision(tmpbuf);
          if (conf.sum_precisions[i] < n)
            conf.sum_precisions[i] = n;
          value->sums[i] += atof(tmpbuf);
        }
      }

      /* averages */
      for (i = 0; i < conf.naverages; i++) {
        tmplen = get_line_field(tmpbuf, in_reader->current_line,
                                AGG_TMP_BUF_SIZE - 1, conf.average_fields[i],
                                delim);
        if (tmplen > 0) {
          n = float_str_precision(tmpbuf);
          if (conf.average_precisions[i] < n)
            conf.average_precisions[i] = n;
          value->average_sums[i] += atof(tmpbuf);
          value->average_counts[i] += 1;
        }
      }

      /* counts */
      for (i = 0; i < conf.ncounts; i++) {
        tmplen = get_line_field(tmpbuf, in_reader->current_line,
                                AGG_TMP_BUF_SIZE - 1, conf.count_fields[i],
                                delim);
        if (tmplen > 0) {
          value->counts[i] += 1;
        }
      }

      /* mins */
      for (i = 0; i < conf.nmins; i++) {
        tmplen = get_line_field(tmpbuf, in_reader->current_line,
                                AGG_TMP_BUF_SIZE - 1, conf.min_fields[i],
                                delim);
        if (tmplen > 0) {
          double cur_val;
          n = float_str_precision(tmpbuf);
          if (conf.min_precisions[i] < n)
            conf.min_precisions[i] = n;
          n = sscanf(tmpbuf, "%lf", &cur_val);
          if (n) {
            if (cur_val < value->numeric_mins[i] ||
                ! value->mins_initialized[i])
              value->numeric_mins[i] = cur_val;
            value->mins_initialized[i] = 1;
          }
        }
      }

      /* maxs */
      for (i = 0; i < conf.nmaxs; i++) {
        tmplen = get_line_field(tmpbuf, in_reader->current_line,
                                AGG_TMP_BUF_SIZE - 1, conf.max_fields[i],
                                delim);
        if (tmplen > 0) {
          double cur_val;
          n = float_str_precision(tmpbuf);
          if (conf.max_precisions[i] < n)
            conf.max_precisions[i] = n;
          n = sscanf(tmpbuf, "%lf", &cur_val);
          if (n) {
            if (cur_val > value->numeric_maxs[i] ||
                ! value->maxs_initialized[i])
              value->numeric_maxs[i] = cur_val;
            value->maxs_initialized[i] = 1;
          }
        }
      }

      if (!in_hash) {
        if (ht_put(&aggregations, outbuf, value) != 0)
          fprintf(stderr, "%s: failed to store value in hashtable.\n",
                  getenv("_"));
        n_hash_elems++;
      }

    }
    dbfr_close(in_reader);
    in = nextfile(argc, argv, &optind, "r");
    if (in) {
      in_reader = dbfr_init(in);
      /* reconfigure fields (needed if labels were used) */
      if (configure_aggregation(&conf, args, in_reader->next_line, delim) != 0) {
        fprintf(stderr, "%s: error parsing field arguments.\n", argv[0]);
        return EXIT_HELP;
      }
      if (args->preserve)
        dbfr_getline(in_reader);
    }
  }

  /* print all of the output */
  if (args->nosort) {
    /* it will be a little faster if the user indicates that
       the output sort order doesn't matter */
    for (i = 0; i < aggregations.arrsz; i++) {
      hash_keys = aggregations.arr[i];
      if (hash_keys != NULL) {
        ll_call_for_each(hash_keys, ht_print_keys_and_agg_vals);
      }
    }
  } else {
    llist_node_t *node;
    struct aggregation *val;
    char **key_array;
    int j = 0;
    key_array = malloc(sizeof(char *) * n_hash_elems);

    /* put all the keys into an array */
    for (i = 0; i < aggregations.arrsz; i++) {
      hash_keys = aggregations.arr[i];
      if (hash_keys != NULL) {
        for (node = hash_keys->head; node; node = node->next) {
          key_array[j] = ((ht_elem_t *) node->data)->key;
          j++;
        }
      }
    }

    /* sort the keys */
    qsort(key_array, n_hash_elems, sizeof(char *),
          (int (*)(const void *, const void *)) key_strcmp);
    /* (int (*)(const void *, const void *))strcmp ); */

    /* print everything out */
    for (i = 0; i < n_hash_elems; i++) {
      val = (struct aggregation *) ht_get(&aggregations, key_array[i]);
      print_keys_and_agg_vals(key_array[i], val);
    }

    free(key_array);
  }

  ht_destroy(&aggregations);

  return EXIT_OKAY;
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

int print_keys_and_agg_vals(char *key, struct aggregation *val) {
  int i;
  fputs(key, stdout);
  for (i = 0; i < conf.nsums; i++) {
    printf("%s%.*f", delim, conf.sum_precisions[i], val->sums[i]);
  }
  for (i = 0; i < conf.ncounts; i++) {
    printf("%s%d", delim, val->counts[i]);
  }
  for (i = 0; i < conf.naverages; i++) {
    printf("%s%.*f", delim, conf.average_precisions[i] + 2,
           val->average_sums[i] / val->average_counts[i]);
  }
  for (i = 0; i < conf.nmins; i++) {
    if (val->mins_initialized[i])
      printf("%s%.*f", delim, conf.min_precisions[i], val->numeric_mins[i]);
    else
      fputs(delim, stdout);
  }
  for (i = 0; i < conf.nmaxs; i++) {
    if (val->maxs_initialized[i])
      printf("%s%.*f", delim, conf.max_precisions[i], val->numeric_maxs[i]);
    else
      fputs(delim, stdout);
  }
  fputs("\n", stdout);
  return 0;
}

int ht_print_keys_and_agg_vals(void *htelem) {
  char *key;
  struct aggregation *val;
  key = ((ht_elem_t *) htelem)->key;
  val = ((ht_elem_t *) htelem)->data;
  return print_keys_and_agg_vals(key, val);
}

void extract_fields_to_string(char *line, char *destbuf, size_t destbuf_sz,
                              int *fields, size_t nfields, char *delim,
                              char *suffix) {
  char *pos;
  int i;
  size_t delim_len = 0, field_len = 0, suffix_len = 0;

  delim_len = strlen(delim);
  if (suffix)
    suffix_len = strlen(suffix);
  pos = destbuf;

  for (i = 0; i < nfields; i++) {
    field_len =
        get_line_field(pos, line, destbuf_sz - (pos - destbuf),
                       fields[i], delim);
    pos += field_len;
    if (suffix) {
    	strncat(pos, suffix, destbuf_sz - (pos - destbuf));
    	pos += suffix_len;
    }
    if (i != nfields - 1) {
    	strncat(pos, delim, destbuf_sz - (pos - destbuf));
      pos += delim_len;
    }
  }
}

void decrement_values(int *array, size_t sz) {
  int j;
  if (array == NULL || sz == 0)
    return;
  for (j = 0; j < sz; j++) {
    array[j]--;
  }
}

struct aggregation *alloc_agg(int nsum, int ncount, int naverage, int nmin,
                              int nmax) {
  struct aggregation *agg;

  agg = malloc(sizeof(struct aggregation));
  if (!agg)
    goto alloc_agg_error;
  memset(agg, 0, sizeof(struct aggregation));

  if (nsum > 0) {
    agg->sums = malloc(sizeof(double) * nsum);
    if (!agg->sums)
      goto alloc_agg_error;
    memset(agg->sums, 0, sizeof(double) * nsum);
  }

  if (ncount > 0) {
    agg->counts = malloc(sizeof(u_int32_t) * ncount);
    if (!agg->counts)
      goto alloc_agg_error;
    memset(agg->counts, 0, sizeof(u_int32_t) * ncount);
  }

  if (naverage > 0) {
    agg->average_sums = malloc(sizeof(double) * naverage);
    if (!agg->average_sums)
      goto alloc_agg_error;
    memset(agg->average_sums, 0, sizeof(double) * naverage);

    agg->average_counts = malloc(sizeof(u_int32_t) * naverage);
    if (!agg->average_counts)
      goto alloc_agg_error;
    memset(agg->average_counts, 0, sizeof(u_int32_t) * naverage);
  }

  if (nmin > 0) {
    agg->numeric_mins = malloc(sizeof(double) * nmin);
    if (! agg->numeric_mins)
      goto alloc_agg_error;
    memset(agg->numeric_mins, 0, sizeof(double) * nmin);
    agg->mins_initialized = malloc(sizeof(int) * nmin);
    if (! agg->mins_initialized)
      goto alloc_agg_error;
    memset(agg->mins_initialized, 0, nmin);
  }

  if (nmax > 0) {
    agg->numeric_maxs = malloc(sizeof(double) * nmax);
    if (! agg->numeric_maxs)
      goto alloc_agg_error;
    memset(agg->numeric_maxs, 0, sizeof(double) * nmax);
    agg->maxs_initialized = malloc(sizeof(int) * nmax);
    if (! agg->maxs_initialized)
      goto alloc_agg_error;
    memset(agg->maxs_initialized, 0, nmax);
  }
  return agg;

alloc_agg_error:
  if (agg) {
    if (agg->sums)
      free(agg->sums);
    if (agg->counts)
      free(agg->counts);
    if (agg->average_sums)
      free(agg->average_sums);
    if (agg->average_counts)
      free(agg->average_counts);
    free(agg);
  }
  return NULL;
}

void free_agg(struct aggregation *agg) {
  if (!agg)
    return;
  if (agg->counts)
    free(agg->counts);
  if (agg->sums)
    free(agg->sums);
  if (agg->average_sums)
    free(agg->average_sums);
  if (agg->average_counts)
    free(agg->average_counts);
  if (agg->numeric_mins)
    free(agg->numeric_mins);
  if (agg->numeric_maxs)
    free(agg->numeric_maxs);
  free(agg);
}
