/*****************************************
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
 *****************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <crush/ffutils.h>
#include <crush/general.h>
#include <fcntl.h>              /* open64() and O_* flags */
#include <ctype.h>              /* isdigit(), isspace() */

#ifndef HAVE_OPEN64
#define open64 open
#endif

size_t fields_in_line(const char *l, const char *d) {
  char *p = (char *) l;
  size_t f = 1;
  size_t dl;

  if (l == NULL || d == NULL)
    return 0;

  dl = strlen(d);
  while ((p = strstr(p, d)) != NULL) {
    f++;
    p += dl;
  }
  return f;
}


/** @brief extract a field from a delimited string.
  *
  * copies at most <i>n</i> characters from field <i>i</i> of
  * <i>delim</i>-delimited string <i>ct</i> into <i>cs</i>.
  *
  * @param dest destination buffer
  * @param line delimited string
  * @param n max chars to copy into buffer
  * @param i field to be copied (0-based)
  * @param delim delimiter of ct
  *
  * @return number of chars copied into buffer, or -1 if i is greater
  * than the number of fields in ct
  */
int get_line_field(char *dest, const char *line, size_t n, int i,
                   const char *delim) {
  int field_len;
  char *fstart, *fend; /* start and end of field at index i */

  if (!(delim && delim[0])) {
    strncpy(dest, line, n);
    dest[n] = 0;
    return (strlen(dest));
  }

  fstart = field_start(line, i + 1, delim);
  if (fstart == NULL) {
    dest[0] = 0x00;
    return -1;
  }

  fend = strstr(fstart, delim);
  if (fend == NULL) {
    /* cast away const-ness of line */
    fend = (char *) line + strlen(line) - 1;
    while (*fend == '\n' || *fend == '\r')
      fend--;
    fend++;
  }

  /* fend is now pointing at the first char after the field */
  field_len = (fend - fstart > n - 1 ? n - 1 : fend - fstart);
  strncpy(dest, fstart, field_len);
  dest[field_len] = '\0';
  return field_len;
}


int copy_field(const char const *line, char **dest, size_t *dest_sz,
               size_t field_no, const char const *delim) {
  int len;
  char *start, *end;
  start = field_start(line, field_no + 1, delim);
  if (!start)
    return -1;
  end = strstr(start, delim);
  if (!end) {
    end = start + strlen(start);
    while (*(end - 1) == '\n' || *(end - 1) == '\r')
      end--;
  }

  len = end - start;

  if (*dest == NULL || dest_sz == 0) {
    *dest = xmalloc(len + 1);
    *dest_sz = len + 1;
  } else if (len + 1 > *dest_sz) {
    *dest = xrealloc(*dest, len + 1);
    *dest_sz = len + 1;
  }

  (*dest)[len] = '\0';
  strncpy(*dest, start, len);
  return len;
}


char *field_start(const char * const line, size_t fn, const char *delim) {
  int i;
  char *p = (char *) line; /* cast away constness */
  size_t dl = strlen(delim);

  for (i = 1; i < fn; i++) {
    p = strstr(p, delim);
    if (!p)
      return NULL;
    p += dl;
  }
  return p;
}


int mdyhms_datecmp(const char *a, const char *b) {
  /* if the years are equal, do a straight string comparison */
  if (!(a[6] - b[6]
        || a[7] - b[7]
        || a[8] - b[8]
        || a[9] - b[9]))
    return strcmp(a, b);

  /* if different years, compare only the years */
  return strncmp(a + 6, b + 6, 4);
}


int dmyhms_datecmp(const char *a, const char *b) {
  int cmp;

  cmp = strncmp(a + 6, b + 6, 4); /* compare years */
  if (cmp)
    return cmp;

  cmp = strncmp(a + 3, b + 3, 2); /* compare months */
  if (cmp)
    return cmp;

  return strcmp(a, b);          /* compare date and time */
}


void chomp(char *s) {
  int l = strlen(s) - 1;
  while (l >= 0 && (s[l] == '\n' || s[l] == '\r'))
    s[l--] = '\0';
}

void trim(char *str) {
  int i = strlen(str) - 1;
  for (; isspace(str[i]) && i > 0; i--)
    str[i] = '\0';
  return;
}

