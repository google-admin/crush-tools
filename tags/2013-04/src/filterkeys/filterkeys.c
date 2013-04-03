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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <crush/general.h>
#include <crush/ffutils.h>
#include <crush/dbfr.h>
#include "filterkeys_main.h"
#include "filterkeys.h"

#ifndef HAVE_OPEN64
#define open64 open
#endif

char default_delim[2] = { 0xfe, 0x00 };
char *delim;
size_t delim_len = 0;
struct fkeys_conf fk_conf;

/* parse key fields */
static int configure_filterkeys(struct fkeys_conf *conf,
                                struct cmdargs *args,
                                dbfr_t *filter_reader,
                                dbfr_t *stream_reader) {
  size_t arrsz=0, brrsz=0;
  int i, j;

  memset(conf, 0x0, sizeof(struct fkeys_conf));

  if (args->key_labels) {

    dbfr_getline(filter_reader);
    dbfr_getline(stream_reader);

    conf->key_count = expand_label_list(args->key_labels,
        filter_reader->current_line,
        delim, &conf->aindexes, &arrsz);

    conf->key_count = expand_label_list(args->key_labels,
                                        stream_reader->current_line,
                                        delim, &conf->bindexes, &brrsz);
    /* preserve header implied */
    args->preserve_header = 1;

  } else if (args->akeys && args->bkeys) {
    ssize_t akeyct, bkeyct;

    akeyct = expand_nums(args->akeys, &conf->aindexes, &arrsz);
    bkeyct = expand_nums(args->bkeys, &conf->bindexes, &brrsz);

    if (akeyct != bkeyct) {
      fprintf(stderr, "a-key and b-key lists must be the same length.\n");
      return EXIT_HELP;
    } else {
      conf->key_count = akeyct;
    }

  } else {
    dbfr_getline(filter_reader);
    dbfr_getline(stream_reader);

    char label_left[MAX_FIELD_LEN + 1], label_right[MAX_FIELD_LEN + 1];
    int nfields_filter = fields_in_line(filter_reader->current_line, delim);
    int nfields_stream = fields_in_line(stream_reader->current_line, delim);

    j = (nfields_filter < nfields_stream ? nfields_filter : nfields_stream);
    conf->aindexes = (int*)malloc(sizeof(int) * j);
    conf->bindexes = (int*)malloc(sizeof(int) * j);

    /* find the keys common to both files */
    for (i = 0; i < nfields_filter; i++)
      for (j = 0; j < nfields_stream; j++) {
        get_line_field(label_left, filter_reader->current_line,
            MAX_FIELD_LEN, i, delim);
        get_line_field(label_right, stream_reader->current_line,
            MAX_FIELD_LEN, j, delim);

        if (strcmp(label_left, label_right) == 0) {
          conf->aindexes[conf->key_count] = i+1;
          conf->bindexes[conf->key_count] = j+1;
          conf->key_count++;
          break;
        }
      }

    /* preserve header implied */
    args->preserve_header = 1;
  }

  for (i = 0; i < conf->key_count; i++) {
    conf->aindexes[i]--;
    conf->bindexes[i]--;
  }

  return (conf->key_count < 1 ? conf->key_count : 0);
}

/* reconfigure_filterkeys() */

/* load the filter from the filter file */
static int load_filter(struct fkeys_conf *conf, dbfr_t *filter_reader) {
  char *t_keybuf;
  int i, acum_len;

  ht_init(&conf->filter, 1024, NULL, NULL);
  while (dbfr_getline(filter_reader) > 0) {

    t_keybuf = (char *) xmalloc(filter_reader->current_line_sz);
    for (acum_len = 0, i = 0; i < conf->key_count; i++) {
      acum_len += get_line_field(t_keybuf + acum_len,
                                 filter_reader->current_line,
                                 filter_reader->current_line_sz - acum_len,
                                 conf->aindexes[i], delim);
      if (i != conf->key_count -1) {
        strcat(t_keybuf + acum_len, delim);
        acum_len += delim_len;
      }
    }
    if (acum_len > 0)
      ht_put(&conf->filter, t_keybuf, (void*)0xDEADBEEF);
      //bst_insert(&conf->ftree, t_keybuf);
  }
  conf->key_buffer_sz = filter_reader->current_line_sz;

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
int filterkeys(struct cmdargs *args, int argc, char *argv[], int optind) {
  FILE *ffile, *outfile;
  dbfr_t *filter_reader, *stream_reader;
  char *t_keybuf;
  int i, acum_len;

  if (args->outfile) {
    if ((outfile = fopen(args->outfile, "w")) == NULL) {
      perror(args->outfile);
      exit(EXIT_FILE_ERR);
    }
  } else {
    outfile = stdout;
  }

  /* choose field delimiter */
  if (!(delim = (args->delim ? args->delim : getenv("DELIMITER"))))
    delim = default_delim;
  expand_chars(delim);
  delim_len = strlen(delim);

  /* get the filter file */
  int fd = open64(args->filter_file, O_RDONLY);
  if (fd != -1) {
    ffile = fdopen(fd, "r");
  } else {
    if (!strcmp(args->filter_file, "-")) {
      ffile = stdin;
    } else {
      warn("Opening filter file %s", args->filter_file);
      return EXIT_FILE_ERR;
    }
  }
  filter_reader = dbfr_init( ffile );

  /* input files */
  if (!(ffile = (optind < argc ? nextfile(argc, argv, &optind, "r") : stdin)))
    return EXIT_FILE_ERR;
  stream_reader = dbfr_init( ffile );


  if (configure_filterkeys(&fk_conf, args, filter_reader, stream_reader) != 0) {
    fprintf(stderr, "%s: error setting up configuration.\n", argv[0]);
    return EXIT_HELP;
  }

  load_filter(&fk_conf, filter_reader);
  dbfr_close( filter_reader );

  if (args->preserve_header) {
    /* if indexes where supplied read the header */
    if (args->akeys && args->bkeys)
      dbfr_getline(stream_reader);
    fputs(stream_reader->current_line, outfile);
  }

  t_keybuf = (char *) xmalloc(fk_conf.key_buffer_sz);
  while (ffile) {
    while (dbfr_getline(stream_reader) > 0) {

      for (acum_len = 0, i = 0;
           i < fk_conf.key_count && fk_conf.key_buffer_sz > acum_len;
           i++) {
        acum_len += get_line_field(t_keybuf + acum_len,
                                   stream_reader->current_line,
                                   fk_conf.key_buffer_sz - acum_len,
                                   fk_conf.bindexes[i], delim);
        if (i != fk_conf.key_count -1) {
          strcat(t_keybuf + acum_len, delim);
          acum_len += delim_len;
        }
      }

      if (acum_len > 0) {
        int found = (ht_get(&fk_conf.filter, t_keybuf) ==
                     (void*) 0xDEADBEEF ? 1 : 0);
        if (found ^ args->invert)
          fputs(stream_reader->current_line, outfile);
      }
    }

    dbfr_close(stream_reader);
    if ((ffile = nextfile(argc, argv, &optind, "r"))) {
      stream_reader = dbfr_init( ffile );
      /* reconfigure fields (needed if labels were used) */
      /* TODO(rgranata): implement reconfigure field
      if (reconfigure_filterkeys(&fk_conf, args, NULL, stream_reader) != 0) {
        fprintf(stderr, "%s: error parsing field arguments.\n", argv[0]);
        return EXIT_HELP;
      }
      */
      if (args->preserve_header)
        dbfr_getline(stream_reader);
    }
  }
  if (t_keybuf)
    free(t_keybuf);

  ht_destroy(&fk_conf.filter);

  return 0;
}
