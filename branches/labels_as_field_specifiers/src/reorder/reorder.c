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
#include "reorder_main.h"
#include "reorder.h"

llist_t *swap_arg_list = NULL;

int reorder(struct cmdargs *args, int argc, char *argv[], int optind) {
  FILE *fp, *fpout;
  dbfr_t *reader;

  char *wbuf = NULL;            /* working buffer */
  size_t wbs = 0;               /* working buffer size */

  int *order = NULL;
  size_t order_sz = 0;
  size_t order_elems = 0;

  llist_t swap_list;

  char default_delim[] = { 0xfe, 0x00 };

  if (!args->delim) {
    args->delim = getenv("DELIMITER");
    if (!args->delim)
      args->delim = default_delim;
  }
  expand_chars(args->delim);

  /* may add output option later */
  fpout = stdout;

  if (optind == argc)
    fp = stdin;
  else
    fp = nextfile(argc, argv, &optind, "r");

  reader = dbfr_init(fp);

  if (reader == NULL)
    return EXIT_FILE_ERR;

  if (args->fields) {
    order_elems = expand_nums(args->fields, &order, &order_sz);

    if (args->verbose) {
      int i;
      fprintf(stderr, "there are %d fields: ", order_elems);
      for (i = 0; i < order_elems; i++) {
        fprintf(stderr, " %d", order[i]);
      }
      fprintf(stderr, "\n");
    }
  } else if (args->field_labels) {
    order_elems = expand_label_list(args->field_labels, reader->next_line,
                                    args->delim, &order, &order_sz);
    if (order_elems == -1) {
      fprintf(stderr, "%s: one or more labels in -F were not found.\n",
              getenv("_"));
      return EXIT_FAILURE;
    } else if (order_elems < 1) {
      fprintf(stderr, "%s: error translating labels in -F.\n",
              getenv("_"));
      return EXIT_FAILURE;
    }
    if (args->verbose) {
    	int idx;
    	fprintf(stderr, "%s: %d field translated from labels: ",
              getenv("_"), order_elems);
    	for (idx = 0; idx < order_elems; idx++) {
    		fprintf(stderr, " %d", order[idx]);
      }
      fputs("\n", stderr);
    }
  } else if (swap_arg_list) {
    ll_list_init(&swap_list, free, NULL);
    if (parse_swap_list(swap_arg_list, &swap_list, reader->next_line,
                        args->delim) != 0) {
      return EXIT_FAILURE;
    }
  }

  while (fp != NULL) {
    while (dbfr_getline(reader) > 0) {
      /* make sure there's enough room in the working buffer */
      if (wbuf == NULL) {
        if ((wbuf = malloc(reader->current_line_sz)) == NULL) {
          fprintf(stderr, "%s: out of memory.\n", getenv("_"));
          return EXIT_MEM_ERR;
        }
        wbs = reader->current_line_sz;
      } else if (wbs < reader->current_line_sz) {
        /* if realloc unsuccessful, we don't want wbuf to end up being NULL */
        char *tmp_ptr;
        if ((tmp_ptr = realloc(wbuf, reader->current_line_sz)) == NULL) {
          fprintf(stderr, "%s: out of memory.\n", getenv("_"));
          return EXIT_MEM_ERR;
        }
        wbuf = tmp_ptr;
        wbs = reader->current_line_sz;
      }

      if (!args->fields && !args->field_labels) {
        doswap(&swap_list, wbuf, reader->current_line, args->delim);
      } else {
        if (docut(&wbuf, reader->current_line, &wbs, args->delim,
                  order, order_elems) < 0) {
          fprintf(stderr, "%s: out of memory.\n", getenv("_"));
          return EXIT_MEM_ERR;
        }
      }
      fputs(wbuf, fpout);
      memset(wbuf, 0, wbs);
    }

    dbfr_close(reader);
    fp = nextfile(argc, argv, &optind, "r");
    if (fp) {
      reader = dbfr_init(fp);
      if (args->field_labels) {
        order_elems = expand_label_list(args->field_labels, reader->next_line,
                                        args->delim, &order, &order_sz);
        if (order_elems == -1) {
          fprintf(stderr, "%s: one or more labels in -F were not found.\n",
                  getenv("_"));
          return EXIT_FAILURE;
        } else if (order_elems < 1) {
          fprintf(stderr, "%s: error translating labels in -F.\n",
                  getenv("_"));
          return EXIT_FAILURE;
        }
      } else if (swap_arg_list) {
        ll_destroy(&swap_list);
        ll_list_init(&swap_list, free, NULL);
        if (parse_swap_list(swap_arg_list, &swap_list, reader->next_line,
                            args->delim) != 0) {
          return EXIT_FAILURE;
        }
      }
      /* TODO(jhinds): should the first line of subsequent files be tossed
       * if labels were used? */
    }
  }

  fflush(fpout);
  fclose(fpout);
  return EXIT_OKAY;
}

