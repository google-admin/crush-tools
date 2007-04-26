/********************************
  copyright
 ********************************/
#include "grepfield_main.h"
#include "grepfield.h"

/** @brief  
  * 
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int grepfield ( struct cmdargs *args, int argc, char *argv[], int optind ){

	char default_delim[2] = { 0xfe, 0x00 };	/* default field separator*/

	regex_t pattern;
	int reg_flags;		/* flags to pass to regcomp() */
	int err_code;		/* holds the return value of regcomp() */
	
	FILE *in, *out;		/* input & output files */
	int field_no;		/* the field number specified by the user */

	char *buffer   = NULL;	/* buffer for file input & its size */
	ssize_t bufsz  = 0;
	char *fieldval = NULL;	/* buffer for the field to scan & its size */
	ssize_t fldsz  = 0;

	/* function to pull the field out of the input line
	   using a pointer so the case of no field can be treated
	   the same as the normal case inside the file reading loop.
	 */
	char * (* field_to_scan)(char **, ssize_t *, char *, char *, int f);

	if ( ! args->field ) {
		field_no = -1;
		field_to_scan = scan_wholeline;
	} else {
		field_no = atoi(args->field) - 1;
		field_to_scan = scan_field;
		if ( field_no < 0 ) {
			fprintf(stderr, "%d: invalid field number.\n", field_no);
			return EXIT_HELP;
		}
	}

	if ( optind >= argc ) {
		usage(argv[0]);
		exit(EXIT_HELP);
	}

	if ( ! args->delim ) {
		args->delim = getenv("DELIMITER");
		if ( ! args->delim )
			args->delim = default_delim;
	}
	expand_chars(args->delim);

	reg_flags = REG_EXTENDED;
	if ( args->ignore_case ) reg_flags |= REG_ICASE;

	err_code = regcomp(&pattern, argv[optind++], reg_flags) ;
	if ( err_code != REG_OK ) {
		re_perror(err_code, pattern);
		return EXIT_HELP;
	}

	if ( args->outfile ) {
		if ( (out = fopen(args->outfile, "w")) == NULL ) {
			perror(args->outfile);
			exit(EXIT_FILE_ERR);
		}
	} else {
		out = stdout;
	}

	if ( optind < argc )
		in = nextfile(argc, argv, &optind, "r");
	else
		in = stdin;


	/* set the flags variable to the expected return value
	   of regexec() */
	if ( args->invert )
		reg_flags = REG_NOMATCH;
	else
		reg_flags = 0;

	while ( in != NULL ) {

		// Should we preserve the header line?
		if ( args->preserve_header ) {
			// Yes => Do we have a line at all?
			if ( getline(&buffer, &bufsz, in) > 0 ) {
				// Yes => Just print this to the output file
				fprintf(out, "%s", buffer);
			}
		}

		while ( getline(&buffer, &bufsz, in) > 0 ) {
			chomp(buffer);
			if ( field_to_scan(&fieldval, &fldsz, buffer, args->delim, field_no) == NULL )
				continue;
			if ( regexec(&pattern, fieldval, 0, NULL, 0) == reg_flags )
				fprintf(out, "%s\n", buffer);
		}

		fclose(in);
		in = nextfile(argc, argv, &optind, "r");
	}

	if ( fieldval != buffer )
		free(fieldval);

	if ( buffer )
		free(buffer);

	regfree(&pattern);
	fclose(out);

	return EXIT_OKAY;
}

/* just points the field buffer at the original line */
char * scan_wholeline(char **field_buffer, ssize_t *field_buffer_size, char *orig_line, char *delim, int field_no){
	*field_buffer = orig_line;
	return *field_buffer;
}

/* (re)allocates memory for the field buffer as necessary and copies
 * the desired field into it.
 */
char * scan_field(char **field_buffer, ssize_t *field_buffer_size, char *orig_line, char *delim, int field_no){
	if ( *field_buffer_size < strlen(orig_line) ) {
		char *tmp;
		if ( *field_buffer )
			tmp = realloc(*field_buffer, strlen(orig_line));
		else
			tmp = malloc(strlen(orig_line));
		if ( tmp == NULL ) {
			fprintf(stderr, "out of memory");
			exit(EXIT_MEM_ERR);
		}
		*field_buffer = tmp;
	}

	if ( get_line_field(*field_buffer, orig_line, *field_buffer_size, field_no, delim) < 0 )
		return NULL;

	return *field_buffer;
}

void re_perror( int err_code, regex_t pattern ) {
	size_t len;
	char *buf;
	len = regerror(err_code, &pattern, NULL, 0);
	buf = malloc(len);
	regerror(err_code, &pattern, buf, len);
	fprintf(stderr, "%s\n", buf);
	free(buf);
}