/* get's the next file specified in the trailing commandline args */
FILE *nextfile(int argc, char *argv[], int *optind, const char *mode) {
  FILE *fp = NULL;
  int fd_flags = 0;             /* file open flags */
  int fd;

  if (strchr(mode, '+'))
    fd_flags |= O_RDWR;
  else if (strchr(mode, 'r'))
    fd_flags |= O_RDONLY;
  else if (strchr(mode, 'w'))
    fd_flags |= O_WRONLY | O_CREAT;
  else if (strchr(mode, 'a'))
    fd_flags |= O_WRONLY | O_CREAT | O_APPEND;

#ifdef O_LARGEFILE
  fd_flags |= O_LARGEFILE;
#endif

  if (*optind < argc && str_eq("-", argv[*optind])) {
    (*optind)++;
    if (fd_flags & (O_WRONLY | O_RDWR))
      return stdout;
    else
      return stdin;
  }

  while (*optind < argc) {
    if ((fd = open64(argv[(*optind)++], fd_flags)) != -1) {
      fp = fdopen(fd, mode);
      break;
    }
    perror(argv[*optind - 1]);
  }

  return fp;
}


void expand_chars(char *s) {
  char *w,                      /* a working copy of the string */
   *c, *p,                      /* position markers w/in the string */
   *e;                          /* the expansion */

  if (strchr(s, '\\') == NULL)
    return;

  w = xmalloc(strlen(s));
  memset(w, 0, strlen(s));

  p = s;
  while ((c = strchr(p, '\\')) != NULL) {
    strncat(w, p, c - p);
    switch (*(c + 1)) {
      case 'a':
        e = "\a";
        break;
      case 'b':
        e = "\b";
        break;
      case 'f':
        e = "\f";
        break;
      case 'n':
        e = "\n";
        break;
      case 'r':
        e = "\r";
        break;
      case 't':
        e = "\t";
        break;
      case 'v':
        e = "\v";
        break;
      case '\\':
        e = "\\";
        break;
      default:
        e = c + 1;
        break;
    }
    strncat(w, e, 1);
    p = c = c + 2;

  }
  strcat(w, p);  /* copy everything after the last escape */
  strcpy(s, w);  /* put the working copy into the orignal */

  free(w);
}


/* used in expand_nums() */
static size_t arr_resize(void **array,
                         size_t dsize, size_t oldsize, size_t add) {
  xrealloc(*array, dsize * (oldsize + add));
  return (oldsize + add);
}


ssize_t expand_nums(char *arg, int **array, size_t * array_size) {
  int i;
  char *token;

  if (arg == NULL || strlen(arg) == 0) {
    return 0;
  }

  /* check the string syntax */
  for (i = 0; arg[i] != '\0'; i++) {
    if ((!isdigit(arg[i]))
        && arg[i] != ',' && arg[i] != '-') {
      return -2;
    }
  }
  i = 0;

  if (*array == NULL && *array_size == 0) {
    *array = xmalloc(sizeof(int) * FFUTILS_RESIZE_AMT);
    *array_size = FFUTILS_RESIZE_AMT;
  }

  if (strchr(arg, ',') == NULL && strchr(arg, '-') == NULL) {
    sscanf(arg, "%u", &((*array)[0]));
    return 1;
  }

  token = strtok(arg, ",");

  while (token != NULL) {
    if (i >= *array_size) {
      if ((*array_size = arr_resize((void **) array, sizeof(int),
                                    *array_size, FFUTILS_RESIZE_AMT)) == 0) {
        return -1;
      }
    }

    if (strchr(token, '-') == NULL) {
      sscanf(token, "%u", &((*array)[i]));
      i++;
    } else {
      unsigned int i0, i1, ii;
      sscanf(token, "%u-%u", &i0, &i1);

      /* make sure the array is big enough to hold the range */
      if (*array_size < (i + i1 - i0)) {
        *array_size = arr_resize((void **) array,
                                 sizeof(int), *array_size, i1 - i0);
        if (*array_size == 0) {
          return -1;
        }
      }

      /* add all numbers in the range to the array */
      for (ii = i0; ii <= i1; ii++) {
        (*array)[i++] = ii;
      }
    }

    /* get the next token */
    token = strtok(NULL, ",");
  }
  return i;
}


