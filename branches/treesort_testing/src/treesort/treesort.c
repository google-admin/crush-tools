/********************************
   Copyright 2009 Google Inc.

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
#include <crush/general.h>

#include "treesort_main.h"
#include "treesort.h"

#define INITIAL_BUF_SZ 512

char *delim;
struct tsort_conf conf;
static char *keysbuf;
static size_t keysbuf_sz = 0;

int configure_treesort(struct tsort_conf *conf,
                       struct cmdargs *args, const char *header) {
  size_t sz = 0, i, j, k = 0;
  char *fval;

  /* parse traversal order */
  if (!(sz = fields_in_line((args->keys ? args->keys : args->key_labels), ",")))
    return 1;

  conf->order = xmalloc(sizeof(traversal_order_t) * sz);
  for (j = 0; j < sz; j++) {
    fval =
      field_start((args->keys ? args->keys : args->key_labels), j + 1, ",");
    if (fval[0] == '^') {
      conf->order[j] = reversed;
      memmove(fval, fval + 1, strlen(fval)); // strlen considers here the \0
    } else
      conf->order[j] = inorder;
  }

  if (args->keys) {
    conf->keys_ct = expand_nums(args->keys, &conf->keys, &conf->keys_sz);
  } else if (args->key_labels) {
    args->preserve = 1;
    conf->keys_ct = expand_label_list(args->key_labels, header,
                                      delim, &conf->keys, &conf->keys_sz);
  }
  if (conf->keys_ct < 0)
    return conf->keys_ct;
  for (j = 0; j < conf->keys_ct; j++)
    conf->keys[j]--;

  /* get non-key fields */
  sz = fields_in_line(header, delim);
  conf->rest_ct = sz - conf->keys_ct;
  conf->rest = xmalloc(sizeof(int) * conf->rest_ct);

  for (j = 0; j < sz; j++) {
    for (i = 0; i < conf->keys_ct; i++) {
      if (conf->keys[i] == j)
        break;
    }
    if (i == conf->keys_ct)
      conf->rest[k++] = j;
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
int treesort(struct cmdargs *args, int argc, char *argv[], int optind) {

  FILE *in;                     /* input file */
  dbfr_t *in_reader;
  int i;

  char default_delim[] = { 0xFE, 0x00 };  /* default delimiter string */

  if (! args->keys && ! args->key_labels) {
    fprintf(stderr, "%s: -k or -K must be specified\n", argv[0]);
    return EXIT_HELP;
  }

  tsort_init(&conf);

  /* choose field delimiter */
  if (!(delim = (args->delim ? args->delim : getenv("DELIMITER"))))
    delim = default_delim;
  expand_chars(delim);

  /* input stream selection */
  if (!(in = (optind == argc ? stdin : nextfile(argc, argv, &optind, "r"))))
    return EXIT_FILE_ERR;

  in_reader = dbfr_init(in);
  if (configure_treesort(&conf, args, in_reader->next_line) != 0) {
    fprintf(stderr, "%s: error parsing field arguments.\n", argv[0]);
    return EXIT_HELP;
  }

  if (args->uniq)
    conf.unique = 1;

  char **keys_idx = xmalloc((conf.keys_ct + conf.rest_ct) * sizeof(char*));
  char *elem, *field;
  size_t dlen = strlen(delim);
  size_t keylen = 0;
  bstree_t *subtree = NULL;
  stree_t *nextree = NULL, bogus_cmp;
  bst_node_t *ret;

  keysbuf_sz = INITIAL_BUF_SZ;
  keysbuf = xmalloc(keysbuf_sz * sizeof(char));
  memset(keysbuf, 0, keysbuf_sz);

  if (args->preserve) {
    if (dbfr_getline(in_reader) > 0) {
      if (in_reader->current_line_len > keysbuf_sz)
        keysbuf = xrealloc(keysbuf, (keysbuf_sz *= 2));

      for (i = 0; i < conf.keys_ct; i++) {
        keylen += get_line_field(keysbuf + keylen, in_reader->current_line,
            keysbuf_sz, conf.keys[i], delim);
        strncat(keysbuf + keylen, delim, dlen);
        keylen += dlen;
      }
      for (i = 0; i < conf.rest_ct; i++) {
        keylen += get_line_field(keysbuf + keylen, in_reader->current_line,
            keysbuf_sz, conf.rest[i], delim);
        strncat(keysbuf + keylen, delim, dlen);
        keylen += dlen;
      }
      keysbuf[keylen - dlen] = '\n';
      keysbuf[keylen - dlen + 1] = '\0';
      fputs(keysbuf, stdout);
    }
  }

  keylen = 0;
  memset(keysbuf, 0, keysbuf_sz);

  /* loop through all files */
  while (in != NULL) {

    /* loop through each line of the file */
    while (dbfr_getline(in_reader) > 0) {
      chomp(in_reader->current_line);

      /* resize the buffer if necesary, keep the longest line size */
      if (in_reader->current_line_len > keysbuf_sz)
        keysbuf = xrealloc(keysbuf, (keysbuf_sz *= 2));

      /* delimit line keys */
      keys_idx[0] = in_reader->current_line;
      for (i = 1; i < conf.keys_ct + conf.rest_ct; i++) {
        keys_idx[i] = strstr(keys_idx[i - 1], delim);
        keys_idx[i][0] = '\0';
        keys_idx[i] += dlen;
      }

      /* search keyspace */
      for (i = 0, subtree = &conf.tree;
           i < conf.keys_ct && subtree;
           i++, subtree = &nextree->tree) {

        field = keys_idx[conf.keys[i]];
        bogus_cmp.key = field;
        if (!(ret = bst_find(subtree, &bogus_cmp))) {

          nextree = (stree_t*)xmalloc(sizeof(stree_t));

          memset(nextree, 0, sizeof(stree_t));
          keylen = strlen(field);
          nextree->key = (char*)xmalloc(keylen + 1);

          memcpy(nextree->key, field, keylen);
          nextree->key[keylen] = '\0';

          nextree->keylevel = conf.keys_ct - (i + 1);
          if (nextree->keylevel) {
            bst_init(&nextree->tree,
                     (int(*)(const void*, const void*))key_strcmp,
                     (void(*)(void*))stree_free);
          } else {
            ll_list_init(&nextree->list, free, NULL);
          }
          bst_insert(subtree, nextree);
        } else {
          nextree = (stree_t*)ret->data;
        }
      }
      if (!args->uniq || args->uniq && !nextree->list.nnodes) {
        for (i = 0; i < conf.rest_ct; i++)
          sprintf(keysbuf, "%s%s%s", keysbuf, keys_idx[conf.rest[i]], delim);

        if (conf.rest_ct)
          keylen = strlen(keysbuf) - dlen;
        else
          keylen = 0;

        elem = (char*)xmalloc(keylen + 1);

        memcpy(elem, keysbuf, keylen);
        elem[keylen] = '\0';
        ll_add_elem(&nextree->list, elem, end);
        memset(keysbuf, 0, keylen);
      }
    }

    dbfr_close(in_reader);

    if ((in = nextfile(argc, argv, &optind, "r"))) {
      in_reader = dbfr_init(in);
      /* reconfigure fields (needed if labels were used) */
      if (configure_treesort(&conf, args, in_reader->next_line) != 0) {
        fprintf(stderr, "%s: error parsing field arguments.\n", argv[0]);
        return EXIT_HELP;
      }
      if (args->preserve)
        dbfr_getline(in_reader);
    }
  }

  bst_call_for_each(&conf.tree, (void(*)(void*))print_data, conf.order[0]);

  if (keys_idx)
    free(keys_idx);

  tsort_destroy(&conf);

  return EXIT_OKAY;
}

void print_data(stree_t *t) {
  size_t clearbuf;
  llist_node_t *node;

  if (t->keylevel == 0) {
    clearbuf = strlen(keysbuf);

    if (t->list.head) {
      if (conf.rest_ct == 0)
        sprintf(keysbuf, "%s%s", keysbuf, t->key);
      else
        sprintf(keysbuf, "%s%s%s", keysbuf, t->key, delim);

      for (node = t->list.head; node; node = node->next)
        fprintf(stdout, "%s%s\n", keysbuf, (char*)node->data);
    } else {
      fprintf(stdout, "%s%s\n", keysbuf, t->key);
    }

    memset(keysbuf + clearbuf, 0, strlen(keysbuf) - clearbuf);

  } else {
    clearbuf = strlen(keysbuf);
    sprintf(keysbuf, "%s%s%s", keysbuf, t->key, delim);
    bst_call_for_each(&t->tree, (void(*)(void*))print_data,
                      conf.order[ conf.keys_ct - t->keylevel ]);
    memset(keysbuf + clearbuf, 0, strlen(keysbuf) - clearbuf);
  }
}

int key_strcmp(const stree_t *a, const stree_t *b) {
  /* avoid comparing nulls */
  if (!a && !b)
    return 0;
  if (!a && b)
    return -1;
  if (a && !b)
    return 1;

  return strcmp(a->key, b->key);
}

void stree_free(stree_t *t) {
  if (!t)
    return;
  if (t->key)
    free(t->key);
  if (t->keylevel == 0) {
    ll_destroy(&t->list);
  } else {
    bst_destroy(&t->tree);
  }
  free(t);
}

int tsort_init(struct tsort_conf *conf) {
  memset(conf, 0, sizeof(struct tsort_conf));
  bst_init(&conf->tree,
      (int(*)(const void*, const void*))key_strcmp,
      (void(*)(void*))stree_free);

  return 0;
}

void tsort_destroy(struct tsort_conf *conf) {
  if (!conf)
    return;
  bst_destroy(&conf->tree);
  if (conf->keys)
    free(conf->keys);
  memset(conf, 0, sizeof(struct tsort_conf));
}
