/********************************
  copyright
 ********************************/
#include "mergekeys_main.h"
#include "mergekeys.h"
#include <locale.h>


/* macros to clarify the semantics of key comparisons. */

/* these are used to compare "keycmp" */
#define LEFT_LT_RIGHT(n) (n) < 0
#define LEFT_LE_RIGHT(n) (n) <= 0
#define LEFT_EQ_RIGHT(n) (n) == 0
#define LEFT_GT_RIGHT(n) (n) > 0
#define LEFT_GE_RIGHT(n) (n) >= 0

/* these are used to assign to keycmp */
#define LEFT_GREATER  1
#define RIGHT_GREATER -1
#define LEFT_RIGHT_EQUAL   0


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
int mergekeys ( struct cmdargs *args, int argc, char *argv[], int optind ){
	char default_delimiter[] = { 0xfe, 0x00 };
	FILE *left, *right, *out;	/* the two inputs & the output file ptrs */
	int fd_tmp, retval;	/* file descriptor and return value*/

	if ( argc - optind != 2 ) {
		fprintf(stderr, "missing file arguments.  see %s -h for usage information.\n", argv[0]);
		return EXIT_HELP;
	}

	if( (fd_tmp = open64(argv[optind], O_RDONLY)) < 0 ){
		perror(argv[optind]);
		return EXIT_FILE_ERR;
	}
	if ( (left = fdopen(fd_tmp, "r")) == NULL ) {
		perror(argv[optind]);
		return EXIT_FILE_ERR;
	}

	if( (fd_tmp = open64(argv[optind+1], O_RDONLY)) < 0 ){
		perror(argv[optind+1]);
		return EXIT_FILE_ERR;
	}
	if ( (right = fdopen(fd_tmp, "r")) == NULL ) {
		perror(argv[optind+1]);
		return EXIT_FILE_ERR;
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

	char *buffer_left, *buffer_right;		/* input line buffers for first & second files */
	ssize_t buffer_left_size, buffer_right_size;		/* size of the buffers */

	size_t nfields_left,		/* the number of fields in the first file */
	       nfields_right;		/* the number of fields in the second file */

	char field_left[MAX_FIELD_LEN+1],	/* buffers for holding field values */
	     field_right[MAX_FIELD_LEN+1];

	int i;				/* general-purpose counter */

	/** @todo take into account that files a & b might have the same fields in
	  * a different order.
	  */
	int *keyfields;			/* array of fields common to both files */
	int *mergefields;		/* fields only in file B - need to be added
					   to those in A */
	size_t nkeys, ntomerge;
	int keycmp = 0;
	int first_data_line = 1;
	int buffer_left_flag = 0;		/* 0 => there is no data line in bufx or the line in bufx has already been written.*/
	int buffer_right_flag = 0;		/* 1 => there is data in bufx which needs to be handled.*/


	buffer_left = buffer_right = NULL;
	buffer_left_size = buffer_right_size = 0;

	if ( getline(&buffer_left, &buffer_left_size, left) <= 0 ) {
		fprintf(stderr, "no header found in left-hand file\n");
		return EXIT_FILE_ERR;
	}
	if ( getline(&buffer_right, &buffer_right_size, right) <= 0 ) {
		fprintf(stderr, "no header found in right-hand file\n");
		return EXIT_FILE_ERR;
	}

	chomp(buffer_left);
	chomp(buffer_right);

	nfields_left = fields_in_line(buffer_left, args->delim);
	nfields_right = fields_in_line(buffer_right, args->delim);

	if ( args->verbose )
		printf("fields in left: %s\nfields in right: %s\n", buffer_left, buffer_right);

	if ( (keyfields = malloc(sizeof(int) * nfields_left)) == NULL ) {
		fprintf(stderr, "allocating key array - out of memory\n");
		free(buffer_left);
		free(buffer_right);
		return EXIT_MEM_ERR;
	}
	if ( (mergefields = malloc(sizeof(int) * nfields_right)) == NULL ) {
		fprintf(stderr, "allocating merge-fields array - out of memory\n");
		free(buffer_left);
		free(buffer_right);
		free(keyfields);
		return EXIT_MEM_ERR;
	}

	nkeys = ntomerge = 0;
	/* find the keys common to both files & the ones that need merged */
	for ( i = 0; i < (nfields_left < nfields_right ? nfields_left : nfields_right); i++ ) {
		get_line_field(field_left, buffer_left, MAX_FIELD_LEN, i, args->delim);
		get_line_field(field_right, buffer_right, MAX_FIELD_LEN, i, args->delim);
		if ( str_eq(field_left, field_right) )
			keyfields[ nkeys++ ] = i;
		else
			mergefields[ ntomerge++ ] = i;
	}

	/* if B had more fields than A, all of those need merged */
	for ( ; i < nfields_right; i++ ) {
		mergefields[ ntomerge++ ] = i;
	}

	/* print the headers which were already read in above */
	fputs(buffer_left, out);
	for ( i = 0; i < ntomerge; i++ ) {
		get_line_field(field_right, buffer_right, MAX_FIELD_LEN, mergefields[i], args->delim);
		fprintf(out, "%s%s", args->delim, field_right);
	}
	fputc('\n', out);

	/* this handles the case where A has only a header, forcing a
	   line-read at the beginning of the section that deals with
	   the tail end of file B. */
	keycmp = LEFT_GREATER;
	buffer_right_flag = 0;

	/* go through the rest of the 2 files */
	while ( !feof(left) ) {
		/* if keys are equal, there is a chance that the next line of file B will
		 * match the keys from A as well.  so keep the current line from file A.
		 */
		if ( first_data_line || LEFT_LT_RIGHT( keycmp ) ) {
			/* keys from A were smaller (A printed out).
			 * need another line from A
			 */
			if ( getline(&buffer_left, &buffer_left_size, left) <= 0 ) {
				/* EOF. make sure we don't print any A fields while
				   handling the rest of file B.
				 */
				keycmp = RIGHT_GREATER;
				break;
			}
			buffer_left_flag = 1;
			chomp(buffer_left);
		}
		if ( first_data_line || LEFT_GE_RIGHT( keycmp ) ) {
			/* keys were either the same for previous line,
			 * or keys from B were smaller (B printed out).
			 * need another line from B
			 */
			if ( getline(&buffer_right, &buffer_right_size, right) > 0 )
			{
				chomp(buffer_right);
				buffer_right_flag = 1;
			}
		}

		first_data_line = 0;

		if ( !feof(right) ) {

			/** @todo make sure the key fields are the same here */
			for ( i = 0; i < nkeys; i++ ) {
				get_line_field(field_left, buffer_left, MAX_FIELD_LEN, keyfields[i], args->delim);
				get_line_field(field_right, buffer_right, MAX_FIELD_LEN, keyfields[i], args->delim);
				if ( (keycmp = strcoll( field_left, field_right ) ) != 0 )
					break;
			}

			if ( args->left && LEFT_GT_RIGHT( keycmp ) ) {
				/* no match in B - don't print this line from A */
				buffer_right_flag = 0;
				continue;
			}

			/* if RIGHT line is greater than LEFT line, and LEFT has already
			   been printed once, just get another line from LEFT
			 */
			if ( LEFT_LT_RIGHT( keycmp ) && buffer_left_flag == 0 )
				continue;

			if ( LEFT_EQ_RIGHT( keycmp ) ) {
				/* keys are equal */
				fprintf(out, "%s", buffer_left);
				for ( i = 0; i < ntomerge; i++ ) {
					get_line_field(field_right, buffer_right, MAX_FIELD_LEN, mergefields[i], args->delim);
					fprintf(out, "%s%s", args->delim, field_right);
				}
				buffer_left_flag = 0;
				buffer_right_flag = 0;

				fputc('\n', out);
			}
			else if ( LEFT_LT_RIGHT( keycmp ) ) {
				/* key from LEFT is less than key from RIGHT:
				 * print line from LEFT with empty RIGHT fields
				 * and continue
				 */
				fprintf(out, "%s", buffer_left);
				for ( i = 0; i < ntomerge; i++ )
					fputs(args->delim, out);
				buffer_left_flag = 0;

				fputc('\n', out);
			}
			else {
				/* key from LEFT is greater than key from RIGHT:
				 * print RIGHT fields with empty LEFT fields
				 * and continue
				 */
				int j = 0;
				/* print the keys from RIGHT */
				for ( i = 0; i < nfields_left; i++ ) {
					if ( keyfields[j] == i ) {
						get_line_field(field_right, buffer_right, MAX_FIELD_LEN, keyfields[j], args->delim);
						fprintf(out, "%s%s", (j > 0 ? args->delim : ""), field_right);
						j++;
					} else {
						fputs(args->delim, out);
					}
				}

				/* print the non-keys from RIGHT */
				for ( i = 0; i < ntomerge; i++ ) {
					get_line_field(field_right, buffer_right, MAX_FIELD_LEN, mergefields[i], args->delim);
					fprintf(out, "%s%s", args->delim, field_right);
				}

				buffer_right_flag = 0;

				fputc('\n', out);
			}
		}
		else {	/* feof(right) */

			/* no more lines in file B - if line from A hasn't been printed yet,
			   just print it with empty B fields and force the comparison 
			   result to show B as greater than A.
			 */

			if ( buffer_left_flag == 1 ) {
				fprintf(out, "%s", buffer_left);
				for ( i = 0; i < ntomerge; i++ )
					fputs(args->delim, out);
				fputc('\n', out);
			}
			keycmp = RIGHT_GREATER;
			buffer_left_flag = 0;
			buffer_right_flag = 0;

		}
	}

	/* if there's anything left in file B, print it out with empty fields for A */
	if ( ! feof(right) ) {

		/* Handle all lines from B with keys less than last line of A */
		while ( LEFT_GT_RIGHT( keycmp ) && ! feof(right) ) {
			int j = 0;
			if ( buffer_right_flag == 0 ) {
				if ( getline(&buffer_right, &buffer_right_size, right) <= 0 )
					break;
				chomp(buffer_right);
				buffer_right_flag = 1;
			}

			for ( i = 0; i < nkeys; i++ ) {
				get_line_field(field_left, buffer_left, MAX_FIELD_LEN, keyfields[i], args->delim);
				get_line_field(field_right, buffer_right, MAX_FIELD_LEN, keyfields[i], args->delim);
				if ( (keycmp = strcoll( field_left, field_right ) ) != 0 )
					break;
			}

			if ( ! LEFT_GT_RIGHT( keycmp ) ) {
				break;
			}

			if ( args->left ) {
				/* skip non-matching lines */
				buffer_right_flag = 0;
				continue;
			}

			/* print the keys from B */
			for ( i = 0; i < nfields_left; i++ ) {
				if ( keyfields[j] == i ) {
					get_line_field(field_right, buffer_right, MAX_FIELD_LEN, keyfields[j], args->delim);
					fprintf(out, "%s%s", (j > 0 ? args->delim : ""), field_right);
					j++;
				} else {
					fputs(args->delim, out);
				}
			}

			/* print the non-keys from B */
			for ( i = 0; i < ntomerge; i++ ) {
				get_line_field(field_right, buffer_right, MAX_FIELD_LEN, mergefields[i], args->delim);
				fprintf(out, "%s%s", args->delim, field_right);
			}

			fputc('\n', out);

			buffer_right_flag = 0;
		}

		/* Handle all lines from B with keys equal to the last line of A */
		while ( LEFT_EQ_RIGHT( keycmp ) && ! feof(right) ) {
			if ( buffer_right_flag == 0 ) {
				if ( getline(&buffer_right, &buffer_right_size, right) <= 0 )
					break;
				chomp(buffer_right);
				buffer_right_flag = 1;
			}

			for ( i = 0; i < nkeys; i++ ) {
				get_line_field(field_left, buffer_left, MAX_FIELD_LEN, keyfields[i], args->delim);
				get_line_field(field_right, buffer_right, MAX_FIELD_LEN, keyfields[i], args->delim);
				if ( (keycmp = strcoll( field_left, field_right ) ) != 0 )
					break;
			}

			if ( ! LEFT_EQ_RIGHT( keycmp ) ) {
				break;
			}

			fprintf(out, "%s", buffer_left);
			for ( i = 0; i < ntomerge; i++ ) {
				get_line_field(field_right, buffer_right, MAX_FIELD_LEN, mergefields[i], args->delim);
				fprintf(out, "%s%s", args->delim, field_right);
			}

			fputc('\n', out);

			buffer_left_flag = 0;
			buffer_right_flag = 0;
		}

		/* if the last line from A hasn't been printed yet, and
		   no match was found in B, print A line now with empty B
		   fields. */
		if ( buffer_left_flag != 0 ) {
			fprintf(out, "%s", buffer_left);
			for ( i = 0; i < ntomerge; i++ )
				fputs(args->delim, out);
			fputc('\n', out);
		}

		/* If a match between A and B is required (i.e. args->left),
		   we can safely skip the rest of file B. */
		if ( LEFT_LT_RIGHT( keycmp ) && ! args->left ) {

			/* The last line of A has definitely now been printed. dump the rest of
			   file B with empties for non-key A fields */
			while (! feof(right) ) {
				int j = 0;

				if ( buffer_right_flag == 0 ) {
					if ( getline(&buffer_right, &buffer_right_size, right) <= 0 )
						break;
					chomp(buffer_right);
					buffer_right_flag = 1;
				}

				/* print the keys from B */
				for ( i = 0; i < nfields_left; i++ ) {
					if ( keyfields[j] == i ) {
						get_line_field(field_right, buffer_right, MAX_FIELD_LEN, keyfields[j], args->delim);
						fprintf(out, "%s%s", (j > 0 ? args->delim : ""), field_right);
						j++;
					} else {
						fputs(args->delim, out);
					}
				}

				/* print the non-keys from B */
				for ( i = 0; i < ntomerge; i++ ) {
					get_line_field(field_right, buffer_right, MAX_FIELD_LEN, mergefields[i], args->delim);
					fprintf(out, "%s%s", args->delim, field_right);
				}

				fputc('\n', out);
				buffer_right_flag = 0;
			}
		}

	}

	free(buffer_left);
	free(buffer_right);
	free(keyfields);
	free(mergefields);
	return EXIT_OKAY;
}