ssize_t expand_label_list(const char *labels,
                          const char *line,
                          const char *delim,
                          int **array, size_t *array_sz) {
  int i = 0, j = 0;
  size_t labels_len = strlen(labels);
  char *labels_copy = xmalloc(labels_len + 1);
  size_t tokens = 0;
  char *pos, *labels_end;

  /* Tokenize the list of labels.  Possible escape sequences are
   * \\ - translates to a literal backslash
   * \, - translates to a literal comma
   *
   * Back-slash literals aren't required to be escaped unless they are
   * followed by a label-separating comma.
   */
  while (i < labels_len + 1) {
    if (labels[i] == '\0' || labels[i] == '\n' || labels[i] == '\r') {
      labels_copy[j] = '\0';
      tokens++;
      break;
    } else if (labels[i] == '\\') {
      if (labels[i+1] == '\\') {
        labels_copy[j] = '\\';
      } else if (labels[i+1] == ',') {
        labels_copy[j] = ',';
      } else {
        /* untranslatable escape sequence - copy it verbatim. */
        labels_copy[j++] = labels[i];
        labels_copy[j] = labels[i+1];
      }
      i += 2;
    } else if (labels[i] == ',') {
      /* label-separator comma */
      labels_copy[j] = '\0';
      tokens++;
      i++;
    } else {
      labels_copy[j] = labels[i];
      i++;
    }
    j++;
  }
  labels_end = labels_copy + j;

  /* make sure the array can hold all of the indexes */
  if (*array == NULL) {
    *array = xmalloc(sizeof(int) * tokens);
    *array_sz = tokens;
  } else {
    if (*array_sz < tokens) {
      *array_sz = arr_resize((void **) array, sizeof(int),
                             *array_sz, tokens - *array_sz);
      if (*array_sz == 0)
        return -2;
    }
  }

  j = 0;
  /* search for each label in the header line */
  for (pos = labels_copy; pos != labels_end + 1; pos += strlen(pos) + 1) {
    i = field_str(pos, line, delim);
    if (i < 0)
      return -1;
    (*array)[j++] = i + 1;
  }

  free(labels_copy);
  return j;
}


int get_line_pos(const char *line, int field_no, const char *d,
                 int *start, int *end) {
  char *field, *field_end;

  field = field_start(line, field_no + 1, d);
  if (field == NULL) {
    *start = -1;
    *end = -1;
    return -1;
  }

  *start = field - line;
  field_end = strstr(field, d);

  if (field_end == NULL) {
    /* last field of line.  comparison against *start
       handles the case where the field is empty. */
    *end = strlen(line) - 1;
    /* don't include linebreaks as field data. */
    while (line[*end] == '\n' || line[*end] == '\r')
      (*end)--;
    if (*end < *start)
      *end = *start;
  } else if (field_end == field) {
    /* empty field */
    *end = field_end - line;
  } else {
    *end = field_end - line - 1;
  }

  /* if start & end indexes are the same, the field could
     either be empty or a single charalineer */
  if (*start == *end) {

    /* if empty, start of field will either be a delimiter
       (if in the middle of the line) or an EOL or
       null terminator (end of line) */
    if (line[*start] == '\0'
        || line[*start] == '\n' || line[*start] == '\r'
        || strncmp(line + *start, d, strlen(d)) == 0) {
      return 0;
    }
    return 1;
  } else {
    return *end - *start + 1;
  }
}


ssize_t field_str(const char *value, const char *line, const char *delim) {
  char *curfield;       /* to hold fields from line */
  int max_field_chars;  /* size of curfield buffer */
  int curfield_len;     /* return value of get_line_field() */
  int i;                /* the index of the field being inspected */
  int found;            /* whether the value was found in line */

  /* no value to look for?  don't waste our time.
     but looking for an empty string may be valid. */
  if (value == NULL)
    return -2;

  /* undefined or empty line?  then it can't contain the value. */
  if (line == NULL || line[0] == '\0')
    return -1;

  /* no delimiter? then treat the line like a single field. */
  if (delim == NULL || delim[0] == '\0') {
    if (str_eq(value, line))
      return 0;
    return -1;
  }

  /* TODO(jhinds): get rid of the malloc'd buffer for holding line fields.
   * This could be done better, e.g. by using get_line_pos(). */

  /* this only needs to be just long enough to see if the
     field matches value (1 char longer), but making it a little
     bigger, just for fun.  and allocating max+1 so there's room for
     the null terminator. */
  max_field_chars = strlen(value) + 3;
  curfield = xmalloc(max_field_chars + 1);

  i = 0;
  curfield_len = 0;
  found = 0;

  while ((curfield_len = get_line_field(curfield, line,
                                        max_field_chars, i, delim)) > -1) {
    if (str_eq(curfield, value)) {
      found = 1;
      break;
    }
    i++;
  }

  free(curfield);

  if (found)
    return i;

  return -1;
}


#if defined(HAVE_CONFIG_H) && \
    defined(HAVE_FGETLN) && \
  ! defined(HAVE_GETLINE)

/* getline:  read a line, return length */
ssize_t getline(char **outbuf, size_t * outsize, FILE * fp) {
  char *buf;
  size_t len = 0;

  buf = fgetln(fp, &len);
  if (buf == NULL)
    return (-1);

  /* Don't assume realloc() accepts NULL for ptr (C99)
     [does not work on darwin] */
  if (*outbuf == NULL || *outsize < len + 1) {
    void *tmp;
    if (*outbuf == NULL) {
      tmp = xmalloc(len + 1);
    } else {
      tmp = xrealloc(*outbuf, len + 1);
    }
    *outbuf = tmp;
    *outsize = len + 1;
  }
  memcpy(*outbuf, buf, len);
  (*outbuf)[len] = '\0';
  return (len);
}
#endif
