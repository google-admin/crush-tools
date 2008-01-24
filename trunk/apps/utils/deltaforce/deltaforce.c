/********************************
  copyright
 ********************************/
#include "deltaforce_main.h"
#include "deltaforce.h"

#ifndef HAVE_OPEN64
# define open64 open
#endif


#define Fputs(s,f)					\
	do { if ( fputs((s), (f)) == EOF ) {		\
		warn("error writing to output:");	\
		return EXIT_FILE_ERR;			\
	} } while ( 0 )

char *delim;

int *keyfields = NULL;			/* array of fields common to both files */
size_t keyfields_sz = 0;
ssize_t nkeys;

/** @brief opens all the files necessary, sets a default
  * delimiter if none was specified, and calls the
  * merge_files() function.
  *
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  *
  * @return exit status for main() to return.
  */
int deltaforce ( struct cmdargs *args, int argc, char *argv[], int optind ){
	char default_delimiter[] = { 0xfe, 0x00 };
	FILE *left, *right, *out;	/* the two inputs & the output file ptrs */
	int fd_tmp, retval;		/* file descriptor and return value*/

	if ( argc - optind != 2 ) {
		fprintf(stderr, "missing file arguments.  see %s -h for usage information.\n", argv[0]);
		return EXIT_HELP;
	}

	if ( str_eq( argv[optind], argv[optind + 1] ) ) {
		fprintf(stderr, "%s: 2 input files cannot be the same\n",
				argv[0]);
		return EXIT_HELP;
	}

	if ( str_eq( argv[optind], "-" ) ) {
		left = stdin;
	}
	else {
		if( (fd_tmp = open64(argv[optind], O_RDONLY)) < 0 ){
			perror(argv[optind]);
			return EXIT_FILE_ERR;
		}
		if ( (left = fdopen(fd_tmp, "r")) == NULL ) {
			perror(argv[optind]);
			return EXIT_FILE_ERR;
		}
	}

	if ( str_eq( argv[optind + 1], "-" ) ) {
		right = stdin;
	}
	else {
		if( (fd_tmp = open64(argv[optind+1], O_RDONLY)) < 0 ){
			perror(argv[optind+1]);
			return EXIT_FILE_ERR;
		}
		if ( (right = fdopen(fd_tmp, "r")) == NULL ) {
			perror(argv[optind+1]);
			return EXIT_FILE_ERR;
		}
	}

	if ( ! args->outfile )
		out = stdout;
	else {
		if( (fd_tmp = open64(args->outfile, O_WRONLY | O_CREAT | O_TRUNC)) < 0 ){
			perror(args->outfile);
			return EXIT_FILE_ERR;
		}
		fchmod(fd_tmp, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		if ( (out = fdopen(fd_tmp, "w")) == NULL ) {
			perror(args->outfile);
			return EXIT_FILE_ERR;
		}
	}

	if ( ! args->delim ) {
		args->delim = getenv("DELIMITER");
		if ( ! args->delim )
			args->delim = default_delimiter;
	}
	expand_chars(args->delim);
	delim = args->delim;

	if ( args->keys ) {
		int i;
		nkeys = expand_nums( args->keys, &keyfields, &keyfields_sz );
		if ( nkeys < 1 ) {
			fprintf( stderr, "%s: bad key specified: \"%s\"\n",
					getenv("_"), args->keys );
			return EXIT_HELP;
		}
		for ( i = 0; i < nkeys; i++ )
			keyfields[i]--;
	}
	else {
		keyfields = malloc( sizeof(int) );
		keyfields[0] = 0;
		nkeys = 1;
	}

	/* set locale with values from the environment so strcoll()
	   will work correctly. */
	setlocale(LC_ALL, "");
	setlocale(LC_COLLATE, "");

	retval = merge_files( left, right, out, args );

	fclose(left);
	fclose(right);
	fclose(out);

	return retval;
}


int merge_files( FILE *left, FILE *right, FILE *out, struct cmdargs *args ) {

	int retval = EXIT_OKAY;

	/* input line buffers for first & second files */
	char *buffer_left = NULL;
	char *buffer_right = NULL;

	/* size of the buffers */
	size_t buffer_left_size = 0;
	size_t buffer_right_size = 0;

	char field_right[MAX_FIELD_LEN+1];	/* buffer for holding field values */

	int i;				/* general-purpose counter */

	/** @todo take into account that files a & b might have the same fields in
	  * a different order.
	  */
	int keycmp = 0;

	/* assume that if there is a header line, it exists
	   in both files. */

	while ( ! feof( left ) ) {

		if ( buffer_left == NULL || buffer_left[0] == '\0' ) {
			/* get a line from the full set */
			if ( getline(&buffer_left, &buffer_left_size, left) <= 0 ) {
				free( buffer_left );
				buffer_left = NULL;
				break;
			}
		}

		if ( buffer_right == NULL || buffer_right[0] == '\0' ) {
			if ( feof( right ) ) {
				/* no more delta data to merge in: just dump
				   the rest of the full data set. */
				while ( getline(&buffer_left, &buffer_left_size, left) > 0 )
					Fputs( buffer_left, out );
				continue;
			}

			/* get a line from the delta set */
			if ( getline(&buffer_right, &buffer_right_size, right) <= 0 ) {
				free(buffer_right);
				buffer_right = NULL;
				continue;
			}
		}

		keycmp = compare_keys ( buffer_left, buffer_right );

		switch ( keycmp ) {
			/* keys equal - print the delta line and scan
			   forward in both files the next time around. */
			case 0:	Fputs( buffer_right, out );
				buffer_right[0] = '\0';
				buffer_left[0] = '\0';
				break;

			/* delta line greater than full-set line.
			   print the full set line and keep the delta
			   line for later.
			 */
			case -1: Fputs( buffer_left, out );
				buffer_left[0] = '\0';
				break;

			/* delta line less than full-set line: the full
			   set did not previously contain the key from
			   delta. */
			case 1: Fputs( buffer_right, out );
				 buffer_right[0] = '\0';
				 break;
		}
	}

	if ( buffer_right != NULL && buffer_right[0] != '\0' )
		Fputs( buffer_right, out );

	if ( ! feof( right ) ) {
		while ( getline(&buffer_right, &buffer_right_size, right) > 0 )
			Fputs( buffer_right, out );
	}

cleanup:
	if ( buffer_left )
		free(buffer_left);
	if ( buffer_right )
		free(buffer_right);
	if ( keyfields )
		free(keyfields);

	return retval;
}


int compare_keys ( char *buffer_left, char *buffer_right ) {
	int keycmp = 0;
	int i;
	char field_left[MAX_FIELD_LEN+1];
	char field_right[MAX_FIELD_LEN+1];

	// printf("inside compare_keys([%s], [%s])\n", buffer_left, buffer_right);
	if ( buffer_left == NULL && buffer_right == NULL ) {
		return LEFT_RIGHT_EQUAL;
	}

	/* these special cases may seem counter-intuitive, but saying that
	   a NULL line is greater than a non-NULL line results in
	   the non-NULL line getting printed and a new line read in.
	 */
	if ( buffer_left == NULL )
		return LEFT_GREATER;

	if ( buffer_right == NULL )
		return RIGHT_GREATER;

	for ( i = 0; i < nkeys; i++ ) {
		get_line_field(field_left, buffer_left, MAX_FIELD_LEN, keyfields[i], delim);
		get_line_field(field_right, buffer_right, MAX_FIELD_LEN, keyfields[i], delim);
		/* printf("Comparing (%s) to (%s) inside compare_keys\n", field_left, field_right); */
		if ( (keycmp = strcoll( field_left, field_right ) ) != 0 )
			break;
	}

	/* ensure predictable return values */
	if ( keycmp == 0 ) return 0;
	if ( keycmp < 0  ) return -1;
	if ( keycmp > 0  ) return 1;
}