void doswap(llist_t *list, char *s, char *ct, const char *d) {
  llist_node_t *cur_node;
  struct swap_pair *pair;
  /* beginning and end positions for fields a and b */
  char *as, *ae, *bs, *be;

  size_t sl, dl;                /* string and delimiter length */
  size_t num_fields;            /* the number of fields in the line */

  sl = strlen(ct);
  dl = strlen(d);
  num_fields = fields_in_line(ct, d);

  chomp(ct);

  for (cur_node = list->head; cur_node; cur_node = cur_node->next) {
    pair = (struct swap_pair *) cur_node->data;
    if (pair->pair[0] > num_fields || pair->pair[1] > num_fields)
      continue;

    memset(s, 0, sl + 1);
    as = bs = ct;

    as = field_start(ct, (size_t) pair->pair[0], d);
    assert(as != NULL);

    bs = field_start(ct, (size_t) pair->pair[1], d);
    assert(bs != NULL);

    ae = strstr(as, d);
    be = strstr(bs, d);

    /* end of A is end of string - should never happen for swaps */
    if (ae == NULL && pair->action_type == REORDER_TYPE_SWAP)
      continue;
    else if (ae != NULL && pair->action_type == REORDER_TYPE_MOVE)
      ae += dl;
    else if (ae == NULL)
      ae = ct + sl;

    /* end of B is end of string - B is last field */
    if (be == NULL)
      be = ct + sl;
    else if (pair->action_type == REORDER_TYPE_MOVE)
      be += dl;

#ifdef CRUSH_DEBUG
    fprintf(stderr, "doswap: beginning of A is at %d, A is %d long\n", as - ct,
            ae - as);
    fprintf(stderr, "doswap: beginning of B is at %d, B is %d long\n", bs - ct,
            be - bs);
#endif

    /* note: the use of strncpy() below is safe because
       s has been nulled out above */
    if (pair->action_type == REORDER_TYPE_SWAP) {
      /* A is lower-indexed field, B is higher-indexed field */

      strncpy(s, ct, as - ct);  /* copy up to field A */
      strncat(s, bs, be - bs);  /* append field B */
      strncat(s, ae, bs - ae);  /* append everything between end of A and beginning of B */
      strncat(s, as, ae - as);  /* append field A */
      strcat(s, be);            /* append everything after b */

    } else if (pair->action_type == REORDER_TYPE_MOVE) {

      /* A is field to be moved, B is destination field */

      if (bs > as) {            /* ---- moving A toward end of line ---- */

        strncpy(s, ct, as - ct);  /* copy up to field A */

        strncat(s, ae, be - ae);  /* append everything between end of A and end of B */

        if (be == ct + sl) {    /* if putting A on the end of the string, */
          strcat(s, d);         /*  append a delimiter to the dest. string first */
          as[ae - as - dl] = 0x00;  /*  drop the delimiter at the end of A */
        }
        strncat(s, as, ae - as);  /* append field A */
        strncat(s, be, sl - (be - ct)); /* append from end of B to the end of line. */

      } else {                  /* ---- moving A toward beginning of line ---- */

        strncpy(s, ct, bs - ct);  /* copy up to field B */

        strncat(s, as, ae - as);  /* append field A */
        if (ae == ct + sl) {    /* if moving A from the end of the line */
          strcat(s, d);         /*  append a delimiter to end of line */
          be -= dl;             /*  move end of B before the delimiter */
          *(as - dl) = 0x00;    /*  remove the delimiter before a */
        }

        strncat(s, bs, as - bs);  /* append from start of B to the start of A. */
        if (ae != ct + sl)      /* if A was the last field, no need to copy stuff after it */
          strcat(s, ae);
      }
    } /* end REORDER_TYPE_MOVE */
    
    /* copy the modified version into original.  required for multiple
     * moves/swaps */
    strcpy(ct, s);

  }  /* end for loop through all swap/move field pairs */

  strcat(ct, "\n");
  strcat(s, "\n");
}



