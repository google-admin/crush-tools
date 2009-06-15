/********************************
  copyright
 ********************************/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h> /* sysconf() */
#endif

#include <ffutils.h>
#include <reutils.h>
#include <hashtbl.h>
#include "fieldsplit_main.h"

#ifdef HAVE_PCRE_H
char *re_pattern = NULL, *re_subst = NULL;
pcre *re = NULL;
pcre_extra *re_extra = NULL;
int re_flags = 0;
int subst_globally = 0;
struct crush_resubst_elem *compiled_subst = NULL;
size_t compiled_subst_sz = 0;
int n_subst_elems;

/** @brief parses user-supplied substitution and initializes related global
  * values.
  *
  * @arg subst_regex a Perl-style substitution regular expression.
  * @return Non-zero on error, or 0 on success.
  */
int init_xform_regex(char *subst_regex);
#endif

/** @brief turns an input field value into the value which will determine which
  * output file the row will go to.
  *
  * Empty field values map to "_blank_value".  White-spaces and slashes are
  * replaced with underscores.
  *
  * @arg data the input field value.
  * @arg copy target for the transformed version of the value.
  * @arg subst_buffer working space for regex substitutions.
  * @arg subst_buffer_sz size of subst buffer.
  *
  * @return copy, or NULL on error.
  */
char * transform_key(const char const *data, char *copy,
                     char **subst_buffer, size_t *subst_buffer_sz);

/** @brief hashes a string.
  * Used for bucketizing input into a limited number of output files.
  * @arg data a string
  * @return a hash of the string.
  */
unsigned int strhash32(unsigned char *data);

/** @brief handle output of rows and caching of file pointers.
  *
  * @arg args commandline options (needed for output filename construction).
  * @arg key output file identifier.
  * @arg line unmodified input row.
  * @arg header the first line of input if args->keep_header is true.
  */
void store_mapping(const struct cmdargs const *args,
                   const char *key, const char *line, const char *header);

/** @brief a wrapper around fclose() to pass to ht_call_for_each().
  *
  * @arg data a pointer to an fp_wrapper.
  * @return 0.
  */
int close_files(void *data);

/** @brief a wrapper around a file pointer to go inside the hashtable.
  * This allows us to keep an entry in the hashtable for files which have been
  * closed.
  */
struct fp_wrapper {
  FILE *fp;
};

hashtbl_t fileptr_cache; /**< @brief a filename-to-fp_wrapper lookup table. */


/** @brief application entry point.
  *
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  *
  * @return exit status for main() to return.
  */
int fieldsplit (struct cmdargs *args, int argc, char *argv[], int optind) {
  FILE *in_file;
  char *header = NULL, *line = NULL,
       *field = NULL, *field_key = NULL,
       *subst_buffer = NULL;
  size_t header_sz = 0, line_sz = 0, field_sz = 0, subst_buffer_sz = 0;
  int field_index, buckets = 0, bucket_len;
  long max_open_files;

  char default_delim[] = {0xfe, 0x00};

#ifdef HAVE_PCRE_H
  if (args->xform_names) {
    if (init_xform_regex(args->xform_names) != 0) {
      fprintf(stderr, "%s: fatal error parsing \"%s\"\n",
              getenv("_"), args->xform_names);
      exit(1);
    }
  }
#else
  if (args->xform_names) {
    fprintf(stderr, "%s was compiled without regular expression support.\n",
            getenv("_"));
    exit(1);
  }
#endif

  if (! args->field && ! args->field_label) {
    fprintf(stderr, "%s: either -f or -F must be specified.\n");
    exit(1);
  }

  if (! args->delim) {
    args->delim = getenv("DELIMITER");
  }
  if (! args->delim) {
    args->delim = default_delim;
  }
  expand_chars(args->delim);

  if (! args->field_label) {
    field_index = atoi(args->field) - 1;
  }
  if (args->buckets) {
    buckets = atoi(args->buckets);
    bucket_len = strlen(args->buckets);
  }

  field = malloc(128);
  field_key = malloc(128);
  field_sz = 128;

#ifdef HAVE_UNISTD_H
  max_open_files = sysconf(_SC_OPEN_MAX);
#else
  max_open_files = -1;
#endif
  if (max_open_files < 0) {
    max_open_files = FOPEN_MAX;
  }
  ht_init(&fileptr_cache, max_open_files, strhash32, free);

  if (optind == argc)
    in_file = stdin;
  else
    in_file = nextfile(argc, argv, &optind, "r");

  while (in_file) {
    if (args->keep_header || args->field_label) {
      if (getline(&header, &header_sz, in_file) > 0 && args->field_label) {
        int *index_list = NULL;
        size_t list_size = 0;
        if (expand_label_list(args->field_label, header, args->delim,
                              &index_list, &list_size) < 1) {
          fprintf(stderr, "%s: %s: error parsing field label argument.\n",
                  argv[0], in_file == stdin ? "<stdin>" : argv[optind - 1]);
          exit(1);
        }
        field_index = index_list[0] - 1;
      }
    }

    while (getline(&line, &line_sz, in_file) > 0) {
      while (get_line_field(field, line, field_sz,
                            field_index, args->delim) == field_sz) {
        if((field = realloc(field, field_sz + 32)) == NULL) {
          DIE("%s: out of memory.\n", getenv("_"));
        }
        if ((field_key = realloc(field_key, field_sz + 32)) == NULL) {
          DIE("%s: out of memory.\n", getenv("_"));
        }
        field_sz += 32;
      }
      transform_key(field, field_key, &subst_buffer, &subst_buffer_sz);
      if (buckets) {
        sprintf(field_key, "%.*d", bucket_len, strhash32(field_key) % buckets);
      }
      store_mapping(args, field_key, line, header);
    }
    in_file = nextfile(argc, argv, &optind, "r");
  }
  ht_call_for_each(&fileptr_cache, close_files);
  return EXIT_OKAY;
}


