/** @file ffutils.h
  * @brief some utilities for working with flat files.
  * @author jeremy hinds
  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef FFUTILS_H
#define FFUTILS_H

/** @brief string comparison - equals */
#define str_eq(a, b) \
	( strcmp(a, b) == 0 )

/** @brief string comparison - less than */
#define str_lt(a, b) \
	( strcmp(a, b) < 0 )

/** @brief string comparison - greater than */
#define str_gt(a, b) \
	( strcmp(a, b) > 0 )

/** @brief MM-DD-YYYY string date comparison - equals */
#define date_eq(a, b) \
	( mdyhms_datecmp(a, b) == 0 )

/** @brief MM-DD-YYYY string date comparison - less than */
#define date_lt(a, b) \
	( mdyhms_datecmp(a, b) < 0 )

/** @brief MM-DD-YYYY string date comparison - greater than */
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

/** @brief the amount by which expand_nums() will increment the size of
  * the target array when reallocating memory.
  */
#define FFUTILS_RESIZE_AMT 16

/* * @brief scans a data transfer file header for the location of some fields. 
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
  * @param i field to be copied (0-based)
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

/** @brief compares two date strings in the format DD-MM-YYYY_HH24:MI:SS
  * 
  * @param a first date
  * @param b second date
  * 
  * @return -1 if a < b; 0 if a == b; 1 if a > b.
  */
int dmyhms_datecmp(const char *a, const char *b);

/** @brief removes linebreak characters from the end of a string.
  * @param s a null-terminated array of characters
  */
void chomp(char *s);

/** @brief replaces trailing white space in a string with null characters.
  *
  * @param str the string to be trimmed.
  */
void trim(char *str);

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
  *  - \\a - 0x07
  *  - \\b - 0x08
  *  - \\f - 0x0c
  *  - \\n - 0x0a
  *  - \\r - 0x0d
  *  - \\t - 0x09
  *  - \\v - 0x0b
  *  - \\\\ - 0x5c
  *
  * @param s string potentially containing escaped characters
  */
void expand_chars(char *s);

/** @brief splits a string of number lists/ranges into an array.
  *
  * The "arg" string should be dynamically allocated, since strtok()
  * doesn't like const char[];
  *
  * "array" should also be dynamically allocated, as it may be realloc()'d
  * to accomodate more elements.  If memory hasn't been allocated for array,
  * set the pointer to NULL and pass 0 as array_size.  Memory will be
  * allocated for you, but remember to call free() on array when finished
  * with it.
  *
  * All of the parameters may be modified during execution.
  *
  * @param arg string representation of numbers
  * @param array pointer to array of ints
  * @param array_size number of elements array can currently hold
  *
  * @return on success, the number of items in populated array,
  *     -1 on memory allocation error,
  * 	-2 if arg has invalid syntax
  */
ssize_t expand_nums ( char *arg, int **array, size_t *array_size );

/** @brief determines the position of the first and the last character
  * of the given field.
  *
  * @param ct the line buffer
  * @param i the field number (0-based)
  * @param d the field delimiter
  * @param start the position of the start character
  * @param end the position of the end character
  *
  * @return 1 on success, 0 otherwise
  */
int get_line_pos( const char *ct, const int i, const char *d, int *start, int *end);

/** @brief removes a field from a line.
  *
  * @param *ct the line buffer (buffer is changed!)
  * @param i the field number (0-based)
  * @param *d the field delimiter
  *
  * @return pointer to ct
  */
char * cut_field( char *ct, const int i, const char *d);

/** @brief locates the index of the first field in a delimited string
  * having a specific value.
  * 
  * @param value the string to be located
  * @param line  delimited string
  * @param delim line's field separator
  * 
  * @return 0-based index of the first field having the specified value,
  * -1 if not found, or -2 on error.
  */
ssize_t field_str( const char *value, const char *line, const char *delim);

#endif /* FFUTILS_H */
