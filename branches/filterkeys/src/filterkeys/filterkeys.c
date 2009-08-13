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
#include <crush/ffutils.h>
#include <crush/bstree.h>
#include <crush/dbfr.h>
#include "filterkeys_main.h"
#include "filterkeys.h"

char default_delim[2] = { 0xfe, 0x00 };
char *delim;
struct fkeys_conf fk_conf;

/** @brief
 *
 * @param args contains the parsed cmd-line options & arguments.
 * @param argc number of cmd-line arguments.
 * @param argv list of cmd-line arguments
 * @param optind index of the first non-option cmd-line argument.
 *
 * @return exit status for main() to return.
 */
int filterkeys(struct cmdargs *args, int argc, char *argv[], int optind) {
  FILE *ffile, *outfile;
  dbfr_t *ffile_reader;
  char *t_keybuf;
  int i, acum_len;

  // setup
  if (!args->keys && !args->key_labels) {
    fprintf(stderr, "%s: -k or -K must be specified.\n", argv[0]);
    return EXIT_HELP;
  }

  if (args->outfile) {
    if ((outfile = fopen(args->outfile, "w")) == NULL) {
      perror(args->outfile);
      exit(EXIT_FILE_ERR);
    }
  } else
    outfile = stdout;

  /* choose field delimiter */
  if (!(delim = (args->delim ? args->delim : getenv("DELIMITER"))))
    delim = default_delim;
  else if(!args->delim)
    expand_chars(delim);

  if (!(ffile = nextfile(argc, argv, &optind, "r")))
    return EXIT_FILE_ERR;

  ffile_reader = dbfr_init( ffile );
  if (configure_filterkeys(&fk_conf, args, ffile_reader, delim) != 0) {
    fprintf(stderr, "%s: error setting up configuration.\n", argv[0]);
    return EXIT_HELP;
  }
  dbfr_close( ffile_reader );

  /* filter */
  if (!(ffile = (optind < argc ? nextfile(argc, argv, &optind, "r") : stdin)))
    return EXIT_FILE_ERR;

  ffile_reader = dbfr_init( ffile );
  if (args->preserve_header) {
    if (dbfr_getline(ffile_reader) > 0)
      fputs(ffile_reader->current_line, outfile);
  }

  t_keybuf = (char*)xmalloc( fk_conf.key_count * MAX_FIELD_SIZE );
  while( ffile ) {
    while( dbfr_getline(ffile_reader) > 0 ) {

      /* TODO(rgranata): check for -1 returning from get_line_field */
      for( acum_len = 0, i = 0; i < fk_conf.key_count; i++ ) {
        acum_len += get_line_field(t_keybuf + acum_len,
                ffile_reader->current_line,
                MAX_FIELD_SIZE, fk_conf.indexes[i], delim);
      }

      if (acum_len > 0) {
        if (!!bst_find(&fk_conf.ftree, t_keybuf) != !!args->invert)
          fputs(ffile_reader->current_line, outfile);
      }
    }

    dbfr_close(ffile_reader);
    if ((ffile = nextfile(argc, argv, &optind, "r"))) {
      ffile_reader = dbfr_init( ffile );
      if (args->preserve_header)
        dbfr_getline(ffile_reader);
    }
  }
  if (t_keybuf)
    free(t_keybuf);

  bst_destroy(&conf->ftree);

  return 0;
}


/* lookup the fields to be used as filter */
static int configure_filterkeys(struct fkeys_conf *conf, struct cmdargs *args,
    dbfr_t *ffile_reader, const char *delim) {
  char *t_keybuf;
  int i, acum_len;

  memset(conf, 0x0, sizeof(struct fkeys_conf));

  /* parse header for keys */
  dbfr_getline(ffile_reader);
  if (args->keys)
    conf->key_count = expand_nums(args->keys, &conf->indexes, &conf->size);
  else if (args->key_labels) {
    conf->key_count = expand_label_list(args->key_labels,
            ffile_reader->current_line,
            delim, &conf->indexes, &conf->size);
  }
  for( i = 0; i < conf->key_count; i++ )
    conf->indexes[i]--;
  if(conf->key_count < 1)
    return conf->key_count;

  /* load filters */
  bst_init(&conf->ftree, (int (*)(const void*, const void*))strcmp, free);
  while( dbfr_getline(ffile_reader) > 0 ) {

    t_keybuf = (char*)xmalloc( conf->key_count * MAX_FIELD_SIZE);
    /* TODO(rgranata): check for -1 returning from get_line_field */
    for( acum_len = 0, i = 0; i < conf->key_count; i++ )
      acum_len += get_line_field(t_keybuf + acum_len,
              ffile_reader->current_line,
              MAX_FIELD_SIZE, conf->indexes[i], delim);
    if (acum_len > 0) {
      bst_insert(&conf->ftree, t_keybuf);
    }
  }

  return 0;
}