int close_files(void *data) {
  struct fp_wrapper *ht_entry;
  if (! data)
    return 0;
  ht_entry = (struct fp_wrapper *) data;
  if (ht_entry->fp) {
    fclose(ht_entry->fp);
    ht_entry->fp = NULL;
  }
  return 0;
}


char * transform_key(const char const *data, char *copy,
                     char **subst_buffer, size_t *subst_buffer_sz) {
  char *p;
  strcpy(copy, data);
  if (! *copy) {
    strcpy(copy, "_blank_value");
    return copy;
  }

  p = copy;
  while (*p) {
    if (isspace(*p) || *p == '/') {
      *p = '_';
    }
    p++;
  }
#ifdef HAVE_PCRE_H
  crush_re_substitute(re, re_extra, compiled_subst, n_subst_elems,
                      copy, re_subst, subst_buffer, subst_buffer_sz,
                      subst_globally);
  strcpy(copy, *subst_buffer);
#endif
  return copy;
}


/* Based on the hashing algorithm used in Perl 5.005
 * See http://www.perl.com/lpt/a/679 */
unsigned int strhash32(unsigned char *data) {
  char *p = (char *) data;
  u_int32_t value = 0;
  while (*p) {
    value = value * 33 + *p;
    p++;
  }
  return value;
}


void store_mapping(const struct cmdargs const *args,
                   const char *key, const char *line, const char *header) {
  char filename[FILENAME_MAX];
  FILE *out = NULL;
  struct fp_wrapper *ht_entry = NULL;
  struct fp_wrapper new_entry;

  sprintf(filename, "%s/%s%s", args->path, key,
          args->suffix ? args->suffix : "");

  ht_entry = ht_get(&fileptr_cache, filename);
  if (ht_entry)
    out = ht_entry->fp;

  /* If `ht_entry' is NULL, this is the first time to need to write to this
   * particular file.  If instead `ht_entry->fp' (out) is NULL, the file
   * existed but was closed to free up resources. */
  while (! out) {
    out = fopen(filename, "a");
    if (! out) {
      if (errno == EMFILE) {
        /** TODO(jhinds): Better than closing everything would be to close
          * N least-recently used files or something like that. */
        ht_call_for_each(&fileptr_cache, close_files);
        continue;
      } else {
        DIE("%s: %s: %s", getenv("_"), filename, strerror(errno));
      }
    }
    if (! ht_entry) {
      ht_entry = malloc(sizeof(struct fp_wrapper));
      ht_entry->fp = out;
      if (ht_put(&fileptr_cache, filename, ht_entry) < 0) {
        DIE("%s: out of memory", getenv("_"));
      }
      if (args->keep_header)
        fputs(header, out);
    }
  }
  fputs(line, out);
}

#ifdef HAVE_PCRE_H
int init_xform_regex(char *subst_regex) {
  const char *re_error;
  int re_err_offset;
  char *re_modifiers = NULL, *p;
  char re_separator;
  int is_escaped;

  if (subst_regex[0] != 's') {
    fprintf(stderr, "%s: unparseable regular expression \"%s\"\n",
            getenv("_"), subst_regex);
    return 1;
  }
  re_separator = subst_regex[1];
  re_pattern = subst_regex + 2;

  for (p = subst_regex + 2; *p; p++) {
    if (*p == '\\') {
      is_escaped = 1;
      continue;
    }
    if (is_escaped || *p != re_separator) {
      is_escaped = 0;
      continue;
    }
    *p = '\0';
    if (! re_subst)
      re_subst = p + 1;
    else if (! re_modifiers)
      re_modifiers = p + 1;
  }

  if (! re_subst) {
    fprintf(stderr, "%s: failed to parse substitution \"%s\"\n",
            getenv("_"), subst_regex);
    return 1;
  }

  re_flags = crush_re_make_flags(re_modifiers, &subst_globally);
  re = pcre_compile(re_pattern, re_flags, &re_error, &re_err_offset, NULL);
  if (! re) {
    fprintf(stderr, "%s: re compile failed: %s\n", getenv("_"), re_error);
    return 1;
  }
  re_extra = pcre_study(re, 0, &re_error);

  n_subst_elems = crush_resubst_compile(re_subst, &compiled_subst,
                                        &compiled_subst_sz);
  if (n_subst_elems < 0) {
    fprintf(stderr, "%s: substitution compile failed.\n", getenv("_"));
    return 1;
  }
  return 0;
}
#endif
