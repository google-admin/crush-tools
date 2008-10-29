/** \file dbfr.h
  *
  * \brief A double-buffered file reader, meaning that the current and
  * next lines of a file are both buffered and available for use.
  *
  * This is primarily useful for peeking at the next line of a file when
  * fseek(3) cannot be reliably used (e.g. when the file is stdin)
  */
#ifndef DOUBLE_BUFFERED_FILE_READER_H
#define DOUBLE_BUFFERED_FILE_READER_H

#include <stdio.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#  ifdef HAVE_SYS_TYPES_H
#    include <sys/types.h> /* ssize_t */
#  endif
#else
#  include <sys/types.h>
#endif

/** \brief a double-buffered file reader type.
  *
  * None of the fields in this structure should be modified by user code.
  * This includes reads, writes, seeks, etc. of the FILE member.
  */
typedef struct {
  size_t line_no;           /**< \brief the line number of current_line. */
  char *current_line;       /**< \brief holds the most recently read line. */
  ssize_t current_line_len; /**< \brief the length of the current line. */
  size_t current_line_sz;   /**< \brief the size of the current_line buffer. */
  char *next_line;          /**< \brief holds the next line to be read. */
  ssize_t next_line_len;    /**< \brief the length of the next line. */
  size_t next_line_sz;      /**< \brief the size of the next_line buffer. */
  FILE *file;               /**< \brief the file being read. */
} dbfr_t;

/** \brief opens FILENAME for reading with a double-buffered reader.
  *
  * Upon successful initialization, the next_line will be initialized to the
  * first line of the file, 
  *
  * \param filename if NULL, the reader will attach to stdin.  Otherwise the
  *                 named file is opened for reading.
  *
  * \returns a double-buffered file reader object, or NULL if the file cannot
  *          be opened.
  */
dbfr_t * dbfr_open(char *filename);

/** \brief initializes a double-buffered reader from an already opened file.
  *
  * \param fp the readable file pointer to use.
  *
  * \return a new double-buffered reader object, or NULL if the file pointer
  *         is invalid.
  */
dbfr_t * dbfr_init(FILE *fp);

/** \brief gets the next line of the file and stores it in the current_line
  *        buffer.
  *
  * External copies of current_line and next_line pointers will be invalidated
  * after calling this function.
  *
  * \param reader a valid double-buffered reader object.
  *
  * \returns the length of the current line, zero on EOF, or a negative value
  *          on error.
  */
ssize_t dbfr_getline(dbfr_t *reader);

/** \brief closes a double-buffered reader's file and releases its resources.
  *
  * \param reader a double-buffered reader object.
  */
void dbfr_close(dbfr_t *reader);

#endif /* DOUBLE_BUFFERED_FILE_READER_H */
