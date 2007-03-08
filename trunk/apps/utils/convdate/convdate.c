/********************************
  copyright
 ********************************/
#include "convdate_main.h"
#include "convdate.h"

/** @brief  
  * 
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int convdate ( struct cmdargs *args, int argc, char *argv[], int optind ){

	char default_delim[2] = { 0xfe, 0x00 };	/* default field separator*/

	FILE *in  = stdin;
	FILE *out = stdout;	/* input & output files */

	int field_no;		/* the field number specified by the user */

	char *buffer   = NULL;	/* buffer for file input & its size */
	ssize_t bufsz  = 0;
	
	struct tm storage = {0,0,0,0,0,0,0,0,0};	/* storage for the time */

	char date[64] = "";

        char *token = NULL;
        int tok_no  = 0;
        size_t size = 0;

	// Set default value for the field if necessary.
	if ( ! args->field ) {
		field_no = 1;
	} else {
		field_no = atoi(args->field);
		if ( field_no < 0 ) {
			fprintf(stderr, "%d: invalid field number.\n", field_no);
			return EXIT_HELP;
		}
	}

	// Set default delimiter if necessary.
	if ( ! args->delim ) {
		args->delim = default_delim;
	}

	// Process the input stream
	if ( in != NULL ) {

		// Should we preserve the header line?
		if ( args->preserve_header ) {

			// Yes => Do we have a line at all?
			if ( getline(&buffer, &bufsz, in) > 0 ) {

				// Yes => Just print this to the output file
				fprintf(out, "%s", buffer);
			}
		}

		// Process each line
		int no_chars = 0;
		int start = 0;
		int end = 0;
		while ( getline(&buffer, &bufsz, in) > 0 ) {

			// Remove new line
			chomp(buffer);

			// Find the field
			if((no_chars = get_line_field(date, buffer, 64, field_no - 1, args->delim)) != -1) {

	                        // Yes => Convert input date into a time value. Success?
        	                if(strptime(date, args->input_format, &storage) != NULL) {

                	              	// Yes => Convert time value into a string
                        	        size = strftime(date, 64, args->output_format, &storage);
        	                } else {

                	                // No => Bail out
                        	        fprintf(stderr, "could not convert date \"%s\"\n", token);
                                	return EXIT_HELP;
                       		}
			} else {

				// Bail out as we have not found the field.
				fprintf(stderr, "did not find the field at %i\n", field_no);
                                return EXIT_HELP;
			} 

			// Find start and end date of the field
			if(get_line_pos(buffer, field_no - 1, args->delim, &start, &end)) {

				// Cut off the first part before the field.
				buffer[start] = '\0';

				// Now write the first part, the date itself and the remainder of the line
				fprintf(out, "%s%s%s\n", buffer, date, buffer + end + 1);
			}
		}
	}

	if ( buffer ) {
		free(buffer);
	}

	if ( token ) {
		free(token);
	}

	return EXIT_OKAY;
}

