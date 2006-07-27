/** @file ffutils.c some utilities for working with flat files.
  * @author jeremy hinds
  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef FFUTILS_H
#define FFUTILS_H

/* if using gnu libc, define this */
#define HAVE_GETLINE

/** string comparison - equals */
#define str_eq(a, b) \
	( strcmp(a, b) == 0 )

/** string comparison - less than */
#define str_lt(a, b) \
	( strcmp(a, b) < 0 )

/** string comparison - greater than */
#define str_gt(a, b) \
	( strcmp(a, b) > 0 )

/** MM-DD-YYYY string date comparison - equals */
#define date_eq(a, b) \
	( mdyhms_datecmp(a, b) == 0 )

/** MM-DD-YYYY string date comparison - less than */
#define date_lt(a, b) \
	( mdyhms_datecmp(a, b) < 0 )

/** MM-DD-YYYY string date comparison - greater than */
#define date_gt(a, b) \
	( mdyhms_datecmp(a, b) > 0 )


/** @brief a duff device for unrolling loops by a factor of 8.
  * @param n the number of times to execute the code.
  * @param c the code to be executed.
  */
#define DUFF_DEV_8( n, c ) \
        do {                                    \
                int dd8_i = (n)/8;              \
                if ( ! (n) )                    \
                        break;                  \
                switch((n)%8) {                 \
                        case 0: do {    (c);    \
                        case 7:         (c);    \
                        case 6:         (c);    \
                        case 5:         (c);    \
                        case 4:         (c);    \
                        case 3:         (c);    \
                        case 2:         (c);    \
                        case 1:         (c);    \
                                } while ( dd8_i-- > 0 ); \
                }                               \
        } while (0)

/** @brief scans a data transfer file header for the location of some fields. 
  * 
  * @param header first line of the dt file
  * @param delim file delimiter
  * 
  * @return populated structure with the 0-based indexes of columns
  */
/* struct field_index findfields( char *header, const char *delim ); */

/** @brief finds the number of fields in a delimited string.
  * 
  * @param l a delimited string
  * @param d delimiter
  * 
  * @return the number of fields in the string
  */
size_t fields_in_line(const char *l, const char *d);

/** @brief extract a field from a delimited string.
  *
  * copies at most <i>n</i> characters from field <i>i</i> of
  * <i>delim</i>-delimited string <i>ct</i> into <i>cs</i>.
  * 
  * @param cs destination buffer
  * @param ct delimited string
  * @param n max chars to copy into buffer
  * @param i field to be copied
  * @param delim delimiter of ct
  * 
  * @return number of chars copied into buffer, or -1 if i is greater
  * than the number of fields in ct
  */
int get_line_field(char *cs, const char *ct, const size_t n, const int i, const char *delim) ;

/** @brief returns a pointer into a delimited string where a particular field begins.
  * 
  * @param cs a delimited string
  * @param fn the desired field number
  * @param delim the delimiting string
  * 
  * @return a pointer into cs where the fn-th field begins
  */
char * field_start(const char *cs, size_t fn, const char *delim) ;

/** @brief compares two date strings in the format MM-DD-YYYY_HH24:MI:SS
  * 
  * @param a first date
  * @param b second date
  * 
  * @return -1 if a < b; 0 if a == b; 1 if a > b.
  */
int mdyhms_datecmp(const char *a, const char *b);

/** @brief removes linebreak characters from the end of a string.
  * @param s a null-terminated array of characters
  */
void chomp(char *s);

/** @brief goes through a list of file names, attempting to open them.
  * it loops through trying to open each file until one is successfully
  * opened or there are no more file names in the list.
  * 
  * @param argc the number of arguments from the commandline
  * @param argv the array of arguments from the commandline
  * @param optind the index if the next argument to be processed
  * @param mode the mode for opening the file
  * 
  * @return the next successfully opened file or NULL
  */
FILE * nextfile( int argc, char *argv[], int *optind, const char *mode );

/** @brief expands escaped special characters within a string, modifying
  * the original string.
  * 
  * the following are valid escape sequences and the ascii character
  * to which they expand:
  *
  *  - \a - 0x07
  *  - \b - 0x08
  *  - \f - 0x0c
  *  - \n - 0x0a
  *  - \r - 0x0d
  *  - \t - 0x09
  *  - \v - 0x0b
  *  - \\ - 0x5c
  *
  * @param s string potentially containing escaped characters
  */
void expand_chars(char *s);


#ifndef HAVE_GETLINE
# ifndef GETLINE_BUF_INC 
# define GETLINE_BUF_INC 64
# endif

/** 
  * @brief reads a line from a file and dynamically allocates enough storage to hold it.
  * 
  * this is a rewrite of the getline() from gnu's c library.
  *
  * @param **lineptr pointer to string which will hold the file line
  * @param *n pointer to the amount of memory allocated to *lineptr
  * @param stream file to read from
  * 
  * @return length of the file line on success, or -1 on memory allocation error
  */
ssize_t getline ( char **lineptr, size_t *n, FILE *stream );

#endif /* HAVE_GETLINE */

#endif /* FFUTILS_H */
