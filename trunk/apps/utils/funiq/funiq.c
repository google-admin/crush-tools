/********************************
  copyright
 ********************************/
#include "funiq_main.h"

#include <ffutils.h>
#include <splitter.h>


#define FIELD_LEN_LIMIT 255


/** @brief  
  * 
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int funiq ( struct cmdargs *args, int argc, char *argv[], int optind ){

	char delim[] = { 0xfe , 0x00 };	/* the delimiter */
	int *fields;			/* array of field indexes */
	size_t n_fields;		/* the size of the array */
	FILE *in;			/* input file pointer */
	char **prev_line;		/* fields from previous line of input */
	char cur_field[FIELD_LEN_LIMIT];

	int i;
	char *buf;
	size_t bufsz;

	/* use the default delimiter if necessary */
	if ( ! args->delim )
		args->delim = delim ;

	/* turn the user-provided list of field numbers
	   into an array of ints */
	fields = malloc(sizeof(int) * 32);
	if ( ! fields ) {
		fprintf(stderr, "out of memory\n");
		return ( EXIT_MEM_ERR );
	}
	n_fields = splitnums( args->fields, &fields, 32 );

	/* prepare the array of previous field values */
	prev_line = malloc ( sizeof( char* ) * n_fields );
	for ( i = 0; i < n_fields; i++ ) {
		prev_line[i] = malloc ( sizeof( char* ) * FIELD_LEN_LIMIT );
	}

	/* get the first file */
	if ( optind < argc )
		in = nextfile( argc, argv, &optind, "r");
	else
		in = stdin;

	/* get the first line to seed the prev_line array */
	buf = NULL;
	bufsz = 0;

	getline(&buf, &bufsz, in);

	for ( i = 0; i < n_fields; i++ ) {
		get_line_field( prev_line[i], buf, FIELD_LEN_LIMIT - 1,
				fields[i] - 1, args->delim);
	}
	printf("%s", buf);	/* first line is never a dup */

	while ( in ) {
		int matching_fields;
		while ( getline(&buf, &bufsz, in) > 0 ) {
			matching_fields = 0;
			for ( i = 0; i < n_fields; i++ ) {
				/* extract the field from the input line */
				get_line_field( cur_field, buf, FIELD_LEN_LIMIT - 1,
						fields[i] - 1, args->delim);

				/* see if the field is a duplicate */
				if ( str_eq(cur_field, prev_line[i]) )
					matching_fields++;

				/* store this line's value */
				strcpy(prev_line[i], cur_field);
			}

			/* if not all of the fields matched, the line
			   wasn't a duplicate */
			if ( matching_fields != n_fields )
				printf("%s", buf);
		}

		fclose(in);
		in = nextfile( argc, argv, &optind, "r");
	}

	for ( i = 0; i < n_fields; i++ ) {
		free(prev_line[i]);
	}
	free(prev_line);
	free(fields);
	return EXIT_OKAY;
}