int docut(char **s, const char *ct, size_t * s_sz, const char *d,
          const int *order, const size_t n) {
  int i;
  char buffer[512];
  size_t s_len;
  size_t buf_len;
  size_t delim_len;

  /* make sure the destination buffer is allocated */
  if (*s == NULL || *s_sz == 0) {
    if ((*s = malloc(strlen(ct))) == NULL) {
      *s_sz = 0;
      return -1;
    }
    *s_sz = strlen(ct);
  }

  (*s)[0] = '\0';
  s_len = 0;
  delim_len = strlen(d);

  for (i = 0; i < n; i++) {

    if ((buf_len = get_line_field(buffer, ct, 511,
                                  order[i] - 1, (char *) d)) >= 0) {

      if (*s_sz < s_len + buf_len + delim_len) {
        char *tmp;
        /* include room for a null terminator and line break. */
        tmp = realloc(*s, *s_sz + buf_len + delim_len + 2);
        if (tmp == NULL)
          return -1;
        *s = tmp;
        *s_sz += buf_len + delim_len + 2;
      }

      chomp(buffer);
      strcat(*s, buffer);
      s_len += buf_len;

      if (i < n - 1) {
        strcat(*s, d);
        s_len += delim_len;
      }
    }
  }
  strcat(*s, "\n");

  return s_len + 1;
}


/* stores --swap and --move options in an ordered list for parsing later. */
int pushswap(char *s, int action_type) {
  struct swap_arg *p = malloc(sizeof(struct swap_arg));
  p->pair_str = strdup(s);
  p->action_type = action_type;

  if (swap_arg_list == NULL) {
    swap_arg_list = malloc(sizeof(llist_t));
    ll_list_init(swap_arg_list, free, NULL);
  }

  ll_append_elem(swap_arg_list, p);

  return 0;
}

int parse_swap_list(llist_t *args, llist_t *pairs,
                    const char *header, const char *delim) {
  /* for each move/swap argument in args, 
   * 1) identify substrings before and after the comma.
   * 2) if a substring is strictly numeric, treat it as an index,
   *    else treat it as a label.
   * 3) store the pair of indexes as a struct swap_pair in the pairs list.
   */

  llist_node_t *cur_node;
  char *pair_elem_a, *pair_elem_b;

  for (cur_node = args->head; cur_node; cur_node = cur_node->next) {
    struct swap_arg *p = (struct swap_arg *) cur_node->data;
    struct swap_pair *cur_pair;

    pair_elem_a = p->pair_str;
    pair_elem_b = strchr(p->pair_str, ',');
    if (! pair_elem_b) {
      fprintf(stderr, "%s: invalid field pair: %s\n",
              getenv("_"), p->pair_str);
      return 1;
    }
    *pair_elem_b = '\0';
    pair_elem_b++;

    cur_pair = malloc(sizeof(struct swap_pair));
    cur_pair->action_type = p->action_type;

    if (strspn(pair_elem_a, "0123456789") == strlen(pair_elem_a)) {
      cur_pair->pair[0] = atoi(pair_elem_a);
    } else {
      cur_pair->pair[0] = field_str(pair_elem_a, header, delim) + 1;
    }
    if (cur_pair->pair[0] < 1) {
      fprintf(stderr, "%s: invalid field pair: %s,%s\n",
              getenv("_"), pair_elem_a, pair_elem_b);
      return 1;
    }

    if (strspn(pair_elem_b, "0123456789") == strlen(pair_elem_b)) {
      cur_pair->pair[1] = atoi(pair_elem_b);
    } else {
      cur_pair->pair[1] = field_str(pair_elem_b, header, delim) + 1;
    }
    if (cur_pair->pair[1] < 1) {
      fprintf(stderr, "%s: invalid field pair: %s,%s => %d,%d\n",
              getenv("_"), pair_elem_a, pair_elem_b);
      return 1;
    }

    if (cur_pair->pair[0] == cur_pair->pair[1]) {
      /* this swap/move is a no-op */
      free(cur_pair);
      continue;
    }

    if (p->action_type == REORDER_TYPE_SWAP &&
        cur_pair->pair[0] > cur_pair->pair[1]) {
      /* for swaps, doswap() assumes the fields are ordered */
      int tmp = cur_pair->pair[0];
      cur_pair->pair[0] = cur_pair->pair[1];
      cur_pair->pair[1] = tmp;
    }
    ll_append_elem(pairs, cur_pair);
  }
  return 0;
}
